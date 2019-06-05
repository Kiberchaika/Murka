#pragma once

#include <functional>
#include "MurkaShapes.h"
#include "MurkaInputEventsRegister.h"

// Here's the global typedefs for cross-render functionality

#ifdef MURKA_OF
#include "ofMain.h"
#endif

class MurkaContext /* TODO: public MurkaEventState */ {
public:
    MurkaContext() {
        
    }
    
    MurkaPoint getSize() const {
        return currentViewShape.size;
    }
    
    bool isHovered() const {
        return transformedShape.inside(eventState.mousePosition); // using absolute coordinates to calc that
    }
    
    void* murkaObject;
    MurkaEventState eventState;
    MurkaEventState previousEventState;
    
    // Pushes a container. Returns a context that you could then use if you want to draw there again later
    // the draw loop. Gets the context depth up a 1 point.
    void pushContainer(MurkaPoint containerPosition, MurkaPoint containerShape) {
        depth++;
        previousViewShape = currentViewShape;
        currentViewShape.position += containerPosition;
        
        previousEventState = eventState;
        eventState = eventState.transformedWith({-containerPosition.x,
                                                 -containerPosition.y}, 1.0);
        
        
        currentViewShape.size = containerShape;
        transformedShape = currentViewShape;
        transformedShape.position = {0, 0};
    }
    
    // Returns to the previous context state. Gets the context depth down a 1 point.
    
    void popContainer() {
        depth--;
        currentViewShape = previousViewShape;
        eventState = previousEventState;
    }
    
    // Utility function to transform the render into the shape of this context.
    // Helpful to draw the view innards.
    void transformTheRenderIntoThisContextShape() {
#ifdef MURKA_OF
        ofPushMatrix();
        ofTranslate(currentViewShape.position.x, currentViewShape.position.y);
#endif // MURKA_OF
    }
    
    void transformTheRenderBackFromThisContextShape() {
#ifdef MURKA_OF
        ofPopMatrix();
#endif // MURKA_OF
    }
    
    
    // All shapes are absolute
    
    MurkaShape rootViewShape;
    MurkaShape previousViewShape;
    MurkaShape currentViewShape;
    MurkaShape transformedShape;
    int depth = 0;
    
    MurkaShape* currentWidgetShapeSource;
    
    ///////////////////////////// TODO: this has to gather all the user input too
    
    /*
    MurkaPoint mousePosition;
    std::vector<std::tuple<MurkaPoint, bool>> fingerData; // TODO: finger type
//    bool didClick[3];
     */
};
