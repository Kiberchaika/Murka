#pragma once

#include "MurVbo.h"
#include "MurTexture.h"
#include "MurImage.h"
#include "MurkaTypes.h"
#include "MurkaAssets.h"

namespace murka {

enum MurBlendMode {
	MUR_BLENDMODE_DISABLED = 0,
	MUR_BLENDMODE_ALPHA = 1,
	MUR_BLENDMODE_ADD = 2,
};

class MurkaRendererBase : public MurkaAssets {
	float screenScale = 1.0;

public:
	virtual void setScreenScale(float newScreenScale) {
		screenScale = newScreenScale;
	}

	virtual float getScreenScale() {
		return screenScale;
	}

    // Object drawing
    virtual void draw(const MurImage & image, float x, float y, float z, float w, float h, float sx, float sy, float sw, float sh) = 0;
	virtual void draw(const MurImage & image, float x, float y, float z, float w, float h) = 0;
	virtual void draw(const MurImage & image, float x, float y) = 0;

    virtual void draw(const MurTexture & texture, float x, float y, float z, float w, float h, float sx, float sy, float sw, float sh) = 0;
    virtual void draw(const MurVbo & vbo, GLuint drawMode, int first, int total) = 0;
    
    // Textures binding
	virtual void bind(const MurImage & img, int location = 0) = 0;
	virtual void unbind(const MurImage & img, int location = 0) = 0;
	virtual void bind(const MurTexture & texture, int location = 0) = 0;
	virtual void unbind(const MurTexture & texture, int location = 0) = 0;

    // transformations
    virtual void pushView() = 0;
    virtual void popView() = 0;

	virtual void pushMatrix() = 0;
	virtual void popMatrix() = 0;

	virtual void translate(float x, float y, float z) = 0;
	virtual void scale(float x, float y, float z) = 0;
    virtual void rotateZRad(float radians) = 0;

    // setup matrices and viewport (upto you to push and pop view before and after)
    virtual void viewport(MurkaShape viewport) = 0;
    virtual void viewport(float x = 0, float y = 0, float width = -1, float height = -1, bool vflip=true) = 0;
	virtual MurkaShape getCurrentViewport() = 0;
	virtual void setupScreen() = 0;

    // rendering setup
    virtual void setCircleResolution(int resolution) = 0;
    virtual void setLineWidth(float lineWidth) = 0;
    virtual void enableFill() = 0;
    virtual void disableFill() = 0;
    virtual void setLineSmoothing(bool smooth) = 0;
    virtual void enableAntiAliasing() = 0;
    virtual void disableAntiAliasing() = 0;
    virtual void enableAlphaBlending() = 0;
    virtual void disableAlphaBlending() = 0;
	virtual void setBlendMode(MurBlendMode blendMode) = 0;

    virtual void pushStyle() = 0;
    virtual void popStyle() = 0;
    
    // color operations
    virtual MurkaColor getColor() = 0;
    virtual void setColor(int r, int g, int b) = 0; // 0-255
    virtual void setColor(int r, int g, int b, int a) = 0; // 0-255
    virtual void setColor(const MurkaColor & color) = 0;
    virtual void setColor(const MurkaColor & color, int _a) = 0;
    virtual void setColor(int gray) = 0; // 0-255
    virtual void setColor(int gray, int _a) = 0; // 0-255

    virtual void clear() = 0;
	virtual void clear(int r, int g, int b, int a = 0) = 0;
	virtual void clear(int gray, int a = 0) = 0;

    virtual void drawRectangle(MurkaShape s) = 0;
	virtual void drawRectangle(float x, float y, float w, float h) = 0;
	virtual void drawCircle(float x, float y, float radius) = 0;
	virtual void drawLine(float x1, float y1, float x2, float y2) = 0;
	virtual void drawVbo(const MurVbo & vbo, GLuint drawMode, int first, int total) = 0;
	virtual void drawPath(const vector<MurkaPoint3D> & verts) = 0;

	virtual int getWindowWidth() = 0;
	virtual int getWindowHeight() = 0;

	virtual bool getUsingArbTex() = 0;
	virtual void disableArbTex() = 0;
	virtual void enableArbTex() = 0;

	virtual uint64_t getFrameNum() = 0;
	virtual float getElapsedTime() = 0;
};

}