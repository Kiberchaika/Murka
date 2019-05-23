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
		shape.position = { 0.0, 0.0 };
		shape.size = { (float)ofGetWidth(), (float)ofGetHeight() }; // the root view's shape
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


	// A recursive draw cycle that starts with this widget
	void drawCycle(MurkaView* widget, MurkaContext context) {
		ofLog() << "drawing at context level " << context.depth;
        context.transformTheRenderIntoThisContextShape();

		context.murkaObject = this;
//        auto result = widget->draw(widget->data, widget, (states[data].widgetParametersObject, context));
        
		//widget->draw(NULL, widget, (states[data].widgetParametersObject, context));
		//murkaObject->updateStateCallResult(data, result);


			for (auto& i : widget->children) {
				auto childWidget = std::get<0>(i);
                context.pushContainer(childWidget->shape.position,
                                      childWidget->shape.size);
                drawCycle(childWidget, context);
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
		currentContext.rootViewShape = shape;
		currentContext.currentViewShape = shape;

	}

	void drawCycle() { // this version is without arguments cause it creates the context
		restartContext();

		drawCycle(this, currentContext);
	}

	// State management

	void addChildToView(MurkaView* parent, MurkaView* child, void* data) {
		parent->children.push_back({ child, data });
		states[data] = MurkaState();
		states[data].widgetParametersObject = child;
        states[data].drawFunction = child->getDrawFunction(child);
//        child->data = data;
	}

	MurkaView* getRootView() {
		return (MurkaView*)this;
	}

    // TODO: MurkaView has to hold a pointer to this descriptor in its children array.
    // this guarantees that we only allocate data here.
    
	struct MurkaState {
		viewDrawFunction drawFunction;
		void* latestReturnResult;
		void* widgetParametersObject;
        
        bool wasUsedInLastFrame = true; // if this becomes false, we unallocate it
	};

	void updateStateCallResult(void* dataPointer, void* result) {
		states[dataPointer].latestReturnResult = result;
	}

	std::map<void*, MurkaState> states;

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);

};
