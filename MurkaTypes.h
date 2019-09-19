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


struct MurkaColor {
    double r = 0, g = 0, b = 0;
    
    MurkaColor(float R, float G, float B) {r = R; g = G; b = B; }
    
    MurkaColor operator *(int multiplier) {
        return MurkaColor(r * float(multiplier), g * float(multiplier), b * float(multiplier));
    }

    MurkaColor operator *(float multiplier) {
        return MurkaColor(r * multiplier, g * multiplier, b * multiplier);
    }

#ifdef MURKA_OF
    operator ofColor() { return ofColor(r, g, b); }
#endif
};

struct MurkaPoint {
	float x, y;

	MurkaPoint operator += (MurkaPoint p2) {
		this->operator=({ x + p2.x, y + p2.y });
        return *this;
	}
    
    MurkaPoint operator + (MurkaPoint p2) {
        return {x + p2.x, y + p2.y};
    }
};

struct MurkaShape {
	MurkaPoint position = { 0, 0 };
	MurkaPoint size = { 0, 0 };

	bool inside(MurkaPoint p) const {
		return ((p.x > position.x) &&
			(p.x < position.x + size.x) &&
			(p.y > position.y) &&
			(p.y < position.y + size.y));
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
    
	MurkaShape() {

	}

	MurkaShape(float x, float y, float width, float height) {
		position.x = x;
		position.y = y;
		size.x = width;
		size.y = height;
	}
};
