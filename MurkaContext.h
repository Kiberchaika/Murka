#pragma once

#include <functional>
#include "MurkaTypes.h"
#include "MurkaInputEventsRegister.h"
#include "MurkaViewHandler.h"
#include "MurkaTypes.h"
#include "MurkaOverlayHolder.h"
#include "MurkaRenderer.h"
#include "MurkaAnimator.h"

// Here's the global typedefs for cross-render functionality

#ifdef MURKA_OF
#include "ofMain.h"
#endif

namespace murka {

class MurkaContext: public MurkaEventState {
    /*
private:
    MurkaContext(const MurkaContext&) {}
    MurkaContext& operator=(const MurkaContext&){}
     */
    
public:
    
    MurkaContext() {
#ifdef MURKA_OF
        runningTime = ofGetElapsedTimef();
#endif
    }
    
    // Assets access functions
    
    FontObject* getCurrentFont() const {
        return ((MurkaAssets*)renderer)->getCurrentFont();
    }

    //
    
    MurkaPoint getPosition() const {
        return {currentViewShape.position.x + getParentContextShape().position.x,
                currentViewShape.position.y + getParentContextShape().position.y};
    }
    
    MurkaPoint getSize() const {
        return currentViewShape.size;
    }
    
    bool isHovered() const {
        return currentViewShape.transformedInside(mousePosition /* / renderer->getScreenScale() */); // using absolute coordinates to calc that
    }
    
    MurkaOverlayHolder* overlayHolder;
    MurkaRenderer* renderer;
    
    MurkaShape getParentContextShape() const {
        return getParentContextShapeInternal();
    }
    
    MurkaShape currentViewShape;
    
    // to add an overlay, give a lambda with the overlays call and an object that asked for it
    void addOverlay(std::function<void()> func, void* object) {
        overlayHolder->addOverlay(func, object);
    }
    
    std::pair<MurkaShape, MurkaPoint> getCroppedViewport(MurkaShape parent, MurkaShape view) const {
        MurkaPoint pos = {std::max(parent.position.x + view.position.x, parent.position.x),
                          std::max(parent.position.y + view.position.y, parent.position.y)};
        MurkaPoint negativePosition = {view.position.x < 0 ? view.position.x : 0,
                                       view.position.y < 0 ? view.position.y : 0};
        MurkaPoint size = {std::min(view.size.x + negativePosition.x, parent.size.x - view.position.x),
                           std::min(view.size.y + negativePosition.y, parent.size.y - view.position.y)};
        
        if ((view.position.x > parent.size.x) ||
            (view.position.y > parent.size.y) ||
            (view.position.x < 0 - view.size.x) ||
            (view.position.y < 0 - view.size.y)) {
            return std::make_pair(MurkaShape(0,0,0,0), MurkaPoint(0, 0));
        }

        return std::make_pair(MurkaShape(pos.x, pos.y, size.x, size.y), negativePosition);
    }
    
    // Utility function to transform the render into the shape of this context.
    // Helpful to draw the view innards.
    // Returns false if the view is not visible
    bool transformTheRenderIntoThisContextShape(bool noCrop = false) const {
        MurkaShape relativeShape = currentViewShape;
        relativeShape.position.x -= getParentContextShape().position.x;
        relativeShape.position.y -= getParentContextShape().position.y;
        
        auto croppedViewport = getCroppedViewport(getParentContextShape(), relativeShape);
        auto viewport = croppedViewport.first;
        auto offset = croppedViewport.second;
        
        if (noCrop) {
            viewport = relativeShape;
            viewport.position.x += getParentContextShape().position.x;
            viewport.position.y += getParentContextShape().position.y;
        }
        
        if (viewport.size.y <= 0) {
            return false;
        }
#ifdef MURKA_OF
        
		renderer->pushView();
		renderer->viewport(viewport.position.x, viewport.position.y, viewport.size.x, viewport.size.y);
		renderer->setupScreen();
		renderer->pushMatrix();
		renderer->translate(offset.x, offset.y, 0); // this is needed to crop the
        // views that are only partly visible
#endif
        
        return true;
    }
    
    void transformTheRenderBackFromThisContextShape() const {
#ifdef MURKA_OF
        
		renderer->popMatrix();
		renderer->popView();
#endif
    }
    
    // Utility function to substitute the matrix for the viewport to occlude
    // the drawing outside the widget
    void startViewport() const {
        transformTheRenderBackFromThisContextShape();

        renderer->pushView();
        auto vport = renderer->getCurrentViewport(); 
        renderer->viewport(MurkaShape(currentViewShape.position.x,
                               currentViewShape.position.y,
                               currentViewShape.size.x,
                               currentViewShape.size.y));
		renderer->scale(vport.size.x / currentViewShape.size.x,
			vport.size.y / currentViewShape.size.y, 
			1);
    }
    
    void endViewport() const {
        renderer->popView();

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
    std::function<void(void*)> claimKeyboardFocus = [](void*) {return; };
    std::function<void(void*)> resetKeyboardFocus = [](void*) {return; };
    std::function<bool(void*)> checkKeyboardFocus = [](void*)->bool {return true; }; // aka "widget is allowed to use keyboard"
    std::function<int()> getMaxHoverIndex = []()->int {return 0;};
    std::function<int()> iterateHoverIndex = []()->int {return 0;};
    std::function<void(MurkaViewHandlerInternal*)> pushContextInternal = [](MurkaViewHandlerInternal* mvhi) {};
    std::function<void()> popContextInternal = []() {};

    // This uses Animator class because you can't include View from here
    std::function<void(MurkaAnimator*)> pushContextInternal_NEW = [](MurkaAnimator* v) {};
    std::function<void()> popContextInternal_NEW = []() {};
    

    std::function<MurkaShape()> getParentContextShapeInternal = []()->MurkaShape {
        return MurkaShape();
    };

    double getRunningTime() const {return runningTime;}
    
private:
    int imCounter = 0; // the counter that we use to distinguish new widgets from the ones we
    // want to reuse in IM mode.

    double runningTime;
};

}
