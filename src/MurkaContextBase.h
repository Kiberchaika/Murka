#pragma once

#include "MurkaRenderer.h"
#include "MurkaInputEventsRegister.h"
#include "MurkaOverlayHolder.h"

namespace murka {

class MurkaContextBase: public MurkaEventState {
public:
    MurkaOverlayHolder* overlayHolder;
    
    
    MurkaShape getParentContextShape() const {
        return getParentContextShapeInternal();
    }
    
    std::function<MurkaShape()> getParentContextShapeInternal = []()->MurkaShape {
        // This is set by the main Murka class when the context is created
        return MurkaShape();
    };

    MurkaPoint getPosition() const {
        return {currentViewShape.position.x + getParentContextShape().position.x,
                currentViewShape.position.y + getParentContextShape().position.y};
    }
    
    MurkaPoint getSize() const {
        return currentViewShape.size;
    }
    
    bool isHovered() const {
        return currentViewShape.transformedInside(mousePosition); // using absolute coordinates to calc that
    }
    
    MurkaShape currentViewShape;
    
    // to add an overlay, give a lambda with the overlays call and an object that asked for it
    void addOverlay(std::function<void()> func, void* object) {
        overlayHolder->addOverlay(func, object);
    }
    
    std::function<int()> iterateHoverIndex = []()->int {return 0;};
    std::function<int()> getMaxHoverIndex = []()->int {return 0;};
};

}
