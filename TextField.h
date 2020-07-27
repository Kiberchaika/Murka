#pragma once

namespace murka {

class TextField : public MurkaViewInterface<TextField> {
    
    class KeyStroke {
        vector<int> keys;
        
        bool fired = false;
        
        double timeItFired = 0;
    public:
        
        operator vector<int>() { return keys; };
        
        KeyStroke(const initializer_list<int> &il) {
            for (auto x: il) { keys.push_back(x); }
        }
        
        KeyStroke() { }
        
        bool isPressed() {
            if (fired) {
                bool shouldResetFired = false;
                for (auto &i: keys) { if (ofGetKeyPressed(i)) shouldResetFired = true; }
                if (shouldResetFired) {
                    fired = false;
                }
                return false;
            }
            
#ifdef MURKA_OF
            
            bool result = true;
            for (auto &i: keys) { if (!ofGetKeyPressed(i)) result = false; }
            
            if ((ofGetElapsedTimef() - timeItFired) < 0.5) {
                result = false; // too little time since it was last pressed
            }
#endif

            return result;
        }
        
        void fire() {
            fired = true;
            
#ifdef MURKA_OF
            timeItFired = ofGetElapsedTimef();
#endif
        }
    };
    
    KeyStroke copyText, cutText, pasteText, goLeft, goRight, shiftLeft, shiftRight, selectAll;
    
public:
    TextField() {
        
        // Setting up keystrokes
#ifdef TARGET_OSX
        copyText = KeyStroke({OF_KEY_COMMAND, 'c'});
        cutText = KeyStroke({OF_KEY_COMMAND, 'x'});
        pasteText = KeyStroke({OF_KEY_COMMAND, 'v'});
        goLeft = KeyStroke({OF_KEY_COMMAND, OF_KEY_LEFT});
        goRight = KeyStroke({OF_KEY_COMMAND, OF_KEY_RIGHT});
        selectAll = KeyStroke({OF_KEY_COMMAND, 'a'});
#endif
        
#ifdef TARGET_WIN32
        copyText = KeyStroke({ OF_KEY_CONTROL, 'c' });
        cutText = KeyStroke({ OF_KEY_CONTROL, 'x' });
        pasteText = KeyStroke({ OF_KEY_CONTROL, 'v' });
		goLeft = KeyStroke({ OF_KEY_CONTROL, OF_KEY_LEFT });
		goRight = KeyStroke({ OF_KEY_CONTROL, OF_KEY_RIGHT });
		shiftLeft = KeyStroke({ OF_KEY_SHIFT, OF_KEY_LEFT });
		shiftRight = KeyStroke({ OF_KEY_SHIFT, OF_KEY_RIGHT });
		selectAll = KeyStroke({ OF_KEY_CONTROL, 'a' });
#endif
        }

        //
        
    MURKA_VIEW_DRAW_FUNCTION  {

        auto params = (Parameters*)parametersObject;
        TextField* thisWidget = (TextField*)thisWidgetObject;
        
        bool inside = context.isHovered() * !areChildrenHovered(context);
        
        std::string* stringData;
        float* floatData;
        double* doubleData;
        int* intData;
        
        // Handling "wants clicks"
        if (!params->activateByDoubleClickOnly) {
            thisWantsClicks = true;
        } else {
            thisWantsClicks = activated;
        }
        
        if (!activated) { // while not activated
            updateInternalRepresenation(dataToControl, params->precision, params->clampNumber, params->minNumber, params->maxNumber);
        }

        params->useCustomFont ? font = params->customFont : font = context.getParagraphFont();
        
        bool doubleClick = false;
        
        // activating if always selected
        
        if (params->alwaysActivated) {
            activated = true;
            updateExternalData(dataToControl, params->clampNumber);
        }

        // activation & deactivation & doubleclick
        if (context.mouseDownPressed[0]) {
            if (inside && !activated) {
                if (params->activateByDoubleClickOnly) {
                    activated = context.doubleClick;
                } else { // activate by any click
                    activated = true;
                }
                
                if (activated) context.claimKeyboardFocus(this);
                
                updateInternalRepresenation(dataToControl, params->precision, params->clampNumber, params->minNumber, params->maxNumber);
                
            }
            
            if (((ofGetElapsedTimef() - lastLeftMousebuttonClicktime) < 0.2) && (activated)) {
                doubleClick = true;
            }

            lastLeftMousebuttonClicktime = ofGetElapsedTimef();
        }

#ifdef MURKA_OF
        MurkaColor c = context.getWidgetForegroundColor();
        
        if (params->drawBounds) {
            context.renderer->pushStyle();
            context.renderer->enableFill();
            context.renderer->setColor(params->widgetBgColor);
            context.renderer->drawRectangle(0, 0, context.getSize().x, context.getSize().y);
            
            context.renderer->disableFill();
            context.renderer->setColor(inside ? params->widgetFgColor : params->widgetFgColor / 2);
            if (activated) context.renderer->setColor(params->widgetFgColor * 1.2);
            context.renderer->drawRectangle(1, 1, context.getSize().x-2, context.getSize().y-2);
            context.renderer->popStyle();
        }
        
        if (isSelectingTextNow()) {

            auto selectionShape = returnSelectionVisualShape();
            MurkaColor selectionColor = (context.getWidgetForegroundColor() * 0.5 +
                                         context.getWidgetBackgroundColor() * 0.5) * 255;
            context.renderer->setColor(selectionColor, 200);
            context.renderer->drawRectangle(10 - cameraPanInsideWidget + selectionShape.x(), 4, selectionShape.width(), context.getSize().y - 8);
        }

        
        recalcGlyphLengths(displayString, &context);
        
        float glyphXCoordinate = 10;
        context.renderer->setColor(params->widgetFgColor);
        font->drawString(displayString, 10 - cameraPanInsideWidget, context.getSize().y / 2  + font->getLineHeight() / 4);
        
        if (displayString.size() == 0) {
            // drawing hint
            MurkaColor hintColor = context.getWidgetForegroundColor() * 0.5 +
                                   context.getWidgetBackgroundColor() * 0.5;
            context.renderer->setColor(hintColor * 255);
            font->drawString(params->hint, 10, context.getSize().y / 2  + font->getLineHeight() / 4);
        }
        
        textHeight = context.getSize().y; // this is cache for text selection rect retrieavl
        
        bool didSetCursorPosition = false;
        float cursorPositionInPixels = 0;
        float sumGlyphWidths;
        for (int i = 0; i < displayString.size(); i++) {
            
            MurkaShape glyphShape = MurkaShape(glyphXCoordinate - cameraPanInsideWidget, 0, currentGlyphLengths[i], context.getSize().y);
            
            bool insideGlyph = glyphShape.inside(context.mousePosition);
            
            /*
            context.renderer->setColor(insideGlyph ? fgColor / 2 : fgColor / 4, 100);
            context.renderer->drawRectangle(glyphXCoordinate - cameraPanInsideWidget, 0, currentGlyphLengths[i], 30);
             */
            
            MurkaShape currentSymbolShape = {glyphXCoordinate, 0, currentGlyphLengths[i], context.getSize().y};
            
            bool safeToUseMouseClickEventsCauseEnoughTimeSinceDoubleClickPassed = ((ofGetElapsedTimef() - lastLeftMousebuttonClicktime) > 0.2);
            
            // Setting cursor position inside the string if pressed inside it
            if ((insideGlyph) && (context.mouseDownPressed[0])) {
                if (((context.mousePosition.x - glyphXCoordinate) / currentGlyphLengths[i]) < 0.5) {
                    cursorPosition = i;
                    didSetCursorPosition = true;
                    updateTextSelectionFirst(i);
                } else {
                    cursorPosition = i + 1;
                    didSetCursorPosition = true;
                    updateTextSelectionFirst(i + 1);
                }
            }
            
            // Moving text selection if mouse was already pressed, and moving the cursror too
            if ((insideGlyph) && (context.mouseDown[0]) && (!context.mouseDownPressed[0])
                && (safeToUseMouseClickEventsCauseEnoughTimeSinceDoubleClickPassed)) {
                if (((context.mousePosition.x - glyphXCoordinate) / currentGlyphLengths[i]) < 0.5) {
                    cursorPosition = i;
                    didSetCursorPosition = true;
                    updateTextSelectionSecond(i);
                    if (doubleClick) {
                        ofLog() << "updating second even tho its a doubleclick";
                    }
                } else {
                    cursorPosition = i + 1;
                    didSetCursorPosition = true;
                    updateTextSelectionSecond(i + 1);
                    if (doubleClick) {
                        ofLog() << "updating second even tho its a doubleclick";
                    }
                }
            }
            
            if (cursorPosition == i) cursorPositionInPixels = glyphXCoordinate;
            glyphXCoordinate += currentGlyphLengths[i];
        }
        sumGlyphWidths = glyphXCoordinate;
        
        
        // Setting cursor position to the end of the string if mouse pressed outside of it
        if ((inside) && (context.mouseDownPressed[0]) && (!didSetCursorPosition)) {
            cursorPosition = displayString.size();
        }
        
        if (cursorPosition == displayString.size()) {
            cursorPositionInPixels = sumGlyphWidths; // this is the full line width
        }
        
        if (activated) {
            // Moving the internal window so the cursor is always visible
            if (cursorPositionInPixels < cameraPanInsideWidget) {
                cameraPanInsideWidget = cursorPositionInPixels - 5;
            }
            
            if (cursorPositionInPixels > (context.getSize().x + cameraPanInsideWidget)) {
                cameraPanInsideWidget = cursorPositionInPixels - context.getSize().x + 5;
            }
        }

        // drawing cursor
        if (activated && !isSelectingTextNow()) {
            float timeMod = context.getRunningTime() / 1.0 - int(context.getRunningTime() / 1.0);
            context.renderer->setColor(200);
            if (timeMod > 0.5)
                context.renderer->drawLine(cursorPositionInPixels - cameraPanInsideWidget, 3,
                           cursorPositionInPixels - cameraPanInsideWidget, 30);
        }
        
#endif
        
        
        bool enterPressed = false;
        
        // Text editing logic
        if (activated) { // remember that drawing occurs even if its not activated

            if (doubleClick) {
                updateTextSelectionFirst(0);
                updateTextSelectionSecond(displayString.length());
                updateTextSelectionRange();
            }

            // Keystrokes support
            
            if ((copyText.isPressed()) && (activated) && (isSelectingTextNow())) {
                ofLog() << "copytext!!";
                
                auto substr = displayString.substr(selectionSymbolsRange.first, selectionSymbolsRange.second - selectionSymbolsRange.first);
                
                ofSetClipboardString(substr);

                copyText.fire();
            } else
            if ((cutText.isPressed()) && (activated) && (isSelectingTextNow())) {
                ofLog() << "cutText!!";
                
                if (isSelectingTextNow()) { // if we select now, backspace just deletes
                    auto substr = displayString.substr(selectionSymbolsRange.first, selectionSymbolsRange.second - selectionSymbolsRange.first);
                    
                    ofSetClipboardString(substr);
                    
                    displayString.replace(selectionSymbolsRange.first, selectionSymbolsRange.second - selectionSymbolsRange.first, "");
                    cursorPosition = selectionSymbolsRange.first;
                    updateTextSelectionFirst(cursorPosition);
                    updateTextSelectionSecond(cursorPosition);
                }
                
                cutText.fire();
            } else
            if ((pasteText.isPressed()) && (activated)) {
                ofLog() << "pasteText!!";
                
                if (isSelectingTextNow()) { // if we select now, it also replaces the selected text
                    displayString.replace(selectionSymbolsRange.first, selectionSymbolsRange.second - selectionSymbolsRange.first, "");
                    cursorPosition = selectionSymbolsRange.first;
                }
                
                displayString.insert(cursorPosition, ofGetClipboardString());
                cursorPosition += ofGetClipboardString().length();
                
                pasteText.fire();
            } else
            if ((goLeft.isPressed()) && (activated)) {
                ofLog() << "goLeft!!";
                
                cursorPosition = 0;
                
                if (!shiftLeft.isPressed()) {
                    updateTextSelectionFirst(cursorPosition);
                    updateTextSelectionSecond(cursorPosition);
                } else { // shift pressed, so we enlarge the selected text shape
                    if (selectionSymbolsRange.first == selectionSymbol1Index) {
                        ofLog() << "updating first to left...";
                        auto second = selectionSymbolsRange.second;
                        updateTextSelectionFirst(cursorPosition);
                        updateTextSelectionSecond(second);
                    } else {
                        ofLog() << "updating second to left...";
                        updateTextSelectionSecond(cursorPosition);
                    }
                }
                

                goLeft.fire();
            } else
            if ((goRight.isPressed()) && (activated)) {
                ofLog() << "goRight!!";

                cursorPosition = displayString.size();

                if (!shiftRight.isPressed()) {
                    updateTextSelectionFirst(cursorPosition);
                    updateTextSelectionSecond(cursorPosition);
                } else {
                    if (selectionSymbolsRange.second == selectionSymbol1Index) {
                        auto first = selectionSymbolsRange.first;
                        updateTextSelectionFirst(first);
                        updateTextSelectionSecond(cursorPosition);
                    } else {
                        updateTextSelectionSecond(cursorPosition);
                    }
                }
                    
                goRight.fire();
            } else
            if ((selectAll.isPressed()) && (activated)) {
                ofLog() << "selectAll!!";
                
                updateTextSelectionFirst(0);
                updateTextSelectionSecond(displayString.length());
                
                selectAll.fire();
            } else
            if (context.keyPresses.size() != 0) {
                
                for (auto key: context.keyPresses) {
//                        ofLog() << key;
                    
                    if (key == OF_KEY_RETURN) { // enter
                        enterPressed = true;
                    }
                    else if (key == OF_KEY_BACKSPACE) { // backspace
						if (isSelectingTextNow()) { // if we select now, backspace just deletes
							displayString.replace(selectionSymbolsRange.first, selectionSymbolsRange.second - selectionSymbolsRange.first, "");
							cursorPosition = selectionSymbolsRange.first;
							updateTextSelectionFirst(cursorPosition);
							updateTextSelectionSecond(cursorPosition);
						}
						else {
							if (cursorPosition > 0) {
								displayString.erase(cursorPosition - 1, 1);
								cursorPosition -= 1;
							}
						}
					}
					else if (key == OF_KEY_DEL) { 
						if (isSelectingTextNow()) { // if we select now, backspace just deletes
							displayString.replace(selectionSymbolsRange.first, selectionSymbolsRange.second - selectionSymbolsRange.first, "");
							cursorPosition = selectionSymbolsRange.first;
							updateTextSelectionFirst(cursorPosition);
							updateTextSelectionSecond(cursorPosition);
						}
						else {
							if (cursorPosition < displayString.size()) {
								displayString.erase(cursorPosition, 1);
							}
						}
					}
					else if (key == OF_KEY_LEFT) { // left
                        if (cursorPosition > 0) {
                            if (isSelectingTextNow()) {
                                if (cursorPosition != selectionSymbolsRange.first) {
                                    cursorPosition = selectionSymbolsRange.first;
                                } else {
                                    cursorPosition --;
                                }
                            } else {
                                cursorPosition --;
                            }
                            
                            if (!shiftLeft.isPressed()) {
                                // pressing left collapses the text selection if shift isn't pressed
                                updateTextSelectionFirst(cursorPosition);
                                updateTextSelectionSecond(cursorPosition);
                                updateTextSelectionRange();
                            } else {
                                updateTextSelectionRangeToIncludeCursor();
                            }
                        }
                        
                    }
                    else if (key == OF_KEY_RIGHT) { // right
                        if (cursorPosition < displayString.size()) {
                            if (isSelectingTextNow()) {
                                if (cursorPosition != selectionSymbolsRange.second) {
                                    cursorPosition = selectionSymbolsRange.second;
                                } else {
                                    cursorPosition ++;
                                }
                            } else {
                                cursorPosition ++;
                            }

                            
                            if (!shiftRight.isPressed()) {
                                // pressing left collapses the text selection if shift isn't pressed
                                updateTextSelectionFirst(cursorPosition);
                                updateTextSelectionSecond(cursorPosition);
                                updateTextSelectionRange();
                            } else {
                                updateTextSelectionRangeToIncludeCursor();
                            }
                        }
                        
                    }
					else if (key >= 32 && key <= 255) { // symbol keys

						ofLog() << "pressed symbol key " << key;
						if (isSelectingTextNow()) {
							ofLog() << "   .. and selecting text now ..";
							displayString.replace(selectionSymbolsRange.first, selectionSymbolsRange.second - selectionSymbolsRange.first, "");
							cursorPosition = selectionSymbolsRange.first;
						}

						displayString.insert(displayString.begin() + cursorPosition, char(key));

						cursorPosition += 1;

						updateTextSelectionFirst(cursorPosition);
						updateTextSelectionSecond(cursorPosition);
					}
                }
            }
        }
        
        // Deactivating the widget and updating the data
        
        if ((activated) && (!params->alwaysActivated))
        if ((enterPressed) || (context.mouseDownPressed[0] && !inside)) {
            activated = false;
            cameraPanInsideWidget = 0;
            
            context.resetKeyboardFocus(this);
            
            updateExternalData(dataToControl, params->clampNumber);
            
            *(bool*)resultObject = true;
        }
        
//            drawWidget<Label>(context, {""});
    };
    
    void updateExternalData(void* dataToControl, bool clamp = false) {
//            ofLog() << "datatype: " << dataTypeName;
//            ofLog() << "stringtype: " << typeid(std::string*).name();
            if (dataTypeName == typeid(std::string*).name()) {
                *((std::string*)dataToControl) = displayString;
            }
            if (dataTypeName == typeid(float*).name()) {
                float* floatData = ((float*)dataToControl);
#ifdef MURKA_OF
                *floatData = ofToFloat(displayString);
#endif
                ofLog() << "setting it to " << *floatData;
            }
            if (dataTypeName == typeid(double*).name()) {
                double* doubleData = ((double*)dataToControl);
#ifdef MURKA_OF
                *doubleData = ofToDouble(displayString);
#endif
            }
            if (dataTypeName == typeid(int*).name()) {
                int* intData = ((int*)dataToControl);
#ifdef MURKA_OF
                *intData = ofToInt(displayString);
#endif
            }
    }
    
    void updateInternalRepresenation(void* dataToControl, int precision, bool clamp = false, double min = 0, double max = 0) {

        bool isItStringData = false;
        
        // Updating the internal representation now
        if (dataTypeName == typeid(std::string*).name()) {
            std::string* stringData = ((std::string*)dataToControl);
            std::string xxx = *stringData;
            displayString = xxx;
            
            isItStringData = true;
        }
        
        if (dataTypeName == typeid(float*).name()) {
            float* floatData = ((float*)dataToControl);
#ifdef MURKA_OF
            displayString = ofToString(*floatData, precision);
            if (clamp) {
                if (*floatData < min) {
                    displayString = ofToString(min, precision);
                }
                if (*floatData > max) {
                    displayString = ofToString(max, precision);
                }
            }
#endif
        }
        if (dataTypeName == typeid(double*).name()) {
            double* doubleData = ((double*)dataToControl);
#ifdef MURKA_OF
            displayString = ofToString(*doubleData);
            if (clamp) {
                if (*doubleData < min) {
                    displayString = ofToString(min, precision);
                }
                if (*doubleData > max) {
                    displayString = ofToString(max, precision);
                }
            }
#endif
        }
        if (dataTypeName == typeid(int*).name()) {
            int* intData = ((int*)dataToControl);
#ifdef MURKA_OF
            displayString = ofToString(*intData);
            if (clamp) {
                if (*intData < min) {
                    displayString = ofToString(int(min));
                }
                if (*intData > max) {
                    displayString = ofToString(int(max));
                }
            }
#endif
        }
        
        if (!isItStringData) {
            if (displayString.find('.') != std::string::npos) {
                displayString.erase ( displayString.find_last_not_of('0') + 1, std::string::npos );
                if (displayString[displayString.length() - 1] == '.') {
                    displayString.erase(displayString.length() - 1, 1);
                }
            }
        }
    }
    
    struct Parameters {
        bool useCustomFont = false;
        
        FontObject* customFont;
        
        bool drawBounds = true;
        
        bool clampNumber = false;
        double minNumber = 0, maxNumber = 0;
        int precision = -1;
        
        bool activateByDoubleClickOnly = false;
        bool alwaysActivated = false;
        
        std::string hint = "";
        
        MurkaColor widgetFgColor = {0.98, 0.98, 0.98};
        MurkaColor widgetBgColor = {0.1, 0.1, 0.1};

        
        Parameters() {
        }
        
        Parameters(int _precision, double _min, double _max) {
            minNumber = _min;
            maxNumber = _max;
            clampNumber = true;
            precision = _precision;
        }
    };
    
    FontObject* font;
    
    // The results type, you also need to define it even if it's nothing.
    typedef bool Results;
    
    // Everything else is for handling the internal state
    
    float cameraPanInsideWidget = 0;

    void recalcGlyphLengths(std::string text, const MurkaContext* context) {
        if (currentGlyphLengths.size() < text.size()) {
            currentGlyphLengths.resize(text.size());
        }
        
        auto symbolsBoundingBoxes = font->getStringSymbolsBoundingBoxes(text, 0, 0, true);

        for (int i = 0; i < text.size(); i++) {
            currentGlyphLengths[i] = symbolsBoundingBoxes[i].width;
        }
    }
    
    // Text selection helpers
    
    void updateTextSelectionRangeToIncludeCursor() {
        if (cursorPosition < selectionSymbolsRange.first) {
            if (selectionSymbolsRange.first == selectionSymbol1Index) {
                selectionSymbol1Index = cursorPosition;
                updateTextSelectionRange();
            } else {
                selectionSymbol2Index = cursorPosition;
                updateTextSelectionRange();
            }
        }
        if (cursorPosition > selectionSymbolsRange.second) {
            if (selectionSymbolsRange.second == selectionSymbol1Index) {
                selectionSymbol1Index = cursorPosition;
                updateTextSelectionRange();
            } else {
                selectionSymbol2Index = cursorPosition;
                updateTextSelectionRange();
            }
        }

    }
    
    void updateTextSelectionFirst(int symbol1Index ) {
        selectionSymbol1Index = symbol1Index;
        updateTextSelectionSecond(symbol1Index);
        updateTextSelectionRange();
    }
    
    void updateTextSelectionSecond(int symbol2Index) {
        selectionSymbol2Index = symbol2Index;
        updateTextSelectionRange();
    }
    
    void updateTextSelectionRange() {
        selectionSymbolsRange = {min(selectionSymbol1Index, selectionSymbol2Index),
                                 max(selectionSymbol1Index, selectionSymbol2Index)};
        
    }
    
    MurkaShape returnSelectionVisualShape() {
        
        MurkaShape selectionFirstSymbolShape = {0, 0, 0, 0};
        MurkaShape selectionLastSymbolShape = {0, 0, 0, 0};

//        ofLog() << "bounds!!!";
        
        float x = 0;
        for (int i = 0; i < currentGlyphLengths.size(); i++) {
            if (i == selectionSymbolsRange.first) {
                selectionFirstSymbolShape = {x, 0, currentGlyphLengths[i], textHeight};
            }
            if ((i + 1) == selectionSymbolsRange.second) {
                selectionLastSymbolShape = {x, 0, currentGlyphLengths[i], textHeight};
            }
            x += currentGlyphLengths[i];
        }
        
        
        if (selectionFirstSymbolShape.x() < selectionLastSymbolShape.x()) {
            return {selectionFirstSymbolShape.x(), 0, selectionLastSymbolShape.x() + selectionLastSymbolShape.width() - selectionFirstSymbolShape.x(), 20};
        } else {
            return {selectionLastSymbolShape.x(), 0, selectionFirstSymbolShape.x() + selectionFirstSymbolShape.width() - selectionLastSymbolShape.x(), 20};
        }
    }
    
    bool thisWantsClicks = true;
    
    virtual bool wantsClicks() override { // this will signal the parent widget's "inside" function to allow to drag the parent widget around or do something else unless this one is activated; but we allow it to activate
        
        return thisWantsClicks;
        
    }

    
    bool isSelectingTextNow() {
        return (activated && (selectionSymbolsRange.first != selectionSymbolsRange.second));
    }
    
    int selectionSymbol1Index = 0, selectionSymbol2Index = 0; // 2 selection symbols, the first is where selection was started, the second is where it is "now"
    pair<int, int> selectionSymbolsRange = {0, 0}; // Range of symbols is calculated from symbols 1 & 2, the lesser of those becomes the beginning of the range, the bigger becomes the end
    
    std::string displayString;
    
    std::vector<float> currentGlyphLengths;
    int cursorPosition = 0;
    bool activated = false;
    
    double lastLeftMousebuttonClicktime = 0;
    
    float textHeight;
};

}
