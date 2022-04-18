#pragma once

// Murka is a mixed mode data driven user interface library.

#include "MurkaTypes.h"
#include "MurkaView.h"
#include "MurkaContext.h"
#include "MurkaInputEventsRegister.h"
#include "MurkaAssets.h"
#include "MurkaLinearLayoutGenerator.h"
#include "MurkaRenderer.h"
#include "MurkaChildContextHolder.h"
/*
 
 // Custom widget template:
 
 #pragma once
 #include "Murka.h"

 namespace murka {

 class Label : public View_NEW<Label> {
 public:
     
     void internalDraw(const MurkaContextBase & c)  {
         
         bool inside = c.isHovered() * !areChildrenHovered(c);
     }
 
     MURKA_CALLBACK(Label, // class name
                    onHoverChangeCallback, // callback variable name
                    onHoverChange) // callback setter
     
     MURKA_CALLBACK(Label,
                    onClickCallback,
                    onClick)
     
     MURKA_CALLBACK_1ARGUMENT(Label, // class name
                              onClickCallbackPosition, // callback variable name
                              onClickPosition, // callback setter
                              MurkaPoint) // callback argument type
     
     MURKA_PARAMETER(Label, // class name
                     TextAlignment, // parameter type
                     alignment, // parameter variable name
                     withAlignment, // setter
                     TEXT_LEFT) // default
     
     MURKA_PARAMETER(Label, // class name
                     std::string, // parameter type
                     label, // parameter variable name
                     text, // setter
                     "") // default
}
 
 */

namespace murka {

class Murka : public View_NEW<Murka>, public MurkaInputEventsRegister, public MurkaRenderer, public MurkaOverlayHolder, public MurkaChildContextHolder {
public:
	Murka() {
        setupEvents();
	}
    
    //////////////////////////////////////////////////////////////////
    // NEW API 1.0
    
    
    std::function<void(Murka &)>* castDeferredViewDrawFunction(void* function) {
        // We need this to use the draw functions for widgets that are anonymously stored inside
        // other widgets.
        
        return (std::function<void(Murka &)>*)function;
    }
    
    MurkaShape getParentContextShape() {
        return getParentContext().currentViewShape;
    }
    
    MurkaShape getSize() {
        return currentContext.currentViewShape;
    }
    
    // Utility function to transform the render into the shape of this context.
    // Helpful to draw the view innards.
    // Returns false if the view is not visible
    bool transformTheRenderIntoThisContextShape(bool noCrop = false)  {
        MurkaShape relativeShape = getSize();
        relativeShape.position.x -= getParentContextShape().position.x;
        relativeShape.position.y -= getParentContextShape().position.y;
        
        auto croppedViewport = currentContext.getCroppedViewport(getParentContextShape(), relativeShape);
        auto viewportShape = croppedViewport.first;
        auto offset = croppedViewport.second;
        
        if (noCrop) {
            viewportShape = relativeShape;
            viewportShape.position.x += getParentContextShape().position.x;
            viewportShape.position.y += getParentContextShape().position.y;
        }
        
        if (viewportShape.size.y <= 0) {
            return false;
        }
        
        pushView();
        viewport(viewportShape.position.x, viewportShape.position.y, viewportShape.size.x, viewportShape.size.y);
        setupScreen();
        pushMatrix();
        translate(offset.x, offset.y, 0); // this is needed to crop the
        // views that are only partly visible
        
        return true;
    }
    
    void transformTheRenderBackFromThisContextShape() {
        popMatrix();
        popView();
    }
    
    // Utility function to substitute the matrix for the viewport to occlude
    // the drawing outside the widget
    void startViewport() {
        transformTheRenderBackFromThisContextShape();

        pushView();
        auto vport = getCurrentViewport();
        viewport(MurkaShape(currentContext.currentViewShape.position.x,
                            currentContext.currentViewShape.position.y,
                            currentContext.currentViewShape.size.x,
                            currentContext.currentViewShape.size.y));
        scale(vport.size.x / currentContext.currentViewShape.size.x,
            vport.size.y / currentContext.currentViewShape.size.y,
            1);
    }
    
    void endViewport()  {
        popView();

        transformTheRenderIntoThisContextShape();
    }
    
//    void commitDeferredView() {
        /*
            assert(deferredView != NULL);
            assert(deferredView != nullptr);

            auto parentViewDrawFunction = currentContext.linkedView_NEW->defferedViewDrawFunc;
            auto parentView = currentContext.linkedView_NEW;
        
            pushContext_NEW(parentView);
            if (transformTheRenderIntoThisContextShape(currentContext.overlayHolder->disableViewportCrop)) {
                
                deferredView->linearLayout.restart(deferredView->shape);
                
//                deferredView->draw(*this);
                
                (*castDeferredViewDrawFunction(parentViewDrawFunction))(*this);
                
                assert(deferredView != NULL);
                assert(deferredView != nullptr);
                
                deferredView->animationRestart();
                deferredView->mosaicLayout.restart();
                
//                //DEBUG - drawing the children frame that we had at the last frame end
//                setColor(255, 100, 0);
//                    disableFill();
//
//                drawRectangle(((View*)currentContext.murkaView)->childrenBounds.position.x, ((View*)currentContext.murkaView)->childrenBounds.position.y, ((View*)currentContext.murkaView)->childrenBounds.size.x, ((View*)currentContext.murkaView)->childrenBounds.size.y);
//                    enableFill();
//                //////

                
                transformTheRenderBackFromThisContextShape();
            }
            popContext();
                
            deferredView->resetChildrenBounds();
        
        cout << "nullified deferredview";
        deferredView = nullptr;
         */
//    }
    
    // Immediate mode custom layout

    template<typename T>
    T & drawWidget_NEW(MurkaShape shape) {

        //        auto context = &(m.currentContext);
        int counter = currentContext.getImCounter();

        
        ViewBase_NEW* parentView = currentContext.linkedView_NEW;
        T* widgetObject = T::getOrCreateImModeWidgetObject_NEW(counter, parentView, shape);
        // TODO: fill the renderer and/or some other helper variables in the new widget object
        
//        parentView->deferredView = widgetObject;
        widgetObject->defferedViewDrawFunc = [&, widgetObject, parentView]() {
            
            auto parentViewCopy = parentView;
            auto widgetObjectCopy = widgetObject;
            pushContext_NEW(widgetObjectCopy);
            if (transformTheRenderIntoThisContextShape(currentContext.overlayHolder->disableViewportCrop)) {
                
                widgetObjectCopy->linearLayout.restart(widgetObjectCopy->shape);
                
                widgetObjectCopy->animationRestart();
                widgetObjectCopy->mosaicLayout.restart();
                
                widgetObjectCopy->internalDraw(*this);

                
//                //DEBUG - drawing the children frame that we had at the last frame end
//                setColor(255, 100, 0);
//                    disableFill();
//
//                drawRectangle(((View*)currentContext.murkaView)->childrenBounds.position.x, ((View*)currentContext.murkaView)->childrenBounds.position.y, ((View*)currentContext.murkaView)->childrenBounds.size.x, ((View*)currentContext.murkaView)->childrenBounds.size.y);
//                    enableFill();
//                //////

                
                transformTheRenderBackFromThisContextShape();
            }
            popContext();
                
            widgetObjectCopy->resetChildrenBounds();

            
        };
//        *(std::function<void(Murka &)>*)(parentView->defferedViewDrawFunc) = std::bind(&T::internalDraw, (T*)widgetObject, std::placeholders::_1);
    //    c.commitDeferredView();

        return *((T*)widgetObject);
    }
    

	void setScreenScale(float newScreenScale) override {
		MurkaRenderer::setScreenScale(newScreenScale);
		MurkaInputEventsRegister::setInputEventsScale(newScreenScale);
	}

	MurkaContext currentContext;
    std::vector<MurkaContext> contextStack;
    
    MurkaContext getParentContext() {
        if (contextStack.size() == 0) {
            return MurkaContext();
        }
        return contextStack[contextStack.size() - 1];
    }
    /*
    void pushContext(MurkaViewHandlerInternal* viewSource) {
        
        ((View*)currentContext.linkedView)->latestContext = currentContext;
        
        MurkaPoint containerPosition = ((View*)viewSource->widgetObjectInternal)->shape.position ;
        MurkaEventState derivedEventState = currentContext.transformedWith({-containerPosition.x, -containerPosition.y }, 1.0);

        contextStack.push_back(currentContext); // this makes it a parent
        
        currentContext.MurkaEventState::operator=(derivedEventState);
        currentContext.currentViewShape.position += containerPosition;
        currentContext.currentViewShape.size = ((View*)viewSource->widgetObjectInternal)->shape.size;
        currentContext.linkedView = ((View*)viewSource->widgetObjectInternal);
        currentContext.overlayHolder = this;
        
        ((View*)viewSource->widgetObjectInternal)->latestContext = this->currentContext;
        
        latestChildContext = currentContext;
    }
     */
    
    void pushContext_NEW(ViewBase_NEW* viewSource) {
        
//        ((ViewBase_NEW*)currentContext.linkedView_NEW)->latestContext = currentContext;
        
        MurkaPoint containerPosition = viewSource->shape.position ;
        MurkaEventState derivedEventState = currentContext.transformedWith({-containerPosition.x, -containerPosition.y }, 1.0);

        contextStack.push_back(currentContext); // this makes it a parent
        
        currentContext.MurkaEventState::operator=(derivedEventState);
        currentContext.currentViewShape.position += containerPosition;
        currentContext.currentViewShape.size = viewSource->shape.size;
        currentContext.linkedView_NEW = viewSource;
//        currentContext.deferredView = nullptr;
        
        currentContext.overlayHolder = this;
        
//        viewSource->latestContext = currentContext;
        
        latestChildContext = currentContext;
         
    }
    
    void popContext() {
        currentContext = contextStack.back();
        contextStack.pop_back();
        
    }
    
    bool transformRenderIntoCurrentContext() {
        return transformTheRenderIntoThisContextShape();
    }

    void transformRenderBackFromLastContext() {
        transformTheRenderBackFromThisContextShape();
    }
    
    View_NEW* keyboardFocusHaver = nullptr;
    
    void setKeyboardFocusHaver(View_NEW* newOwner) {
        keyboardFocusHaver = newOwner;
    }
    
    bool allowedToUseKeyboard(View_NEW* asker) {
        return ((keyboardFocusHaver == asker) || (keyboardFocusHaver == nullptr));
    }
    
    void resetKeyboardFocus(View_NEW* asker) {
        if (asker == keyboardFocusHaver) {
            keyboardFocusHaver = nullptr;
        }
    }
    
    bool doesHaveAWidgetThatHoldsKeyboardFocus() {
        return keyboardFocusHaver != nullptr;
    }
    
    int getMaxHoverIndex() {
        return maxHoverIndex;
    }
    
    int iterateHoverIndex() {
        hoverIndex++;
        return hoverIndex;
    }

    void begin() { // this version is without arguments cause it creates the context
        
        restartContext();
        
		currentContext.mousePosition /= getScreenScale();

        hoverIndex = 0;
        
    }
    
    void end() {
        disableViewportCrop = true;
        for (auto &overlay: overlays) {
            overlay.func();
        }

        disableViewportCrop = false;
        overlays.clear();
        
        maxHoverIndex = hoverIndex;

	}
    
    template<typename T>
    T& draw(MurkaShape place) {
        return drawWidget_NEW<T>(place);
    }



	// This function gets called in the beginning of the step. It clears everything and initialises context.
	// it also packs the user input into the context and clears the user input buffer data.
	// It should also free the memory used for function responses.
	void restartContext () {
//        if (currentContext.parentContext != NULL) {
//            delete currentContext.parentContext;
//        }
        
        contextStack.resize(0);
        
        
		currentContext = MurkaContext();
        
        *((MurkaEventState*)&currentContext) = eventState; // copying eventState
//        MurkaRenderer* pointerToRenderer = (MurkaRenderer*)&currentContext;
//        pointerToRenderer = this; // Switching this MurkaContext's Renderer base class for a pointer to this renderer

        
        currentContext.pointerToRenderer = this;
        currentContext.linkedView = this;
        currentContext.linkedView_NEW = this;
        currentContext.overlayHolder = this;
        
        currentContext.currentViewShape = MurkaShape{0,
                                                  0,
                                                  float(getWindowWidth()),
                                                  float(getWindowHeight())};
//        currentContext.currentViewShape = currentContext.rootViewShape;

        currentContext.pushContextInternal_NEW = [&](ViewBase_NEW* v) {
            pushContext_NEW(v);
        };

        currentContext.popContextInternal_NEW = [&]() {
            popContext();
        };

        currentContext.getParentContextShapeInternal = [&]()->MurkaShape {
            return getParentContext().currentViewShape;
        };
        
        currentContext.iterateHoverIndex = [&]()->int {
            return iterateHoverIndex();
        };
        
        currentContext.getMaxHoverIndex = [&]()->int {
            return getMaxHoverIndex();
        };
        
        currentContext.claimKeyboardFocus = [&](void* asker) {
            setKeyboardFocusHaver((View_NEW*)asker);
            return;
        };
        
        currentContext.resetKeyboardFocus = [&](void* asker) {
            resetKeyboardFocus((View_NEW*)asker);
            return;
        };
        
        currentContext.checkKeyboardFocus = [&](void* asker)->bool {
            return allowedToUseKeyboard((View_NEW*)asker);
        };
        
//        latestContext = currentContext;

        clearEvents();
	}

    ////////
    //////// Immediate mode
    ////////
    
    operator MurkaContext&() { return currentContext; }
    
    MurkaContext getContextFromMurkaView(View_NEW* view) {
//        view->latestContext.resetImCounter();
        return view->latestContext;
    }

    void beginDrawingInView(View_NEW* view) {
        currentContext = getContextFromMurkaView(view);
    }
    
    void beginDrawingInLatestView() {
        currentContext = latestChildContext;
    }
    
    View_NEW* getLatestView() {
        return (View_NEW*)latestChildContext.linkedView;
    }

    ViewBase_NEW* getLatestView_NEW() {
        return (ViewBase_NEW*)latestChildContext.linkedView_NEW;
    }

    MurkaShape getLatestChildShape() {
        return ((View_NEW*)latestChildContext.linkedView)->shape;
    }

    MurkaShape getLatestChildShape_NEW() {
        return ((ViewBase_NEW*)latestChildContext.linkedView)->shape;
    }

    void setCurrentLayoutStructure(std::initializer_list<MurkaLinearLayoutGenerator::ShapePartDescriptor> list) {
       ((View_NEW*)currentContext.linkedView)->linearLayout.setLayoutStructure(list);
    }
    
    float getLayoutLineHeight() {
        return ((View_NEW*)currentContext.linkedView)->linearLayout.getLayoutLineHeight();
    }
    
    void setLayoutLinearOffset(float offset) {
        ((View_NEW*)currentContext.linkedView)->linearLayout.setLinearOffset(offset);
    }
    
    void setLayoutLineHeight(float height) {
        ((View_NEW*)currentContext.linkedView)->linearLayout.setLayoutLineHeight(height);
    }
    
    void addLayoutSpacing(float spacing) {
        ((View_NEW*)currentContext.linkedView)->linearLayout.addSpacing(spacing);
    }

    ////////
    //////// Object oriented view heirarchy management
    ////////
    

    
    ///////////////////
    
    View_NEW* getRootView() {
		return (View_NEW*)this;
	}

    struct Parameters {};

    typedef bool Results;

private:
    
    int hoverIndex = 0;
    int maxHoverIndex = 0;

};

}
