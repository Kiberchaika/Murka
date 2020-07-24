#pragma once

#include "MurkaRendererBase.h"

#ifdef MURKA_OF // oF version of render

#include "ofMain.h"

class MurkaRenderer: public MurkaRendererBase {
	ofAppBaseWindow* ofWindow;
    ofBaseGLRenderer* ofRenderer;
    
public:
	void setWindow(ofAppBaseWindow* window) {
		ofWindow = window;
	}

	void setRenderer(ofBaseGLRenderer* renderer) {
		ofRenderer = renderer;
	}

	// Object drawing
    virtual void draw(const MurImage & image, float x, float y, float z, float w, float h, float sx, float sy, float sw, float sh) const override {
        ofRenderer->draw(image.internal, x, y, z, w, h, sx, sy, sw, sh);
    }
    
    virtual void draw(const MurTexture & texture, float x, float y, float z, float w, float h, float sx, float sy, float sw, float sh) const override {
        ofRenderer->draw(texture.internal, x, y, z, w, h, sx, sy, sw, sh);
    }
    
    virtual void draw(const MurVbo & vbo, GLuint drawMode, int first, int total) const override{
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
		ofRenderer->translate(x, y, z);
	}

	virtual void scale(float x, float y, float z) override {
		ofRenderer->scale(x, y, z);
	}

    // setup matrices and viewport (upto you to push and pop view before and after)
    virtual void viewport(MurkaShape viewport) override{
        ofRenderer->viewport(viewport);
    }
    
    virtual void viewport(float x = 0, float y = 0, float width = -1, float height = -1, bool vflip=true) override{
        ofRenderer->viewport(x, y, width, height, vflip);
    }

	virtual void setupScreen() override {
		ofRenderer->setupScreen();
	}

    // rendering setup
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
    
    virtual void pushStyle() override{
        ofRenderer->pushStyle();
    };
    
    virtual void popStyle() override{
        ofRenderer->popStyle();
    };
    
    // color options
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
    
    virtual void setColor(float gray) override{
        ofRenderer->setColor(gray * 255);
    }; // new set a color as grayscale with one argument

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
		ofRenderer->drawRectangle(x, y, 0.0f, w, h);
	}
    
    virtual void drawRectangle(MurkaShape s) override {
        ofRenderer->drawRectangle(s.position.x, s.position.y, 0., s.size.x, s.size.y);
    }
    

	virtual void drawCircle(float x, float y, float radius) override {
		ofRenderer->drawCircle(x, y, 0.0f, radius);
	}

	virtual void drawLine(float x1, float y1, float x2, float y2) override {
		ofRenderer->drawLine(x1, y1, 0.0f, x2, y2, 0.0f);
	}

	virtual void drawVbo(MurVbo vbo, int drawMode, int first, int total) {
		ofRenderer->draw(vbo.internal, drawMode, first, total);
	}

	virtual int getWindowWidth() {
		return ofWindow->getWidth();
	}

	virtual int getWindowHeight() {
		return ofWindow->getHeight();
	}

};

#else // "Default" render version

class MurkaRender: public MurkaRenderBase {
    
};

#endif
