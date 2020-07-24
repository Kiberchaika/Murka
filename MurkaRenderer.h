#pragma once

#include "MurkaRendererBase.h"

#include "MurkaTypes.h"
#include "MurVbo.h"
#include "MurTexture.h"
#include "MurImage.h"

#ifdef MURKA_OF // oF version of render

#include "ofMain.h"

class MurkaRenderer: public MurkaRendererBase {

    ofBaseRenderer* ofRenderer;
    
public:
    void setRenderer(ofBaseRenderer* renderer) {
        ofRenderer = renderer;
    }
    
    // Object drawing
    virtual void draw(const MurImage & image, float x, float y, float z, float w, float h, float sx, float sy, float sw, float sh) const override {
        
    }
    
    virtual void draw(const MurTexture & image, float x, float y, float z, float w, float h, float sx, float sy, float sw, float sh) const override {
        
    }
    
    virtual void draw(const MurVbo & vbo, GLuint drawMode, int first, int total) const override{
        
    }
    
    // Textures binding
    virtual void bind(const ofTexture & texture, int location) override {
           
       };
    virtual void unbind(const ofTexture & texture, int location) override{
           
       };
    
    // transformations
    virtual void pushView() override{
           
       };
    virtual void popView() override{
           
       };

    // setup matrices and viewport (upto you to push and pop view before and after)
    virtual void viewport(MurkaShape viewport) override{
           
       };
    virtual void viewport(float x = 0, float y = 0, float width = -1, float height = -1, bool vflip=true) override{
           
       };

    // rendering setup
    virtual void setLineWidth(float lineWidth) override{
           
       };
    virtual void enableFill() override{
           
       };
    virtual void disableFill() override{
           
       };
    virtual void setLineSmoothing(bool smooth) override{
           
       };
    virtual void enableAntiAliasing() override{
           
       };
    virtual void disableAntiAliasing() override{
           
       };
    virtual void pushStyle() override{
           
       };
    virtual void popStyle() override{
           
       };
    
    // color options
    virtual void setColor(int r, int g, int b) override{
           
       }; // 0-255
    virtual void setColor(int r, int g, int b, int a) override{
           
       }; // 0-255
    virtual void setColor(const MurkaColor & color) override{
           
       };
    virtual void setColor(const MurkaColor & color, int _a) override{
           
       };
    virtual void setColor(int gray) override{
           
       }; // new set a color as grayscale with one argument

    virtual void clear() override{
           
       };
    virtual void clear(float r, float g, float b, float a=0) override{
           
       };
    virtual void clear(float brightness, float a=0) override{
           
       };
};

#else // "Default" render version

class MurkaRender: public MurkaRenderBase {
    
};

#endif
