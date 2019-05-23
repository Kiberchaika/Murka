#pragma once
#include "Murka.h"

class MurkaWidgetBlankPanel : public MurkaView {
public:
	MurkaWidgetBlankPanel() {
		draw = [](void* data, void* parametersObject, MurkaContext & context)->void* {
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


class MurkaWidgetButton : public MurkaView {
public:
	MurkaWidgetButton() {
		draw = [](void* data, void* parametersObject, MurkaContext & context)->void* {
//            auto castedParametersObject = ((MurkaWidgetButton*)parametersObject);
            
            auto castedParametersObject = ((MurkaWidgetButton*)parametersObject);
            
            ofLog() << "when called differently, now the label is " << castedParametersObject->Label;
            
/*
            
//            ofLog() << "calling the button draw...";
#ifdef MURKA_OF
			ofPushStyle();
			ofFill();
			ofSetColor(100);
			ofDrawRectangle(0, 0, context.getSize().x, context.getSize().y);
			ofSetColor(30);
			ofNoFill();
			ofDrawRectangle(0, 0, context.getSize().x, context.getSize().y);
			ofPopStyle();

			ofSetColor(255);
			ofDrawBitmapStringHighlight(castedParametersObject->Label, context.getSize().x / 2, 25);

			auto label = ((MurkaWidgetButton*)parametersObject)->Label;
#endif // MURKA_OF

			if ((context.didClick[0]) && (context.isHovered())) {
				castedParametersObject->Callback();
				return new bool(true);
			} else return new bool(false);

 */
            
		};
	}

    MurkaWidgetButton(std::string label, std::function<void()> callback, MurkaShape shape): MurkaWidgetButton() {
		Label = label;
		Callback = callback;
	}

	std::function<void()> Callback;
	std::string Label;
};

class MurkaWidgetTypeInt : public MurkaView {
public:
	MurkaWidgetTypeInt() {

		draw = [](void* data, void* parametersObject, MurkaContext & context)->void* {
#ifdef MURKA_OF
			ofSetColor(20, 150, 20);
			ofDrawRectangle(0, 0, context.getSize().x, context.getSize().y);
			ofDrawBitmapStringHighlight("this is the widget of type 1 that refers to the value", *((int*)data), 50, 50);
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

		draw = [](void* data, void* parametersObject, MurkaContext & context)->void* {
#ifdef MURKA_OF
			ofSetColor(20, 20, 150);
			ofDrawRectangle(0, 0, context.getSize().x, context.getSize().y);
			ofDrawBitmapStringHighlight("this is the widget of type 2 that refers to the value", *((float*)data), 50, 50);
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
