#pragma once

// Murka is a mixed mode data driven user interface library.

#define MURKA_OF // This defines the oF render. TODO: move it to ofxMurka.h instead


#include <functional>
#include "MurkaShapes.h"
#include "MurkaContext.h"
#include "MurkaView.h"
#include "MurkaInputEventsRegister.h"

#include "LayoutGenerator.h"

// Here's the global typedefs for cross-render functionality

#ifdef MURKA_OF
    #include "ofMain.h"
    typedef ofTrueTypeFont* FontObject; // It's important for this object to exist in a render for Murka to work
#endif


class Murka : public MurkaViewInterface<Murka>, MurkaInputEventsRegister {
public:
	Murka() {
        /* // Should I return to having the root view's shape or nah?
		shape.position = { 0.0, 0.0 };
		shape.size = { (float)ofGetWidth(), (float)ofGetHeight() }; // the root view's shape
         */
        
        setupEvents();
	}



	MurkaContext currentContext;

    void drawCycle() { // this version is without arguments cause it creates the context
        restartContext();
        
        currentContext.eventState = eventState;
        currentContext.currentMurkaView = this;
        latestContext = currentContext;
        
        for (auto& i : children) {
            currentContext.pushContainer(((MurkaView*)i->widgetObjectInternal)->shape.position,
                                          ((MurkaView*)i->widgetObjectInternal)->shape.size, (MurkaView*)i->widgetObjectInternal);
            currentContext.currentWidgetShapeSource = &( ((MurkaView*)i->widgetObjectInternal)->shape);
            
            drawCycle(i, currentContext);
            currentContext.popContainer();
            
        }
        
        clearEvents();
    }

	// A recursive OOP draw cycle that starts with this widget
	void drawCycle(MurkaViewHandlerInternal* widget, MurkaContext context) {
        context.transformTheRenderIntoThisContextShape();

		context.murkaObject = this;
        context.currentMurkaView = ((MurkaViewHandler<MurkaView>*)widget)->widgetObject;
        ((MurkaViewHandler<MurkaView>*)widget)->widgetObject->latestContext = context;
        
        ((MurkaViewHandler<MurkaView>*)widget)->widgetObject->draw(widget->dataToControl, widget->parametersInternal, widget->widgetObjectInternal, context, widget->resultsInternal);

        context.transformTheRenderBackFromThisContextShape();

        for (auto& i : ((MurkaView*)widget->widgetObjectInternal)->children) {
            context.pushContainer(((MurkaView*)i->widgetObjectInternal)->shape.position,
                                  ((MurkaView*)i->widgetObjectInternal)->shape.size, ((MurkaView*)i->widgetObjectInternal));
            context.currentWidgetShapeSource = &(((MurkaView*)i->widgetObjectInternal)->shape);
                drawCycle(i, context);
            context.popContainer();
        }
        
        
	}

	// Calls drawing a view heirarchy with a context. The should change appropriately during this loop,
	// e.g. the push and pop calls have to be made.
	void drawViewWithContext(viewDrawFunction drawFunction, void* viewData, void* viewParams, MurkaContext context) {

	}

	// This function gets called in the beginning of the step. It clears everything and initialises context.
	// it also packs the user input into the context and clears the user input buffer data.
	// It should also free the memory used for function responses.
	void restartContext () {
		currentContext = MurkaContext();
        
#ifdef MURKA_OF
        currentContext.rootViewShape = MurkaShape{0,
                                                  0,
                                                  float(ofGetWidth()),
                                                  float(ofGetHeight())};
        currentContext.currentViewShape = currentContext.rootViewShape;
#endif

	}

    ////////
    //////// Immediate mode
    ////////

    MurkaContext getContextFromMurkaView(MurkaView* view) {
        view->latestContext.resetImCounter();
        return view->latestContext;
    }

    MurkaContext getContextFromMurkaViewHandler(MurkaViewHandlerInternal* viewHandler) {
        ((MurkaViewHandler<MurkaView>*)viewHandler)->widgetObject->latestContext.resetImCounter();
        return ((MurkaViewHandler<MurkaView>*)viewHandler)->widgetObject->latestContext;
    }
    
    void beginDrawingInView(MurkaViewHandlerInternal* viewHandler) {
        currentContext = getContextFromMurkaViewHandler(viewHandler);
    }

    void beginDrawingInView(MurkaView* view) {
        currentContext = getContextFromMurkaView(view);
    }
    
    void beginDrawingInLatestView() {
        currentContext = latestContext;
    }

    ////////
    //////// Heirarchy management
    ////////
    
    template <typename Z>
    MurkaViewHandler<Z>* addChildToViewT(MurkaView* parent, MurkaViewInterface<Z>* child, void* data, void* parameters, MurkaShape shapeInParentContainer)
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
        return (MurkaViewHandler<Z>*)parent->children[parent->children.size() - 1];
    }
    
    template <typename Z>
    MurkaViewHandler<Z>* addChildToViewT(MurkaViewInterface<Z>* child, void* data, void* parameters, MurkaShape shapeInParentContainer)
    {
        return addChildToViewT(getRootView(), child, data, parameters,                           shapeInParentContainer);
    }

    // A version that accepts a handler rather than the widget object as parent
    template <typename Z>
    MurkaViewHandler<Z>* addChildToViewT(MurkaViewHandlerInternal* parent, MurkaViewInterface<Z>* child, void* data, typename Z::Parameters parameters, MurkaShape shapeInParentContainer)
    {
        auto newParameters = new typename Z::Parameters();
        *newParameters = parameters;

        return addChildToViewT((MurkaView*)parent->widgetObjectInternal, child, data, newParameters,                           shapeInParentContainer);
    }

    // A version that accepts parameters reference rather a pointer
    template <typename Z>
    MurkaViewHandler<Z>* addChildToViewT(MurkaViewInterface<Z>* child, void* data, typename Z::Parameters parameters, MurkaShape shapeInParentContainer)
    {
        auto newParameters = new typename Z::Parameters();
        *newParameters = parameters;
        
        return addChildToViewT(getRootView(), child, data, newParameters,                           shapeInParentContainer);
    }

    
    
    ///////////////////

	MurkaView* getRootView() {
		return (MurkaView*)this;
	}


    void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);

    typedef bool Results;
};
