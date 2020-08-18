#pragma once

#ifdef MURKA_OF

#include "ofMain.h"
#include "MurkaTypes.h"

class MurImage {
public:
    MurImage(){};
    
    ofImage internal;
	
	void allocate(int w, int h) {
		internal.allocate(w, h, OF_IMAGE_COLOR_ALPHA);
	}

	bool load(const std::filesystem::path& fileName, const ofImageLoadSettings &settings = ofImageLoadSettings()) {
		internal.load(fileName, settings);
	}

	void setColor(int x, int y, const MurkaColor color) {
		internal.setColor(x, y, color);
	}

	bool isUsingTexture() const {
		internal.isUsingTexture();
	}

	bool isAllocated() const {
		internal.isAllocated();
	}

	void update() {
		internal.update();
	}

};

#else // Default version

class MurImage {
public:
    MurImage(){};
};

#endif
