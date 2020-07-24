#pragma once

#include "MurkaTypes.h"
#include "MurkaContext.h"
#include "MurkaView.h"
#include "MurkaInputEventsRegister.h"
#include "MurkaLinearLayoutGenerator.h"
#include "MurkaSurface.h"

using namespace murka;

class ScrollBar : public MurkaViewInterface<ScrollBar> {
public:
    MURKA_VIEW_DRAW_FUNCTION  {

        auto params = (Parameters*)parametersObject;
        auto results = (Results*)resultObject;
        bool inside = context.isHovered() * !areInteractiveChildrenHovered(context) * hasMouseFocus(context);

        if (params->kind == 0) {
            if (params->surfaceToControl->panLimits.size.y > 0) {

               float scrollbarHeight = (context.getSize().y / (params->surfaceToControl->panLimits.size.y + context.getSize().y)) * context.getSize().y;
                
               MurkaShape surfaceShape = ((View*)params->surfaceToControl)->shape;
                
               MurkaShape handleShape = {0,
                                          (context.getSize().y - scrollbarHeight) *
                                            (params->surfaceToControl->panOffset.y / (params->surfaceToControl->panLimits.size.y)),
                                            context.getSize().x,
                                            scrollbarHeight};
               MurkaShape scrollbarShape = {0, 0, context.getSize().x, context.getSize().y};
                
               params->drawScrollBar(context, scrollbarShape, handleShape, handleShape.inside(context.mousePosition), false);
                
                if (draggingNow) {
                    params->surfaceToControl->panOffset.y -= (lastFrameMousePosition.y - context.mousePosition.y) * (context.getSize().y / scrollbarHeight);
                    if (params->surfaceToControl->panOffset.y < 0) params->surfaceToControl->panOffset.y = 0;
                    if (params->surfaceToControl->panOffset.y > params->surfaceToControl->panLimits.size.y) params->surfaceToControl->panOffset.y = params->surfaceToControl->panLimits.size.y;

                }
                
                if ((handleShape.inside(context.mousePosition)) && inside && context.mouseDownPressed[0]) {
                    draggingNow = true;
                }

                if ((draggingNow) && (!context.mouseDown[0])) {
                    draggingNow = false;
                }

                lastFrameMousePosition = context.mousePosition;

            }
        }
    };
    
    bool draggingNow = false;
    MurkaPoint lastFrameMousePosition;
//    double initialPanOffset = 0;

    struct Parameters {
        MurkaSurface* surfaceToControl;
        
        std::function<void(MurkaContext & c, MurkaShape scrollbarShape, MurkaShape handleShape, bool hover, bool draggingNow)> drawScrollBar = [](MurkaContext & c, MurkaShape scrollbarShape, MurkaShape handleShape, bool hover, bool draggingNow) {
                            c.renderer->setColor((80 + hover * 40 + draggingNow * 80) / 255.);
                            c.renderer->drawRectangle(handleShape);
                        };
        int kind = 0; // 0 = vertical
                      // 1 = horizontal

        Parameters() {}
        Parameters(MurkaSurface* _surfaceToControl, std::function<void(MurkaContext&, MurkaShape, MurkaShape, bool, bool)> _drawScrollBar, int _kind = 0) {
            surfaceToControl = _surfaceToControl;
            drawScrollBar = _drawScrollBar;
            kind = _kind;
        }
        
        Parameters(MurkaSurface* _surfaceToControl, int _kind = 0) {
            surfaceToControl = _surfaceToControl;
            kind = _kind;
        }
    };

    // The results type, you also need to define it even if it's nothing.
    typedef bool Results;


};
