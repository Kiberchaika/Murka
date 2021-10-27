#pragma once

// Murka is a mixed mode data driven user interface library.

#include "MurkaTypes.h"
#include "MurkaView.h"
#include "MurkaContext.h"
#include "MurkaInputEventsRegister.h"
#include "MurkaAssets.h"
#include "MurkaLinearLayoutGenerator.h"
#include "MurkaRenderer.h"

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
                     getAlignment, // getter
                     withAlignment, // setter
                     TEXT_LEFT) // default
     
     MURKA_PARAMETER(Label, // class name
                     std::string, // parameter type
                     label, // parameter variable name
                     getLabel, // getter
                     text, // setter
                     "") // default
}
 
 */

namespace murka {

class Murka : public View_NEW<Murka>, public MurkaInputEventsRegister, public MurkaRenderer, public MurkaOverlayHolder {
public:
	Murka() {
        setupEvents();
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
        return currentContext.transformTheRenderIntoThisContextShape();
    }

    void transformRenderBackFromLastContext() {
        currentContext.transformTheRenderBackFromThisContextShape();
    }
    
    View* keyboardFocusHaver = nullptr;
    
    void setKeyboardFocusHaver(View* newOwner) {
        keyboardFocusHaver = newOwner;
    }
    
    bool allowedToUseKeyboard(View* asker) {
        return ((keyboardFocusHaver == asker) || (keyboardFocusHaver == nullptr));
    }
    
    void resetKeyboardFocus(View* asker) {
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
        return drawWidget_NEW<T>(*this, place);
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
        
        currentContext.commitDeferredView_casted = [](const MurkaContextBase* cc) { ((MurkaContext*)cc)->commitDeferredView();};

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
            setKeyboardFocusHaver((View*)asker);
            return;
        };
        
        currentContext.resetKeyboardFocus = [&](void* asker) {
            resetKeyboardFocus((View*)asker);
            return;
        };
        
        currentContext.checkKeyboardFocus = [&](void* asker)->bool {
            return allowedToUseKeyboard((View*)asker);
        };
        
//        latestContext = currentContext;

        clearEvents();
	}

    ////////
    //////// Immediate mode
    ////////
    
    operator MurkaContext&() { return currentContext; }
    
    MurkaContext latestChildContext;

    MurkaContext getContextFromMurkaView(View* view) {
//        view->latestContext.resetImCounter();
        return view->latestContext;
    }

    void beginDrawingInView(View* view) {
        currentContext = getContextFromMurkaView(view);
    }
    
    void beginDrawingInLatestView() {
        currentContext = latestChildContext;
    }
    
    View* getLatestView() {
        return (View*)latestChildContext.linkedView;
    }

    ViewBase_NEW* getLatestView_NEW() {
        return (ViewBase_NEW*)latestChildContext.linkedView_NEW;
    }

    MurkaShape getLatestChildShape() {
        return ((View*)latestChildContext.linkedView)->shape;
    }

    MurkaShape getLatestChildShape_NEW() {
        return ((ViewBase_NEW*)latestChildContext.linkedView)->shape;
    }

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

    ////////
    //////// Object oriented view heirarchy management
    ////////
    

    
    ///////////////////
    
	View* getRootView() {
		return (View*)this;
	}

    struct Parameters {};

    typedef bool Results;

private:
    
    int hoverIndex = 0;
    int maxHoverIndex = 0;

};

}
