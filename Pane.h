#pragma once

#include "MurkaTypes.h"
#include "MurkaContext.h"
#include "MurkaView.h"
#include "MurkaInputEventsRegister.h"
#include "MurkaLinearLayoutGenerator.h"

#include "LambdaPanel.h"

using namespace murka;

class Pane : public MurkaViewInterface<Pane> {
public:
    MURKA_VIEW_DRAW_FUNCTION  {

        auto params = (Parameters*)parametersObject;
        auto results = (Results*)resultObject;
        bool inside = context.isHovered() * !areInteractiveChildrenHovered(context) * hasMouseFocus(context);
        
        double* paneSplitValue = (double*)dataToControl;
        
        MurkaShape paneControlShape;
        
        if (params->kind == 1) {  // horizontal drawing
            drawWidget<LambdaPanel>(context,
                                    {[&](MurkaContext& c) { params->firstPanelDraw(c); }},
                                    {0, 0,
                                    context.getSize().x * float(*paneSplitValue),
                                    context.getSize().y});
            drawWidget<LambdaPanel>(context,
                                    {[&](MurkaContext& c) { params->secondPanelDraw(c); }},
                                    {context.getSize().x * float(*paneSplitValue), 0,
                                    context.getSize().x * float(1.0 - *paneSplitValue),
                                    context.getSize().y});
            paneControlShape = MurkaShape(context.getSize().x *
                                                     float(*paneSplitValue) - params->paneControlWidth / 2, 0,
                                                     params->paneControlWidth, context.getSize().y);
            params->drawPaneControl(paneControlShape,
                                    paneControlShape.inside(context.mousePosition, 5), draggingNow);
        }
        
        if (params->kind == 2) { // vertical drawing

            drawWidget<LambdaPanel>(context,
                                    {[params](MurkaContext& c) { params->firstPanelDraw(c); }},
                                    {0, 0,
                                    context.getSize().x,
                                    context.getSize().y * float(*paneSplitValue)});
            drawWidget<LambdaPanel>(context,
                                    {[params](MurkaContext& c) { params->secondPanelDraw(c); }},
                                    {0, context.getSize().y * float(*paneSplitValue),
                                    context.getSize().x,
                                    context.getSize().y * float(1.0 - *paneSplitValue)});
            paneControlShape = MurkaShape(0,
                                          context.getSize().y *
                                                     float(*paneSplitValue) - params->paneControlWidth / 2,
                                          context.getSize().x,
                                                     params->paneControlWidth);
            params->drawPaneControl(paneControlShape,
                                    paneControlShape.inside(context.mousePosition, 5), draggingNow);


        }
        
        if ((!draggingNow) && (!areInteractiveChildrenHovered(context)) && (paneControlShape.inside(context.mousePosition, 5)) && (context.mouseDownPressed[0])) {
            draggingNow = true;
        }
        
        if ((draggingNow) && (!context.mouseDown[0])) {
            draggingNow = false;
        }
        
        if (draggingNow) {
            if (params->kind == 1) { // horizontal dragging
                *paneSplitValue = context.mousePosition.x / context.getSize().x;
            }
            
            if (params->kind == 2) { // vertical dragging
                *paneSplitValue = context.mousePosition.y / context.getSize().y;
            }

            if (*paneSplitValue < params->minValue) {
                if (params->collapseable) {
                    *paneSplitValue = 0;
                } else {
                    *paneSplitValue = params->minValue;
                }
            }
            if (*paneSplitValue > params->maxValue) {
                if (params->collapseable) {
                    *paneSplitValue = 1.0;
                } else {
                    *paneSplitValue = params->maxValue;
                }
            }
        }
    };
    
    bool draggingNow = false;

    struct Parameters {
        std::function<void(MurkaContext&)> firstPanelDraw, secondPanelDraw;
        double minValue = 0.1, maxValue = 0.9;
        bool collapseable = true;
        double paneControlWidth = 8;
        int kind = 1; // 0 = nothing
                      // 1 = horizontal
                      // 2 = vertical
        
        std::function<void(MurkaShape, bool, bool)> drawPaneControl =
            [](MurkaShape shape, bool hover, bool draggingNow) {
                ofSetColor(80 + (hover || draggingNow) * 40 + draggingNow * 80);
                ofDrawRectangle(shape);
                
                ofSetColor(75 + hover * 100);
                ofDrawCircle(shape.x() + shape.width() / 2, shape.y() + shape.height() / 2, 4);
            };
    
        Parameters() {};
        
        Parameters(std::function<void(MurkaContext&)> first,
                   std::function<void(MurkaContext&)> second, int Kind) {
            firstPanelDraw = first;
            secondPanelDraw = second;
            kind = Kind;
        }
    };

    // The results type, you also need to define it even if it's nothing.
    typedef bool Results;


};
