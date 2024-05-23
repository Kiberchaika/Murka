#pragma once

#include "MurkaTypes.h"
#include "MurkaContext.h"
#include "MurkaView.h"
#include "MurkaInputEventsRegister.h"
#include "MurkaLinearLayoutGenerator.h"

#include "LambdaPanel.h"

namespace murka {

class Pane : public View_NEW<Pane> {
    
    double paneSplitValue = 0.5;
    
public:
	void internalDraw(Murka & m) {
		MurkaContext& c = m.currentContext;

        bool inside = c.isHovered() * !areInteractiveChildrenHovered(c) * hasMouseFocus(m);
        
        MurkaShape paneControlShape;
        
        if (paneKind == PaneKind::HORIZONTAL) {  // horizontal drawing
            m.draw<LambdaPanel>({0, 0,
                                c.getSize().x * paneSplitValue,
                                c.getSize().y})
                .lambda({[&](const MurkaContextBase& c) { firstPanelDraw(c); }});
            
            m.draw<LambdaPanel>({c.getSize().x * paneSplitValue, 0,
                                    c.getSize().x * float(1.0 - paneSplitValue),
                                    c.getSize().y})
                .lambda({[&](const MurkaContextBase& c) { secondPanelDraw(c); }});
            paneControlShape = MurkaShape(c.getSize().x *
                                                     paneSplitValue - paneControlWidth / 2, 0,
                                                     paneControlWidth, c.getSize().y);
            
            
            
//            params->drawPaneControl(c, paneControlShape,
//                                    paneControlShape.inside(c.mousePosition, 5), draggingNow);
             
        }
        
        
        if (paneKind == PaneKind::VERTICAL) { // vertical drawing

            m.draw<LambdaPanel>({0, 0,
                                    c.getSize().x,
                                    c.getSize().y * paneSplitValue})
                .lambda({[&](const MurkaContextBase& c) { firstPanelDraw(c); }});
            
            m.draw<LambdaPanel>({0, c.getSize().y * paneSplitValue,
                                    c.getSize().x,
                                    c.getSize().y * float(1.0 - paneSplitValue)})
                .lambda({[&](const MurkaContextBase& c) { firstPanelDraw(c); }});
            
            paneControlShape = MurkaShape(0,
                                          c.getSize().y *
                                                     paneSplitValue - paneControlWidth / 2,
                                          c.getSize().x,
                                                     paneControlWidth);
            
//            params->drawPaneControl(c, paneControlShape,
//                                    paneControlShape.inside(c.mousePosition, 5), draggingNow);


        }
        
        if ((!draggingNow) && (!areInteractiveChildrenHovered(c)) && (paneControlShape.inside(c.mousePosition, 5)) && (c.mouseDownPressed[0])) {
            draggingNow = true;
        }
        
        if ((draggingNow) && (!c.mouseDown[0])) {
            draggingNow = false;
        }
        
        if (draggingNow) {
            if (paneKind == PaneKind::HORIZONTAL) { // horizontal dragging
                paneSplitValue = c.mousePosition.x / c.getSize().x;
            }
            
            if (paneKind == PaneKind::VERTICAL) { // vertical dragging
                paneSplitValue = c.mousePosition.y / c.getSize().y;
            }

            if (paneSplitValue < minValue) {
                if (collapseable) {
                    paneSplitValue = 0;
                } else {
                    paneSplitValue = minValue;
                }
            }
            if (paneSplitValue > maxValue) {
                if (collapseable) {
                    paneSplitValue = 1.0;
                } else {
                    paneSplitValue = maxValue;
                }
            }
        }
         
         
    };
    
    bool draggingNow = false;
    
    MURKA_PARAMETER(Pane, // class name
        std::function<void(const MurkaContextBase&)>, // parameter type
        firstPanelDraw, // parameter variable name
        withFirstPanelCallback, // setter
        [](const MurkaContextBase&){} // default
    )

    MURKA_PARAMETER(Pane, // class name
        std::function<void(const MurkaContextBase&)>, // parameter type
        secondPanelDraw, // parameter variable name
        withSecondPanelCallback, // setter
        [](const MurkaContextBase&){} // default
    )
    
    MURKA_PARAMETER(Pane, // class name
        double, // parameter type
        minValue, // parameter variable name
        withMinValue, // setter
        0 // default
    )

    MURKA_PARAMETER(Pane, // class name
        double, // parameter type
        maxValue, // parameter variable name
        withMaxValue, // setter
        0 // default
    )
    
    MURKA_PARAMETER(Pane, // class name
        bool, // parameter type
        collapseable, // parameter variable name
        setCollapseable, // setter
        true // default
    )
    
    enum PaneKind { HORIZONTAL, VERTICAL };

    MURKA_PARAMETER(Pane, // class name
        PaneKind, // parameter type
        paneKind, // parameter variable name
        withKind, // setter
        HORIZONTAL // default
    )
    
    MURKA_PARAMETER(Pane, // class name
        double, // parameter type
        paneControlWidth, // parameter variable name
        withPaneCopntrolWidth, // setter
        8 // default
    )

};

}
