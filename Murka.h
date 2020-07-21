#pragma once

#define MURKA_OF

// Murka is a mixed mode data driven user interface library.


#include <functional>
#include "MurkaTypes.h"
#include "MurkaContext.h"
#include "MurkaView.h"
#include "MurkaInputEventsRegister.h"
#include "MurkaAssets.h"
#include "MurkaLinearLayoutGenerator.h"


/*
 
 // Custom widget template:
 
 #include "MurkaTypes.h"
 #include "MurkaContext.h"
 #include "MurkaView.h"
 #include "MurkaInputEventsRegister.h"
 #include "MurkaAssets.h"
 #include "MurkaLinearLayoutGenerator.h"

 class Widget : public MurkaViewInterface<Widget> {
 public:
     MURKA_VIEW_DRAW_FUNCTION  {
                    auto params = (Parameters*)parametersObject;
                    bool inside = context.isHovered() * !areChildrenHovered(context) * hasMouseFocus(context);
 
                    // Your drawing and interaction code goes here.
                    // Don't forget that all of this executes at each frame for each
                    // widget that is drawn on screen.
                };
 
     // Here go parameters and any parameter convenience constructors. You need to define something called Parameters, even if it's NULL.
     struct Parameters {
        float somevalue = 0.0;
 
        Parameters() {}
        Parameters(float someval) { somevalue = someval; } // a convenience initializer
     };
 
    // The results type, you also need to define it even if it's nothing.
    typedef bool Results;
 
     // The size that this widget wants if the layout generator asks for it.
 
 
     // The two functions needed for optional UI state saving. It's up to you
     // to use those.
     // std::vector<MurkaVar> saveInternalData(int indexX, int indexY) override { }
     // void loadInternalData(std::vector<MurkaVar>) override {}
 
 
     // Everything else in the class is for handling the internal state. It persist.
 };
 
 */

namespace murka {

class Murka : public MurkaViewInterface<Murka>, public MurkaInputEventsRegister, public MurkaAssets, public MurkaOverlayHolder {
public:
	Murka() {
        setupEvents();
	}



	MurkaContext currentContext;
    std::vector<MurkaContext> contextStack;
    
    MurkaContext getParentContext() {
        if (contextStack.size() == 0) {
            return MurkaContext();
            throw ;
        }
        return contextStack[contextStack.size() - 1];
    }
    
    void pushContext(MurkaViewHandlerInternal* viewSource) {
        
        ((View*)currentContext.murkaView)->latestContext = currentContext;
        
        MurkaPoint containerPosition = ((View*)viewSource->widgetObjectInternal)->shape.position;
        MurkaEventState derivedEventState = currentContext.transformedWith({-containerPosition.x,
            -containerPosition.y}, 1.0);

        contextStack.push_back(currentContext); // this makes it a parent
        
        currentContext.MurkaEventState::operator=(derivedEventState);
        currentContext.currentViewShape.position += containerPosition;
        currentContext.currentViewShape.size = ((View*)viewSource->widgetObjectInternal)->shape.size;
        currentContext.murkaView = ((View*)viewSource->widgetObjectInternal);
        currentContext.overlayHolder = this;
        currentContext.setUIScale(getUIScale());
        
        ((View*)viewSource->widgetObjectInternal)->latestContext = currentContext;
        
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
    
    View* keyboardFocusHaver = NULL;
    
    void setKeyboardFocusHaver(View* newOwner) {
        keyboardFocusHaver = newOwner;
    }
    
    bool allowedToUseKeyboard(View* asker) {
        return ((keyboardFocusHaver == asker) || (keyboardFocusHaver == NULL));
    }
    
    void resetKeyboardFocus(View* asker) {
        if (asker == keyboardFocusHaver) {
            keyboardFocusHaver = NULL;
        }
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

	// A recursive OOP draw cycle that starts with this widget
	void drawCycleRecursive(MurkaViewHandlerInternal* widget) {
        
        currentContext.transformTheRenderIntoThisContextShape();
        
        ((MurkaViewHandler<View>*)widget)->widgetObject->draw(widget->dataToControl, widget->parametersInternal, widget->widgetObjectInternal, currentContext, widget->resultsInternal);
        
        // restarting layout generator
    ((MurkaViewHandler<View>*)widget)->widgetObject->linearLayout.restart(((MurkaViewHandler<View>*)widget)->widgetObject->shape, getUIScale());
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
        *((MurkaEventState*)&currentContext) = eventState;
        currentContext.assetsObject = this;
        currentContext.murkaView = this;
        currentContext.overlayHolder = this;
        
#ifdef MURKA_OF
        currentContext.currentViewShape = MurkaShape{0,
                                                  0,
                                                  float(ofGetWidth()),
                                                  float(ofGetHeight())};
//        currentContext.currentViewShape = currentContext.rootViewShape;
#endif
        currentContext.pushContextInternal = [&](MurkaViewHandlerInternal* mvhi) {
            pushContext(mvhi);
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
            return;
        };
        
        currentContext.resetKeyboardFocus = [&](void* asker) {
            resetKeyboardFocus((View*)asker);
            return;
        };
        
        currentContext.checkKeyboardFocus = [&](void* asker)->bool {
            return allowedToUseKeyboard((View*)asker);
        };
        
        currentContext.setUIScale(getUIScale());

        latestContext = currentContext;

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
        return (View*)latestChildContext.murkaView;
    }
    
    MurkaShape getLatestChildShape() {
        return ((View*)latestChildContext.murkaView)->shape;
    }
    
    void setCurrentLayoutStructure(std::initializer_list<MurkaLinearLayoutGenerator::ShapePartDescriptor> list) {
       ((View*)currentContext.murkaView)->linearLayout.setLayoutStructure(list);
    }
    
    float getLayoutLineHeight() {
        return ((View*)currentContext.murkaView)->linearLayout.getLayoutLineHeight();
    }
    
    void setLayoutLinearOffset(float offset) {
        ((View*)currentContext.murkaView)->linearLayout.setLinearOffset(offset);
    }
    
    void setLayoutLineHeight(float height) {
        ((View*)currentContext.murkaView)->linearLayout.setLayoutLineHeight(height * getUIScale());
    }
    
    void addLayoutSpacing(float spacing) {
        ((View*)currentContext.murkaView)->linearLayout.addSpacing(spacing);
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
        if ((parameters != NULL) && (newHandler->tParams != NULL)) {
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

        shapeInParentContainer = shapeInParentContainer * getUIScale();
        
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

    
    
    ///////////////////
    
    void setUIScale(float newScale) {
        uiScale = newScale;
    }
    
    float getUIScale() {
        return uiScale;
    }
    
	View* getRootView() {
		return (View*)this;
	}

    struct Parameters {};

    void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);

    typedef bool Results;

private:
    float uiScale = 1;
    
    int hoverIndex = 0;
    int maxHoverIndex = 0;

};

}
