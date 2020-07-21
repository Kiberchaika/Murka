#pragma once

#ifdef MURKA_OF
#include "ofMain.h"
#endif

// Here's the global typedefs for cross-render functionality

#ifdef MURKA_OF
#include "ofMain.h"
#include "ofxFontStash.h"
typedef ofxFontStash FontObject; // It's important for this object to exist in a render for Murka to work
#endif

#include <iostream>
#include <cmath>

struct MurkaColor {
    double r = 0, g = 0, b = 0, a = 1;
    
    MurkaColor(float R, float G, float B) {r = R; g = G; b = B; }
    MurkaColor(float R, float G, float B, float A) {r = R; g = G; b = B; a = A;}

    MurkaColor operator *(int multiplier) {
        return MurkaColor(r * float(multiplier), g * float(multiplier), b * float(multiplier), a * float(multiplier));
    }

    MurkaColor operator *(float multiplier) {
        return MurkaColor(r * multiplier, g * multiplier, b * multiplier, a * multiplier);
    }
    
    MurkaColor operator *(double multiplier) {
        return MurkaColor(r * multiplier, g * multiplier, b * multiplier, a * multiplier);
    }
    
    MurkaColor operator +(MurkaColor rightColor) {
        return MurkaColor(r + rightColor.r, g + rightColor.g, b + rightColor.b, a + rightColor.a);
    }

#ifdef MURKA_OF
    operator ofColor() { return ofColor(r, g, b, a); }
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

    MurkaPoint operator + (const MurkaPoint p2) const {
        return {x + p2.x, y + p2.y};
    }
    
//    MurkaPoint operator - (MurkaPoint p2) {
//        return {x - p2.x, y - p2.y};
//    }
//
    MurkaPoint operator - (const MurkaPoint p2) const {
        return {x - p2.x, y - p2.y};
    }

    MurkaPoint operator * (float multiplier) {
        return {x * multiplier, y * multiplier};
    }
    
    MurkaPoint operator / (float divider) {
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
	
    friend std::ostream& operator<<(std::ostream& os, const MurkaPoint& dt);


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
    
#ifdef MURKA_OF
    operator ofPoint() { return ofPoint(x, y); }
    operator ofVec2f() { return ofVec2f(x, y); }
#endif
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
