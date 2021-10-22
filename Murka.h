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
        
        for (auto& i : children) {
            pushContext(i);
                drawCycleRecursive(i);
            popContext();
        }
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

	// A recursive OOP draw cycle that starts with this widget
	void drawCycleRecursive(MurkaViewHandlerInternal* widget) {
        
        currentContext.transformTheRenderIntoThisContextShape();
        
        ((MurkaViewHandler<View>*)widget)->widgetObject->draw(widget->dataToControl, widget->parametersInternal, widget->widgetObjectInternal, currentContext, widget->resultsInternal);
        
        // restarting layout generator
    ((MurkaViewHandler<View>*)widget)->widgetObject->linearLayout.restart(((MurkaViewHandler<View>*)widget)->widgetObject->shape);
        ((MurkaViewHandler<View>*)widget)->widgetObject->animationRestart();
        ((MurkaViewHandler<View>*)widget)->widgetObject->mosaicLayout.restart();
        
        currentContext.transformTheRenderBackFromThisContextShape();

        for (auto& i : ((View*)widget->widgetObjectInternal)->children) {
            pushContext(i);
                drawCycleRecursive(i);
            popContext();
        }
        
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

		currentContext.pushContextInternal = [&](MurkaViewHandlerInternal* mvhi) {
            pushContext(mvhi);
        };

        currentContext.pushContextInternal_NEW = [&](ViewBase_NEW* v) {
            pushContext_NEW(v);
        };

//        currentContext.popContextInternal_NEW = [&]() {
//            popContext();
//        };

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
    
    void clearChildren() {
        for (auto &i: children) {
            ((View*)i->widgetObjectInternal)->clearChildren();
            delete i;
        }
        children.clear();
    }
    
    template <typename Z, class B>
    MurkaViewHandler<Z>* addChildToView(View* parent, MurkaViewInterface<Z>* child, B data, void* parameters, MurkaShape shapeInParentContainer)
    {
        
        MurkaViewHandler<Z>* newHandler = new MurkaViewHandler<Z>();
        
        newHandler->tParams = newHandler->castParameters(new typename Z::Parameters());
        if ((parameters != nullptr) && (newHandler->tParams != NULL)) {
            newHandler->tParams = newHandler->castParameters(parameters);
            newHandler->parametersInternal = newHandler->tParams;
        }

        newHandler->resultsInternal = child->returnNewResultsObject();
        newHandler->manuallyControlled = true;
        newHandler->widgetObject = (Z*)child->returnNewWidgetObject();
        newHandler->widgetObjectInternal = newHandler->widgetObject;
        newHandler->widgetObject->parametersInternal = (typename Z::Parameters*)newHandler->parametersInternal;
        newHandler->widgetObject->resultsInternal = (typename Z::Results*)newHandler->resultsInternal;
        newHandler->dataToControl = data;
        
        newHandler->widgetObject->dataTypeName = typeid(data).name();

        shapeInParentContainer = shapeInParentContainer;
        
        newHandler->widgetObject->shape = shapeInParentContainer;
        
        
        auto handlerPointer = newHandler;
        parent->children.push_back((MurkaViewHandlerInternal*)handlerPointer);
        
        // Updating children bounds
        if (parent->childrenBounds.position.x > shapeInParentContainer.position.x) {
            parent->childrenBounds.position.x = shapeInParentContainer.position.x;
        }
        if (parent->childrenBounds.position.y > shapeInParentContainer.position.y) {
            parent->childrenBounds.position.y = shapeInParentContainer.position.y;
        }
        if ((parent->childrenBounds.size.x + parent->childrenBounds.position.x) <= (shapeInParentContainer.position.x + shapeInParentContainer.size.x)) {
            parent->childrenBounds.size.x = shapeInParentContainer.position.x + shapeInParentContainer.size.x - parent->childrenBounds.position.x;
        }
        if ((parent->childrenBounds.size.y + parent->childrenBounds.position.y) <= (shapeInParentContainer.position.y + shapeInParentContainer.size.y)) {
            parent->childrenBounds.size.y = shapeInParentContainer.position.y + shapeInParentContainer.size.y - parent->childrenBounds.position.y;
        }

        return (MurkaViewHandler<Z>*)parent->children[parent->children.size() - 1];
    }
    
    // A version that accepts a handler rather than the widget object as parent
    template <typename Z, class B>
    MurkaViewHandler<Z>* addChildToView(MurkaViewHandlerInternal* parent, MurkaViewInterface<Z>* child, B data, typename Z::Parameters parameters, MurkaShape shapeInParentContainer)
    {
        auto newParameters = new typename Z::Parameters();
        *newParameters = parameters;

        return addChildToView((View*)parent->widgetObjectInternal, child, data, newParameters,                           shapeInParentContainer);
    }

    // A version that accepts parameters reference rather a pointer
    template <typename Z, class B>
    MurkaViewHandler<Z>* addChildToView(MurkaViewInterface<Z>* child, B data, typename Z::Parameters parameters, MurkaShape shapeInParentContainer)
    {
        auto newParameters = new typename Z::Parameters();
        *newParameters = parameters;
        
        return addChildToView(getRootView(), child, data, newParameters,                           shapeInParentContainer);
    }

    void draw(MurkaContextBase & c)  {
        
    }
    
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
