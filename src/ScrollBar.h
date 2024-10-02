#pragma once

#include "MurkaTypes.h"
#include "MurkaContext.h"
#include "MurkaView.h"
#include "MurkaInputEventsRegister.h"
#include "MurkaLinearLayoutGenerator.h"
#include "MurkaSurface.h"

namespace murka {

	class ScrollBar : public View_NEW<ScrollBar> {
	public:
		void internalDraw(Murka & m) {
			MurkaContext& c = m.currentContext;

			bool inside = c.isHovered() * !areInteractiveChildrenHovered(c) * hasMouseFocus(m);

			if (kind == 0) { // Vertical scrollbar
				if (surfaceToControl->panLimits.size.y > 0) {

				   float scrollbarHeight = (c.getSize().y / (surfaceToControl->panLimits.size.y + c.getSize().y)) * c.getSize().y;
                
				   MurkaShape surfaceShape = ((View_NEW*)surfaceToControl)->shape;
                
				   MurkaShape handleShape = {0,
											  (c.getSize().y - scrollbarHeight) *
												(surfaceToControl->panOffset.y / (surfaceToControl->panLimits.size.y)),
												c.getSize().x,
												scrollbarHeight};
				   MurkaShape scrollbarShape = {0, 0, c.getSize().x, c.getSize().y};
                
				   drawScrollBar(c, scrollbarShape, handleShape, handleShape.inside(c.mousePosition), false);
                
					if (draggingNow) {
						surfaceToControl->panOffset.y -= (lastFrameMousePosition.y - c.mousePosition.y) * (c.getSize().y / scrollbarHeight);
						if (surfaceToControl->panOffset.y < 0) surfaceToControl->panOffset.y = 0;
						if (surfaceToControl->panOffset.y > surfaceToControl->panLimits.size.y) surfaceToControl->panOffset.y = surfaceToControl->panLimits.size.y;

					}
                
					if ((handleShape.inside(c.mousePosition)) && inside && c.mouseDownPressed[0]) {
						draggingNow = true;
					}

					if ((draggingNow) && (!c.mouseDown[0])) {
						draggingNow = false;
					}

					lastFrameMousePosition = c.mousePosition;
				}
			}
        
			if (kind == 1) { // Horizontal scrollbar
				if (surfaceToControl->panLimits.size.x > 0) {

				   float scrollbarWidth = (c.getSize().x / (surfaceToControl->panLimits.size.x + c.getSize().x)) * c.getSize().x;
                
				   MurkaShape surfaceShape = ((View_NEW*)surfaceToControl)->shape;
                
				   MurkaShape handleShape = {(c.getSize().x - scrollbarWidth) *
												(surfaceToControl->panOffset.x / (surfaceToControl->panLimits.size.x)),
												0,
												scrollbarWidth,
												c.getSize().x};
				   MurkaShape scrollbarShape = {0, 0, c.getSize().x, c.getSize().y};
                
				   drawScrollBar(c, scrollbarShape, handleShape, handleShape.inside(c.mousePosition), false);
                
					if (draggingNow) {
						surfaceToControl->panOffset.x += (lastFrameMousePosition.x - c.mousePosition.x) * (c.getSize().x / scrollbarWidth);
						if (surfaceToControl->panOffset.x < 0) surfaceToControl->panOffset.x = 0;
						if (surfaceToControl->panOffset.x > surfaceToControl->panLimits.size.x) surfaceToControl->panOffset.x = surfaceToControl->panLimits.size.x;
					}
                
					if ((handleShape.inside(c.mousePosition)) && inside && c.mouseDownPressed[0]) {
						draggingNow = true;
					}

					if ((draggingNow) && (!c.mouseDown[0])) {
						draggingNow = false;
					}

					lastFrameMousePosition = c.mousePosition;
				}
			}
		};
    
		bool draggingNow = false;
		MurkaPoint lastFrameMousePosition;
	//    double initialPanOffset = 0;

		MURKA_PARAMETER(ScrollBar, // class name
			MurkaSurface*, // parameter type
			surfaceToControl, // parameter variable name
			withSurfaceToControl, // setter
			nullptr // default
		)

		std::function<void(MurkaContext & c, MurkaShape scrollbarShape, MurkaShape handleShape, bool hover, bool draggingNow)> drawScrollBar = [](MurkaContext & c, MurkaShape scrollbarShape, MurkaShape handleShape, bool hover, bool draggingNow) {
			c.pointerToRenderer->setColor((80 + hover * 40 + draggingNow * 80));
			c.pointerToRenderer->drawRectangle(handleShape);
		};
		int kind = 0; // 0 = vertical
					  // 1 = horizontal
		bool results;
	};

} // end of namespace Murka
