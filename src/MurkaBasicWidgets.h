#pragma once

#include "Murka.h"
#include "MurkaView.h"
#include "Label.h"

namespace murka {


class Header : public View<Header> {
public:
    Header() {

    }

    void internalDraw(Murka & m) {
        // Your drawing and interaction code goes here.
        // Don't forget that all of this executes at each frame for each
        // widget that is drawn on screen.
        auto font = m.getCurrentFont();
        
        MurkaColor fgColor = customColor ? color : m.getColor();
        m.setColor(fgColor);
        if (alignment == TEXT_LEFT) {
            font->drawString(label, 5, 5);
        }
        if (alignment == TEXT_CENTER) {
            float textX = 5 + (shape.size.x - 10) / 2 - font->getStringBoundingBox(label, 0, 0).width / 2;
            font->drawString(label, textX, 5);
        }
        if (alignment == TEXT_RIGHT) {
            float textX = (shape.size.x - 10) - font->getStringBoundingBox(label, 0, 0).width;
            font->drawString(label, textX, 5);
        }
    }

	std::string label;

	TextAlignment alignment = TextAlignment::TEXT_LEFT;

	MurkaColor color = { 0.98f, 0.98f, 0.98f };

	bool customColor = false;
   
    virtual bool wantsClicks() override { return false; } // override this if you want to signal that you don't want clicks

};


class Checkbox : public View<Checkbox> {
public:
    void internalDraw(Murka & m) {
        
        bool* booleanToControl = ((bool*)dataToControl);

		pressed = false;

		MurkaColor fgColor = m.getColor();

        float pushed = 0.2f - (float)(m.getElapsedTime() - lastTimeClicked);
        if (pushed < 0) pushed = 0;
        pushed /= 0.2f;

        m.pushStyle();
        m.setColor((inside() ? fgColor : fgColor / 2) * (1.0 + 0.2 * pushed));

        m.disableFill();
        m.drawRectangle(5, 5, 20, 20);
        if (*booleanToControl) {
            float symbolOffset = 4;
            m.drawLine(5 + symbolOffset, 5 + symbolOffset, 25 - symbolOffset, 25 - symbolOffset);
            m.drawLine(25 - symbolOffset, 5 + symbolOffset, 5 + symbolOffset, 25 - symbolOffset);
        }
        
        m.enableFill();
        
        auto font = m.getCurrentFont();
        font->drawString(label, 30, font->getStringBoundingBox(label, 0, 0).height / 2);
        
        if (inside() && mouseDownPressed(0)) {
            *booleanToControl = !*booleanToControl;
            lastTimeClicked = m.getElapsedTime();
			results = booleanToControl;
		}
		else results = false;

        m.popStyle();
    }
    
	std::string label;

	bool pressed;


	bool results;


	MURKA_PARAMETER(Checkbox, // class name
		bool*, // parameter type
		dataToControl, // parameter variable name
		setDataToControl, // setter
		nullptr
	) // default
   
    // The two functions needed for optional UI state saving. It's up to you to use those.
//    std::vector<MurkaVar> saveInternalData(int indexX, int indexY) override { }
//    void loadInternalData(std::vector<MurkaVar>) override {}
     
    // Everything else in the class is for handling the internal state. It persistant.
    double lastTimeClicked = 0;
};


class Radiobutton : public View<Radiobutton> {
public:
	void internalDraw(Murka & m) {
		results = false;

		MurkaColor fgColor = m.getColor();

		float pushed = 0.2f - (float)(m.getElapsedTime() - lastTimeClicked);
		if (pushed < 0) pushed = 0;
		pushed /= 0.2f;
            
        int lineHeight = 25;

		for (int i = 0; i < labels.size(); i++) {
			m.pushMatrix();
			m.pushStyle();
			m.translate(0, lineHeight * i, 0);
			MurkaShape rowShape = { 0, lineHeight * i, getSize().x, lineHeight };
			bool rowHover = rowShape.inside(mousePosition());

            m.setColor(((rowHover * inside()) ? fgColor : fgColor / 2) * (1.0f + 0.2f * pushed));

			m.disableFill();
			m.drawCircle(5 + 20 / 2, 5 + 20 / 2, 10);
			if (*dataToControl == i) {
				m.enableFill();
				m.drawCircle(5 + 20 / 2, 5 + 20 / 2, 6);
			}
			m.enableFill();

			auto font = m.getCurrentFont();
			font->drawString(labels[i], 30, 10);

			if (rowHover * inside() && mouseDownPressed(0)) {
				*dataToControl = i;
				results = true;
				lastTimeClicked = m.getElapsedTime();
			}

			m.popStyle();
			m.popMatrix();
		}
	}

	MURKA_PARAMETER(Radiobutton, // class name
		int*, // parameter type
		dataToControl, // parameter variable name
		setDataToControl, // setter
		nullptr
	) // default

	std::vector<std::string> labels;
	bool results;

	// Everything else in the class is for handling the internal state. It persistы.
	double lastTimeClicked = 0;
};


class DropdownElementButton : public View<DropdownElementButton> {
public:
	void internalDraw(Murka & m) {


		m.setColor(MurkaColor(90 / 255.0f, 90 / 255.0f, 90 / 255.0f) * (0.8f + 0.2f * inside()));
		m.drawRectangle(0, 0, getSize().x, getSize().y);

		if (justInitialised) {
			justInitialised = false;
			pressed = false;
			return;
			// ^ ghetto way to prevent this button from activating right away
		}

		auto currentLastFrame = m.getFrameNum();

		m.setColor(255);
		m.drawString(label, 10, 5);

		if ((currentLastFrame - lastFrameItWasRendered) <= 1) { // only allow clicking if it was rendered at last frame - to avoid instant clicks in overlays
			if ((mouseDownPressed(0)) && (inside())) {
				// click if it's not the same frame it was shown
#ifdef MURKA_DEBUG
				std::cout << "btn pressd. justInitialised? " << justInitialised << std::endl;
#endif
				pressed = true;

				return;
			}
		}

		lastFrameItWasRendered = currentLastFrame;

		pressed = false;
		return;
	}

	std::string label;
	uint64_t lastFrameItWasRendered = -1;
	bool pressed;
	bool justInitialised = true;
};


class DropdownButton : public View<DropdownButton> {
public:
	void internalDraw(Murka & m) {

		m.setColor(MurkaColor(90 / 255.0f, 90 / 255.0f, 90 / 255.0f) * (0.5f + 0.3f * inside()));
		m.drawRectangle(0, 0, getSize().x, getSize().y);

		if ((mouseDownPressed(0)) && (inside())) {
			if (!showingTheDropdown) {
				showingTheDropdown = true;
			}
		}

		if (!initialised) {
			initialised = true;
			selectedOption = selectedOption;
		}

		options = options;

		m.setColor(MurkaColor(66 / 255.0f, 67 / 255.0f, 71 / 255.0f) * 3.2f);
		m.drawString(options[selectedOption], 5, 5);

		if (showingTheDropdown) {
			contextPosition = getAbsoluteViewPosition();
			m.addOverlay([&]() {
				//                         ofLog() << m.currentContext.currentViewShape.position.x;
				for (int i = 0; i < options.size(); i++) {
					std::string buttonLabel = options[i];
					auto el = m.prepare<DropdownElementButton>({ contextPosition.x, contextPosition.y + 30 * i, 150, 30 });
					el.label = buttonLabel;
                    el.draw();
					if (el.pressed) {
#ifdef MURKA_DEBUG
						std::cout << i << " pressed" << std::endl;
#endif
						showingTheDropdown = false;
						changedSelection = true;
						selectedOption = i;
					}
				}
			}, this);

		}

		results = changedSelection;
		if (changedSelection) {
			*dataToControl = selectedOption;
			changedSelection = false;
		}
	}

	MURKA_PARAMETER(DropdownButton, // class name
		int*, // parameter type
		dataToControl, // parameter variable name
		setDataToControl, // setter
		nullptr
	) // default

    std::vector<std::string> options = {};
	int selectedOption = 0;
	bool results;
	MurkaPoint contextPosition = { 0,0 };
	bool initialised = false;
	bool showingTheDropdown = false;
	bool changedSelection = false;
};


class BlankPanel : public View<BlankPanel> {
public:
    void internalDraw(Murka & m) {
        
        bool gonnaResize = false;
        if (moveable) {
            if ((mousePosition().x < getSize().x - 30) &&
                (mousePosition().y < getSize().y - 30)) {
                gonnaResize = false;
            } else gonnaResize = true;
        }

        m.pushStyle();
        m.enableFill();
        m.setColor(r, g, b, a);
        m.drawRectangle(0, 0, getSize().x, getSize().y);
        if (drawBorder) {
            m.setColor(30);
            m.disableFill();
            m.drawRectangle(0, 0, getSize().x, getSize().y);
        }
        
        m.setColor(255);
        auto font = m.getCurrentFont();
        font->drawString(label, 0, 0);
        
        // Drawing symbols to show that we're going to resize this widget
        if ((gonnaResize) && (inside())) {
            m.drawLine(getSize().x - 5, getSize().y - 5,
                       getSize().x - 15, getSize().y - 5);
            m.drawLine(getSize().x - 5, getSize().y - 5,
                       getSize().x - 5, getSize().y - 15);
        }
        m.popStyle();

        // Moving & resizing logic
        
        if (moveable)
        if (inside() && mouseDownPressed(0) && (!dragging) && (!resizing)) {
            if (gonnaResize) resizing = true;
            else dragging = true;
        }
        
        if (((dragging) || (resizing))  &&
            !mouseDown(0)) {
            dragging = false;
            resizing = false;
        }
        
        if (dragging) {
            shape.position.x -= mouseDelta().x;
            shape.position.y -= mouseDelta().y;
        }
        
        if (resizing) {
            shape.size.x -= mouseDelta().x;
            shape.size.y -= mouseDelta().y;
            
            if (shape.size.x < minimumWidth) {
                shape.size.x = minimumWidth;
            }
            if (shape.size.y < minimumHeight) {
                shape.size.y = minimumHeight;
            }
        }
    }
    
    
    ///
	float r = 45, g = 45, b = 45, a = 255;
	bool drawBorder = true;
	std::string label;

	bool moveable = true;
	float minimumWidth = 200, minimumHeight = 200;

	bool results;

	MurkaPoint initialPosition, initialMousePosition;
    bool dragging = false, resizing = false;
};


class Button : public View<Button> {
public:
    void internalDraw(Murka & m) {
         
         bool inside = isHovered();

         if ((mouseDownPressed(0)) && (isHovered())) {
			 pressed = true;
            lastTimeClicked = m.getElapsedTime();
         } else pressed = false;
        
         auto font = m.getCurrentFont();
        
			float pushed = 0.2f - (float)(m.getElapsedTime() - lastTimeClicked);
			if (pushed < 0) pushed = 0;
			pushed /= 0.2f;

			m.pushStyle();
			m.enableFill();
			if (inside) {
			 m.setColor(MurkaColor(r, g, b) * (1.0 + 0.2 * pushed));
			} else m.setColor(75 * (1.0f + 0.2f * pushed));
			m.drawRectangle(0, 0, getSize().x, getSize().y);
			m.setColor(30);
			m.disableFill();
			m.drawRectangle(0, 0, getSize().x, getSize().y);

			m.setColor(255);

			float offset = (font->stringWidth(label) / 2);

			font->drawString(label, getSize().x / 2 - offset, getSize().y / 2 - font->getLineHeight() / 2);

			m.popStyle();
    }

    
	MURKA_PARAMETER(Button, // class name
		std::string, // parameter type
		label, // parameter variable name
		text, // setter
		"") // default


    // Whatever the parameters and the results are, the functions have to be defined here
	float r = 80, g = 80, b = 80;
	std::function<void()> Callback;
	bool pressed;
    // Internal state
	std::string Label;
    float lastTimeClicked = 0;
};


class SliderFloat : public View<SliderFloat> {
public:
    void internalDraw(Murka & m) {
        results = false;
        
        bool inside = isHovered();
        
        if ((mouseDownPressed(0)) && (isHovered())) {
            dragging = true;
        }
        
        if ((dragging) && (!mouseDown(0))) {
            dragging = false;
            
            results = true;
        }
        
        if (dragging) {
            float newValue = (mousePosition().x / getSize().x) * (maxValue - minValue) + minValue;
            
            if (newValue > maxValue) newValue = maxValue;
            if (newValue < minValue) newValue = minValue;
            *(dataToControl) = newValue;
        }
        
        auto font = m.getCurrentFont();
		MurkaColor fgColor = m.getColor();

        m.pushStyle();
        m.enableFill();
        m.setColor(15);
        m.drawRectangle(0, 0, getSize().x, getSize().y);
        
        m.setColor(r + (inside ? 20 : 0),
                   g + (inside ? 20 : 0),
                   b + (inside ? 20 : 0), 200);
        float currentValue = *(dataToControl);
        if (currentValue > maxValue) {
            currentValue = maxValue;
        }
        if (currentValue < minValue) {
            currentValue = minValue;
        }
        m.drawRectangle(0, 0, getSize().x * ((currentValue - minValue) / (maxValue - minValue)), getSize().y);
        m.setColor(80);
		m.disableFill();
        if (inside) {
            m.drawRectangle(0, 0, getSize().x, getSize().y);
        }
        
        m.setColor(inside ? fgColor : fgColor / 2);
        auto resultString = label + ": " + std::to_string(*((float*)dataToControl));
        

        float offset = font->stringWidth(resultString) / 2;
        
        font->drawString(resultString, getSize().x / 2 - offset, getSize().y / 2 - font->getLineHeight()/2);
        m.popStyle();
    }
    
    
	MURKA_PARAMETER(SliderFloat, // class name
		float*, // parameter type
		dataToControl, // parameter variable name
		setDataToControl, // setter
		nullptr
	) // default


    // Whatever the parameters and the results are, the functions have to be defined here
	float r = 80, g = 80, b = 80;
	std::string label;
	float minValue, maxValue;
	bool clamp = true;

    bool results;
    
    // Internal state
    
    std::string Label;
    float lastTimeClicked = 0;
    bool dragging = false;
    
    // Helpers that didn't work in templated class yet :(
};

}
