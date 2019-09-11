#pragma once

#include <functional>
#include "MurkaTypes.h"
#include "MurkaInputEventsRegister.h"
#include "MurkaViewHandler.h"
#include "MurkaTypes.h"
#include "MurkaAssets.h"

// Here's the global typedefs for cross-render functionality

#ifdef MURKA_OF
#include "ofMain.h"
#endif

namespace murka {

class MurkaContext: public MurkaEventState {
public:
    MurkaContext() {
#ifdef MURKA_OF
        runningTime = ofGetElapsedTimef();
#endif
    }
    
    // Assets access functions
    
    MurkaColor getWidgetForegroundColor() const {
        return ((MurkaAssets*)assetsObject)->widgetFgColor;
    }

    MurkaColor getWidgetBackgroundColor() const {
        return ((MurkaAssets*)assetsObject)->widgetBgColor;
    }
    
    FontObject* getHeaderFont() const {
        return ((MurkaAssets*)assetsObject)->headerFont;
    }

    FontObject* getParagraphFont() const {
        return ((MurkaAssets*)assetsObject)->paragraphFont;
    }

    FontObject* getMonospaceFont() const {
        return ((MurkaAssets*)assetsObject)->monoFont;
    }
    
    FontObject* getFont(std::string fontId) const {
        return ((MurkaAssets*)assetsObject)->fontMap[fontId];
    }

    //
    
    MurkaPoint getSize() const {
        return currentViewShape.size;
    }
    
    bool isHovered() const {
        return currentViewShape.transformedInside(mousePosition); // using absolute coordinates to calc that
    }
    
    MurkaAssets* assetsObject;
    MurkaContext* parentContext = NULL;
    
    MurkaShape currentViewShape;
    
    
    // Utility function to transform the render into the shape of this context.
    // Helpful to draw the view innards.
    void transformTheRenderIntoThisContextShape() const {
#ifdef MURKA_OF
//        ofLog() << "pushed matrix";
        ofPushMatrix();
        ofTranslate(currentViewShape.position.x, currentViewShape.position.y);
#endif 
    }
    
    void transformTheRenderBackFromThisContextShape() const {
#ifdef MURKA_OF
//        ofLog() << "popped matrix";
        ofPopMatrix();
#endif
    }
    
    // Utility function to substitute the matrix for the viewport to occlude
    // the drawing outside the widget
    void startViewport() const {
        transformTheRenderBackFromThisContextShape();

        ofPushView();
        auto vport = ofGetCurrentViewport();
        ofViewport(ofRectangle(currentViewShape.position.x,
                               currentViewShape.position.y,
                               currentViewShape.size.x,
                               currentViewShape.size.y));
        ofScale(vport.getWidth() / currentViewShape.size.x,
                vport.getHeight() / currentViewShape.size.y);
    }
    
    void endViewport() const {
        ofPopView();

        transformTheRenderIntoThisContextShape();
    }
    
    
    // All shapes are absolute
    MurkaShape* currentWidgetShapeSource; // this shape pointer points to a shape of the current
    // view that you could use inside the widget to use or reshape it if needed
    void* murkaView; // the MurkaView that this context once represented

    int getImCounter() {
        imCounter++;
        return imCounter;
    }
    
    void resetImCounter() {
        imCounter = 0;
    }
    
    void pushContext(MurkaViewHandlerInternal* viewSource) {
        pushContextInternal(viewSource);
    }

    void popContext() {
        popContextInternal();
    }
    
    std::function<void(MurkaViewHandlerInternal*)> pushContextInternal = [](MurkaViewHandlerInternal* mvhi) {};
    std::function<void()> popContextInternal = []() {};

    double getRunningTime() const {return runningTime;}
    
private:
    int imCounter = 0; // the counter that we use to distinguish new widgets from the ones we
    // want to reuse in IM mode.

    double runningTime;

};

}
