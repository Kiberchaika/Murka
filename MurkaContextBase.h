
#pragma once

namespace murka {

class MurkaContextBase: public MurkaEventState {
public:
    MurkaOverlayHolder* overlayHolder;
    MurkaRenderer* renderer;
    
    // Assets access functions
    
    FontObject* getCurrentFont() const {
        return ((MurkaAssets*)renderer)->getCurrentFont();
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
    

};

};