#pragma once

// Murka is a mixed mode data driven user interface library.

#define MURKA_OF // This defines the oF render


#include <functional>
#include "ofMain.h"
#include "MurkaShapes.h"
#include "LayoutGenerator.h"

// Here's the global typedefs for cross-render functionality

#ifdef MURKA_OF
typedef ofTrueTypeFont* FontObject; // It's important for this object to exist in a render for Murka to work
#endif



class MurkaContext {
public:
	MurkaContext() {

	}

	MurkaPoint getSize() {
		return currentViewShape.size;
	}

	bool isHovered() {
		return currentViewShape.inside(mousePosition); // using absolute coordinates to calc that
	}

	void* murkaObject;

	// Pushes a container. Returns a context that you could then use if you want to draw there again later
	// the draw loop. Gets the context depth up a 1 point.
	MurkaContext pushContainer(MurkaPoint containerPosition, MurkaPoint containerShape) {
		depth++;
		previousViewShape = currentViewShape;
		currentViewShape.position += containerPosition;
	}

	// Returns to the previous context state. Gets the context depth down a 1 point.
	MurkaContext popContainer() {
		depth--;
		currentViewShape = previousViewShape;
	}

	// Utility function to transform the render into the shape of this context.
	// Helpful to draw the view innards.
	void transformTheRenderIntoThisContextShape() {
#ifdef MURKA_OF
		ofPushMatrix();
		ofTranslate(currentViewShape.position.x, currentViewShape.position.y);
#endif // MURKA_OF
	}

	void transformTheRenderBackFromThisContextShape() {
#ifdef MURKA_OF
		ofPopMatrix();
#endif // MURKA_OF
	}


	// All shapes are absolute

	MurkaShape rootViewShape;
	MurkaShape previousViewShape;
	MurkaShape currentViewShape;
	int depth = 0;


	///////////////////////////// TODO: this has to gather all the user input too

	MurkaPoint mousePosition;
	std::vector<std::tuple<MurkaPoint, bool>> fingerData; // TODO: finger type
	bool didClick[3];
};


typedef std::function<void* (void*, void*, MurkaContext&)> viewDrawFunction;

class MurkaView {
public:
	MurkaView() {

	}


	static viewDrawFunction getDrawFunction(MurkaView* view) {
		return (*view).draw;
	}

	// A recursive draw cycle that starts with this widget
	void drawCycle() {

	}

	// This children list contains all the children objects and their respective data
	std::list <std::tuple<MurkaView, void*>> children; // std::list because we may want to store a pointer to the view

	MurkaShape shape; // for the widgets that are instantiated

	//////////////////////////////////////////////// The following is to be overriden by children classes

	// A parameters initialiser. Gets called if Murka wants widget to allocate memory for its parameters.
	void* returnNewParametersObject() {
		return new MurkaView();
	}


	viewDrawFunction draw = [](void* data, void* parametersObject, MurkaContext & context)->void* {
		return false;
	};
	// draw arguments: data void pointer, parameters void pointer, context object pointer

	// create function ("factory"): returns the object initialised with the default parameter set

};





class Murka : MurkaView {
public:
	Murka() {
		shape.position = { 0.0, 0.0 };
		shape.size = { (float)ofGetWidth(), (float)ofGetHeight() }; // the root view's shape
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

	// Calls drawing a view heirarchy with a context. The should change appropriately during this loop,
// e.g. the push and pop calls have to be made.
	void drawViewWithContext(viewDrawFunction drawFunction, void* viewData, void* viewParams, MurkaContext context) {

	}

	// This function gets called in the beginning of the step. It clears everything and initialises context.
	// it also packs the user input into the context and clears the user input buffer data.
	// It should also free the memory used for function responses.
	void update() {
		currentContext = MurkaContext();
		currentContext.rootViewShape = shape;
		currentContext.currentViewShape = shape;

	}


	struct MurkaState {
		void* dataPointer;
		viewDrawFunction drawFunction;
		void* latestReturnResult;
		void* widgetParametersObject;
	};


	std::map<void*, MurkaState> states;

	void draw(); // drawing everything and making all interactions

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);

};