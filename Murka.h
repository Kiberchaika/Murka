#pragma once

// Murka is a mixed mode data driven user interface library.

#define MURKA_OF // This defines the oF render. TODO: move it to ofxMurka.h instead


#include <functional>
#include "MurkaShapes.h"
#include "MurkaContext.h"
#include "MurkaView.h"

#include "LayoutGenerator.h"

// Here's the global typedefs for cross-render functionality

#ifdef MURKA_OF
    #include "ofMain.h"
    typedef ofTrueTypeFont* FontObject; // It's important for this object to exist in a render for Murka to work
#endif


class Murka : public MurkaView {
public:
	Murka() {
        /* // Should I return to having the root view's shape or nah?
		shape.position = { 0.0, 0.0 };
		shape.size = { (float)ofGetWidth(), (float)ofGetHeight() }; // the root view's shape
         */
	}

    void setupEvents() {
        
    }

	/*
		for each and every widget, we need this:

		WidgetStruct {
			void* widgetParams;
			void* widgetDrawFunction;
		}

		std::map<std::unique_ptr pointerToData, WidgetStruct>

		// ^ we draw a graph with this and when we stumble upon a stack of deferred
		// draw calls (immediate mode), we either create a data struct for a widget,
		// or use the one provided in a call itself

		// TODO:
		// 1) both fully immediate (widget parameters are separated) and half-immediate (widget parameters  are 
		// allocated by Murka itself) draw calls that do their thing and then call their own draw function
		// (recursive tree architecture)
		// 2) panel and widget structures - are they integrated or not? probably they are
		// hence it's the View, not the Widget
	*/

	MurkaContext currentContext;


	// A recursive OOP draw cycle that starts with this widget
	void drawCycle(MurkaViewHandler* widget, MurkaContext context) {
		ofLog() << "drawing at context level " << context.depth;
        context.transformTheRenderIntoThisContextShape();

        
        
		context.murkaObject = this;
        
        widget->drawFunction(widget->dataToControl, widget->parametersObject, widget->widgetObject, context, widget->result);

			for (auto& i : ((MurkaView*)widget->widgetObject)->children) {
                context.pushContainer(i->shape.position,
                                      i->shape.size);
                    drawCycle(i, context);
				context.popContainer();
			}
  
        
        
		context.transformTheRenderBackFromThisContextShape();
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

	void drawCycle() { // this version is without arguments cause it creates the context
		restartContext();

        for (auto& i : children) {
            currentContext.pushContainer(i->shape.position,
                                  i->shape.size);

                drawCycle(i, currentContext);
            currentContext.popContainer();
            
        }
	}

	// State management

	MurkaViewHandler* addChildToView(MurkaView* parent, MurkaView* child, void* data, void* parameters, MurkaShape shapeInParentContainer)
    {
        
        MurkaViewHandler newHandler;
        newHandler.parametersObject = child->returnNewParametersObject();
        if (parameters != NULL) {
            newHandler.parametersObject = parameters;
        }
        newHandler.drawFunction = child->getDrawFunction(child);
        newHandler.result = child->returnNewResultsObject();
        newHandler.manuallyControlled = true;
        newHandler.widgetObject = child->returnNewWidgetObject();
        newHandler.shape = shapeInParentContainer;
        
        viewHandlers.push_back(newHandler);
        
        auto handlerPointer = &(viewHandlers.back());
        parent->children.push_back(handlerPointer);
        return handlerPointer;
	}
    
    MurkaViewHandler* addChildToView(MurkaView* child, void* data, void* parameters, MurkaShape shapeInParentContainer)
    {
        addChildToView(getRootView(), child, data, parameters, shapeInParentContainer);
    }

	MurkaView* getRootView() {
		return (MurkaView*)this;
	}

    // TODO: MurkaView has to hold a pointer to this descriptor in its children array.
    // this guarantees that we only allocate data here.
    


//    void updateStateCallResult(void* dataPointer, void* result) {
//        states[dataPointer].result = result; // !!!TODO!!!: is this ok? should I delete the previous pointer?
//    }

	std::list<MurkaViewHandler> viewHandlers; // States is a central data holder, a map that maps a data to the widget

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);

};
