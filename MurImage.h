#pragma once

#if defined(MURKA_OF)

#include "ofMain.h"
#include "MurkaTypes.h"

class MurImage {
public:
    MurImage(){
        
    };
    
    ~MurImage(){
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


#elif defined(MURKA_JUCE) 

#include "../JuceLibraryCode/JuceHeader.h"

#include "MurkaTypes.h"

class MurImage {

	GLuint textureID;
	int width, height;

	OpenGLContext* openGLContext = nullptr;
	bool bAllocated = false;

	vector<float> data;

public:
	MurImage() {

	};

	~MurImage() {
		clear();
	};

	void setOpenGLContext(OpenGLContext* openGLContext) {
		this->openGLContext = openGLContext;
	}

	void allocate(int w, int h) {
		width = w;
		height = h;

		data.resize(width * height * 4, 0.0);

		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);

#if JUCE_OPENGL3
		typedef void(*glGenerateMipmap_type)(GLuint array);
		glGenerateMipmap_type pglGenerateMipmap;
		pglGenerateMipmap = (glGenerateMipmap_type)OpenGLHelpers::getExtensionFunction("glGenerateMipmap");
		if (pglGenerateMipmap) {
			pglGenerateMipmap(GL_TEXTURE_2D);
		}
#else
#endif
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glBindTexture(GL_TEXTURE_2D, 0);

		bAllocated = true;
	}

	bool load(const std::string& fileName) {
		Image image = ImageFileFormat::loadFrom(File(fileName));
		
		clearTexture();
		allocate(image.getWidth(), image.getHeight());

		Image::BitmapData srcData(image, Image::BitmapData::readOnly);
		Image::PixelFormat fmt = srcData.pixelFormat;

		data.resize(width * height * 4);
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				Colour col = srcData.getPixelColour(x, y);
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
		glBindTexture(GL_TEXTURE_2D, textureID);
	}

	void unbind() const {
		glBindTexture(GL_TEXTURE_2D, 0);
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
		glBindTexture(GL_TEXTURE_2D, textureID);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, data.data());
		//glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_FLOAT, image.getPixelData());
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void clear() {
		clearTexture();
	}

	void clearTexture() {
		if (bAllocated) glDeleteTextures(1, &textureID);
	}

	float getWidth() const  {
		return width;
	}

	float getHeight() const {
		return height;
	}
};



#endif

