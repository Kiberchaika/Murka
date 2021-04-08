#pragma once

#ifndef GL_TEXTURE_RECTANGLE
#define GL_TEXTURE_RECTANGLE 0x84F5
#endif

#include "MurkaTypes.h"

#if defined(MURKA_OF)

#include "ofMain.h"

namespace murka {

class MurImage {
public:
	MurImage() {

	};

	~MurImage() {
		internal.clear();
	};

	ofImage internal;

	void allocate(int w, int h) {
		internal.allocate(w, h, OF_IMAGE_COLOR_ALPHA);
	}

	bool load(const std::filesystem::path& fileName, const ofImageLoadSettings &settings = ofImageLoadSettings()) {
		return internal.load(fileName, settings);
	}

	void setColor(int x, int y, const MurkaColor color) {
		internal.setColor(x, y, color);
	}

	bool isUsingTexture() const {
		return internal.isUsingTexture();
	}

	bool isAllocated() const {
		return internal.isAllocated();
	}

	void update() {
		internal.update();
	}

	void clear() {
		internal.clear();
	}

	void clearTexture() {
		internal.getTexture().clear();
	}

	float getWidth() {
		return internal.getWidth();
	}

	float getHeight() {
		return internal.getHeight();
	}
};

}

#elif defined(MURKA_JUCE) 

namespace murka {

class MurImage {

	GLuint textureID;
	int width, height;

	juce::OpenGLContext* openGLContext = nullptr;
	bool bAllocated = false;

	vector<float> data;
	bool arb = false;
	int gltype = 0;

public:
	MurImage() {

	};

	~MurImage() {
		clear();
	};

	void setOpenGLContext(juce::OpenGLContext* openGLContext) {
		this->openGLContext = openGLContext;
	}

	void allocate(int w, int h) {
		gltype = !arb ? GL_TEXTURE_2D : GL_TEXTURE_RECTANGLE;

		width = w;
		height = h;

		data.resize(width * height * 4, 0.0);

		glGenTextures(1, &textureID);
		glBindTexture(gltype, textureID);

#if JUCE_OPENGL3
		typedef void(*glGenerateMipmap_type)(GLuint array);
		glGenerateMipmap_type pglGenerateMipmap;
		pglGenerateMipmap = (glGenerateMipmap_type)juce::OpenGLHelpers::getExtensionFunction("glGenerateMipmap");
		if (pglGenerateMipmap) {
			pglGenerateMipmap(gltype);
		}
#else
#endif
		glTexParameteri(gltype, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(gltype, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(gltype, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(gltype, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glBindTexture(gltype, 0);

		bAllocated = true;
	}

	bool load(const std::string& fileName) {
        if(!juce::File(fileName).exists()) return false;
        
		juce::Image image = juce::ImageFileFormat::loadFrom(juce::File(fileName));

		clearTexture();
		allocate(image.getWidth(), image.getHeight());

		juce::Image::BitmapData srcData(image, juce::Image::BitmapData::readOnly);
		juce::Image::PixelFormat fmt = srcData.pixelFormat;

		data.resize(width * height * 4);
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				juce::Colour col = srcData.getPixelColour(x, y);
				int idx = y * width * 4 + x * 4;

				data[idx + 0] = col.getFloatRed();
				data[idx + 1] = col.getFloatGreen();
				data[idx + 2] = col.getFloatBlue();
				data[idx + 3] = col.getFloatAlpha();
			}
		}

		update();

		return true;
	}

	void bind() const {
		glBindTexture(gltype, textureID);
	}

	void unbind() const {
		glBindTexture(gltype, 0);
	}

	void setColor(int x, int y, const MurkaColor col) {
		int idx = y * width * 4 + x * 4;
		data[idx + 0] = col.r;
		data[idx + 1] = col.g;
		data[idx + 2] = col.b;
		data[idx + 3] = col.a;
	}

	bool isUsingTexture() const {
		return true;
	}

	bool isAllocated() const {
		return bAllocated;
	}

	void update() {
		glBindTexture(gltype, textureID);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(gltype, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, data.data());
		//glTexSubImage2D(gltype, 0, 0, 0, width, height, GL_RGBA, GL_FLOAT, image.getPixelData());
		glBindTexture(gltype, 0);
	}

	void clear() {
		clearTexture();
	}

	void clearTexture() {
        if (bAllocated) {
            glDeleteTextures(1, &textureID);
            bAllocated = false;
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
