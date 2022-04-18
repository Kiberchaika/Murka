#pragma once

#include "MurkaTypes.h"
#include "MurkaInputEventsRegister.h"
#include "MurkaViewHandler.h"
#include "MurkaTypes.h"
#include "MurkaOverlayHolder.h"
#include "MurkaRenderer.h"
#include "MurkaAnimator.h"
#include "ViewBase.h"
#include "MurkaContextBase.h"

#ifdef MURKA_OF
#include "ofMain.h"
#endif

namespace murka {

class MurkaContext: public MurkaContextBase {
    /*
private:
    MurkaContext(const MurkaContext&) {}
    MurkaContext& operator=(const MurkaContext&){}
     */
    
public:
    
    int drawingTicketId = -1;
   
    std::pair<MurkaShape, MurkaPoint> getCroppedViewport(MurkaShape parent, MurkaShape view) const {
        MurkaPoint pos = {(std::max)(parent.position.x + view.position.x, parent.position.x),
						  (std::max)(parent.position.y + view.position.y, parent.position.y)};
        MurkaPoint negativePosition = {view.position.x < 0 ? view.position.x : 0,
                                       view.position.y < 0 ? view.position.y : 0};
        MurkaPoint size = {(std::min)(view.size.x + negativePosition.x, parent.size.x - view.position.x),
						   (std::min)(view.size.y + negativePosition.y, parent.size.y - view.position.y)};
        
        if ((view.position.x > parent.size.x) ||
            (view.position.y > parent.size.y) ||
            (view.position.x < 0 - view.size.x) ||
            (view.position.y < 0 - view.size.y)) {
            return std::make_pair(MurkaShape(0,0,0,0), MurkaPoint(0, 0));
        }

        return std::make_pair(MurkaShape(pos.x, pos.y, size.x, size.y), negativePosition);
    }
    

    
    
    // All shapes are absolute
    MurkaShape* currentWidgetShapeSource; // this shape pointer points to a shape of the current
    // view that you could use inside the widget to reshape it if needed // TODO: get rid of this (v2)
    void* linkedView = nullptr; // the MurkaView that this context once represented
    
    ViewBase_NEW* linkedView_NEW = nullptr; // the MurkaView that this context last represented
    


    int getImCounter() {
        imCounter++;
        return imCounter;
    }
    
    void resetImCounter() {
        imCounter = 0;
    }
    
//    void pushContext(MurkaViewHandlerInternal* viewSource) {
//        pushContextInternal(viewSource);
//    }

    void pushContext_NEW(ViewBase_NEW* viewSource) {
        pushContextInternal_NEW(viewSource);
    }

    void popContext() {
        popContextInternal_NEW();
    }
    
    std::function<void(void*)> claimKeyboardFocus = [](void*) {return; };
    std::function<void(void*)> resetKeyboardFocus = [](void*) {return; };
    std::function<bool(void*)> checkKeyboardFocus = [](void*)->bool {return true; }; // aka "widget is allowed to use keyboard"

    // This uses Animator class because you can't include View from here
    std::function<void(ViewBase_NEW*)> pushContextInternal_NEW = [](ViewBase_NEW* v) {};
    std::function<void()> popContextInternal_NEW = []() {};

    double getRunningTime()  {return pointerToRenderer->getElapsedTime();}

private:
    int imCounter = 0; // the counter that we use to distinguish new widgets from the ones we
    // want to reuse in IM mode.
     
};

}
