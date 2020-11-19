#pragma once

#include "MurkaRendererBase.h"

#ifdef MURKA_OF // oF version of render

#include "ofMain.h"

class MurkaRenderer: public MurkaRendererBase {
	ofAppBaseWindow* ofWindow = nullptr;
    ofBaseGLRenderer* ofRenderer = nullptr;
    
	std::function<std::string(void)> getClipboardCallback = nullptr;
	std::function<void(std::string)> setClipboardCallback = nullptr;

public:
	void setWindow(ofAppBaseWindow* window) {
		ofWindow = window;
	}

	void setRenderer(ofBaseGLRenderer* renderer) {
		ofRenderer = renderer;
	}

	// Clipboard
	void setClipboardCallbacks(std::function<std::string(void)> getCallback, std::function<void(std::string)> setCallback) {
		getClipboardCallback = getCallback;
		setClipboardCallback = setCallback;
	}

	void setClipboardString(std::string clipboard) {
		if (setClipboardCallback) setClipboardCallback(clipboard);  
	} 

	std::string getClipboardString() {
		return getClipboardCallback ? getClipboardCallback() : "";
	}

	// This font loading function needs to be here because
    // downcasting from Renderer to MurkaAssets doesn't work for some reason.
    // TODO: figure a better way to make setFont() accessible like this.
    
    void setFont(std::string name, int size) {
        murka::MurkaAssets::setFont(name, size * getScreenScale(), this);
    }

	void drawString(const string & s, float x, float y) {
		murka::MurkaAssets::getCurrentFont()->drawString(s, x, y);
	} 

	// Object drawing
	virtual void draw(const MurImage & image, float x, float y, float z, float w, float h, float sx, float sy, float sw, float sh) override {
		ofRenderer->draw(image.internal, x * getScreenScale(), y * getScreenScale(), z * getScreenScale(), w * getScreenScale(), h * getScreenScale(), sx * getScreenScale(), sy * getScreenScale(), sw * getScreenScale(), sh * getScreenScale());
	}

	virtual void draw(const MurImage & image, float x, float y, float z, float w, float h) override {
		ofRenderer->draw(image.internal, x * getScreenScale(), y * getScreenScale(), z * getScreenScale(), w * getScreenScale(), h * getScreenScale(), 0, 0, image.internal.getWidth(), image.internal.getHeight());
	}

	virtual void draw(const MurImage & image, float x, float y) override {
		ofRenderer->draw(image.internal, x * getScreenScale(), y * getScreenScale(), 0, image.internal.getWidth() * getScreenScale(), image.internal.getHeight() * getScreenScale(), 0, 0, image.internal.getWidth() * getScreenScale(), image.internal.getHeight() * getScreenScale());
	}

	virtual void draw(const MurTexture & texture, float x, float y, float z, float w, float h, float sx, float sy, float sw, float sh) override {
        ofRenderer->draw(texture.internal, x * getScreenScale(), y * getScreenScale(), z * getScreenScale(), w * getScreenScale(), h * getScreenScale(), sx * getScreenScale(), sy * getScreenScale(), sw * getScreenScale(), sh * getScreenScale());
    }
    
    virtual void draw(const MurVbo & vbo, GLuint drawMode, int first, int total) override{
        ofRenderer->draw(vbo.internal, drawMode, first, total);
    }
    
    // Textures binding
	virtual void bind(const MurImage & img, int location = 0) override {
		ofRenderer->bind(img.internal.getTexture(), location);
	}

	virtual void unbind(const MurImage & img, int location = 0) override {
		ofRenderer->unbind(img.internal.getTexture(), location);
	}

	virtual void bind(const MurTexture & texture, int location = 0) override {
		ofRenderer->bind(texture.internal, location);
	}

	virtual void unbind(const MurTexture & texture, int location = 0) override {
		ofRenderer->unbind(texture.internal, location);
	}

	// transformations
	virtual void pushView() override {
		ofRenderer->pushView();
	}

	virtual void popView() override {
		ofRenderer->popView();
	}

	virtual void pushMatrix() override {
		ofRenderer->pushMatrix();
	}

	virtual void popMatrix() override {
		ofRenderer->popMatrix();
	}

	virtual void translate(float x, float y, float z)  override {
		ofRenderer->translate(x * getScreenScale(), y * getScreenScale(), z * getScreenScale());
	}
    
    virtual void rotateZRad(float radians) override {
        ofRenderer->rotateZRad(radians);
    }

	virtual void scale(float x, float y, float z) override {
		ofRenderer->scale(x * getScreenScale(), y * getScreenScale(), z * getScreenScale());
	}

    // setup matrices and viewport (upto you to push and pop view before and after)
    virtual void viewport(MurkaShape viewport) override {
        ofRenderer->viewport(viewport * getScreenScale());
    }
    
    virtual void viewport(float x = 0, float y = 0, float width = -1, float height = -1, bool vflip=true) override {
        ofRenderer->viewport(x * getScreenScale(), y * getScreenScale(), width * getScreenScale(), height * getScreenScale(), vflip);
    }

	virtual MurkaShape getCurrentViewport() override {
		ofRectangle rect = ofRenderer->getCurrentViewport();
		return MurkaShape(rect.x, rect.y, rect.width, rect.height) / getScreenScale();
	}

	virtual void setupScreen() override {
		ofRenderer->setupScreen();
	}

    // rendering setup
    virtual void setCircleResolution(int resolution) override{
        ofRenderer->setCircleResolution(resolution);
    }
    
    virtual void setLineWidth(float lineWidth) override{
        ofRenderer->setLineWidth(lineWidth);
    };
    
    virtual void enableFill() override{
        ofRenderer->setFillMode(OF_FILLED);
    };
    
    virtual void disableFill() override{
        ofRenderer->setFillMode(OF_OUTLINE);
    };
    
    virtual void setLineSmoothing(bool smooth) override{
        ofRenderer->setLineSmoothing(smooth);
    };
    
    virtual void enableAntiAliasing() override{
        ofRenderer->enableAntiAliasing();
    };
    
    virtual void disableAntiAliasing() override{
        ofRenderer->disableAntiAliasing();
    };
    
    virtual void enableAlphaBlending() override{
        ofRenderer->setBlendMode(OF_BLENDMODE_ALPHA);
    };
    
    virtual void disableAlphaBlending() override{
        ofRenderer->setBlendMode(OF_BLENDMODE_DISABLED);
    };
    
    virtual void pushStyle() override{
        ofRenderer->pushStyle();
    };
    
    virtual void popStyle() override{
        ofRenderer->popStyle();
    };
    
    // color operations
    virtual MurkaColor getColor() override {
        auto color = ofRenderer->getStyle().color;
        return {float(color.r / 255.),
                float(color.g / 255.),
                float(color.b / 255.),
                float(color.a / 255.)};
    }
    
    virtual void setColor(int r, int g, int b) override{
        ofRenderer->setColor(r, g, b);
    }; // 0-255
    
    virtual void setColor(int r, int g, int b, int a) override{
        ofRenderer->setColor(r, g, b, a);
    }; // 0-255
    
    virtual void setColor(const MurkaColor & color) override{
        ofRenderer->setColor(color);
    };
    
    virtual void setColor(const MurkaColor & color, int _a) override{
        ofRenderer->setColor(color, _a);
    };
    
    virtual void setColor(int gray) override{
        ofRenderer->setColor(gray);
    }; // 0 - 255
    
    virtual void setColor(int gray, int _a) override {
        ofRenderer->setColor(gray, _a);
    }; // 0-255


    virtual void clear() override{
        ofRenderer->clear();
    };
    
    virtual void clear(float r, float g, float b, float a=0) override{
        ofRenderer->clear(r, g, b, a);
    };
    
    virtual void clear(float brightness, float a=0) override{
        ofRenderer->clear(brightness, a);
    };

	virtual void drawRectangle(float x, float y, float w, float h) override {
		ofRenderer->drawRectangle(x * getScreenScale(), y * getScreenScale(), 0.0f, w * getScreenScale(), h * getScreenScale());
	}
    
    virtual void drawRectangle(MurkaShape s) override {
        ofRenderer->drawRectangle(s.position.x * getScreenScale(), s.position.y * getScreenScale(), 0., s.size.x * getScreenScale(), s.size.y * getScreenScale());
    }
    

	virtual void drawCircle(float x, float y, float radius) override {
		ofRenderer->drawCircle(x * getScreenScale(), y * getScreenScale(), 0.0f, radius * getScreenScale());
	}

	virtual void drawLine(float x1, float y1, float x2, float y2) override {
		ofRenderer->drawLine(x1 * getScreenScale(), y1 * getScreenScale(), 0.0f, x2 * getScreenScale(), y2 * getScreenScale(), 0.0f);
	}

	virtual void drawVbo(const MurVbo & vbo, GLuint drawMode, int first, int total) override {
        draw(vbo, drawMode, first, total);
    }

	virtual void drawPath(const vector<MurkaPoint> & verts)  {
		ofPolyline polyline;
		polyline.addVertices(*(const vector<glm::vec3>*)&verts);
		ofRenderer->draw(polyline);
	}

	virtual int getWindowWidth() override {
		return ofWindow->getWidth() / getScreenScale();
	}

	virtual int getWindowHeight() override {
		return ofWindow->getHeight() / getScreenScale();
	}

};

#else // "Default" render version

class MurkaRender: public MurkaRenderBase {
    
};

#endif
