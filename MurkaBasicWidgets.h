#pragma once
#include "Murka.h"

class MurkaBlankPanel : public MurkaViewInterface<MurkaBlankPanel> {
public:
	MurkaBlankPanel() {
        ofLog() << "created blank panel...";
	}
    
    static viewDrawFunction staticDraw() {
        return [](void* dataToControl,
                  void* parametersObject,
                  void* thisWidgetObject,
                  const MurkaContext & context,
                  void* resultObject)  {
            
            auto params = (Parameters*)parametersObject;
            MurkaBlankPanel* thisWidget = (MurkaBlankPanel*)thisWidgetObject;
            
            thisWidget->count++;
            bool inside = context.isHovered();

        #ifdef MURKA_OF
                ofPushStyle();
                ofFill();
                ofSetColor(params->r, params->g, params->b, 100 + 125 * (context.eventState.mouseDown && inside));
                ofDrawRectangle(0, 0, context.getSize().x, context.getSize().y);
                ofSetColor(30);
                ofNoFill();
                ofDrawRectangle(0, 0, context.getSize().x, context.getSize().y);
                ofPopStyle();
            
                ofSetColor(255);
                ofDrawBitmapString(params->label, 0, 0);
                ofDrawBitmapString(ofToString(thisWidget->count), 0, 20);

        #endif // MURKA_OF
            
            bool gonnaResize = false;
            if ((context.eventState.mousePosition.x < context.currentViewShape.size.x - 30) &&
                (context.eventState.mousePosition.y < context.currentViewShape.size.y - 30)) {
                gonnaResize = false;
            } else gonnaResize = true;
            
        #ifdef MURKA_OF
            if ((gonnaResize) && (inside)) {
                ofDrawLine(context.currentViewShape.size.x - 5, context.currentViewShape.size.y - 5,
                           context.currentViewShape.size.x - 15, context.currentViewShape.size.y - 5);
                ofDrawLine(context.currentViewShape.size.x - 5, context.currentViewShape.size.y - 5,
                           context.currentViewShape.size.x - 5, context.currentViewShape.size.y - 15);
            }
            
            if ((inside) && (!gonnaResize)) {
                ofDrawLine(context.currentViewShape.size.x, context.eventState.mousePosition.y,
                           0, context.eventState.mousePosition.y);
                ofDrawLine(context.eventState.mousePosition.x, context.currentViewShape.size.y,
                           context.eventState.mousePosition.x, 0);
            }

        #endif // MURKA_OF

            if (inside && context.eventState.mouseDown) {
                thisWidget->dragOrResize = true;
            }
            
            if (thisWidget->dragOrResize && !context.eventState.mouseDown) {
                thisWidget->dragOrResize = false;
            }
            if (thisWidget->dragOrResize) {
                if (!gonnaResize) {
                    context.currentWidgetShapeSource->position.x -= context.eventState.mouseDelta.x;
                    context.currentWidgetShapeSource->position.y -= context.eventState.mouseDelta.y;
                } else {
                    context.currentWidgetShapeSource->size.x -= context.eventState.mouseDelta.x;
                    context.currentWidgetShapeSource->size.y -= context.eventState.mouseDelta.y;
                }
            }
            
            
//                return new bool(true); // should I add the return here tho?
            };
    }
    
    struct Parameters {
        float r, g, b;
        std::string label;
    };
    
    typedef bool Results;
    
    int count = 0;
    MurkaPoint initialPosition, initialMousePosition;
    bool dragOrResize = false;
};


class MurkaWidgetButton : public MurkaViewInterface<MurkaWidgetButton> {
public:
	MurkaWidgetButton() {

	}
    
    static viewDrawFunction staticDraw() {
        
        return [](void* dataToControl,
                  void* parametersObject,
                  void* thisWidgetObject,
                  const MurkaContext & context,
                  void* resultObject)  {
            
            
            auto parameters = ((Parameters*)parametersObject);
            
            bool inside = context.transformedShape.inside(context.eventState.mousePosition);

#ifdef MURKA_OF
            
            ofPushStyle();
            ofFill();
            if (inside) {
                ofSetColor(parameters->r, parameters->g, parameters->b);
            } else ofSetColor(75);
            ofDrawRectangle(0, 0, context.getSize().x, context.getSize().y);
            ofSetColor(30);
            ofNoFill();
            ofDrawRectangle(0, 0, context.getSize().x, context.getSize().y);
            ofPopStyle();
            
            ofSetColor(255);
            ofDrawBitmapStringHighlight(parameters->label, context.getSize().x / 2, 25);
            
            auto label = ((Parameters*)parametersObject)->label;
#endif // MURKA_OF
            
            /*
            if ((context.didClick[0]) && (context.isHovered())) {
                parameters->Callback();
                return new bool(true);
            } else return new bool(false);
            */
        };
    }
    
    
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
