#pragma once
#include "Murka.h"
#include "MurkaBasicWidgets.h"

using namespace murka;

#define DOUBLECLICK_TIME 0.2

class MurkaSurface {
public:
    MurkaSurface() {
        
    }
    
    struct DragContext {
        float deltaX = 0., deltaY = 0.;
        float startX, startY;
        bool panActive = false;
        bool internalDragCaptureActive = false;
        int fingerIndex = 0; // for multitouch
        
        MurkaPoint panOffsetStart = {0, 0};
        MurkaPoint screenSpaceStart = {0, 0};
        MurkaPoint widgetSpaceDelta = {0, 0};
    };
    
    void surfaceUpdate(const MurkaContext & c) {
        
        bool inside = c.isHovered();
        
        auto transformedCursorPosition = getTransformedPointerPosition(c.mousePosition); // TODO: change this for multitouch

        if (c.mouseDownPressed && inside) {
            pointerDownCallback();
            
            float time = ofGetElapsedTimef();
            float timeSinceLastClick = time - lastClickTime;
            lastClickTime = time;
            
            bool doubleClick = false;
            if (timeSinceLastClick < DOUBLECLICK_TIME) {
                doubleClickCallback();
                doubleClick = true;
            }
            
            
            if ((!draggingData.panActive) && (!draggingData.internalDragCaptureActive)) {
                draggingData.startX = transformedCursorPosition.x;
                draggingData.startY = transformedCursorPosition.y;
                draggingData.panOffsetStart = panOffset;
                draggingData.screenSpaceStart = c.mousePosition;
                if (clickShouldStartPanning()) {
                    draggingData.panActive = true;
                } else {
                    draggingData.internalDragCaptureActive = true;
                }
            }
            // Activating dragging from here.
            // There are 2 kinds of dragging - surface dragging and dragging an object.
            // 1) Asking the child class whether we should pan after this click or not
            // ("implying" checking which objects are in the area of a click)
            // 2) If we should pan, start panning and create a structure for that. Otherwise, send the cursor data to the child instance that wants to
            // move some child object.
            // 3) On mouse release cease panning if we panned, call a callback in a child
            // to cease on their end.
        }
        
        if ((draggingData.panActive) || (draggingData.internalDragCaptureActive)) {
//            auto pPositionScreenspace = getPointerPositionForTransformedPosition({draggingData.startX, draggingData.startY});
            
            draggingData.widgetSpaceDelta = draggingData.screenSpaceStart - c.mousePosition;
            
            draggingData.deltaX = draggingData.startX - transformedCursorPosition.x;
            draggingData.deltaY = draggingData.startY - transformedCursorPosition.y;
            
            if (!c.mouseDown) {
                if (draggingData.panActive) {
                    ofLog() << "stopping pan.";
                }
                draggingData.panActive = false;
                draggingData.internalDragCaptureActive = false;
                
                pointerUpCallback();
            } else {
                dragUpdate(draggingData);
            }
            
            if (draggingData.panActive) {
                panOffset = draggingData.panOffsetStart + draggingData.widgetSpaceDelta;
            }
            
        }
        
        if (((c.mouseScroll.y != 0) ||
            (c.mouseScroll.x != 0)) && inside) {
            // Scrolling somewhere
            
            if (scrollwheelShouldZoom()) {
                auto previousScale = scale;
                
                scale -= c.mouseScroll.y * zoomSpeed;
                if (scale < 0.1) scale = 0.1;
                if (scale > 3.0) scale = 3.0;
                
                auto scaleDiff = scale - previousScale;
                
                MurkaPoint zoomPosition = {c.getSize().x / 2,
                                           c.getSize().y / 2};
                
                MurkaPoint mouseDiff = {0, 0};
                
                MurkaPoint zoomPositionInSurfaceSpaceNow = {(zoomPosition.x + panOffset.x) * scale,
                    (zoomPosition.y + panOffset.y) * scale
                };
                MurkaPoint zoomPositionInSurfaceSpaceBefore = {(zoomPosition.x + panOffset.x) * previousScale,
                    (zoomPosition.y + panOffset.y) * previousScale
                };

                mouseDiff = zoomPositionInSurfaceSpaceNow - zoomPositionInSurfaceSpaceBefore;
                
                if (scaleDiff != 0.0) {
                    MurkaPoint zoomTargetPoint = MurkaPoint(c.mousePosition.x + panOffset.x,
                                                            c.mousePosition.y + panOffset.y) / previousScale;

                    panOffset += MurkaPoint(zoomTargetPoint.x,
                                            zoomTargetPoint.y) * scaleDiff;
                    limitPan(); // probably should limit zoom/scale in relation to pan too, not only pan
                }
            } else
            if (scrollwheelShouldPan()) {
                panOffset -= MurkaPoint(c.mouseScroll.x, c.mouseScroll.y);
                limitPan();
            }
        }
         
        
        latestContextDebug = c;
    }
    
    MurkaContext latestContextDebug;
    
    virtual bool scrollwheelShouldZoom() {
        return false;
    }

    virtual bool scrollwheelShouldPan() {
        return false;
    }
    
    virtual bool clickShouldStartPanning() {
        return true;
    }

    virtual void pointerDownCallback() {
        ofLog() << "clickd from base class";
    }
    
    virtual bool isAnythingSelected() {};
    
    virtual void pointerUpCallback() {
        
    }
    
    virtual void dragUpdate(DragContext & c) {
        
    }
    
    virtual void doubleClickCallback() {
        ofLog() << "doubleclick!";
    }
    
    MurkaPoint getTransformedPointerPosition(MurkaPoint pointer) {
        return {(pointer.x + panOffset.x) / scale,
                (pointer.y + panOffset.y) / scale};
    }
    
    MurkaPoint getPointerPositionForTransformedPosition(MurkaPoint transformedPosition) {
        return {(transformedPosition.x * scale) - panOffset.x,
                (transformedPosition.y * scale) - panOffset.y};
    }
    
    MurkaShape panLimits = {-std::numeric_limits<float>::max() / 2,
                            -std::numeric_limits<float>::max() / 2,
                            std::numeric_limits<float>::max(),
                            std::numeric_limits<float>::max()};
    
    void limitPan() {
        if (panOffset.x < panLimits.position.x) {
            panOffset.x = panLimits.position.x;
        }
        if (panOffset.y < panLimits.position.y) {
            panOffset.y = panLimits.position.y;
        }
        if (panOffset.x > panLimits.position.x + panLimits.size.x) {
            panOffset.x = panLimits.position.x + panLimits.size.x;
        }
        if (panOffset.y > panLimits.position.y + panLimits.size.y) {
            panOffset.y = panLimits.position.y + panLimits.size.y;
        }
    }
    
    void drawDebugGrid() {
            ofSetColor(130);
            ofSetLineWidth(4);
            ofDrawLine(-10000, 0 - panOffset.y, 10000, 0 - panOffset.y);
            ofDrawLine(0 - panOffset.x, -10000, 0 - panOffset.x, 10000);
            
            ofSetLineWidth(1);
            
            for (int i = 0; i < 300; i++) {
                ofDrawLine(((i - 150) * 50) * scale - panOffset.x, -10000,
                           ((i - 150) * 50) * scale - panOffset.x, 10000);
                ofDrawLine( -10000, ((i - 150) * 50) * scale - panOffset.y,
                           10000, ((i - 150) * 50) * scale - panOffset.y);
            }
            
            ofSetColor(255, 0, 0);
            
            ofSetColor(255, 0, 0);
            ofPushMatrix();
            ofTranslate(-panOffset.x, -panOffset.y);
            ofScale(scale);

                // this is what you do to test that you could draw the 0 properly
                ofDrawCircle(0, 0, 25);
            /*
                bool hover = MurkaShape(100 * scale, 100 * scale,
                                        100 * scale, 100 * scale).
                                inside({latestContextDebug.mousePosition.x + panOffset.x,
                                        latestContextDebug.mousePosition.y + panOffset.y});
                ofSetColor(75 + 100 * hover, 20, 20);
                ofDrawRectangle(100 , 100 , 100 , 100);
             */
            
                ofDrawCircle(latestContextDebug.mousePosition.x / scale + panOffset.x / scale,
                             latestContextDebug.mousePosition.y / scale + panOffset.y / scale, 5);
                // this is the mouse coordinate in the virtual space, including the scale.
                // somehow this is different to the hover code above, hmm...
            
            ofPopMatrix();
            
            
            ofSetColor(0, 200, 60, 155);
            auto font = latestContextDebug.getMonospaceFont();
            font->drawString("scale: " + ofToString(scale), 50, 50);
            font->drawString("panOffset: " + ofToString(panOffset.x) + " : " + ofToString(panOffset.y), 0, 90);

    //        ofDrawBitmapString("scale: " + ofToString(scale), 50, 50);
        }
    
    
    MurkaPoint panOffset = {0, 0};
    float scale = 1.0;
    
    float zoomSpeed = 0.01;
    
    double lastClickTime = 0; // TODO: this, but for multitouch
    
    DragContext draggingData;
};
