#include "Murka.h"


namespace murka {

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
            float textX = 5 + (shape.size.x - 10) / 2 - font->getStringBoundingBox(params->label, 0, 0).getWidth()  / 2;
            font->drawString(params->label, textX, 0);
        }
        if (params->alignment == TEXT_RIGHT) {
            float textX = (shape.size.x - 10) - font->getStringBoundingBox(params->label, 0, 0).getWidth();
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

}
