#pragma once

#include "MurkaTypes.h"

#ifdef MURKA_OF
#include "ofMain.h"
#endif

namespace murka {

struct MurkaEventState {
    bool mouseUp = false, mouseDown = false, mouseDragged = false, mouseDownPressed = false;
    MurkaPoint mouseDraggedStartPoint = {0, 0}; // TODO: how?
    MurkaPoint mousePosition = {0, 0};
    MurkaPoint mouseDelta = {0, 0};
    std::vector<int> keyPresses;
    
    MurkaEventState transformedWith(MurkaPoint translatePosition, float scale) {
        MurkaEventState newState = *this;
        newState.mousePosition += translatePosition;
//        newState.mouseDraggedStartPoint += translatePosition;
        
        return newState;
        // TODO: multiplied by scale
    }
};

class MurkaInputEventsRegister {

public:
    
#ifdef MURKA_OF

    void setupEvents() {
        ofRegisterKeyEvents(this);
        ofRegisterMouseEvents(this);
        ofRegisterTouchEvents(this);
    }
    
    MurkaEventState eventState;
    
    void clearEvents() {
        // mouse persistence
        MurkaPoint mousePosition = eventState.mousePosition;
        bool mouseDown = eventState.mouseDown;
        
        eventState = MurkaEventState();
        eventState.mousePosition = mousePosition;
        eventState.mouseDown = mouseDown;
    }
    

    // Keyboard events
    
    void keyPressed(ofKeyEventArgs & args) {
        eventState.keyPresses.push_back(args.key);
    }
    
    void keyReleased(ofKeyEventArgs & args) {
    }
    
    // Mouse events
    
    void mouseDragged(ofMouseEventArgs & args) {
        eventState.mouseDragged = true;
        eventState.mouseDelta = {eventState.mousePosition.x - args.x,
                                 eventState.mousePosition.y - args.y};
        eventState.mousePosition = {args.x, args.y};
    }
    
    void mouseMoved(ofMouseEventArgs & args) {
        eventState.mouseDelta = {eventState.mousePosition.x - args.x,
            eventState.mousePosition.y - args.y};
        eventState.mousePosition = {args.x, args.y};
    }
    
    void mousePressed(ofMouseEventArgs & args) {
        eventState.mousePosition = {args.x, args.y};
        eventState.mouseDown = true;
        eventState.mouseDownPressed = true;
    }
    
    void mouseReleased(ofMouseEventArgs & args) {
        eventState.mousePosition = {args.x, args.y};
        eventState.mouseUp = true;
        eventState.mouseDown = false;
    }
    
    void mouseScrolled(ofMouseEventArgs & args) {}
    
    void mouseEntered(ofMouseEventArgs & args) {}
    
    void mouseExited(ofMouseEventArgs & args) {}
    
    // Touch events
    
    void touchDown(ofTouchEventArgs & args) {}
    
    void touchMoved(ofTouchEventArgs & args) {}
    
    void touchUp(ofTouchEventArgs & args) {}
    
    void touchCancelled(ofTouchEventArgs & args) {}
    
    void touchDoubleTap(ofTouchEventArgs & args) {}
    
#endif 

};

}
