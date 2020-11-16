#include "Murka.h"


namespace murka {

class Label : public View_NEW<Label> {
public:
    
    MURKA_VIEW_DRAW_FUNCTION_NEW  {
        
        bool inside = c.isHovered() * !areChildrenHovered(c);
        
        auto font = c.getCurrentFont();
        
        /*
        MurkaColor bgColor = MurkaColor(0, 0, 0, 255);
        c.renderer->enableFill();
        
        if (bgColor.a != 0.0) {
            c.renderer->setColor(bgColor);
            if (alignment == TEXT_LEFT) {
                c.renderer->drawRectangle(0, 0, font->getStringBoundingBox(label, 0, 0).width + 10, c.getSize().y);
            }
        }
        */
        
        
//        MurkaColor fgColor = c.renderer->getColor();
//        c.renderer->setColor(fgColor);
        
//        c.renderer->drawRectangle(0, 0, c.getSize().x, c.getSize().y);
        
        if (alignment == TEXT_LEFT) {
            font->drawString(label, 5, 0);
        }
        if (alignment == TEXT_CENTER) {
            float textX = 5 + (shape.size.x - 10) / 2 - font->getStringBoundingBox(label, 0, 0).getWidth()  / 2;
            font->drawString(label, textX, 0);
        }
        if (alignment == TEXT_RIGHT) {
            float textX = (shape.size.x - 10) - font->getStringBoundingBox(label, 0, 0).getWidth();
            font->drawString(label, textX, 0);
        }
        
        // Testing vertical centering
            //context.renderer->setColor(255);
            //ofDrawLine(0, 0, context.getSize().x, 0);
            //context.renderer->setColor(255, 0, 0);
            //ofDrawLine(0, context.getSize().y / 2, context.getSize().x, context.getSize().y / 2);
            //context.renderer->setColor(255);
            //ofDrawLine(0, context.getSize().y, context.getSize().x, context.getSize().y);
    };
    
    static Label & draw2(MurkaContext &c, MurkaShape shape) {
        int counter = c.getImCounter();

        auto parentView = (ViewBase_NEW*)c.linkedView_NEW;
        auto widgetObject = Label::getOrCreateImModeWidgetObject_NEW(counter, parentView, shape);

        return (Label&)widgetObject;
    }
    
//    MURKA_SUPPORT_DIRECT_DRAW(Label)
    
    operator bool() {
        return true;
    }
    
    // Here go parameters and any parameter convenience constructors. You need to define something called Parameters, even if it's NULL.
    
    TextAlignment alignment = TEXT_LEFT;
    std::string label;
    
    
    // The results type, you also need to define it even if it's nothing.
    typedef bool Results;
    
    virtual bool wantsClicks() override { return false; } // override this if you want to signal that you don't want clicks

};

}
