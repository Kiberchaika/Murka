#pragma once
#include "Murka.h"


namespace murka {

enum TextAlignment {TEXT_LEFT, TEXT_CENTER, TEXT_RIGHT};

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
        
        MurkaColor fgColor = params->customColor ? params->color : context.renderer->getColor();
        context.renderer->setColor(fgColor);
        if (params->alignment == TEXT_LEFT) {
            font->drawString(params->label, 5, 5 + font->getLineHeight());
        }
        if (params->alignment == TEXT_CENTER) {
            float textX = 5 + (shape.size.x - 10) / 2 - font->getStringBoundingBox(params->label, 0, 0).getWidth() / 2;
            font->drawString(params->label, textX, 5 + font->getLineHeight());
        }
        if (params->alignment == TEXT_RIGHT) {
            float textX = (shape.size.x - 10) - font->getStringBoundingBox(params->label, 0, 0).getWidth();
            font->drawString(params->label, textX, 5 + font->getLineHeight());
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
            font->drawString(params->label, 5, font->getLineHeight());
        }
        if (params->alignment == TEXT_CENTER) {
            float textX = 5 + (shape.size.x - 10) / 2 - font->getStringBoundingBox(params->label, 0, 0).getWidth() / 2;
            font->drawString(params->label, textX, font->getLineHeight());
        }
        if (params->alignment == TEXT_RIGHT) {
            float textX = (shape.size.x - 10) - font->getStringBoundingBox(params->label, 0, 0).getWidth();
            font->drawString(params->label, textX, font->getLineHeight());
        }
        
        // Testing vertical centering
//            context.renderer->setColor(255);
//            ofDrawLine(0, 0, context.getSize().x, 0);
//            context.renderer->setColor(255, 0, 0);
//            ofDrawLine(0, context.getSize().y / 2, context.getSize().x, context.getSize().y / 2);
//            context.renderer->setColor(255);
//            ofDrawLine(0, context.getSize().y, context.getSize().x, context.getSize().y);
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
    
class Checkbox : public MurkaViewInterface<Checkbox> {
public:
    MURKA_VIEW_DRAW_FUNCTION  {
        
        auto params = (Parameters*)parametersObject;
		auto &castedResults = *(castResults(resultObject));
	
		Checkbox* thisWidget = (Checkbox*)thisWidgetObject;
		
		bool inside = context.isHovered() * !areChildrenHovered(context);
        
        bool* booleanToControl = ((bool*)dataToControl);

        MurkaColor fgColor = context.renderer->getColor();

        float pushed = 0.2 - (context.getRunningTime() - lastTimeClicked);
        if (pushed < 0) pushed = 0;
        pushed /= 0.2;

        context.renderer->pushStyle();
        context.renderer->setColor((inside ? fgColor : fgColor / 2) * (1.0 + 0.2 * pushed));

        context.renderer->disableFill();
        context.renderer->drawRectangle(5, 5, 20, 20);
        if (*booleanToControl) {
            float symbolOffset = 4;
            context.renderer->drawLine(5 + symbolOffset, 5 + symbolOffset, 25 - symbolOffset, 25 - symbolOffset);
            context.renderer->drawLine(25 - symbolOffset, 5 + symbolOffset, 5 + symbolOffset, 25 - symbolOffset);
        }
        
        context.renderer->enableFill();
        
        auto font = context.getCurrentFont();
        font->drawString(params->label, 30, 22);
        
        if (inside && context.mouseDownPressed[0]) {
            *booleanToControl = !*booleanToControl;
            lastTimeClicked = context.getRunningTime();
			castedResults = booleanToControl;
		}
		else castedResults = false;

        context.renderer->popStyle();
    };
    
    // Here go parameters and any parameter convenience constructors. You need to define something called Parameters, even if it's NULL.
    struct Parameters {
        std::string label;
        
        Parameters() {}
        Parameters(std::string Label) { label = Label; } // a convenience initializer
    };

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

		bool inside = context.isHovered() * !areChildrenHovered(context);

		int* intToControl = ((int*)dataToControl);

		MurkaColor fgColor = context.renderer->getColor();

		float pushed = 0.2 - (context.getRunningTime() - lastTimeClicked);
		if (pushed < 0) pushed = 0;
		pushed /= 0.2;

		for (int i = 0; i < params->labels.size(); i++) {
			context.renderer->pushMatrix();
			context.renderer->pushStyle();
			context.renderer->translate(0, 25 * i, 0);
			MurkaShape rowShape = { 0, 25 * i, context.getSize().x, 25 };
			bool rowHover = rowShape.inside(context.mousePosition);

			context.renderer->setColor((rowHover * inside ? fgColor : fgColor / 2) * (1.0 + 0.2 * pushed));

			context.renderer->disableFill();
			context.renderer->drawCircle(5 + 20 / 2, 5 + 20 / 2, 10);
			if (*intToControl == i) {
				context.renderer->enableFill();
				context.renderer->drawCircle(5 + 20 / 2, 5 + 20 / 2, 6);
			}
			context.renderer->enableFill();

			auto font = context.getCurrentFont();
			font->drawString(params->labels[i], 30, 22);

			if (rowHover * inside && context.mouseDownPressed[0]) {
				*intToControl = i;
				lastTimeClicked = context.getRunningTime();
			}

			context.renderer->popStyle();
			context.renderer->popMatrix();
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

        context.renderer->pushStyle();
        context.renderer->enableFill();
        context.renderer->setColor(params->r, params->g, params->b, params->a);
        context.renderer->drawRectangle(0, 0, context.getSize().x, context.getSize().y);
        if (params->drawBorder) {
            context.renderer->setColor(30);
            context.renderer->disableFill();
            context.renderer->drawRectangle(0, 0, context.getSize().x, context.getSize().y);
        }
        
        context.renderer->setColor(255);
        auto font = context.getCurrentFont();
        font->drawString(params->label, 0, font->getLineHeight());
        
        // Drawing symbols to show that we're going to resize this widget
        if ((gonnaResize) && (inside)) {
            context.renderer->drawLine(context.currentViewShape.size.x - 5, context.currentViewShape.size.y - 5,
                       context.currentViewShape.size.x - 15, context.currentViewShape.size.y - 5);
            context.renderer->drawLine(context.currentViewShape.size.x - 5, context.currentViewShape.size.y - 5,
                       context.currentViewShape.size.x - 5, context.currentViewShape.size.y - 15);
        }
        context.renderer->popStyle();

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
        
         #ifdef MURKA_OF
        
             float pushed = 0.2 - (context.getRunningTime() - lastTimeClicked);
             if (pushed < 0) pushed = 0;
             pushed /= 0.2;
         
             context.renderer->pushStyle();
             context.renderer->enableFill();
             if (inside) {
                 context.renderer->setColor(MurkaColor(parameters->r, parameters->g, parameters->b) * (1.0 + 0.2 * pushed));
             } else context.renderer->setColor(75 * (1.0 + 0.2 * pushed));
             context.renderer->drawRectangle(0, 0, context.getSize().x, context.getSize().y);
             context.renderer->setColor(30);
             context.renderer->disableFill();
             context.renderer->drawRectangle(0, 0, context.getSize().x, context.getSize().y);
        
             context.renderer->setColor(255);
        
             float offset = font->stringWidth(parameters->label) / 2;
        
             font->drawString(parameters->label, context.getSize().x / 2 - offset, context.getSize().y / 2 + font->getLineHeight() / 4);

//                 float offset = parameters->label.length() * 4;
//                 ofDrawBitmapString(parameters->label, context.getSize().x / 2 - offset, context.getSize().y / 2 + 5);
             context.renderer->popStyle();

             auto label = ((Parameters*)parametersObject)->label;
        
         #endif // MURKA_OF
        
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
        

        
		MurkaColor fgColor = context.renderer->getColor();

        context.renderer->pushStyle();
        context.renderer->enableFill();
        context.renderer->setColor(15);
        context.renderer->drawRectangle(0, 0, context.getSize().x, context.getSize().y);
        
        context.renderer->setColor(parameters->r + (inside ? 20 : 0),
                   parameters->g + (inside ? 20 : 0),
                   parameters->b + (inside ? 20 : 0), 200);
        float currentValue = *((float*)dataToControl);
        if (currentValue > parameters->maxValue) {
            currentValue = parameters->maxValue;
        }
        if (currentValue < parameters->minValue) {
            currentValue = parameters->minValue;
        }
        context.renderer->drawRectangle(0, 0, context.getSize().x * ((currentValue - parameters->minValue) / (parameters->maxValue - parameters->minValue)), context.getSize().y);
        context.renderer->setColor(80);
		context.renderer->disableFill();
        if (inside) {
            context.renderer->drawRectangle(0, 0, context.getSize().x, context.getSize().y);
        }
        

        context.renderer->setColor(inside ? fgColor : fgColor / 2);
        auto label = ((Parameters*)parametersObject)->label;
        auto resultString = label + ": " + ofToString(*((float*)dataToControl));
        
        
        float offset = font->stringWidth(resultString) / 2;
        
        font->drawString(resultString, context.getSize().x / 2 - offset, context.getSize().y / 2  + font->getLineHeight() / 4);
        context.renderer->popStyle();

        

        
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
