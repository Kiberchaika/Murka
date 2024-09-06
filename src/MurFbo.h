#pragma once

#include "MurkaTypes.h"

#if defined(MURKA_OF)

#include "ofMain.h"

namespace murka {

class MurFbo {
public:
	MurFbo() {

	};

	~MurFbo() {
		fbo.clear();
	};

	ofFbo fbo;

	void allocate(int w, int h) {
		fbo.allocate(w, h, OF_IMAGE_COLOR_ALPHA);
	}

	void begin() const {
		fbo.begin();
	}

	void end() const {
		fbo.end();
	}

	void bind() const {
		fbo.bind();
	}

	void unbind() const {
		fbo.unbind();
	}

	int getWidth() {
		return fbo.getWidth();
	}

	int getHeight() {
		return fbo.getHeight();
	}
};

}

#elif defined(MURKA_JUCE) 

namespace murka {

class MurFbo {
	friend class MurkaRenderer;

	juce::OpenGLFrameBuffer openGLFrameBuffer;
	GLuint textureID;

	int width = 0, height = 0;

	juce::OpenGLContext* openGLContext = nullptr;
	bool bAllocated = false;

protected:
	void begin() {
		openGLFrameBuffer.makeCurrentRenderingTarget();
	}

	void end() {
		openGLFrameBuffer.releaseAsRenderingTarget();
	}

	void bind() const {
		glBindTexture(GL_TEXTURE_2D, textureID);
	}

	void unbind() const {
		glBindTexture(GL_TEXTURE_2D, 0);
	}

public:

	MurFbo() {
	}

	~MurFbo() {
		if (bAllocated) {
			openGLFrameBuffer.release();
		}
	}

	bool isAllocated() const {
		return bAllocated;
	}

	void setOpenGLContext(juce::OpenGLContext* openGLContext) {
		this->openGLContext = openGLContext;
	}

	void allocate(int w, int h) {
		bAllocated = openGLFrameBuffer.initialise(*openGLContext, w, h);
		if (bAllocated) {
			textureID = openGLFrameBuffer.getTextureID();

			width = w;
			height = h;
		}
	}

	float getWidth() const {
		return width;
	}

	float getHeight() const {
		return height;
	}
};

}

#endif
