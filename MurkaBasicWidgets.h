#pragma once
#include "Murka.h"

class MurkaBlankPanel : public MurkaViewInterface<MurkaBlankPanel> {
public:
	MurkaBlankPanel() {
        ofLog() << "created blank panel...";
        
        draw = [&](void* dataToControl,
                  void* parametersObject,
                  void* thisWidgetObject,
                  const MurkaContext & context,
                  void* resultObject)  {
            
            auto params = (Parameters*)parametersObject;
            MurkaBlankPanel* thisWidget = (MurkaBlankPanel*)thisWidgetObject;
            
            bool inside = context.isHovered() * !isChildrenHovered(context);

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
            
            if (inside && context.eventState.mouseDownPressed && (!thisWidget->dragging) && (!thisWidget->resizing)) {
                if (gonnaResize) thisWidget->resizing = true;
                else thisWidget->dragging = true;
                
            }
            
            if (((thisWidget->dragging) || (thisWidget->resizing))  &&
                !context.eventState.mouseDown) {
                thisWidget->dragging = false;
                thisWidget->resizing = false;
            }
            
            if (thisWidget->dragging) {
                context.currentWidgetShapeSource->position.x -= context.eventState.mouseDelta.x;
                context.currentWidgetShapeSource->position.y -= context.eventState.mouseDelta.y;
                
                ofDrawBitmapString("dragging", 50, 200);
            }
            
            if (thisWidget->resizing) {
                context.currentWidgetShapeSource->size.x -= context.eventState.mouseDelta.x;
                context.currentWidgetShapeSource->size.y -= context.eventState.mouseDelta.y;
                
                ofDrawBitmapString("resizing", 50, 200);
            }
            
            
            //                return new bool(true); // should I add the return here tho?
        };
	}
    

    
    struct Parameters {
        float r, g, b;
        std::string label;
    };
    
    typedef bool Results;
    
    MurkaPoint initialPosition, initialMousePosition;
    bool dragging = false, resizing = false;
};


class MurkaWidgetButton : public MurkaViewInterface<MurkaWidgetButton> {
public:
	MurkaWidgetButton() {
        draw = [&](void* dataToControl,
                   void* parametersObject,
                   void* thisWidgetObject,
                   const MurkaContext & context,
                   void* resultObject)  {
            
             
             auto parameters = ((Parameters*)parametersObject);
             Results& results = castResults();
            
             bool inside = context.transformedShape.inside(context.eventState.mousePosition);
             MurkaWidgetButton* thisWidget = (MurkaWidgetButton*)thisWidgetObject;
             
             if ((context.eventState.mouseDown) && (context.isHovered())) {
                 results = true;
             } else results = false;
            
            
             
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
             float offset = parameters->label.length() * 4;
             ofDrawBitmapString(parameters->label, context.getSize().x / 2 - offset, 25);
             
             auto label = ((Parameters*)parametersObject)->label;
            
             #endif // MURKA_OF
            
        };
	}
    
    
    // Whatever the parameters and the results are, the functions have to be defined here
    
    struct Parameters {
        float r = 80, g = 80, b = 80;
        std::string label;
        std::function<void()> Callback;

        Parameters() {}
        
        Parameters(const char* Label) {
            label = Label;
        }

        Parameters(float R, float G, float B, std::string Label) {
            r = R; g = G; b = B; label = Label;
        }

        Parameters(float R, float G, float B, std::string Label, std::function<void()> callback) {
            r = R; g = G; b = B; label = Label; Callback = callback;
        }

    };
    
    typedef bool Results;
    
    // Internal state

	std::string Label;
    float lastTimeClicked = 0;
    
    // Helpers that didn't work in templated class yet :(
    
    Results& castResults() {
        return  *((Results*)resultsInternal);
    }
    
    Parameters& castParameters() {
        return  *((Parameters*)parametersInternal);
    }

};


class MurkaSliderFloat : public MurkaViewInterface<MurkaSliderFloat> {
public:
    MurkaSliderFloat() {
        draw = [&](void* dataToControl,
                   void* parametersObject,
                   void* thisWidgetObject,
                   const MurkaContext & context,
                   void* resultObject)  {
            
            
            auto parameters = ((Parameters*)parametersObject);
            Results& results = castResults();
            
            bool inside = context.transformedShape.inside(context.eventState.mousePosition);
            MurkaWidgetButton* thisWidget = (MurkaWidgetButton*)thisWidgetObject;
            
            if ((context.eventState.mouseDownPressed) && (context.isHovered())) {
                dragging = true;
            }
            
            if ((dragging) && (!context.eventState.mouseDown)) {
                dragging = false;
            }
            
            if (dragging) {
                float newValue = context.eventState.mousePosition.x / context.currentViewShape.size.x;
                
                if (newValue > parameters->maxValue) newValue = parameters->maxValue;
                if (newValue < parameters->minValue) newValue = parameters->minValue;
                *((float*)dataToControl) = newValue;
            }
            
#ifdef MURKA_OF
            
            ofPushStyle();
            ofFill();
            ofSetColor(15);
            ofDrawRectangle(0, 0, context.getSize().x, context.getSize().y);
            ofSetColor(parameters->r + (inside ? 10 : 0),
                       parameters->g + (inside ? 10 : 0),
                       parameters->b + (inside ? 10 : 0), 200);
            float currentValue = *((float*)dataToControl);
            ofDrawRectangle(0, 0, context.getSize().x * ((currentValue - parameters->minValue) / parameters->maxValue), context.getSize().y);
            ofSetColor(30);
            ofNoFill();
            if (inside) {
                ofDrawRectangle(0, 0, context.getSize().x, context.getSize().y);
            }
            ofPopStyle();
            
            ofSetColor(255);
            auto resultString = parameters->label + ": " + ofToString(*((float*)dataToControl));
            float offset = resultString.length() * 4;
            ofDrawBitmapString(resultString, context.getSize().x / 2 - offset, 25);
            
            auto label = ((Parameters*)parametersObject)->label;
            
#endif // MURKA_OF
            
        };
    }
    
    
    // Whatever the parameters and the results are, the functions have to be defined here
    
    struct Parameters {
        float r = 80, g = 80, b = 80;
        std::string label;
        float minValue, maxValue;
        bool clamp = true;
        
        Parameters() {}
        
        Parameters(const char* Label) {
            label = Label;
        }
        
        Parameters(float MinValue, float MaxValue, std::string Label) {
            minValue = MinValue; maxValue = MaxValue; label = Label;
        }
        
    };
    
    typedef bool Results;
    
    // Internal state
    
    std::string Label;
    float lastTimeClicked = 0;
    bool dragging = false;
    
    // Helpers that didn't work in templated class yet :(
    
    Results& castResults() {
        return  *((Results*)resultsInternal);
    }
    
    Parameters& castParameters() {
        return  *((Parameters*)parametersInternal);
    }
    
};
