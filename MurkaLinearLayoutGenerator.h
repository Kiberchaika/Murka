#pragma once

#include "MurkaTypes.h"

namespace murka {

enum ShapeAlignment {ALIGN_LEFT, ALIGN_RIGHT};
enum LayoutAnchoring {TOP, BOTTOM, LEFT, RIGHT};
    
#define MURKA_DEFAULT_AUTOLAYOUT_LINE_HEIGHT 30
    
class MurkaLinearLayoutGenerator {
public:
	MurkaLinearLayoutGenerator() {

	};

	MurkaShape boundingShape;
    MurkaShape externalShape;
    MurkaShape shapeAlreadyOccupied;
    MurkaShape availableShape;
    
    LayoutAnchoring layoutAnchoring = TOP;
    
    int cursorX = 0, cursorY = 0; // the cursor that we iterate through the graph
    float currentLineHeight = MURKA_DEFAULT_AUTOLAYOUT_LINE_HEIGHT, currentLineWidth = 0;
    float baseLayoutLineHeight = MURKA_DEFAULT_AUTOLAYOUT_LINE_HEIGHT;
    float linearOffset = 0;
    
    float spacing = 2;
    
    // Value means how many pixels. Proportion means how much of what's left
    enum ShapePartDescriptorKind { Value, Proportion };
    
    struct ShapePartDescriptor {
        ShapePartDescriptorKind kind;
        float value;
        ShapeAlignment alignment = ALIGN_LEFT;
        
        ShapePartDescriptor(int v) {
            kind = ShapePartDescriptorKind::Value;
            value = v;
        }

        ShapePartDescriptor(int v, ShapeAlignment align) {
            kind = ShapePartDescriptorKind::Value;
            value = v;
            alignment = align;
        }

        ShapePartDescriptor(float v) {
            kind = Proportion;
            value = v;
        }

        ShapePartDescriptor(float v, ShapeAlignment align) {
            kind = Proportion;
            value = v;
            alignment = align;
        }

        ShapePartDescriptor(double v) {
            kind = Proportion;
            value = v;
        }
        
        ShapePartDescriptor(double v, ShapeAlignment align) {
            kind = Proportion;
            value = v;
            alignment = align;
        }
    };
    
    std::vector<ShapePartDescriptor> currentShapeStructure = {1.0};
    
    bool extendingVertically = true;
    
	void restart(MurkaShape externalBounds) {
        
        currentShapeStructure.clear();
        setLayoutStructure({1.0});
        
        // Figuring out the actual bounds
        availableShape = externalBounds;
        
        linearOffset = 0;
        setLayoutLineHeight(MURKA_DEFAULT_AUTOLAYOUT_LINE_HEIGHT);
        linearOffset -= currentLineHeight;
        
        cursorX = 0;
        cursorY = -1;
        nextLine();
    }
    
    void setAnchoring(LayoutAnchoring newLayoutAnchoring) {
        layoutAnchoring = newLayoutAnchoring;
    }
    
    void setLayoutStructure(std::initializer_list<ShapePartDescriptor> list) {
        currentShapeStructure.clear();
        for (auto elem : list) {
            currentShapeStructure.push_back(elem);
        }
    }

    float getLayoutLineHeight() {
        return currentLineHeight;
    }

    void setLayoutLineHeight(float height) {
        baseLayoutLineHeight = height;
        
        if (cursorX == 0) {
            currentLineHeight = height;
        }
    }
    
    void setLinearOffset(float offset) {
        linearOffset = offset;
    }
    
    void addSpacing(float spacing) {
        linearOffset += spacing;
    }
    
    float remainingLineStart = 0, remainingLineEnd = 1;
    
    void nextLine() {
        cursorX = 0;
        cursorY += 1;
        remainingLineStart = 0;
        remainingLineEnd = availableShape.size.x;
        currentLineHeight = baseLayoutLineHeight;
        
        linearOffset += currentLineHeight;
    }

    // Going to call this first
    MurkaShape getNextShapeOffering() {
//        ofLog() << "line X: " << cursorX;
//        ofLog() << "line Y: " << cursorY;
        
        auto descriptor = currentShapeStructure[cursorX];
        MurkaShape resultingShape;
        resultingShape.position.y = linearOffset;
        resultingShape.size.y = currentLineHeight;
        
        if (descriptor.alignment == ALIGN_LEFT) {
            if (descriptor.kind == Proportion) {
                float w = (remainingLineEnd - remainingLineStart) * descriptor.value;
                resultingShape.position.x = remainingLineStart;
                resultingShape.size.x = w;
                
                remainingLineStart += w; // increasing the "cursor" X position by the widget width
            }
            if (descriptor.kind == Value) {
                resultingShape.position.x = remainingLineStart;
                resultingShape.size.x = descriptor.value;
                
                remainingLineStart += descriptor.value; // increasing the "cursor" X position by the widget width
            }
        }

        if (descriptor.alignment == ALIGN_RIGHT) {
            if (descriptor.kind == Proportion) {
                float w = (remainingLineEnd - remainingLineStart) * descriptor.value;
                resultingShape.position.x = remainingLineEnd - w;
                resultingShape.size.x = w;
                
                remainingLineEnd -= w; // increasing the "cursor" X position by the widget width
            }
            if (descriptor.kind == Value) {
                resultingShape.position.x = remainingLineEnd - descriptor.value;
                resultingShape.size.x = descriptor.value;
                
                remainingLineEnd -= descriptor.value; // increasing the "cursor" X position by the widget width
            }
        }

        cursorX ++;
        if (cursorX >= currentShapeStructure.size()) {
            nextLine();
        }

        resultingShape.position.x += spacing;
        resultingShape.position.y += spacing;
        resultingShape.size.x -= spacing * 2;
        resultingShape.size.y -= spacing * 2;

        return resultingShape;
    }

    // And this next
    MurkaShape getNextShape(MurkaShape desiredShape) {
        
    }

};

}
