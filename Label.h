#include "Murka.h"

namespace murka {

class Label : public View_NEW<Label> {
public:
    
    // TODO: rename it to internalDraw ?
    void internalDraw(const MurkaContextBase & c)  {
        
        bool inside = c.isHovered() * !areChildrenHovered(c);
        
        if (isHovered != inside) {
            isHovered = inside;
            onHoverChangeCallback(*this);
        }
        
//        if (c.mouseButtonsChanged[0] && c.mouseDown[0])
        
        if (c.mouseDownPressed[0]) {
            onClickCallback(*this);
        }
        
        auto font = c.getCurrentFont();
        
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
        
        c.renderer->setColor(200, 0, 0, 200);
        
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
    
    bool isHovered = false;
    
//    private:
//    std::function<void()> callbackVariable;
//    public:
//    Label & setCallback(std::function<void()> cb) {
//        callbackVariable = cb;
//    }
    
    
    MURKA_CALLBACK(Label, // class name
                   onHoverChangeCallback, // callback variable name
                   onHoverChange) // callback setter
    
    MURKA_CALLBACK(Label,
                   onClickCallback,
                   onClick)
    
    MURKA_PARAMETER(Label, // class name
                    TextAlignment, // parameter type
                    alignment, // parameter variable name
                    getAlignment, // getter
                    withAlignment, // setter
                    TEXT_LEFT) // default
    
    MURKA_PARAMETER(Label, // class name
                    std::string, // parameter type
                    label, // parameter variable name
                    getLabel, // getter
                    text, // setter
                    "") // default
    
//    MURKA_SUPPORT_DIRECT_DRAW(Label)
    
    virtual bool wantsClicks() override { return false; } // override this if you want to signal that you don't want clicks

};

}
