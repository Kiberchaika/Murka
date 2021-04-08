#pragma once

#include "Murka.h"

namespace murka {
    
// Custom widget template:

class DraggableNumberEditor : public MurkaViewInterface<DraggableNumberEditor> {
public:
    MURKA_VIEW_DRAW_FUNCTION  {
        
        auto params = (Parameters*)parametersObject;
        DraggableNumberEditor* thisWidget = (DraggableNumberEditor*)thisWidgetObject;
        
        bool inside = context.isHovered() * !areChildrenHovered(context);
        
        
        double* numberData = ((double*)dataToControl);
        
        context.startViewport();
        
        auto font = context.getCurrentFont();
        float monospaceSymbolWidth = float(font->stringWidth("x"));
        int highlightedNumber = (context.mousePosition.x - float(10)) / monospaceSymbolWidth;


        std::string numberString = to_string_with_precision(*numberData, params->floatPrecision);
        int dotIndex = numberString.size();
        for (int i = 0; i < numberString.size(); i++) {
            if (numberString[i] == '.') {
                dotIndex = i;
            }
        }
        
        if (highlightedNumber == dotIndex) highlightedNumber = dotIndex + 1;
        if (highlightedNumber >= numberString.size()) highlightedNumber = numberString.size() - 1;
        

        MurkaColor c = context.renderer->getColor();
		MurkaColor bgColor = context.renderer->getColor();
		MurkaColor fgColor = context.renderer->getColor();
        
		context.renderer->pushStyle();
		context.renderer->enableFill();
		context.renderer->setColor(bgColor);
		context.renderer->drawRectangle(0, 0, context.getSize().x, context.getSize().y);
		context.renderer->disableFill();
		context.renderer->setColor(inside ? fgColor : fgColor / 2);
        if (activated) context.renderer->setColor(fgColor * 1.2);
		context.renderer->drawRectangle(1, 1, context.getSize().x-2, context.getSize().y-2);

        font->drawString(to_string_with_precision(*numberData, params->floatPrecision), 10, 22);

		context.renderer->disableFill();
        if ((inside || dragging) && (highlightedNumber != -1)) {
            if (!dragging) {
				context.renderer->drawLine(10 + highlightedNumber * monospaceSymbolWidth, 28,
                           10 + highlightedNumber * monospaceSymbolWidth + monospaceSymbolWidth, 28);
            } else {
				context.renderer->drawLine(10 + draggingNubmerIndex * monospaceSymbolWidth, 28,
                           10 + draggingNubmerIndex * monospaceSymbolWidth + monospaceSymbolWidth, 28);
            }
        }
		context.renderer->enableFill();
		context.renderer->popStyle();


        // text editing logic
        
        if (context.mouseDownPressed[0]) {
            if (inside) {
                activated = true;
            } else activated = false;
        }

        if (inside && context.mouseDownPressed[0] && (!thisWidget->dragging)) {
            draggingNubmerIndex = highlightedNumber;
            std::string numberString = to_string_with_precision(*numberData, params->floatPrecision);
            if (draggingNubmerIndex < dotIndex) {
                changeScale = 1. / powf(0.1, dotIndex - draggingNubmerIndex - 1);
            } else {
                changeScale = 1. / powf(0.1, dotIndex - draggingNubmerIndex);
            }
            dragging = true;
        }
            
        if (thisWidget->dragging  && !context.mouseDown[0]) {
            thisWidget->dragging = false;
        }

        if (thisWidget->dragging) {
            float delta = (context.mouseDelta.y) / 10;
            float change = changeScale * delta;
            *numberData += change;
            if (*numberData < params->minValue) {
                *numberData = params->minValue;
            }
            if (*numberData > params->maxValue) {
                *numberData = params->maxValue;
            }
        }

        context.endViewport();
    };
    
    // Here go parameters and any parameter convenience constructors. You need to define something called Parameters, even if it's NULL.
    struct Parameters {
        double minValue = 0.0, maxValue = 1.0;
        int floatPrecision = 3;
        
        Parameters() {}
        Parameters(float min, float max) { minValue = min; maxValue = max; } // a convenience initializer
        Parameters(int floatingPointPrecision, float min, float max) {
            floatPrecision = floatingPointPrecision;
            minValue = min;
            maxValue = max; }
        Parameters(int floatingPointPrecision) {
            floatPrecision = floatingPointPrecision;
        }
    };
    
    // The results type, you also need to define it even if it's nothing.
    typedef bool Results;
    
    // Everything else in the class is for handling the internal state. It persist.
    
    bool activated = false;
    
    MurkaPoint initialPosition, initialMousePosition;
    bool dragging = false;
    int draggingNubmerIndex = 0;
    float changeScale = 1.0;
};



}

