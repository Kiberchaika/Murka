#pragma once

#include "MurkaTypes.h"
#include "MurkaInputEventsRegister.h"
#include "MurkaViewHandler.h"
#include "MurkaTypes.h"
#include "MurkaOverlayHolder.h"
#include "MurkaRenderer.h"
#include "MurkaAnimator.h"
#include "ViewBase.h"
#include "MurkaContextBase.h"

#ifdef MURKA_OF
#include "ofMain.h"
#endif

namespace murka {

class MurkaContext: public MurkaContextBase {
    /*
private:
    MurkaContext(const MurkaContext&) {}
    MurkaContext& operator=(const MurkaContext&){}
     */
    
public:
    
    MurkaContext() {
    }
    
    std::pair<MurkaShape, MurkaPoint> getCroppedViewport(MurkaShape parent, MurkaShape view) const {
        MurkaPoint pos = {(std::max)(parent.position.x + view.position.x, parent.position.x),
						  (std::max)(parent.position.y + view.position.y, parent.position.y)};
        MurkaPoint negativePosition = {view.position.x < 0 ? view.position.x : 0,
                                       view.position.y < 0 ? view.position.y : 0};
        MurkaPoint size = {(std::min)(view.size.x + negativePosition.x, parent.size.x - view.position.x),
						   (std::min)(view.size.y + negativePosition.y, parent.size.y - view.position.y)};
        
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
        
		pointerToRenderer->pushView();
		pointerToRenderer->viewport(viewport.position.x, viewport.position.y, viewport.size.x, viewport.size.y);
		pointerToRenderer->setupScreen();
		pointerToRenderer->pushMatrix();
		pointerToRenderer->translate(offset.x, offset.y, 0); // this is needed to crop the
        // views that are only partly visible
        
        return true;
    }
    
    void transformTheRenderBackFromThisContextShape() const {
		pointerToRenderer->popMatrix();
		pointerToRenderer->popView();
    }
    
    // Utility function to substitute the matrix for the viewport to occlude
    // the drawing outside the widget
    void startViewport() const {
        transformTheRenderBackFromThisContextShape();

        pointerToRenderer->pushView();
        auto vport = pointerToRenderer->getCurrentViewport(); 
        pointerToRenderer->viewport(MurkaShape(currentViewShape.position.x,
                               currentViewShape.position.y,
                               currentViewShape.size.x,
                               currentViewShape.size.y));
		pointerToRenderer->scale(vport.size.x / currentViewShape.size.x,
			vport.size.y / currentViewShape.size.y, 
			1);
    }
    
    void endViewport() const {
        pointerToRenderer->popView();

        transformTheRenderIntoThisContextShape();
    }
    
    
    // All shapes are absolute
    MurkaShape* currentWidgetShapeSource; // this shape pointer points to a shape of the current
    // view that you could use inside the widget to reshape it if needed // TODO: get rid of this (v2)
    void* linkedView = nullptr; // the MurkaView that this context once represented
    
    ViewBase_NEW* linkedView_NEW = nullptr; // the MurkaView that this context last represented
    
    ViewBase_NEW* deferredView = nullptr;
    std::function<void(MurkaContextBase &)> defferedViewDrawFunc;
    
    void commitDeferredView() {
        if (deferredView == nullptr) return;
        
            pushContext_NEW(deferredView);
            if (transformTheRenderIntoThisContextShape(overlayHolder->disableViewportCrop)) {
                deferredView->linearLayout.restart(deferredView->shape);
                
//                deferredView->draw(*this);
                
                defferedViewDrawFunc(*this);
                
                deferredView->animationRestart();
                deferredView->mosaicLayout.restart();
                
                /*
                //DEBUG - drawing the children frame that we had at the last frame end
                ofSetColor(255, 100, 0);
                    ofNoFill();

                ofDrawRectangle(((View*)c.murkaView)->childrenBounds.position.x, ((View*)c.murkaView)->childrenBounds.position.y, ((View*)c.murkaView)->childrenBounds.size.x, ((View*)c.murkaView)->childrenBounds.size.y);
                    ofFill();
                //////
                 */

                
                transformTheRenderBackFromThisContextShape();
            }
            popContext();
                
            deferredView->resetChildrenBounds();
        
        deferredView = nullptr;
    }

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

    void pushContext_NEW(ViewBase_NEW* viewSource) {
        pushContextInternal_NEW(viewSource);
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
    std::function<void(ViewBase_NEW*)> pushContextInternal_NEW = [](ViewBase_NEW* v) {};
//    std::function<void()> popContextInternal_NEW = []() {};

    double getRunningTime() const {return pointerToRenderer->getElapsedTime();}

private:
    int imCounter = 0; // the counter that we use to distinguish new widgets from the ones we
    // want to reuse in IM mode.
     
};

}
