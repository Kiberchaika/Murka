#pragma once

#include "MurkaRenderer.h"
#include "MurkaInputEventsRegister.h"
#include "MurkaOverlayHolder.h"

namespace murka {

class MurkaContextBase: public MurkaEventState, public MurkaRenderer {
public:
    MurkaOverlayHolder* overlayHolder;
    MurkaRenderer* pointerToRenderer;
    
    // Assets access functions
    
    FontObject* getCurrentFont() const {
        return ((MurkaAssets*)pointerToRenderer)->getCurrentFont();
    }

    //
    
    MurkaShape getParentContextShape() const {
        return getParentContextShapeInternal();
    }
    
    std::function<MurkaShape()> getParentContextShapeInternal = []()->MurkaShape {
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
        return currentViewShape.transformedInside(mousePosition /* / renderer->getScreenScale() */); // using absolute coordinates to calc that
    }
    
    MurkaShape currentViewShape;
    
    // to add an overlay, give a lambda with the overlays call and an object that asked for it
    void addOverlay(std::function<void()> func, void* object) {
        overlayHolder->addOverlay(func, object);
    }
    
    std::function<int()> iterateHoverIndex = []()->int {return 0;};
    std::function<int()> getMaxHoverIndex = []()->int {return 0;};
    std::function<void(const MurkaContextBase*)> commitDeferredView_casted = [](const MurkaContextBase*){};

    template<typename T>
    T& draw(MurkaShape place) const {
        return drawWidget_NEW<T>(this, place);
    }

    void commitDeferredView() const {
        commitDeferredView_casted(this);
    }
};

}
