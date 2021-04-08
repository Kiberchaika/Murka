#pragma once
#include "Murka.h"


namespace murka {


class Header : public MurkaViewInterface<Header> {
public:
    Header() {

    }

    MURKA_VIEW_DRAW_FUNCTION  {
        
        auto params = (Parameters*)parametersObject;
        Header* thisWidget = (Header*)thisWidgetObject;
        
        bool inside = context.isHovered() * !areChildrenHovered(context);
        
        // Your drawing and interaction code goes here.
        // Don't forget that all of this executes at each frame for each
        // widget that is drawn on screen.
        auto font = context.getCurrentFont();
        
        MurkaColor fgColor = params->customColor ? params->color : context.pointerToRenderer->getColor();
        context.pointerToRenderer->setColor(fgColor);
        if (params->alignment == TEXT_LEFT) {
            font->drawString(params->label, 5, 5);
        }
        if (params->alignment == TEXT_CENTER) {
            float textX = 5 + (shape.size.x - 10) / 2 - font->getStringBoundingBox(params->label, 0, 0).width / 2;
            font->drawString(params->label, textX, 5);
        }
        if (params->alignment == TEXT_RIGHT) {
            float textX = (shape.size.x - 10) - font->getStringBoundingBox(params->label, 0, 0).width;
            font->drawString(params->label, textX, 5);
        }
    };
    
    // Here go parameters and any parameter convenience constructors. You need to define something called Parameters, even if it's NULL.
    struct Parameters {
        std::string label;
        
        TextAlignment alignment = TEXT_LEFT;
        
        MurkaColor color = {0.98, 0.98, 0.98};
        
        bool customColor = false;
        
        Parameters() {}
        Parameters(std::string labelText) { label = labelText; } // a convenience initializer
        Parameters(std::string labelText, MurkaColor color_) {
            label = labelText;
            color = color_;
            customColor = true;
        } // a convenience initializer
        
        Parameters(std::string labelText, TextAlignment align) {
            label = labelText;
            alignment = align;
        } // a convenience initializer
        
        Parameters(std::string labelText, MurkaColor color_, TextAlignment align) {
            label = labelText;
            alignment = align;
            color = color_;
            customColor = true;
        } // a convenience initializer
    };
    
    // The results type, you also need to define it even if it's nothing.
    typedef bool Results;
    
    virtual bool wantsClicks() override { return false; } // override this if you want to signal that you don't want clicks

};

/*
class Label : public MurkaViewInterface<Label> {
public:
    MURKA_VIEW_DRAW_FUNCTION  {
        
        auto params = (Parameters*)parametersObject;
        
        bool inside = context.isHovered() * !areChildrenHovered(context);
        
        auto font = context.getCurrentFont();
        if (params->customFont) {
            font = params->font;
        }
        
		MurkaColor bgColor = params->backgroundColor;
        context.renderer->enableFill();
        if (bgColor.a != 0.0) {
            context.renderer->setColor(bgColor);
            if (params->alignment == TEXT_LEFT) {
                context.renderer->drawRectangle(0, 0, font->getStringBoundingBox(params->label, 0, 0).width + 10, context.getSize().y);
            }
        }
		MurkaColor fgColor = params->customColor ? params->color : context.renderer->getColor();
        context.renderer->setColor(fgColor);
        if (params->alignment == TEXT_LEFT) {
            font->drawString(params->label, 5, 0);
        }
        if (params->alignment == TEXT_CENTER) {
            float textX = 5 + (shape.size.x - 10) / 2 - font->getStringBoundingBox(params->label, 0, 0).width  / 2;
            font->drawString(params->label, textX, 0);
        }
        if (params->alignment == TEXT_RIGHT) {
            float textX = (shape.size.x - 10) - font->getStringBoundingBox(params->label, 0, 0).width;
            font->drawString(params->label, textX, 0);
        }
        
        // Testing vertical centering
            //context.renderer->setColor(255);
            //ofDrawLine(0, 0, context.getSize().x, 0);
            //context.renderer->setColor(255, 0, 0);
            //ofDrawLine(0, context.getSize().y / 2, context.getSize().x, context.getSize().y / 2);
            //context.renderer->setColor(255);
            //ofDrawLine(0, context.getSize().y, context.getSize().x, context.getSize().y);
    };
    
    // Here go parameters and any parameter convenience constructors. You need to define something called Parameters, even if it's NULL.
    struct Parameters {
        std::string label;
        TextAlignment alignment = TEXT_LEFT;
        
        MurkaColor color = {0.98, 0.98, 0.98};
		MurkaColor backgroundColor = {0., 0., 0., 0.};
        
        FontObject* font;
        
        bool customColor = false;
        bool customFont = false;
        
        Parameters() {}
        Parameters(std::string labelText) { label = labelText; } // a convenience initializer
        Parameters(std::string labelText, MurkaColor color_) {
            label = labelText;
            color = color_;
            customColor = true;
        } // a convenience initializer
        
		Parameters(std::string labelText, MurkaColor color_, MurkaColor backgroundColor_) {
			label = labelText;
			color = color_;
			customColor = true;
			backgroundColor = backgroundColor_;
		} // a convenience initializer

		Parameters(std::string labelText, MurkaColor color_, MurkaColor backgroundColor_, TextAlignment align) {
			label = labelText;
			color = color_;
			customColor = true;
			backgroundColor = backgroundColor_;
			alignment = align;
		} // a convenience initializer

		Parameters(std::string labelText, TextAlignment align) {
            label = labelText;
            alignment = align;
        } // a convenience initializer
        
        Parameters(std::string labelText, MurkaColor color_, TextAlignment align) {
            label = labelText;
            alignment = align;
            color = color_;
            customColor = true;
        } // a convenience initializer
        
        // Custom font initializers
        Parameters(std::string labelText, FontObject* CustomFont) {
            label = labelText;
            font = CustomFont;
            customFont = true;
        } // a convenience initializer
        Parameters(std::string labelText, FontObject* CustomFont, MurkaColor color_) {
            label = labelText;
            color = color_;
            customColor = true;
            font = CustomFont;
            customFont = true;
        } // a convenience initializer
        
        Parameters(std::string labelText, FontObject* CustomFont, MurkaColor color_, MurkaColor backgroundColor_) {
            label = labelText;
            color = color_;
            customColor = true;
            backgroundColor = backgroundColor_;
            font = CustomFont;
            customFont = true;
        } // a convenience initializer
        
        Parameters(std::string labelText, FontObject* CustomFont, TextAlignment align) {
            label = labelText;
            alignment = align;
            font = CustomFont;
            customFont = true;
        } // a convenience initializer
        
        Parameters(std::string labelText, FontObject* CustomFont, MurkaColor color_, TextAlignment align) {
            label = labelText;
            alignment = align;
            color = color_;
            customColor = true;
            font = CustomFont;
            customFont = true;
        } // a convenience initializer
    };
    
    // The results type, you also need to define it even if it's nothing.
    typedef bool Results;
    
    virtual bool wantsClicks() override { return false; } // override this if you want to signal that you don't want clicks

};
 */
    
class Checkbox : public MurkaViewInterface<Checkbox> {
public:
    MURKA_VIEW_DRAW_FUNCTION  {
        
        auto params = (Parameters*)parametersObject;
		auto &castedResults = *(castResults(resultObject));
	
		Checkbox* thisWidget = (Checkbox*)thisWidgetObject;
		
		bool inside = context.isHovered() * !areChildrenHovered(context);
        
        bool* booleanToControl = ((bool*)dataToControl);

		Results* results = (Results*)resultObject;
		*results = false;

		MurkaColor fgColor = context.pointerToRenderer->getColor();

        float pushed = 0.2 - (context.getRunningTime() - lastTimeClicked);
        if (pushed < 0) pushed = 0;
        pushed /= 0.2;

        context.pointerToRenderer->pushStyle();
        context.pointerToRenderer->setColor((inside ? fgColor : fgColor / 2) * (1.0 + 0.2 * pushed));

        context.pointerToRenderer->disableFill();
        context.pointerToRenderer->drawRectangle(5, 5, 20, 20);
        if (*booleanToControl) {
            float symbolOffset = 4;
            context.pointerToRenderer->drawLine(5 + symbolOffset, 5 + symbolOffset, 25 - symbolOffset, 25 - symbolOffset);
            context.pointerToRenderer->drawLine(25 - symbolOffset, 5 + symbolOffset, 5 + symbolOffset, 25 - symbolOffset);
        }
        
        context.pointerToRenderer->enableFill();
        
        auto font = context.getCurrentFont();
        font->drawString(params->label, 30, 0);
        
        if (inside && context.mouseDownPressed[0]) {
            *booleanToControl = !*booleanToControl;
            lastTimeClicked = context.getRunningTime();
			castedResults = booleanToControl;
			*results = true;
		}
		else castedResults = false;

        context.pointerToRenderer->popStyle();
    };
    
    
    // Here go parameters and any parameter convenience constructors. You need to define something called Parameters, even if it's NULL.
    struct Parameters {
        std::string label;
        
        Parameters() {}
        Parameters(std::string Label) { label = Label; } // a convenience initializer
    };
     /*
    
    int* dataToControl = NULL;
    Checkbox& controlData(int* data) {
        dataToControl = data;
        return *this;
    }

    std::function<void()> onChange = [](){};
    Checkbox& withOnChange(std::function<void()> callback) {
        onChange = callback;
        return *this;
    }

    
    MURKA_PARAMETER_SETTER(int*, dataToControl, controlData);
    MURKA_PARAMETER_SETTER(std::function<void()>, onChange, withOnChange);
*/
    
    /*
    class ParameterSetter {
        Checkbox* widget;
        Parameters(std::string Label) {
            widget->label = Label;
        } // a convenience initializer
    };
    ParameterSetter p;
     */

	// The results type, you also need to define it even if it's nothing.
	typedef bool Results;

	Results* castResults(void* results) {
		auto resultsPointer = (Results*)results;
		return resultsPointer;
	}

	Parameters& castParameters() {
		return  *((Parameters*)parametersInternal);
	}

   
    // The two functions needed for optional UI state saving. It's up to you
    // to use those.
//    std::vector<MurkaVar> saveInternalData(int indexX, int indexY) override { }
//    void loadInternalData(std::vector<MurkaVar>) override {}
    
    
    // Everything else in the class is for handling the internal state. It persistы.
    double lastTimeClicked = 0;
};


class Radiobutton : public MurkaViewInterface<Radiobutton> {
public:
	MURKA_VIEW_DRAW_FUNCTION{

		auto params = (Parameters*)parametersObject;
		Radiobutton* thisWidget = (Radiobutton*)thisWidgetObject;

		Results* results = (Results*)resultObject;
		*results = false;

		bool inside = context.isHovered() * !areChildrenHovered(context);

		int* intToControl = ((int*)dataToControl);

		MurkaColor fgColor = context.pointerToRenderer->getColor();

		float pushed = 0.2 - (context.getRunningTime() - lastTimeClicked);
		if (pushed < 0) pushed = 0;
		pushed /= 0.2;
            
        int lineHeight = 25;

		for (int i = 0; i < params->labels.size(); i++) {
			context.pointerToRenderer->pushMatrix();
			context.pointerToRenderer->pushStyle();
			context.pointerToRenderer->translate(0, lineHeight * i, 0);
			MurkaShape rowShape = { 0, lineHeight * i, context.getSize().x, lineHeight };
			bool rowHover = rowShape.inside(context.mousePosition);

			context.pointerToRenderer->setColor((rowHover * inside ? fgColor : fgColor / 2) * (1.0 + 0.2 * pushed));

			context.pointerToRenderer->disableFill();
			context.pointerToRenderer->drawCircle(5 + 20 / 2, 5 + 20 / 2, 10);
			if (*intToControl == i) {
				context.pointerToRenderer->enableFill();
				context.pointerToRenderer->drawCircle(5 + 20 / 2, 5 + 20 / 2, 6);
			}
			context.pointerToRenderer->enableFill();

			auto font = context.getCurrentFont();
			font->drawString(params->labels[i], 30, 10);

			if (rowHover * inside && context.mouseDownPressed[0]) {
				*intToControl = i;
				*results = true;
				lastTimeClicked = context.getRunningTime();
			}

			context.pointerToRenderer->popStyle();
			context.pointerToRenderer->popMatrix();
		}
	};

	// Here go parameters and any parameter convenience constructors. You need to define something called Parameters, even if it's NULL.
	struct Parameters {
		std::vector<std::string> labels;

		Parameters() {}
		Parameters(std::vector<std::string> Labels) { labels = Labels; } // a convenience initializer
	};

	// The results type, you also need to define it even if it's nothing.
	typedef bool Results;

	// The two functions needed for optional UI state saving. It's up to you
	// to use those.
//    std::vector<MurkaVar> saveInternalData(int indexX, int indexY) override { }
//    void loadInternalData(std::vector<MurkaVar>) override {}


	// Everything else in the class is for handling the internal state. It persistы.
	double lastTimeClicked = 0;
};



class DropdownElementButton : public MurkaViewInterface<DropdownElementButton> {
public:
	MURKA_VIEW_DRAW_FUNCTION{

		auto params = (Parameters*)parametersObject;
		DropdownElementButton* thisWidget = (DropdownElementButton*)thisWidgetObject;

		auto &castedResults = *(castResults(resultObject));

		bool inside = context.isHovered() * !areChildrenHovered(context);

		context.pointerToRenderer->setColor(MurkaColor(90 / 255.0, 90 / 255.0, 90 / 255.0) * (0.8 + 0.2 * inside));
		context.pointerToRenderer->drawRectangle(0, 0, context.getSize().x, context.getSize().y);

		if (justInitialised) {
			justInitialised = false;
			castedResults = false;
			return;
			// ^ ghetto way to prevent this button from activating right away
		}

		auto currentLastFrame = context.pointerToRenderer->getFrameNum();

		context.pointerToRenderer->setColor(255);
		context.pointerToRenderer->drawString(params->label, 10, 5);

		if ((currentLastFrame - lastFrameItWasRendered) <= 1) { // only allow clicking if it was rendered at last frame - to avoid instant clicks in overlays
			if ((context.mouseDownPressed[0]) && (inside)) {
				// click if it's not the same frame it was shown
				std::cout << "btn pressd. justInitialised? " << justInitialised;
				castedResults = true;

				return;
			}
		}

		lastFrameItWasRendered = currentLastFrame;

		castedResults = false;
		return;
	};



	struct Parameters {
		string label;

		Parameters() {}
		Parameters(string Label) {
			label = Label;
		}
	};

	uint64_t lastFrameItWasRendered = -1;

	// The results type, you also need to define it even if it's nothing.
	typedef bool Results;

	Results* castResults(void* results) {
		auto resultsPointer = (Results*)results;
		return resultsPointer;
	}

	bool justInitialised = true;

};


class DropdownButton : public MurkaViewInterface<DropdownButton> {
public:
	MURKA_VIEW_DRAW_FUNCTION{

		auto params = (Parameters*)parametersObject;
		DropdownButton* thisWidget = (DropdownButton*)thisWidgetObject;
		auto &castedResults = *(castResults(resultObject));

		bool inside = context.isHovered() * !areChildrenHovered(context);

		int* intToControl = ((int*)dataToControl);

		context.pointerToRenderer->setColor(MurkaColor(90 / 255.0, 90 / 255.0, 90 / 255.0) * (0.5 + 0.3 * inside));
		context.pointerToRenderer->drawRectangle(0, 0, context.getSize().x, context.getSize().y);

		if ((context.mouseDownPressed[0]) && (inside)) {
			if (!showingTheDropdown) {
				showingTheDropdown = true;
			}
		}

		if (!initialised) {
			initialised = true;
			selectedOption = params->selectedOption;
		}

		options = params->options;

		context.pointerToRenderer->setColor(MurkaColor(66 / 255.0, 67 / 255.0, 71 / 255.0) * 3.2);
		context.pointerToRenderer->drawString(options[selectedOption], 5, 5);

		if (showingTheDropdown) {
			contextPosition = context.currentViewShape.position;
			context.addOverlay([&]() {
				//                         ofLog() << context.currentViewShape.position.x;
				for (int i = 0; i < options.size(); i++) {
					std::string buttonLabel = options[i];
					if (
						drawWidget<DropdownElementButton>(context, {buttonLabel.c_str()}, {contextPosition.x, contextPosition.y + 30 * i, 150, 30})) {
						std::cout << i << " pressed";
						showingTheDropdown = false;
						changedSelection = true;
						selectedOption = i;
					}
				}
			}, thisWidgetObject);

		}

		castedResults = changedSelection;
		if (changedSelection) {
			*intToControl = selectedOption;
			changedSelection = false;
		}
	};

	struct Parameters {
		vector<string> options;
		int selectedOption;

		Parameters() {}
		Parameters(vector<string> Options, int SelectedOption) {
			options = Options;
			selectedOption = SelectedOption;
		}
	};

	// The results type, you also need to define it even if it's nothing.
	typedef bool Results;

	Results* castResults(void* results) {
		auto resultsPointer = (Results*)results;
		return resultsPointer;
	}

	MurkaPoint contextPosition = { 0,0 };

	bool initialised = false;

	bool showingTheDropdown = false;

	bool changedSelection = false;
	
	int selectedOption = 0;

	vector<string> options;
};


class BlankPanel : public MurkaViewInterface<BlankPanel> {
public:
    MURKA_VIEW_DRAW_FUNCTION  {
        
        auto params = (Parameters*)parametersObject;
        BlankPanel* thisWidget = (BlankPanel*)thisWidgetObject;
        
        bool inside = context.isHovered() * !areInteractiveChildrenHovered(context);
        
        bool gonnaResize = false;
        if (params->moveable) {
            if ((context.mousePosition.x < context.currentViewShape.size.x - 30) &&
                (context.mousePosition.y < context.currentViewShape.size.y - 30)) {
                gonnaResize = false;
            } else gonnaResize = true;
        }

        context.pointerToRenderer->pushStyle();
        context.pointerToRenderer->enableFill();
        context.pointerToRenderer->setColor(params->r, params->g, params->b, params->a);
        context.pointerToRenderer->drawRectangle(0, 0, context.getSize().x, context.getSize().y);
        if (params->drawBorder) {
            context.pointerToRenderer->setColor(30);
            context.pointerToRenderer->disableFill();
            context.pointerToRenderer->drawRectangle(0, 0, context.getSize().x, context.getSize().y);
        }
        
        context.pointerToRenderer->setColor(255);
        auto font = context.getCurrentFont();
        font->drawString(params->label, 0, 0);
        
        // Drawing symbols to show that we're going to resize this widget
        if ((gonnaResize) && (inside)) {
            context.pointerToRenderer->drawLine(context.currentViewShape.size.x - 5, context.currentViewShape.size.y - 5,
                       context.currentViewShape.size.x - 15, context.currentViewShape.size.y - 5);
            context.pointerToRenderer->drawLine(context.currentViewShape.size.x - 5, context.currentViewShape.size.y - 5,
                       context.currentViewShape.size.x - 5, context.currentViewShape.size.y - 15);
        }
        context.pointerToRenderer->popStyle();

        // Moving & resizing logic
        
        if (params->moveable)
        if (inside && context.mouseDownPressed[0] && (!thisWidget->dragging) && (!thisWidget->resizing)) {
            if (gonnaResize) thisWidget->resizing = true;
            else thisWidget->dragging = true;
        }
        
        if (((thisWidget->dragging) || (thisWidget->resizing))  &&
            !context.mouseDown[0]) {
            thisWidget->dragging = false;
            thisWidget->resizing = false;
        }
        
        if (thisWidget->dragging) {
            shape.position.x -= context.mouseDelta.x;
            shape.position.y -= context.mouseDelta.y;
        }
        
        if (thisWidget->resizing) {
            shape.size.x -= context.mouseDelta.x;
            shape.size.y -= context.mouseDelta.y;
            
            if (shape.size.x < params->minimumWidth) {
                shape.size.x = params->minimumWidth;
            }
            if (shape.size.y < params->minimumHeight) {
                shape.size.y = params->minimumHeight;
            }
        }
        
    };
    
    
    ///
    
    struct Parameters {
        float r = 45, g = 45, b = 45, a = 255;
        bool drawBorder = true;
        std::string label;
        
        bool moveable = true;
        float minimumWidth = 200, minimumHeight = 200;
        
        Parameters() {}
        Parameters(float R, float G, float B, float A) { r = R; g = G; b = B; a = A; }
        Parameters(float R, float G, float B, float A, bool Moveable) { r = R; g = G; b = B; a = A; moveable = Moveable;}
        Parameters(float R, float G, float B, const char* Label) { r = R; g = G; b = B; label = Label; }
        Parameters(const char* Label, bool Moveable) { label = Label; moveable = Moveable;}
        Parameters(const char* Label, bool Moveable, float minWidth, float minHeight) { label = Label; moveable = Moveable; minimumWidth = minWidth; minimumHeight = minHeight; }
    };
    
    typedef bool Results;
    
    MurkaPoint initialPosition, initialMousePosition;
    bool dragging = false, resizing = false;
    
    
};


class Button : public MurkaViewInterface<Button> {
public:
    MURKA_VIEW_DRAW_FUNCTION  {
         
         auto parameters = ((Parameters*)parametersObject);
         auto &castedResults = *(castResults(resultObject));

         bool inside = context.isHovered();
         Button* thisWidget = (Button*)thisWidgetObject;
        

         if ((context.mouseDownPressed[0]) && (context.isHovered())) {
            castedResults = true;
            lastTimeClicked = context.getRunningTime();
         } else castedResults = false;
        
         auto font = context.getCurrentFont();
        
        
             float pushed = 0.2 - (context.getRunningTime() - lastTimeClicked);
             if (pushed < 0) pushed = 0;
             pushed /= 0.2;
         
             context.pointerToRenderer->pushStyle();
             context.pointerToRenderer->enableFill();
             if (inside) {
                 context.pointerToRenderer->setColor(MurkaColor(parameters->r, parameters->g, parameters->b) * (1.0 + 0.2 * pushed));
             } else context.pointerToRenderer->setColor(75 * (1.0 + 0.2 * pushed));
             context.pointerToRenderer->drawRectangle(0, 0, context.getSize().x, context.getSize().y);
             context.pointerToRenderer->setColor(30);
             context.pointerToRenderer->disableFill();
             context.pointerToRenderer->drawRectangle(0, 0, context.getSize().x, context.getSize().y);
        
             context.pointerToRenderer->setColor(255);
			 context.pointerToRenderer->drawRectangle(0, 0, 2,2);

             float offset = (font->stringWidth(parameters->label) / 2);
        
			 font->drawString(parameters->label, context.getSize().x / 2 - offset, context.getSize().y / 2 - font->getLineHeight() / 2);

//                 float offset = parameters->label.length() * 4;
//                 ofDrawBitmapString(parameters->label, context.getSize().x / 2 - offset, context.getSize().y / 2 + 5);
             context.pointerToRenderer->popStyle();

             auto label = ((Parameters*)parametersObject)->label;
        
        
    };
    
    
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
    
    Results* castResults(void* results) {
        auto resultsPointer = (Results*)results;
        return resultsPointer;
    }
    
    Parameters& castParameters() {
        return  *((Parameters*)parametersInternal);
    }

};


class SliderFloat : public MurkaViewInterface<SliderFloat> {
public:
    MURKA_VIEW_DRAW_FUNCTION  {
        
        
        auto parameters = ((Parameters*)parametersObject);
        Results* results = (Results*)resultObject;
        
        *results = false;
        
        bool inside = context.isHovered();
        Button* thisWidget = (Button*)thisWidgetObject;
        
        if ((context.mouseDownPressed[0]) && (context.isHovered())) {
            dragging = true;
        }
        
        if ((dragging) && (!context.mouseDown[0])) {
            dragging = false;
            
            *results = true;
        }
        
        if (dragging) {
            float newValue = (context.mousePosition.x / context.currentViewShape.size.x) * (parameters->maxValue - parameters->minValue) + parameters->minValue;
            
            if (newValue > parameters->maxValue) newValue = parameters->maxValue;
            if (newValue < parameters->minValue) newValue = parameters->minValue;
            *((float*)dataToControl) = newValue;
        }
        
        auto font = context.getCurrentFont();
        

        
		MurkaColor fgColor = context.pointerToRenderer->getColor();

        context.pointerToRenderer->pushStyle();
        context.pointerToRenderer->enableFill();
        context.pointerToRenderer->setColor(15);
        context.pointerToRenderer->drawRectangle(0, 0, context.getSize().x, context.getSize().y);
        
        context.pointerToRenderer->setColor(parameters->r + (inside ? 20 : 0),
                   parameters->g + (inside ? 20 : 0),
                   parameters->b + (inside ? 20 : 0), 200);
        float currentValue = *((float*)dataToControl);
        if (currentValue > parameters->maxValue) {
            currentValue = parameters->maxValue;
        }
        if (currentValue < parameters->minValue) {
            currentValue = parameters->minValue;
        }
        context.pointerToRenderer->drawRectangle(0, 0, context.getSize().x * ((currentValue - parameters->minValue) / (parameters->maxValue - parameters->minValue)), context.getSize().y);
        context.pointerToRenderer->setColor(80);
		context.pointerToRenderer->disableFill();
        if (inside) {
            context.pointerToRenderer->drawRectangle(0, 0, context.getSize().x, context.getSize().y);
        }
        

        context.pointerToRenderer->setColor(inside ? fgColor : fgColor / 2);
        auto label = ((Parameters*)parametersObject)->label;
        auto resultString = label + ": " + to_string(*((float*)dataToControl));
        
        
        float offset = font->stringWidth(resultString) / 2;
        
        font->drawString(resultString, context.getSize().x / 2 - offset, context.getSize().y / 2 - font->getLineHeight()/2);
        context.pointerToRenderer->popStyle();

        

        
    };
    
    
    // Whatever the parameters and the results are, the functions have to be defined here
    
    struct Parameters {
        float r = 80, g = 80, b = 80;
        std::string label;
        float minValue, maxValue;
        bool clamp = true;
        
        Parameters() {}
        
        Parameters(const char* Label) {
            label = Label; minValue = 0; maxValue = 1;
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

}
