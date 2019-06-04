#pragma once
#include "Murka.h"

class MurkaWidgetBlankPanel : public MurkaDrawFuncGetter<MurkaWidgetBlankPanel> {
public:
	MurkaWidgetBlankPanel() {
        draw = staticDraw();
	}
    
    static viewDrawFunction staticDraw() {
        return [](void* dataToControl,
                  void* parametersObject,
                  void* thisWidgetObject,
                  MurkaContext & context,
                  void* resultObject)  {
        #ifdef MURKA_OF
                ofPushStyle();
                ofFill();
                ofSetColor(100);
                ofDrawRectangle(0, 0, context.getSize().x, context.getSize().y);
                ofSetColor(30);
                ofNoFill();
                ofDrawRectangle(0, 0, context.getSize().x, context.getSize().y);
                ofPopStyle();
        #endif // MURKA_OF
        
                return new bool(true);
            };
    }
};


class MurkaWidgetButton : public MurkaDrawFuncGetter<MurkaWidgetButton> {
public:
	MurkaWidgetButton() {

	}
    
    static viewDrawFunction staticDraw() {
        
        return [](void* dataToControl,
                  void* parametersObject,
                  void* thisWidgetObject,
                  MurkaContext & context,
                  void* resultObject)  {
            
            
            auto parameters = ((Parameters*)parametersObject);
            
#ifdef MURKA_OF
            
            ofPushStyle();
            ofFill();
            ofSetColor(parameters->r, parameters->g, parameters->b);
            ofDrawRectangle(0, 0, context.getSize().x, context.getSize().y);
            ofSetColor(30);
            ofNoFill();
            ofDrawRectangle(0, 0, context.getSize().x, context.getSize().y);
            ofPopStyle();
            
            ofSetColor(255);
            ofDrawBitmapStringHighlight(parameters->label, context.getSize().x / 2, 25);
            
            auto label = ((Parameters*)parametersObject)->label;
#endif // MURKA_OF
            
            if ((context.didClick[0]) && (context.isHovered())) {
                parameters->Callback();
                return new bool(true);
            } else return new bool(false);
            
        };
    }
    
    
    // Results object is used for internal state management and is crucial to get right.
    /*
    void* returnNewResultsObject() {
        return new bool(false);
    }
    
    void* returnNewWidgetObject() {
        return new MurkaWidgetButton();
    }
     */

    // Whatever the parameters and the results are, the functions have to be defined here
    
    struct Parameters {
        float r, g, b;
        std::string label;
        std::function<void()> Callback;
    };
    
    typedef int Results;
    
    // Internal state

	std::string Label;
    float lastTimeClicked = 0;
};

/*
class MurkaWidgetTypeInt : public MurkaView {
public:
	MurkaWidgetTypeInt() {

        draw = [](void* dataToControl,
                  void* parametersObject,
                  void* thisWidgetObject,
                  MurkaContext & context,
                  void* resultObject)  {
#ifdef MURKA_OF
			ofSetColor(20, 150, 20);
			ofDrawRectangle(0, 0, context.getSize().x, context.getSize().y);
//            ofDrawBitmapStringHighlight("this is the widget of type 1 that refers to the value", *((int*)data), 50, 50);
#endif // MURKA_OF

			return new bool (false);
		};

	}

	// This is the widget state. Presumably it gets recreated whenever the pointer re-enters the upper level structure

	std::vector<int> memory;

	//////////////////////////////////////// The following two functions have to be there in each widget
	//////////////////////////////////////// for the immediate mode support.

	// A helper function for immediate mode. (later thought: but how????)
	viewDrawFunction widgetTypeInt() {
		return draw;
	}

	// TODO: maybe this should ask for a void* parameters object, or another kind of object?
	// in case someone wants to change the object look or function (i.e. number of tabs etc)
	void* returnNewParametersObject() {
		return new MurkaWidgetTypeInt();
	}
};

class MurkaWidgetTypeFloat : public MurkaView {
public:
	MurkaWidgetTypeFloat() {

        draw = [](void* dataToControl,
                  void* parametersObject,
                  void* thisWidgetObject,
                  MurkaContext & context,
                  void* resultObject)  {
#ifdef MURKA_OF
			ofSetColor(20, 20, 150);
			ofDrawRectangle(0, 0, context.getSize().x, context.getSize().y);
//            ofDrawBitmapStringHighlight("this is the widget of type 2 that refers to the value", *((float*)data), 50, 50);
#endif // MURKA_OF

			return new bool(false);
		};

	}


	//////////////////////////////////////// The following two functions have to be there in each widget
	//////////////////////////////////////// for the immediate mode support.

	// A helper function for immediate mode (later thought: but how????)
	viewDrawFunction widgetTypeFloat() {
		return draw;
	}

	// TODO: maybe this should ask for a void* parameters object, or another kind of object?
	// in case someone wants to change the object look or function (i.e. number of tabs etc)
	void* returnNewParametersObject() {
		return new MurkaWidgetTypeInt();
	}
};

*/
