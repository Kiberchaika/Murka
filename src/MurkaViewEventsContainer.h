#include "MurkaContext.h"
#include "MurkaInputEventsRegister.h"

class MurkaViewEventsContainer {
    MurkaEventState viewEventsState;
    
public:
    MurkaViewEventsContainer() {
        
    }
    
    void updateEvents(MurkaEventState e) {
        viewEventsState = e;
    }
    
    bool mouseReleased(int index) {
        return viewEventsState.mouseReleased[index];
    }
    
    bool mouseDown(int index) {
        return viewEventsState.mouseDown[index];
    }
    
    bool mouseDragged(int index) {
        return viewEventsState.mouseDragged[index];
    }
    
    bool mouseDownPressed(int index) {
        return viewEventsState.mouseDownPressed[index];
    }
    
    bool doubleClick() {
        return viewEventsState.doubleClick;
    }
    
    MurkaPoint mouseDraggedStartPoint() {
        return viewEventsState.mouseDraggedStartPoint;
    }
    
    MurkaPoint mousePosition() {
        return viewEventsState.mousePosition;
    }
    
    MurkaPoint mouseDelta() {
        return viewEventsState.mouseDelta;
    }
    
    MurkaPoint mouseScroll() {
        return viewEventsState.mouseScroll;
    }
    
    MurkaPoint swipeDelta() {
        return viewEventsState.swipeDelta;
    }
    
    MurkaPoint mouseDeltaSinceMouseLeftPressed() {
        return viewEventsState.mouseDeltaSinceMouseLeftPressed;
    }
    
    bool trackpadGesturePerformed() {
        return viewEventsState.trackpadGesturePerformed;
    }
    
    float pinchMagnification() {
        return viewEventsState.pinchMagnification;
    }
    
    // The keys that were just pressed
    std::vector<int> keyPresses() {
        return viewEventsState.keyPresses;
    }
    
    // The keys that were just released
    std::vector<int> keyReleases() {
        return viewEventsState.keyReleases;
    }
    
    // The keys that were presed in the past frames and not yet released
    std::vector<int> keyHolds() {
        return viewEventsState.keyHolds;
    }

    bool isKeyHeld(int key) {
        return std::find(std::begin(viewEventsState.keyHolds), std::end(viewEventsState.keyHolds), key) != std::end(viewEventsState.keyHolds);
    }

    bool isKeyPressed(int key) {
        return std::find(std::begin(viewEventsState.keyPresses), std::end(viewEventsState.keyPresses), key) != std::end(viewEventsState.keyPresses);
    }

    bool isKeyReleased(int key) {
        return std::find(std::begin(viewEventsState.keyReleases), std::end(viewEventsState.keyReleases), key) != std::end(viewEventsState.keyReleases);
    }

};
