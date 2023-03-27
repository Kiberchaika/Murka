#pragma once
#include "Murka.h"

namespace murka {


class Label : public View<Label> {
public:
	void internalDraw(Murka & m) {
		MurkaContext& c = m.currentContext;

        bool inside = c.isHovered() * !areChildrenHovered(c);
        
        if (isHovered != inside) {
            isHovered = inside;
            onHoverChangeCallback(*this);
        }
        
        if (isHovered) onHoverCallback(*this);
         
        if (c.mouseDownPressed[0] && inside) {
            onClickCallback(*this);
            onClickCallbackPosition(*this, {25, 11});
        }
        
        auto font = m.getCurrentFont();
        
        if (alignment == TEXT_LEFT) {
            font->drawString(label, 5, 0);
        }
        if (alignment == TEXT_CENTER) {
            float textX = 5 + (shape.size.x - 10) / 2 - font->getStringBoundingBox(label, 0, 0).width / 2;
            font->drawString(label, textX, 0);
        }
        if (alignment == TEXT_RIGHT) {
            float textX = (shape.size.x - 10) - font->getStringBoundingBox(label, 0, 0).width;
            font->drawString(label, textX, 0);
        }
   }
   
    bool isHovered = false;
 
	MURKA_CALLBACK(Label, // class name
                   onHoverChangeCallback, // callback variable name
                   onHoverChange) // callback setter

    MURKA_CALLBACK(Label, // class name
                   onHoverCallback, // callback variable name
                   onHover) // callback setter

    MURKA_CALLBACK(Label,
                   onClickCallback,
                   onClick)
    
    MURKA_CALLBACK_1ARGUMENT(Label, // class name
                             onClickCallbackPosition, // callback variable name
                             onClickPosition, // callback setter
                             MurkaPoint) // callback argument type
    
    MURKA_PARAMETER(Label, // class name
                    TextAlignment, // parameter type
                    alignment, // parameter variable name
                    withAlignment, // setter
                    TEXT_LEFT) // default
    
    MURKA_PARAMETER(Label, // class name
                    std::string, // parameter type
                    label, // parameter variable name
                    text, // setter
                    "") // default
      
    virtual bool wantsClicks() override { return false; } // override this if you want to signal that you don't want clicks

};

}
