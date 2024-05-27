#pragma once

#include "MurkaView.h"
#include <chrono>

namespace murka {

class TextField : public murka::View<TextField> {
    
    class KeyStroke {
        std::vector<int> keys;
        
        bool fired = false;
        
		std::chrono::steady_clock::time_point timeItFired;
    public:
        
        operator std::vector<int>() { return keys; };
        
        KeyStroke(const std::initializer_list<int> &il) {
            for (auto x: il) { keys.push_back(x); }
        }
        
        KeyStroke() { }
        
        bool isPressed(MurkaViewEventsContainer m) {
            if (fired) {
                bool shouldResetFired = false;
                for (auto &i: keys) { if (m.isKeyHeld(i)) shouldResetFired = true; }
                if (shouldResetFired) {
                    fired = false;
                }
                return false;
            }
    
			bool result = true;

            for (auto &i: keys) { if (!m.isKeyHeld(i)) result = false; }
            

            if ((std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - timeItFired).count() / 1000.0) < 0.5) {
                result = false; // too little time since it was last pressed
            }

            return result;
        }
        
        void fire() {
            fired = true;
            
			timeItFired = std::chrono::steady_clock::now();
        }
    };
    
    KeyStroke copyText, cutText, pasteText, goLeft, goRight, shiftLeft, shiftRight, selectAll;
    
public:
	TextField() {
		lastLeftMousebuttonClicktime = 0.0;

		// Setting up keystrokes
		copyText = KeyStroke({ MURKA_KEY_COMMAND, 'c' });
		cutText = KeyStroke({ MURKA_KEY_COMMAND, 'x' });
		pasteText = KeyStroke({ MURKA_KEY_COMMAND, 'v' });
		goLeft = KeyStroke({ MURKA_KEY_COMMAND, MURKA_KEY_LEFT });
		goRight = KeyStroke({ MURKA_KEY_COMMAND, MURKA_KEY_RIGHT });
		shiftLeft = KeyStroke({ MURKA_KEY_SHIFT, MURKA_KEY_LEFT });
		shiftRight = KeyStroke({ MURKA_KEY_SHIFT, MURKA_KEY_RIGHT });
		selectAll = KeyStroke({ MURKA_KEY_COMMAND, 'a' });
	}
    
    void internalDraw(Murka & m) {

        bool isInside = inside();

        std::string* stringData;
        float* floatData;
        double* doubleData;
        int* intData;
        
		boundReached = false;

        // Handling "wants clicks"
        if (!activateByDoubleClickOnly) {
            thisWantsClicks = true;
        } else {
            thisWantsClicks = activated;
        }
        
        if (!activated) { // while not activated
            updateInternalRepresenation(dataToControl, precision, clampNumber, minNumber, maxNumber);
        }

        useCustomFont ? font = customFont : font = m.getCurrentFont();
        
        bool doubleClicked = false;
        
        if (shouldForceEditorToSelectAll) {
            updateTextSelectionFirst(0);
            updateTextSelectionSecond(displayString.length());
            lastLeftMousebuttonClicktime = m.getElapsedTime();

			shouldForceEditorToSelectAll = false;
        }
        
        // grabbing focus is asked to explicitly
        
        if (shouldGrabFocusThisFrame) {
            claimKeyboardFocus();
        }
        
        // activating if always selected
        
        if (alwaysActivated || forceActivate) {
            activated = true;
            updateExternalData(dataToControl, clampNumber);
            claimKeyboardFocus();
			forceActivate = false;
        }

		bool forceSelectAll = false;

        // activation & deactivation & doubleclick
        if (mouseDownPressed(0)) {
            if (isInside && !activated) {

				if (shouldSelectAllWhenClicked) {
					forceSelectAll = true;
				}

                if (activateByDoubleClickOnly) {
                    activated = doubleClick();
                } else { // activate by any click
                    activated = true;
                }
                
                if (activated) claimKeyboardFocus();
                
                updateInternalRepresenation(dataToControl, precision, clampNumber, minNumber, maxNumber);
            }
            
            if (((m.getElapsedTime() - lastLeftMousebuttonClicktime) < 0.2) && (activated)) {
                doubleClicked = true;
            }

            lastLeftMousebuttonClicktime = m.getElapsedTime();
        }

        MurkaColor c = m.getColor();
        
        if (drawBounds) {
            m.pushStyle();
            m.enableFill();
            m.setColor(widgetBgColor);
            m.drawRectangle(0, 0, getSize().x, getSize().y);
            
            m.disableFill();
            m.setColor(isInside ? widgetFgColor : widgetFgColor / 2);
            if (activated) m.setColor(widgetFgColor * 1.2);
            m.drawRectangle(0, 0, getSize().x, getSize().y);
            m.popStyle();
        }
        
        if (isSelectingTextNow()) {

            auto selectionShape = returnSelectionVisualShape();
            MurkaColor selectionColor = (m.getColor() * 0.7 +
                                         m.getColor() * 0.7) * 255;
            m.setColor(100, 100, 100, 200);
            m.drawRectangle(10 - cameraPanInsideWidget + selectionShape.x(), 4, selectionShape.width(), getSize().y - 8);
        }
        
        recalcGlyphLengths(displayString);
        
        float glyphXCoordinate = 10;
        m.setColor(widgetFgColor);
        font->drawString(displayString, 10 - cameraPanInsideWidget, getSize().y / 2 - font->getLineHeight() / 2);
        
        if (displayString.size() == 0) {
            // drawing hint
            MurkaColor hintColor = m.getColor() * 0.5 +
                                   m.getColor() * 0.5;
            m.setColor(hintColor * 255);
            font->drawString(hint, 10, getSize().y / 2 - font->getLineHeight() / 2);
        }
        
        textHeight = getSize().y; // this is cache for text selection rect retrieavl
        
        bool didSetCursorPosition = false;
        float cursorPositionInPixels = 0;
        float sumGlyphWidths;
        for (int i = 0; i < displayString.size(); i++) {
            
            MurkaShape glyphShape = MurkaShape(glyphXCoordinate - cameraPanInsideWidget, 0, currentGlyphLengths[i], getSize().y);
            
            bool insideGlyph = glyphShape.inside(mousePosition());
            
            /*
			ctx.renderer->disableFill();
			ctx.renderer->setColor(insideGlyph ? params->widgetFgColor / 2 : params->widgetFgColor / 4, 100);
			ctx.renderer->drawRectangle(glyphXCoordinate - cameraPanInsideWidget, 0, currentGlyphLengths[i], 30);
			ctx.renderer->enableFill();
			*/
            
            MurkaShape currentSymbolShape = {glyphXCoordinate, 0, currentGlyphLengths[i], getSize().y};
            
            bool safeToUseMouseClickEventsCauseEnoughTimeSinceDoubleClickPassed = ((m.getElapsedTime() - lastLeftMousebuttonClicktime) > 0.2);
            
            // Setting cursor position inside the string if pressed inside it
            if ((insideGlyph) && (mouseDownPressed(0))) {
                if (((mousePosition().x - glyphXCoordinate) / currentGlyphLengths[i]) < 0.5) {
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
            
            if ((insideGlyph) && (mouseDown(0)) && (!mouseDownPressed(0))
                && (safeToUseMouseClickEventsCauseEnoughTimeSinceDoubleClickPassed)) {
                if (((mousePosition().x - glyphXCoordinate) / currentGlyphLengths[i]) < 0.5) {
                    cursorPosition = i;
                    didSetCursorPosition = true;
                    updateTextSelectionSecond(i);
                    if (doubleClicked) {
#ifdef MURKA_DEBUG
                        std::cout << "updating second even tho its a doubleclick" << std::endl;
#endif
                    }
                } else {
                    cursorPosition = i + 1;
                    didSetCursorPosition = true;
                    updateTextSelectionSecond(i + 1);
                    if (doubleClicked) {
#ifdef MURKA_DEBUG
                        std::cout << "updating second even tho its a doubleclick" << std::endl;
#endif
                    }
                }
            }
            
            if (cursorPosition == i) cursorPositionInPixels = glyphXCoordinate;
            glyphXCoordinate += currentGlyphLengths[i];
        }
        sumGlyphWidths = glyphXCoordinate;
        
        
        // Setting cursor position to the end of the string if mouse pressed outside of it
        if ((isInside) && (mouseDownPressed(0)) && (!didSetCursorPosition)) {
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
            
            if (cursorPositionInPixels > (getSize().x + cameraPanInsideWidget)) {
                cameraPanInsideWidget = cursorPositionInPixels - getSize().x + 5;
            }
        }

        // drawing cursor
        if (activated && !isSelectingTextNow()) {
            float timeMod = m.getElapsedTime() / 1.0 - int(m.getElapsedTime() / 1.0);
            m.setColor(200);
            if (timeMod > 0.5)
                m.drawLine(cursorPositionInPixels - cameraPanInsideWidget, 3,
                           cursorPositionInPixels - cameraPanInsideWidget, 30);
        }

        bool enterPressed = false;

        // Text editing logic
        if (activated) { // remember that drawing occurs even if its not activated

            if (doubleClicked || forceSelectAll) {
                updateTextSelectionFirst(0);
                updateTextSelectionSecond(displayString.length());
                updateTextSelectionRange();
            }

            // Keystrokes support
            
            if ((copyText.isPressed(*this)) && (activated) && (isSelectingTextNow())) {
#ifdef MURKA_DEBUG
                std::cout << "copytext!!" << std::endl;
#endif
                
                auto substr = displayString.substr(selectionSymbolsRange.first, selectionSymbolsRange.second - selectionSymbolsRange.first);
                
				m.setClipboardString(substr);

                copyText.fire();
            } else
            if ((cutText.isPressed(*this)) && (activated) && (isSelectingTextNow())) {
#ifdef MURKA_DEBUG
                std::cout << "cutText!!" << std::endl;
#endif
                
                if (isSelectingTextNow()) { // if we select now, backspace just deletes
                    auto substr = displayString.substr(selectionSymbolsRange.first, selectionSymbolsRange.second - selectionSymbolsRange.first);
                    
					m.setClipboardString(substr);
                    
                    displayString.replace(selectionSymbolsRange.first, selectionSymbolsRange.second - selectionSymbolsRange.first, "");
                    cursorPosition = selectionSymbolsRange.first;
                    updateTextSelectionFirst(cursorPosition);
                    updateTextSelectionSecond(cursorPosition);
                }
                
                cutText.fire();
            } else
            if ((pasteText.isPressed(*this)) && (activated)) {
#ifdef MURKA_DEBUG
                std::cout << "pasteText!!" << std::endl;
#endif
                
                if (isSelectingTextNow()) { // if we select now, it also replaces the selected text
                    displayString.replace(selectionSymbolsRange.first, selectionSymbolsRange.second - selectionSymbolsRange.first, "");
                    cursorPosition = selectionSymbolsRange.first;
                }
				
                displayString.insert(cursorPosition, m.getClipboardString());
                cursorPosition += m.getClipboardString().length();
                
                pasteText.fire();
            } else
            if ((goLeft.isPressed(*this)) && (activated)) {
#ifdef MURKA_DEBUG
                std::cout << "goLeft!!" << std::endl;
#endif
                
                cursorPosition = 0;
                
                if (!shiftLeft.isPressed(*this)) {
                    updateTextSelectionFirst(cursorPosition);
                    updateTextSelectionSecond(cursorPosition);
                } else { // shift pressed, so we enlarge the selected text shape
                    if (selectionSymbolsRange.first == selectionSymbol1Index) {
#ifdef MURKA_DEBUG
                        std::cout << "updating first to left..." << std::endl;
#endif
                        auto second = selectionSymbolsRange.second;
                        updateTextSelectionFirst(cursorPosition);
                        updateTextSelectionSecond(second);
                    } else {
#ifdef MURKA_DEBUG
                        std::cout << "updating second to left..." << std::endl;
#endif
                        updateTextSelectionSecond(cursorPosition);
                    }
                }
                

                goLeft.fire();
            } else
            if ((goRight.isPressed(*this)) && (activated)) {
#ifdef MURKA_DEBUG
                std::cout << "goRight!!" << std::endl;
#endif

                cursorPosition = displayString.size();

                if (!shiftRight.isPressed(*this)) {
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
            if ((selectAll.isPressed(*this)) && (activated)) {
#ifdef MURKA_DEBUG
                std::cout << "selectAll!!" << std::endl;
#endif
                
                updateTextSelectionFirst(0);
                updateTextSelectionSecond(displayString.length());
                
                selectAll.fire();
            } else
            if (keyReleases().size() != 0) {
                
                for (auto key: keyReleases()) {
#ifdef MURKA_DEBUG
                    std::cout << key << std::endl;
#endif
                    if (key == MURKA_KEY_RETURN) { // enter
                        enterPressed = true;
                    }
                }
            }
            else
            if (keyPresses().size() != 0) {
           
                for (auto key: keyPresses()) {
#ifdef MURKA_DEBUG
                    std::cout << key << std::endl;
#endif
                    
                    if (key == MURKA_KEY_RETURN) { // enter
                        enterPressed = true;
                    }
                    else if (key == MURKA_KEY_BACKSPACE) { // backspace
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
					else if (key == MURKA_KEY_DEL) {
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
					else if (key == MURKA_KEY_LEFT) { // left
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
                            
                            if (!shiftLeft.isPressed(*this)) {
                                // pressing left collapses the text selection if shift isn't pressed
                                updateTextSelectionFirst(cursorPosition);
                                updateTextSelectionSecond(cursorPosition);
                                updateTextSelectionRange();
                            } else {
                                updateTextSelectionRangeToIncludeCursor();
                            }
                        }
						else {
							boundReached = true;
						}
                        
                    }
                    else if (key == MURKA_KEY_RIGHT) { // right
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

                            
                            if (!shiftRight.isPressed(*this)) {
                                // pressing left collapses the text selection if shift isn't pressed
                                updateTextSelectionFirst(cursorPosition);
                                updateTextSelectionSecond(cursorPosition);
                                updateTextSelectionRange();
                            } else {
                                updateTextSelectionRangeToIncludeCursor();
                            }
                        }
						else {
							boundReached = true;
						}
                        
                    }
                    else if (key >= 45 && key <= 57) { // number keys, - and .
#ifdef MURKA_DEBUG
                        std::cout << "pressed numeric key " << key << std::endl;
#endif
                        if (isSelectingTextNow()) {
#ifdef MURKA_DEBUG
                            std::cout << "   .. and selecting text now .." << std::endl;
#endif
                            displayString.replace(selectionSymbolsRange.first, selectionSymbolsRange.second - selectionSymbolsRange.first, "");
                            cursorPosition = selectionSymbolsRange.first;
                        }

                        displayString.insert(displayString.begin() + cursorPosition, char(key));

                        cursorPosition += 1;

						if (cursorPosition >= fillWidth || displayString.size() >= fillWidth) {
							boundReached = true;
						}

                        updateTextSelectionFirst(cursorPosition);
                        updateTextSelectionSecond(cursorPosition);
                    }
					else if (!numbersOnly && key >= 32 && key <= 255) { // symbol keys
#ifdef MURKA_DEBUG
						std::cout << "pressed symbol key " << key << std::endl;
#endif
						if (isSelectingTextNow()) {
#ifdef MURKA_DEBUG
							std::cout << "   .. and selecting text now .." << std::endl;
#endif
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
        
        editingFinished = false;
        
        if ((activated) && (!alwaysActivated))
        if (enterPressed || (mouseDownPressed(0) && !isInside) || forceDeactivate) {
            activated = false;
            cameraPanInsideWidget = 0;
            
			if (hasKeyboardFocus()) {
				resetKeyboardFocus();
			}
            
            updateExternalData(dataToControl, clampNumber);
            
            editingFinished = true;

			forceDeactivate = false;
        }
		 

//            drawWidget<Label>(ctx, {""});
    };
    
    bool editingFinished = false;
    
    void updateExternalData(void* dataToControl, bool clamp = false) {
#ifdef MURKA_DEBUG
            std::cout << "datatype: " << dataTypeName << std::endl;
            std::cout << "stringtype: " << typeid(std::string*).name() << std::endl;
#endif
            if (dataTypeName == typeid(std::string*).name()) {
                *((std::string*)dataToControl) = displayString;
            }
            if (dataTypeName == typeid(float*).name()) {
                float* floatData = ((float*)dataToControl);

				*floatData = !displayString.empty() ? std::stof(displayString) : 0;
                
#ifdef MURKA_DEBUG
				std::cout << "setting it to " << *floatData << std::endl;
#endif
            }
            if (dataTypeName == typeid(double*).name()) {
                double* doubleData = ((double*)dataToControl);
                
				*doubleData = !displayString.empty() ? std::stod(displayString) : 0;
            }
            if (dataTypeName == typeid(int*).name()) {
                int* intData = ((int*)dataToControl);

                *intData = !displayString.empty() ? std::stoi(displayString) : 0;
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
			displayString = to_string_with_precision(*floatData, precision);
			if (clamp) {
				if (*floatData < min) {
					displayString = to_string_with_precision(min, precision);
				}
				if (*floatData > max) {
					displayString = to_string_with_precision(max, precision);
				}
			}
		}
		if (dataTypeName == typeid(double*).name()) {
			double* doubleData = ((double*)dataToControl);

			displayString = to_string_with_precision(*doubleData);
			if (clamp) {
				if (*doubleData < min) {
					displayString = to_string_with_precision(min, precision);
				}
				if (*doubleData > max) {
					displayString = to_string_with_precision(max, precision);
				}
			}
		}
		if (dataTypeName == typeid(int*).name()) {
			int* intData = ((int*)dataToControl);

			displayString = to_string_with_precision(*intData, precision, fillWidth);
			if (clamp) {
				if (*intData < min) {
					displayString = to_string_with_precision(int(min), precision, fillWidth);
				}
				if (*intData > max) {
					displayString = to_string_with_precision(int(max), precision, fillWidth);
				}
			}
		}

		if (!isItStringData) {
			if (displayString.find('.') != std::string::npos) {
				displayString.erase(displayString.find_last_not_of('0') + 1, std::string::npos);
				if (displayString[displayString.length() - 1] == '.') {
					displayString.erase(displayString.length() - 1, 1);
				}
			}
		}
    }
    
    void* dataToControl = nullptr;
    
//    struct Parameters {
    bool useCustomFont = false;
    
    FontObject* customFont;
    
    bool drawBounds = true;
    
    bool clampNumber = false;
    double minNumber = 0, maxNumber = 0;
//    int precision = -1;
    
    bool activateByDoubleClickOnly = false;
	bool alwaysActivated = false;

	bool boundReached = false;

	bool forceActivate = false;
	bool forceDeactivate = false;

	TextField & activate() {
		forceActivate = true;
		return *this;
	}

	TextField & deactivate() {
		forceDeactivate = true;
		return *this;
	}

    std::string hint = "";
    
    MurkaColor widgetFgColor = {0.98 * 255, 0.98 * 255, 0.98 * 255 };
    MurkaColor widgetBgColor = {0.1 * 255, 0.1 * 255, 0.1 * 255 };

//    bool numbersOnly = false;
    MURKA_PARAMETER(TextField, // class name
                    bool, // parameter type
                    numbersOnly, // parameter variable name
                    onlyAllowNumbers, // setter
                    false // default
    )

    MURKA_PARAMETER(TextField, // class name
        int, // parameter type
        fillWidth, // parameter variable name
        fillWidthWithZeroes, // setter
        0 // default
    )
    
    MURKA_PARAMETER(TextField, // class name
                    int, // parameter type
                    precision, // parameter variable name
                    withPrecision, // setter
                    -1 // default
    )

    MURKA_PARAMETER(TextField, // class name
                    bool, // parameter type
                    shouldForceEditorToSelectAll, // parameter variable name
                    forcingEditorToSelectAll, // setter
                    false // default
    )


//    bool shouldForceEditorToSelectAll = false;
        
	TextField & controlling(std::string* dataToControl_) {
		dataToControl = dataToControl_;
		dataTypeName = typeid(std::string*).name();

		return *this;
	}

	TextField & controlling(float* dataToControl_) {
		dataToControl = dataToControl_;
		dataTypeName = typeid(float*).name();

		return *this;
	}

	TextField & controlling(double* dataToControl_) {
        dataToControl = dataToControl_;
        dataTypeName = typeid(double*).name();

        return *this;
    }

    TextField & controlling(int* dataToControl_) {
        dataToControl = dataToControl_;
        dataTypeName = typeid(int*).name();

        return *this;
    }
    
    TextField & grabKeyboardFocus() {
        shouldGrabFocusThisFrame = true;
        
        return *this;
    }
    
    bool shouldGrabFocusThisFrame = false;

    
    FontObject* font;
    
    float cameraPanInsideWidget = 0;

    void recalcGlyphLengths(std::string text) {
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
        selectionSymbolsRange = {(std::min)(selectionSymbol1Index, selectionSymbol2Index),
                                 (std::max)(selectionSymbol1Index, selectionSymbol2Index)};
        
    }
    
    MurkaShape returnSelectionVisualShape() {
        
        MurkaShape selectionFirstSymbolShape = {0, 0, 0, 0};
        MurkaShape selectionLastSymbolShape = {0, 0, 0, 0};
        
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

    bool pastSelectingTextResult = false;
    
    bool isSelectingTextNow() {
        bool result = (activated && (selectionSymbolsRange.first != selectionSymbolsRange.second));
        
        if (pastSelectingTextResult != result) {
#ifdef MURKA_DEBUG
            std::cout << "ok switch!" << std::endl;
#endif
        }
        pastSelectingTextResult = result;
        
        return (activated && (selectionSymbolsRange.first != selectionSymbolsRange.second));
    }
    
    int selectionSymbol1Index = 0, selectionSymbol2Index = 0; // 2 selection symbols, the first is where selection was started, the second is where it is "now"
    std::pair<int, int> selectionSymbolsRange = {0, 0}; // Range of symbols is calculated from symbols 1 & 2, the lesser of those becomes the beginning of the range, the bigger becomes the end
    
    std::string displayString;
    
    std::vector<float> currentGlyphLengths;
    int cursorPosition = 0;
    bool activated = false;
    
	bool shouldSelectAllWhenClicked = false;

    double lastLeftMousebuttonClicktime = 0;
    
    float textHeight;
};
}
