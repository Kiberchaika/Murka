#pragma once

#include "Murka.h"

namespace murka {

// PRE-1.0 made draggable number editor.
// TO BE UPDATED TO 1.0

class DraggableNumberEditor : public View<DraggableNumberEditor> {
public:
	void internalDraw(Murka & m) {
	    MurkaContext& c = m.currentContext;

		bool inside = c.isHovered() * !areChildrenHovered(c);
        
        double* numberData = dataToControl;
        
		m.startViewport();
        
        auto font = c.getCurrentFont();
        float monospaceSymbolWidth = float(font->stringWidth("x"));
        int highlightedNumber = (c.mousePosition.x - float(10)) / monospaceSymbolWidth;

        std::string numberString = to_string_with_precision(*numberData, floatPrecision);
        int dotIndex = numberString.size();
        for (int i = 0; i < numberString.size(); i++) {
            if (numberString[i] == '.') {
                dotIndex = i;
            }
        }
        
        if (highlightedNumber == dotIndex) highlightedNumber = dotIndex + 1;
        if (highlightedNumber >= numberString.size()) highlightedNumber = numberString.size() - 1;

        MurkaColor col = c.pointerToRenderer->getColor();
		MurkaColor bgColor = c.pointerToRenderer->getColor();
		MurkaColor fgColor = c.pointerToRenderer->getColor();
        
		m.pushStyle();
		m.enableFill();
		m.setColor(bgColor);
		m.drawRectangle(0, 0, shape.size.x, shape.size.y);
		m.disableFill();
		m.setColor(inside ? fgColor : fgColor / 2);
        if (activated) c.pointerToRenderer->setColor(fgColor * 1.2);
		m.drawRectangle(1, 1, shape.size.x-2, shape.size.y-2);

        font->drawString(to_string_with_precision(*numberData, floatPrecision), 10, 22);

		c.pointerToRenderer->disableFill();
        if ((inside || dragging) && (highlightedNumber != -1)) {
            if (!dragging) {
				c.pointerToRenderer->drawLine(10 + highlightedNumber * monospaceSymbolWidth, 28,
                           10 + highlightedNumber * monospaceSymbolWidth + monospaceSymbolWidth, 28);
            } else {
				c.pointerToRenderer->drawLine(10 + draggingNubmerIndex * monospaceSymbolWidth, 28,
                           10 + draggingNubmerIndex * monospaceSymbolWidth + monospaceSymbolWidth, 28);
            }
        }
		c.pointerToRenderer->enableFill();
		c.pointerToRenderer->popStyle();

        // text editing logic
        
        if (c.mouseDownPressed[0]) {
            if (inside) {
                activated = true;
            } else activated = false;
        }

        if (inside && c.mouseDownPressed[0] && (!dragging)) {
            draggingNubmerIndex = highlightedNumber;
            std::string numberString = to_string_with_precision(*numberData, floatPrecision);
            if (draggingNubmerIndex < dotIndex) {
                changeScale = 1. / powf(0.1, dotIndex - draggingNubmerIndex - 1);
            } else {
                changeScale = 1. / powf(0.1, dotIndex - draggingNubmerIndex);
            }
            dragging = true;
        }
            
        if (dragging  && !c.mouseDown[0]) {
            dragging = false;
        }

        if (dragging) {
            float delta = (c.mouseDelta.y) / 10;
            float change = changeScale * delta;
            *numberData += change;
            if (*numberData < minValue) {
                *numberData = minValue;
            }
            if (*numberData > maxValue) {
                *numberData = maxValue;
            }
        }

		m.endViewport();
    };
    
    // Here go parameters and any parameter convenience constructors. You need to define something called Parameters, even if it's NULL.

    double minValue = 0.0, maxValue = 1.0;
    int floatPrecision = 3;

	double* dataToControl = nullptr;
           
	DraggableNumberEditor& controlling(double* value) {
		dataToControl = value;
	}
    
    // Everything else in the class is for handling the internal state. It persist.
    
    bool activated = false;
    MurkaPoint initialPosition, initialMousePosition;
    bool dragging = false;
    int draggingNubmerIndex = 0;
    float changeScale = 1.0;
};

} // end of namespace Murka
