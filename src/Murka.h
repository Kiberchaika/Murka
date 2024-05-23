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


namespace murka {

class Murka : public View<Murka>, public MurkaInputEventsRegister, public MurkaRenderer, public MurkaOverlayHolder, public MurkaChildContextHolder {
private:
    MurkaShape getParentContextShape() {
        return getParentContext().currentViewShape;
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
    
    // Immediate mode custom layout

    template<typename T>
    T & prepareWidget(MurkaShape shape) {

        //        auto context = &(m.currentContext);
        int counter = currentContext.getImCounter();
        
        ViewBase* parentView = currentContext.linkedView;
        T* widgetObject = T::getOrCreateImModeWidgetObject_NEW(counter, parentView, shape);
        // TODO: fill the renderer and/or some other helper variables in the new widget object
        
//        parentView->deferredView = widgetObject;
        widgetObject->defferedViewDrawFunc = [&, widgetObject, parentView]() {
            
            auto parentViewCopy = parentView;
            auto widgetObjectCopy = widgetObject;
            pushContext(widgetObjectCopy);
            if (transformTheRenderIntoThisContextShape(currentContext.overlayHolder->disableViewportCrop)) {
                
                widgetObjectCopy->linearLayout.restart(widgetObjectCopy->shape);
                
                widgetObjectCopy->animationRestart();
                widgetObjectCopy->mosaicLayout.restart();
                widgetObjectCopy->updateEvents(currentContext);
                
                widgetObjectCopy->registerCurrentContext(currentContext);
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

        return *((T*)widgetObject);
    }
    
    MurkaContext getParentContext() { // MOVE TO PRIVATE
        if (contextStack.size() == 0) {
            return MurkaContext();
        }
        return contextStack[contextStack.size() - 1];
    }

    void pushContext(ViewBase* viewSource) {
        
        MurkaPoint containerPosition = viewSource->shape.position ;
        MurkaEventState derivedEventState = currentContext.transformedWith({-containerPosition.x, -containerPosition.y }, 1.0);

        contextStack.push_back(currentContext); // this makes it a parent
        
        currentContext.MurkaEventState::operator=(derivedEventState);
        currentContext.currentViewShape.position += containerPosition;
        currentContext.currentViewShape.size = viewSource->shape.size;
        currentContext.linkedView = viewSource;
        
        currentContext.overlayHolder = this;
        
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
    
    View* keyboardFocusHaver = nullptr;
    
    // A lambda functions that the plugins or hosts should fill in
    // to make the keyboard work in case they need to react to Murka
    // asking for keyboard focus (i.e. Juce wants the component to
    // ask for focus to send it keyboard events)
    
    std::function<void()> keyboardFocusGrabExternal = [](){};
    std::function<void()> keyboardFocusReleaseExternal = [](){};

    void setKeyboardFocusHaver(View* newOwner) {
        keyboardFocusHaver = newOwner;
    }
    
	bool allowedToUseKeyboard(View* asker) {
		return ((keyboardFocusHaver == asker) || (keyboardFocusHaver == nullptr));
	}

	bool hasKeyboardFocus(View* asker) {
		return (keyboardFocusHaver == asker);
	}

    void resetKeyboardFocus(View* asker) {
        if (asker == keyboardFocusHaver) {
            keyboardFocusHaver = nullptr;
        }
    }
    
    
    int getMaxHoverIndex() {
        return maxHoverIndex;
    }
    
    int iterateHoverIndex() {
        hoverIndex++;
        return hoverIndex;
    }
    
    
    // This function gets called in the beginning of the rendering. It clears everything and initialises context.
    // it also packs the user input into the context and clears the user input buffer data.
    // It should also free the memory used for function responses.
    void restartContext () {
        contextStack.resize(0);
        
        
        currentContext = MurkaContext();
        
        *((MurkaEventState*)&currentContext) = eventState; // copying eventState
        
        currentContext.linkedView = this;
        currentContext.linkedView = this;
        currentContext.overlayHolder = this;
        
        currentContext.currentViewShape = MurkaShape{0,
                                                  0,
                                                  float(getWindowWidth()),
                                                  float(getWindowHeight())};
//        currentContext.currentViewShape = currentContext.rootViewShape;

        currentContext.pushContextInternal = [&](ViewBase* v) {
            pushContext(v);
        };

        currentContext.popContextInternal = [&]() {
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
            setKeyboardFocusHaver((View*)asker);
            keyboardFocusGrabExternal();
            return;
        };
        
        currentContext.resetKeyboardFocus = [&](void* asker) {
            resetKeyboardFocus((View*)asker);
            keyboardFocusReleaseExternal();
            return;
        };
        
		currentContext.allowedToUseKeyboard = [&](void* asker)->bool {
			return allowedToUseKeyboard((View*)asker);
		};

		currentContext.hasKeyboardFocus = [&](void* asker)->bool {
			return hasKeyboardFocus((View*)asker);
		};

	}

    void beginDrawingInLatestView() {
        currentContext = latestChildContext;
    }
    
    /// TODO: evaluate if still needed (v1)
    ViewBase* getLatestView() {
        return (ViewBase*)latestChildContext.linkedView;
    }

    /// TODO: evaluate if still needed (v1)
    MurkaShape getLatestChildShape() {
        return ((ViewBase*)latestChildContext.linkedView)->shape;
    }

    std::vector<MurkaContext> contextStack; // MOVE TO PRIVATE
    
	MurkaContext currentContext; // MOVE TO PRIVATE

public:
	Murka() {
        setupEvents();
	}

    //////////////////////////////////////////////////////////////////

    bool doesHaveAWidgetThatHoldsKeyboardFocus() {
        return keyboardFocusHaver != nullptr;
    }

    MurkaShape getSize() {
        return currentContext.currentViewShape;
    }

	void setScreenScale(float newScreenScale) override {
		MurkaRenderer::setScreenScale(newScreenScale);
		MurkaInputEventsRegister::setInputEventsScale(newScreenScale);
	}
    
    float getScreenScale() override {
        return MurkaRenderer::getScreenScale();
    }



    void begin() { // this version is without arguments cause it creates the context
        startFrame();
        
        restartContext();
        clearEvents();
        
		currentContext.mousePosition /= getScreenScale();

        updateEvents(currentContext);
        
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
    T& prepare(MurkaShape place) {
        return prepareWidget<T>(place);
    }

    
    operator MurkaContext&() { return currentContext; }
    

    void setCurrentLayoutStructure(std::initializer_list<MurkaLinearLayoutGenerator::ShapePartDescriptor> list) {
       ((View*)currentContext.linkedView)->linearLayout.setLayoutStructure(list);
    }
    
    float getLayoutLineHeight() {
        return ((View*)currentContext.linkedView)->linearLayout.getLayoutLineHeight();
    }
    
    void setLayoutLinearOffset(float offset) {
        ((View*)currentContext.linkedView)->linearLayout.setLinearOffset(offset);
    }
    
    void setLayoutLineHeight(float height) {
        ((View*)currentContext.linkedView)->linearLayout.setLayoutLineHeight(height);
    }
    
    void addLayoutSpacing(float spacing) {
        ((View*)currentContext.linkedView)->linearLayout.addSpacing(spacing);
    }

    
    ///////////////////
    
    void setKeyboardFocusRequestCallbacks(std::function<void()> grabCallback, std::function<void()> releaseCallback) {
        keyboardFocusGrabExternal = grabCallback;
        keyboardFocusReleaseExternal = releaseCallback;
    }
    
    View* getRootView() {
		return (View*)this;
	}

    struct Parameters {};

private:
    
    int hoverIndex = 0;
    int maxHoverIndex = 0;

};

}
