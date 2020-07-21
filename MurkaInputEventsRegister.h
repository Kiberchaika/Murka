#pragma once

#include "MurkaTypes.h"

#ifdef MURKA_OF
#include "ofMain.h"

#ifdef TARGET_OSX
#include "mac/ofxMacTrackpad.h"
#endif

#endif



namespace murka {

struct MurkaEventState {
    bool mouseReleased[3] = {false, false, false},
         mouseDown[3] = {false, false, false},
         mouseDragged[3] = {false, false, false},
         mouseDownPressed[3] = {false, false, false},
         doubleClick = false;
    MurkaPoint mouseDraggedStartPoint = {0, 0}; // TODO: how?
    MurkaPoint mousePosition = {0, 0};
    MurkaPoint mouseDelta = {0, 0};
    MurkaPoint mouseScroll = {0, 0};
    MurkaPoint swipeDelta = {0, 0};
    MurkaPoint mouseDeltaSinceMouseLeftPressed = {0, 0};
    bool trackpadGesturePerformed = false;
    float pinchMagnification = 0;
    std::vector<int> keyPresses;
    
    MurkaEventState transformedWith(MurkaPoint translatePosition, float scale) {
        MurkaEventState newState = *this;
        newState.mousePosition += translatePosition;
//        newState.mouseDraggedStartPoint += translatePosition;
        
        return newState;
        // TODO: multiplied by scale?
    }
};

class MurkaInputEventsRegister {

    double lastLeftMousebuttonClicktime = -10000;
    MurkaPoint mousePositionWhenMouseLeftPressed = {0, 0};
public:
    
    MurkaEventState eventState;
    
    void setupEvents() {
    #ifdef MURKA_OF
            ofRegisterKeyEvents(this);
            ofRegisterMouseEvents(this);
            ofRegisterTouchEvents(this);
            
        #ifdef TARGET_OSX
                ofAddListener(ofxMacTrackpad::pinch, this, &MurkaInputEventsRegister::didPinch);
                ofAddListener(ofxMacTrackpad::swipe, this, &MurkaInputEventsRegister::didSwipe);
                ofAddListener(ofxMacTrackpad::multitouch, this, &MurkaInputEventsRegister::didMultitouch);

                ofxMacTrackpad::startListening();
                
                ofSetBackgroundAuto(true);

        #endif // TARGET_OSX - trackpad
        
    #endif // MURKA_OF
    }
    
    void clearEvents() {
        // mouse persistence
        MurkaPoint mousePosition = eventState.mousePosition;
        bool mouseDown[3] = {eventState.mouseDown[0],
                             eventState.mouseDown[1],
                             eventState.mouseDown[2]};
        
        eventState = MurkaEventState();
        eventState.mousePosition = mousePosition;
        eventState.mouseDown[0] = mouseDown[0];
        eventState.mouseDown[1] = mouseDown[1];
        eventState.mouseDown[2] = mouseDown[2];
    }

    // custom event registration
    
    void keyPressed(int key) {
        eventState.keyPresses.push_back(key);
    }
    
    void keyReleased(int key) {
    }
    
    void mouseDragged(int mouseX, int mouseY, int mouseButtonIndex) {
            eventState.mouseDragged[mouseButtonIndex] = true;
            eventState.mouseDelta = {eventState.mousePosition.x - mouseX,
                                     eventState.mousePosition.y - mouseY};
            eventState.mousePosition = {mouseX, mouseY};
            
            // Pressed > Released delta support
            
            if (mouseButtonIndex == 0) {
                eventState.mouseDeltaSinceMouseLeftPressed = eventState.mousePosition -  mousePositionWhenMouseLeftPressed;
            }
    }
        
        void mouseMoved(int mouseX, int mouseY, int mouseButtonIndex) {
            eventState.mouseDelta = {eventState.mousePosition.x - mouseX,
                eventState.mousePosition.y - mouseY};
            eventState.mousePosition = {mouseX, mouseY};
        }
        
        void mousePressed(int mouseX, int mouseY, int mouseButtonIndex) {
            eventState.mousePosition = {mouseX, mouseY};
            eventState.mouseDown[mouseButtonIndex] = true;
            eventState.mouseDownPressed[mouseButtonIndex] = true;
            
            // Pressed > Released delta support
            
            if (mouseButtonIndex == 0) {
                mousePositionWhenMouseLeftPressed = eventState.mousePosition;
            }
            
            // Doubleclick support
    /*
    #ifdef MURKA_OF
            if ((ofGetElapsedTimef() - lastLeftMousebuttonClicktime) < 0.2) {
                eventState.doubleClick = true;
            } else eventState.doubleClick = false;
            lastLeftMousebuttonClicktime = ofGetElapsedTimef();
    #endif
     */
        }
        
        void mouseReleased(int mouseX, int mouseY, int mouseButtonIndex) {
            eventState.mousePosition = {mouseX, mouseY};
            eventState.mouseReleased[mouseButtonIndex] = true;
            eventState.mouseDown[mouseButtonIndex] = false;

            // Pressed > Released delta support
            
            if (mouseButtonIndex == 0) {
                eventState.mouseDeltaSinceMouseLeftPressed = eventState.mousePosition -  mousePositionWhenMouseLeftPressed;
            }
        }
    
    // oF event registration
    
#ifdef MURKA_OF


    // oF Keyboard events
    
    void keyPressed(ofKeyEventArgs & args) {
        eventState.keyPresses.push_back(args.key);
    }
    
    void keyReleased(ofKeyEventArgs & args) {
    }
    
    // oF Trackpad events
    
#ifdef TARGET_OSX
	void didPinch(ofxMacTrackpad::PinchArg &arg) {
        eventState.pinchMagnification = arg.magnification;
    }
    
    void didSwipe(ofxMacTrackpad::SwipeArg &arg) {
        eventState.swipeDelta = {arg.delta.x, arg.delta.y};
    }

    void didMultitouch(ofxMacTrackpad::TouchArg &arg) {
        eventState.trackpadGesturePerformed = true;
    }
#endif

    // oF Mouse events
    
    void mouseDragged(ofMouseEventArgs & args) {
        eventState.mouseDragged[args.button] = true;
        eventState.mouseDelta = {eventState.mousePosition.x - args.x,
                                 eventState.mousePosition.y - args.y};
        eventState.mousePosition = {args.x, args.y};
        
        // Pressed > Released delta support
        
        if (args.button == 0) {
            eventState.mouseDeltaSinceMouseLeftPressed = eventState.mousePosition -  mousePositionWhenMouseLeftPressed;
        }
    }
    
    void mouseMoved(ofMouseEventArgs & args) {
        eventState.mouseDelta = {eventState.mousePosition.x - args.x,
            eventState.mousePosition.y - args.y};
        eventState.mousePosition = {args.x, args.y};
    }
    
    void mousePressed(ofMouseEventArgs & args) {
        eventState.mousePosition = {args.x, args.y};
        eventState.mouseDown[args.button] = true;
        eventState.mouseDownPressed[args.button] = true;
        
        // Pressed > Released delta support
        
        if (args.button == 0) {
            mousePositionWhenMouseLeftPressed = eventState.mousePosition;
        }
        
        // Doubleclick support
        
#ifdef MURKA_OF
        if ((ofGetElapsedTimef() - lastLeftMousebuttonClicktime) < 0.2) {
            eventState.doubleClick = true;
        } else eventState.doubleClick = false;
        lastLeftMousebuttonClicktime = ofGetElapsedTimef();
#endif

    }
    
    void mouseReleased(ofMouseEventArgs & args) {
        eventState.mousePosition = {args.x, args.y};
        eventState.mouseReleased[args.button] = true;
        eventState.mouseDown[args.button] = false;

        // Pressed > Released delta support
        
        if (args.button == 0) {
            eventState.mouseDeltaSinceMouseLeftPressed = eventState.mousePosition -  mousePositionWhenMouseLeftPressed;
        }
    }
    
    void mouseScrolled(ofMouseEventArgs & args) {
        eventState.mouseScroll = {args.scrollX, args.scrollY};
    }
    
    void mouseEntered(ofMouseEventArgs & args) {

                
#ifdef TARGET_OSX
        ofxMacTrackpad::startListening();
#endif
        
        
    }
    
    void mouseExited(ofMouseEventArgs & args) {}
    
    // oF Touch events
    
    void touchDown(ofTouchEventArgs & args) {}
    
    void touchMoved(ofTouchEventArgs & args) {}
    
    void touchUp(ofTouchEventArgs & args) {}
    
    void touchCancelled(ofTouchEventArgs & args) {}
    
    void touchDoubleTap(ofTouchEventArgs & args) {}
    
#endif // MURKA_OF

};

}
