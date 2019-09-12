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
 
 class Widget : public MurkaViewInterface<Widget> {
 public:
 Widget() {
     draw = [&](void* dataToControl,
                void* parametersObject,
                void* thisWidgetObject,
                const MurkaContext & context,
                void* resultObject)  {
 
                    auto params = (Parameters*)parametersObject;
                    Widget* thisWidget = (Widget*)thisWidgetObject;
 
                    bool inside = context.isHovered() * !areChildrenHovered(context);
 
                    // Your drawing and interaction code goes here.
                    // Don't forget that all of this executes at each frame for each
                    // widget that is drawn on screen.
                };
     }
 
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

class Murka : public MurkaViewInterface<Murka>, MurkaInputEventsRegister, public MurkaAssets {
public:
	Murka() {
        setupEvents();
	}



	MurkaContext currentContext;
    
    void pushContext(MurkaViewHandlerInternal* viewSource) {
        
        ((View*)currentContext.murkaView)->latestContext = currentContext;
        
        MurkaPoint containerPosition = ((View*)viewSource->widgetObjectInternal)->shape.position;
        MurkaEventState derivedEventState = currentContext.transformedWith({-containerPosition.x,
            -containerPosition.y}, 1.0);
        if (currentContext.parentContext == NULL) {
            currentContext.parentContext = new MurkaContext();
//            ofLog() << "created context...";
        }
        *(currentContext.parentContext) = currentContext;
        currentContext.MurkaEventState::operator=(derivedEventState);
        currentContext.currentViewShape.position += containerPosition;
        currentContext.currentViewShape.size = ((View*)viewSource->widgetObjectInternal)->shape.size;
        currentContext.murkaView = ((View*)viewSource->widgetObjectInternal);
        
        ((View*)viewSource->widgetObjectInternal)->latestContext = currentContext;
        
        latestChildContext = currentContext;
    }
    
    void popContext() {
        currentContext = *(currentContext.parentContext);
    }
    
    void transformRenderIntoCurrentContext() {
        currentContext.transformTheRenderIntoThisContextShape();
    }

    void transformRenderBackFromLastContext() {
        currentContext.transformTheRenderBackFromThisContextShape();
    }
    


    void drawCycle() { // this version is without arguments cause it creates the context
        
        restartContext();
        
        for (auto& i : children) {
            pushContext(i);
                drawCycleRecursive(i);
            popContext();
        }
    }

	// A recursive OOP draw cycle that starts with this widget
	void drawCycleRecursive(MurkaViewHandlerInternal* widget) {
        
        currentContext.transformTheRenderIntoThisContextShape();
        
        // DEBUG
        /*
        ofNoFill();
        ofSetColor(255, 0, 0);
       
        ofDrawRectangle(((View*)widget->widgetObjectInternal)->childrenBounds.position.x,
                        ((View*)widget->widgetObjectInternal)->childrenBounds.position.y,
                        ((View*)widget->widgetObjectInternal)->childrenBounds.size.x - 1,
                        ((View*)widget->widgetObjectInternal)->childrenBounds.size.y - 1);

        ofFill();
         */
        ////
        
        ((MurkaViewHandler<View>*)widget)->widgetObject->draw(widget->dataToControl, widget->parametersInternal, widget->widgetObjectInternal, currentContext, widget->resultsInternal);
        
        // restarting layout generator
        ((MurkaViewHandler<View>*)widget)->widgetObject->layoutGenerator.restart(((MurkaViewHandler<View>*)widget)->widgetObject->shape /*, ((View*)widget->widgetObjectInternal)->childrenBounds */);
//        ((View*)widget->widgetObjectInternal)->childrenBounds = {0, 0, 0, 0};
        
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
        if (currentContext.parentContext != NULL) {
            delete currentContext.parentContext;
//            ofLog() << "deleted context...";
        }
		currentContext = MurkaContext();
        *((MurkaEventState*)&currentContext) = eventState;
        currentContext.assetsObject = this;
        currentContext.murkaView = this;
        
#ifdef MURKA_OF
        currentContext.currentViewShape = MurkaShape{0,
                                                  0,
                                                  float(ofGetWidth()),
                                                  float(ofGetHeight())};
//        currentContext.currentViewShape = currentContext.rootViewShape;
#endif
        currentContext.pushContextInternal = [&](MurkaViewHandlerInternal* mvhi) { pushContext(mvhi); };
        currentContext.popContextInternal = [&]() { popContext(); };
        

        latestContext = currentContext;

        currentContext.parentContext = new MurkaContext();
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
       ((View*)currentContext.murkaView)->layoutGenerator.setLayoutStructure(list);
    }
    
    float getLayoutLineHeight() {
        return ((View*)currentContext.murkaView)->layoutGenerator.getLayoutLineHeight();
    }
    
    void setLayoutLinearOffset(float offset) {
        ((View*)currentContext.murkaView)->layoutGenerator.setLinearOffset(offset);
    }
    
    void setLayoutLineHeight(float height) {
        ((View*)currentContext.murkaView)->layoutGenerator.setLayoutLineHeight(height);
    }
    
    void addLayoutSpacing(float spacing) {
        ((View*)currentContext.murkaView)->layoutGenerator.addSpacing(spacing);
    }

    ////////
    //////// Object oriented view heirarchy management
    ////////
    
    template <typename Z>
    MurkaViewHandler<Z>* addChildToView(View* parent, MurkaViewInterface<Z>* child, void* data, void* parameters, MurkaShape shapeInParentContainer)
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
    template <typename Z>
    MurkaViewHandler<Z>* addChildToView(MurkaViewHandlerInternal* parent, MurkaViewInterface<Z>* child, void* data, typename Z::Parameters parameters, MurkaShape shapeInParentContainer)
    {
        auto newParameters = new typename Z::Parameters();
        *newParameters = parameters;

        return addChildToView((View*)parent->widgetObjectInternal, child, data, newParameters,                           shapeInParentContainer);
    }

    // A version that accepts parameters reference rather a pointer
    template <typename Z>
    MurkaViewHandler<Z>* addChildToView(MurkaViewInterface<Z>* child, void* data, typename Z::Parameters parameters, MurkaShape shapeInParentContainer)
    {
        auto newParameters = new typename Z::Parameters();
        *newParameters = parameters;
        
        return addChildToView(getRootView(), child, data, newParameters,                           shapeInParentContainer);
    }

    
    
    ///////////////////

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
};

}
