#pragma once
#include "Murka.h"

namespace murka {
    
// Custom widget template:

class DraggableNumberEditor : public MurkaViewInterface<DraggableNumberEditor> {
public:
    DraggableNumberEditor() {
        draw = [&](void* dataToControl,
                   void* parametersObject,
                   void* thisWidgetObject,
                   const MurkaContext & context,
                   void* resultObject)  {
            
            auto params = (Parameters*)parametersObject;
            DraggableNumberEditor* thisWidget = (DraggableNumberEditor*)thisWidgetObject;
            
            bool inside = context.isHovered() * !areChildrenHovered(context);
            
            
            double* numberData = ((double*)dataToControl);
            
            context.startViewport();
            
            auto font = context.getMonospaceFont();
            float monospaceSymbolWidth = float(font->stringWidth("x"));
            int highlightedNumber = (context.mousePosition.x - float(10)) / monospaceSymbolWidth;

#ifdef MURKA_OF

            std::string numberString = ofToString(*numberData, params->floatPrecision);
            int dotIndex = numberString.size();
            for (int i = 0; i < numberString.size(); i++) {
                if (numberString[i] == '.') {
                    dotIndex = i;
                }
            }
            
            if (highlightedNumber == dotIndex) highlightedNumber = dotIndex + 1;
            if (highlightedNumber >= numberString.size()) highlightedNumber = numberString.size() - 1;
            

            MurkaColor c = context.getWidgetForegroundColor();
            ofColor bgColor = context.getWidgetBackgroundColor() * 255;
            ofColor fgColor = context.getWidgetForegroundColor() * 255;
            
			ofPushStyle();
            ofFill();
            ofSetColor(bgColor);
            ofDrawRectangle(0, 0, context.getSize().x, context.getSize().y);
            ofNoFill();
            ofSetColor(inside ? fgColor : fgColor / 2);
            if (activated) ofSetColor(fgColor * 1.2);
            ofDrawRectangle(1, 1, context.getSize().x-2, context.getSize().y-2);

            font->drawString(ofToString(*numberData, params->floatPrecision), 10, 22);

            ofNoFill();
            if ((inside || dragging) && (highlightedNumber != -1)) {
                if (!dragging) {
                    ofDrawLine(10 + highlightedNumber * monospaceSymbolWidth, 28,
                               10 + highlightedNumber * monospaceSymbolWidth + monospaceSymbolWidth, 28);
                } else {
                    ofDrawLine(10 + draggingNubmerIndex * monospaceSymbolWidth, 28,
                               10 + draggingNubmerIndex * monospaceSymbolWidth + monospaceSymbolWidth, 28);
                }
            }
            ofFill();
			ofPopStyle();
#endif

            // text editing logic
            
            if (context.mouseDownPressed) {
                if (inside) {
                    activated = true;
                } else activated = false;
            }

            if (inside && context.mouseDownPressed && (!thisWidget->dragging)) {
                draggingNubmerIndex = highlightedNumber;
                std::string numberString = ofToString(*numberData, params->floatPrecision);
                if (draggingNubmerIndex < dotIndex) {
                    changeScale = 1. / pow(0.1, dotIndex - draggingNubmerIndex - 1);
                } else {
                    changeScale = 1. / pow(0.1, dotIndex - draggingNubmerIndex);
                }
                dragging = true;
            }
                
            if (thisWidget->dragging  && !context.mouseDown) {
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
    }
    
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

class PlainTextField : public MurkaViewInterface<PlainTextField> {
public:
    PlainTextField() {
        draw = [&](void* dataToControl,
                   void* parametersObject,
                   void* thisWidgetObject,
                   const MurkaContext & context,
                   void* resultObject)  {
            
            auto params = (Parameters*)parametersObject;
            PlainTextField* thisWidget = (PlainTextField*)thisWidgetObject;
            
            bool inside = context.isHovered() * !areChildrenHovered(context);
            
            std::string* stringData = ((std::string*)dataToControl);
            
            params->useCustomFont ? font = params->customFont : font = context.getParagraphFont();
            
#ifdef MURKA_OF
            MurkaColor c = context.getWidgetForegroundColor();
            ofColor bgColor = context.getWidgetBackgroundColor() * 255;
            ofColor fgColor = context.getWidgetForegroundColor() * 255;
            
            
            ofFill();
            ofSetColor(bgColor);
            ofDrawRectangle(0, 0, context.getSize().x, context.getSize().y);
            ofNoFill();
            ofSetColor(inside ? fgColor : fgColor / 2);
            if (activated) ofSetColor(fgColor * 1.2);
            ofDrawRectangle(1, 1, context.getSize().x-2, context.getSize().y-2);
            
            recalcGlyphLengths(*stringData, &context);
            
            float offset = 10;
            font->drawString(*stringData, 10 - managedOffset, 22);
            
            bool didSetCursorPosition = false;
            float cursorPositionInPixels = 0;
            float sumGlyphWidths;
            for (int i = 0; i < (*stringData).size(); i++) {
                
                MurkaShape glyphShape = MurkaShape(offset - managedOffset, 0, currentGlyphLengths[i], 30);
                
                
                bool insideGlyph = glyphShape.inside(context.mousePosition);
                //                    ofNoFill();
                //                    ofSetColor(insideGlyph ? fgColor : fgColor / 2);
                //                    ofDrawRectangle(offset, 0, currentGlyphLengths[i], 30);
                //                    ofFill();
                if ((insideGlyph) && (context.mouseDownPressed)) {
                    if (((context.mousePosition.x - offset) / currentGlyphLengths[i]) < 0.5) {
                        cursorPosition = i;
                        didSetCursorPosition = true;
//                        ofLog() << "set cursor position";
                    } else {
                        cursorPosition = i + 1;
                        didSetCursorPosition = true;
//                        ofLog() << "set cursor position";
                    }
                }
                if (cursorPosition == i) cursorPositionInPixels = offset;
                offset += currentGlyphLengths[i];
            }
            sumGlyphWidths = offset;
            
            if ((inside) && (context.mouseDownPressed) && (!didSetCursorPosition)) {
//                ofLog() << "didn't set cursor position";
                cursorPosition = (*stringData).size();
            }
            
            if (cursorPosition == (*stringData).size()) {
                cursorPositionInPixels = sumGlyphWidths; // this is the full line width
            }
            
            if (activated) {
                // Moving the internal window so the cursor is always visible
                if (cursorPositionInPixels < managedOffset) {
                    managedOffset = cursorPositionInPixels - 5;
                    
                }
                
                if (cursorPositionInPixels > (context.getSize().x + managedOffset)) {
                    managedOffset = cursorPositionInPixels - context.getSize().x + 5;
                }
            }

            if (activated) {
                float timeMod = context.getRunningTime() / 1.0 - int(context.getRunningTime() / 1.0);
                if (timeMod > 0.5)
                    ofDrawLine(cursorPositionInPixels - managedOffset, 3,
                               cursorPositionInPixels - managedOffset, 30);
            }
            
#endif
            if (context.mouseDownPressed) {
                if (inside) {
                    activated = true;
                } else activated = false;
            }
            
            
            // Text editing logic
            if (activated) { // remember that drawing occurs even if its not activated
                if (context.keyPresses.size() != 0) {
                    //                    ofLog() << "some key pressed!";
                    //                    ofLog() << int(context.keyPresses[0]);
                    
                    for (auto key: context.keyPresses) {
                        if ((key >= 32) && (key <= 255)) { // symbol keys
                            (*stringData).insert((*stringData).begin() + cursorPosition, char(key));
                            
                            cursorPosition += 1;
                        }
                        
                        if (key == 8) { // backspace
                            if (cursorPosition > 0) {
                                (*stringData).erase(cursorPosition - 1, 1);
                                cursorPosition -= 1;
                            }
                        }
                        
                        if (key == 57356) { // left
                            if (cursorPosition > 0) {
                                cursorPosition --;
                            }
                        }
                        if (key == 57358) { // right
                            if (cursorPosition < (*stringData).size()) {
                                cursorPosition ++;
                            }
                        }
                    }
                }
            }
            
        };
    }
    
    // Here go parameters and any parameter convenience constructors. You need to define it, even if it's NULL.
    struct Parameters {
        bool useCustomFont = false;
        
        FontObject* customFont;
        
        bool drawBounds = true;
        
        Parameters() {
            
        }
    };
    
    FontObject* font;
    
    // The results type, you also need to define it even if it's nothing.
    typedef bool Results;
    
    // Everything else is for handling the internal state
    
    float managedOffset = 0;

    void recalcGlyphLengths(std::string text, const MurkaContext* context) {
        if (currentGlyphLengths.size() < text.size()) {
            currentGlyphLengths.resize(text.size());
        }
        
        auto symbolsBoundingBoxes = font->getStringSymbolsBoundingBoxes(text, 0, 0, true);

        for (int i = 0; i < text.size(); i++) {
            currentGlyphLengths[i] = symbolsBoundingBoxes[i].width;
        }
        /*
        auto font = context->getParagraphFont();
        for (int i = 0; i < text.size(); i++) {
            if (i > 0) {
                currentGlyphLengths[i] = font->stringWidth(text.substr(i, 1)) + 0.28;
                if (text[i] == ' ') currentGlyphLengths[i] += 3.8;
            } else {
                currentGlyphLengths[i] = font->stringWidth(text.substr(0, 1));
            }
        }
         */
    }
    
    std::vector<float> currentGlyphLengths;
    int cursorPosition = 0;
    bool activated = false;
};

}
