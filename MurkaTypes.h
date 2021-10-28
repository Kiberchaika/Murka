#pragma once

enum TextAlignment {TEXT_LEFT, TEXT_CENTER, TEXT_RIGHT};


#if defined(MURKA_OF)
#include "ofMain.h"
#elif defined(MURKA_JUCE)

#include <JuceHeader.h>
#include <juce_opengl/juce_opengl.h>

#if defined(MURKA_JUCE) && defined(WIN32)
#define GL_STREAM_DRAW juce::GL_STREAM_DRAW
#define GL_ARRAY_BUFFER juce::GL_ARRAY_BUFFER
#define GL_RGBA32F juce::GL_RGBA32F
#define GL_STATIC_DRAW juce::GL_STATIC_DRAW
#define GL_TEXTURE0 juce::GL_TEXTURE0
#endif

#ifndef M_PI
#define M_PI           3.14159265358979323846264338327950288  /* pi */
#endif


#include <stdint.h>
#include <filesystem>
#include <limits>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <random>
#include <map>

using namespace std;

#endif

// Here's the global typedefs for cross-render functionality
#if defined(MURKA_OF)
#include "ofMain.h"
#include "ofxFontStash.h"
typedef ofxFontStash FontObject; // It's important for this object to exist in a render for Murka to work
#elif defined(MURKA_JUCE)
#include "juce_murka/juce_fontStash.h"
typedef juceFontStash FontObject; // It's important for this object to exist in a render for Murka to work
#endif

#include <iostream>
#include <sstream>
#include <cmath>

template <typename T>
std::string to_string_with_precision(const T a_value, const int n = 6)
{
    std::ostringstream out;
    out.precision(n);
    out << std::fixed << a_value;
    return out.str();
}

namespace murka {

enum MurkaKey {
	MURKA_KEY_SHIFT = 0x1,
	MURKA_KEY_CONTROL = 0x2,
	MURKA_KEY_ALT = 0x4,
	MURKA_KEY_SUPER = 0x10,
	MURKA_KEY_COMMAND = MURKA_KEY_SUPER,

	MURKA_KEY_BACKSPACE = 8,
	MURKA_KEY_DEL = 127,

	MURKA_KEY_RETURN = 13,
	MURKA_KEY_ESC = 27,
	MURKA_KEY_TAB = 9,

	MURKA_KEY_F1 = 0xe000,
	MURKA_KEY_F2 = 0xe001,
	MURKA_KEY_F3 = 0xe002,
	MURKA_KEY_F4 = 0xe003,
	MURKA_KEY_F5 = 0xe004,
	MURKA_KEY_F6 = 0xe005,
	MURKA_KEY_F7 = 0xe006,
	MURKA_KEY_F8 = 0xe007,
	MURKA_KEY_F9 = 0xe008,
	MURKA_KEY_F10 = 0xe009,
	MURKA_KEY_F11 = 0xe00A,
	MURKA_KEY_F12 = 0xe00B,
	MURKA_KEY_LEFT = 0xe00C,
	MURKA_KEY_UP = 0xe00D,
	MURKA_KEY_RIGHT = 0xe00E,
	MURKA_KEY_DOWN = 0xe00F,
	MURKA_KEY_PAGE_UP = 0xe010,
	MURKA_KEY_PAGE_DOWN = 0xe011,
	MURKA_KEY_HOME = 0xe012,
	MURKA_KEY_END = 0xe013,
	MURKA_KEY_INSERT = 0xe014,
};

struct MurkaColor {
    float r = 1, g = 1, b = 1, a = 1;
   
	MurkaColor() { }

	MurkaColor(float gray) { r = gray; g = gray; b = gray; }
	MurkaColor(float R, float G, float B) { r = R; g = G; b = B; }
	MurkaColor(float R, float G, float B, float A) {r = R; g = G; b = B; a = A;}

    MurkaColor operator *(int multiplier) const {
        return MurkaColor(r * float(multiplier), g * float(multiplier), b * float(multiplier), a * float(multiplier));
    }

    MurkaColor operator *(float multiplier) const  {
        return MurkaColor(r * multiplier, g * multiplier, b * multiplier, a * multiplier);
    }
    
	MurkaColor operator *(double multiplier) const {
		return MurkaColor(r * multiplier, g * multiplier, b * multiplier, a * multiplier);
	}

	MurkaColor operator /(int devider) const {
		return MurkaColor(r / float(devider), g / float(devider), b / float(devider), a / float(devider));
	}

	MurkaColor operator /(float devider) const {
		return MurkaColor(r / devider, g / devider, b / devider, a / devider);
	}

	MurkaColor operator /(double devider) const {
		return MurkaColor(r / devider, g / devider, b / devider, a / devider);
	}


	MurkaColor operator +(MurkaColor rightColor) const  {
        return MurkaColor(r + rightColor.r, g + rightColor.g, b + rightColor.b, a + rightColor.a);
    }
    
    float limit() {
        return 255.;
    }

    
    void setHsb(float hue, float saturation, float brightness, float alpha) {
//        saturation = glm::clamp(saturation, 0.f, limit());
//        brightness = glm::clamp(brightness, 0.f, limit());
        if(brightness == 0) { // black
            r = 0;
            g = 0;
            b = 0;
        } else if(saturation == 0) { // grays
            r = brightness;
            g = brightness;
            b = brightness;
        } else {
            float hueSix = hue * 6.f / limit();
            float saturationNorm = saturation / limit();
            int hueSixCategory = (int) floorf(hueSix);
            float hueSixRemainder = hueSix - hueSixCategory;
            float pv = (float) ((1.f - saturationNorm) * brightness);
            float qv = (float) ((1.f - saturationNorm * hueSixRemainder) * brightness);
            float tv = (float) ((1.f - saturationNorm * (1.f - hueSixRemainder)) * brightness);
            switch(hueSixCategory) {
                case 0: case 6: // r
                    r = brightness / 255.;
                    g = tv / 255.;
                    b = pv / 255.;
                    break;
                case 1: // g
                    r = qv / 255.;
                    g = brightness / 255.;
                    b = pv / 255.;
                    break;
                case 2:
                    r = pv / 255.;
                    g = brightness / 255.;
                    b = tv / 255.;
                    break;
                case 3: // b
                    r = pv / 255.;
                    g = qv / 255.;
                    b = brightness / 255.;
                    break;
                case 4:
                    r = tv / 255.;
                    g = pv / 255.;
                    b = brightness / 255.;
                    break;
                case 5: // back to r
                    r = brightness / 255.;
                    g = pv / 255.;
                    b = qv / 255.;
                    break;
            }
        }
        
        // finally assign the alpha
        a = alpha;
    }

#ifdef MURKA_OF
    operator ofColor() const { return ofColor(r * 255.0, g * 255.0, b * 255.0, a * 255.0); }
#endif
};

struct MurkaPoint {
	float x = 0, y = 0;

	MurkaPoint operator += (MurkaPoint p2) {
		this->operator=({ x + p2.x, y + p2.y });
        return *this;
	}

    MurkaPoint operator -= (MurkaPoint p2) {
        this->operator=({ x - p2.x, y - p2.y });
        return *this;
    }

	MurkaPoint operator /= (float arg) {
		this->operator=({ x / arg, y / arg });
		return *this;
	}

	MurkaPoint operator *= (float arg) {
		this->operator=({ x * arg, y * arg });
		return *this;
	}

    MurkaPoint operator + (const MurkaPoint & p2) const {
        return {x + p2.x, y + p2.y};
    }
    
    MurkaPoint operator * (const MurkaPoint & p2) const {
        return {x * p2.x, y * p2.y};
    }

    MurkaPoint operator / (const MurkaPoint & p2) const {
        return {x / p2.x, y / p2.y};
    }

    //    MurkaPoint operator - (MurkaPoint p2) {
//        return {x - p2.x, y - p2.y};
//    }
//
    MurkaPoint operator - (const MurkaPoint & p2) const {
        return {x - p2.x, y - p2.y};
    }

    MurkaPoint operator * (float multiplier) const {
        return {x * multiplier, y * multiplier};
    }
    
    MurkaPoint operator / (float divider) const {
        return {x / divider, y / divider};
    }
    
    MurkaPoint operator-() const
    {
        return MurkaPoint(-(this->x), -(this->y) );
    }
    
    bool operator== (MurkaPoint & right) {
        return ((x == right.x) && (y == right.y));
    }

	
	bool operator== (const MurkaPoint right) const {
		return ((x == right.x) && (y == right.y));
	}
	
	friend std::ostream& operator<<(std::ostream& os, const MurkaPoint& p) {
		os << p.x << '/' << p.y;
		return os;
	}

	friend MurkaPoint operator * (const float& f, const MurkaPoint& p) {
		return p * f;
	}

    MurkaPoint (float x_, float y_) {
        x = x_;
        y = y_;
    }
    
    MurkaPoint () {
        x = 0;
        y = 0;
    }
    
    template<typename A, typename B>
    MurkaPoint(A x_, B y_) {
        x = x_;
        y = y_;
    }
    
    float length() const  {
        return sqrt(pow(x, 2) + pow(y, 2));
    }

	float lengthSquared() { 
		return x * x + y * y; 
	}

	float distance(const MurkaPoint& p) {
		return sqrt(pow(x - p.x, 2) + pow(y - p.y, 2));
	}

#ifdef MURKA_OF
    operator ofPoint() { return ofPoint(x, y); }
    operator ofVec2f() { return ofVec2f(x, y); }
#endif
};

struct MurkaPoint3D {
	float x = 0, y = 0, z = 0;

	MurkaPoint3D(float x_, float y_, float z_) {
		x = x_;
		y = y_;
		z = z_;
	}

	MurkaPoint3D(const MurkaPoint& p) {
		x = p.x;
		y = p.y;
		z = 0;
	}

	MurkaPoint3D() {
		x = 0;
		y = 0;
		z = 0;
	}

	MurkaPoint3D operator - (const MurkaPoint3D & p2) const {
		return { x - p2.x, y - p2.y, z - p2.z };
	}

	float length() const {
		return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
	}

	

};

struct MurkaPoint4D {
	float x = 0, y = 0, z = 0, w = 0;

	MurkaPoint4D(float x_, float y_, float z_, float w_) {
		x = x_;
		y = y_;
		z = z_;
		w = w_;
	}

	MurkaPoint4D() {
		x = 0;
		y = 0;
		z = 0;
		w = 0;
	}

	MurkaPoint4D operator * (const MurkaPoint4D & p2) const {
		return { x * p2.x, y * p2.y, z * p2.z, w * p2.w };
	}

	MurkaPoint4D operator * (float multiplier) const {
		return { x * multiplier, y * multiplier , z * multiplier , w * multiplier };
	}

	friend MurkaPoint4D operator * (const float& f, const MurkaPoint4D& p) {
		return p * f;
	}

	MurkaPoint4D operator - (const MurkaPoint4D & p2) const {
		return { x - p2.x, y - p2.y, z - p2.z, w - p2.w };
	}

	MurkaPoint4D operator + (const MurkaPoint4D & p2) const {
		return { x + p2.x, y + p2.y, z +  p2.z, w + p2.w };
	}

};

struct MurkaShape {
	MurkaPoint position = { 0, 0 };
	MurkaPoint size = { 0, 0 };

	bool inside(MurkaPoint p, double enlargeShape = 0) const {
		return ((p.x > position.x - enlargeShape) &&
			(p.x < position.x + size.x + enlargeShape * 2) &&
			(p.y > position.y - enlargeShape) &&
			(p.y < position.y + size.y + enlargeShape * 2));
	}
    
    inline bool intersectsWithRectangle(MurkaShape b) {
        if (b.inside({position.x, position.y}) ||
            b.inside({position.x + size.x, position.y}) ||
            b.inside({position.x, position.y + size.y}) ||
            b.inside({position.x + size.x, position.y + size.y})) {
            return true;
        }
        return false;
    }

    // inside function that ignores position
    bool transformedInside(MurkaPoint p) const {
        return ((p.x > position.x * 0.0) &&
                (p.x < position.x * 0.0 + size.x) &&
                (p.y > position.y * 0.0) &&
                (p.y < position.y * 0.0 + size.y));
    }

    bool operator != (MurkaShape b) {
        if ((position.x != b.position.x) ||
            (position.y != b.position.y) ||
            (size.x != b.size.x) ||
            (size.y != b.size.y)) {
            return true;
        } else return false;
    }

    bool operator == (const MurkaShape b) const {
        if ((position.x == b.position.x) &&
            (position.y == b.position.y) &&
            (size.x == b.size.x) &&
            (size.y == b.size.y)) {
            return true;
        } else return false;
    }

    
	MurkaShape() {

	}
    
    float& x () {return position.x; }
    float& y () {return position.y; }
    float& width () { return size.x; }
    float& height () { return size.y; }
    
    MurkaShape(MurkaPoint Position, MurkaPoint Size) {
        position = Position;
        size = Size;
    }

	MurkaShape(float x, float y, float width, float height) {
		position.x = x;
		position.y = y;
		size.x = width;
		size.y = height;
	}

    template<typename Number>
    MurkaShape(Number x, Number y, Number width, Number height) {
        position.x = x;
        position.y = y;
        size.x = width;
        size.y = height;
    }
    
    template<typename A, typename B, typename C, typename D>
    MurkaShape(A x, B y, C width, D height) {
        position.x = x;
        position.y = y;
        size.x = width;
        size.y = height;
    }
    
    MurkaShape operator * (float b) {
        return {position.x * b, position.y * b, size.x * b, size.y * b};
    }
    
    MurkaShape operator * (MurkaPoint b) {
        return {position.x * b.x, position.y * b.y, size.x * b.x, size.y * b.y};
    }
    
    MurkaShape operator / (float b) {
        return {position.x / b, position.y / b, size.x / b, size.y / b};
    }
    
    MurkaShape operator / (MurkaPoint b) {
        return {position.x / b.x, position.y / b.y, size.x / b.x, size.y / b.y};
    }
    
    MurkaShape operator - (MurkaShape b) {
        return {position.x - b.position.x, position.y - b.position.y, size.x - b.size.x, size.y - b.size.y};
    }
    
#ifdef MURKA_OF
    operator ofRectangle() {
        return {position.x, position.y, size.x, size.y};
    }
#endif
};

}
