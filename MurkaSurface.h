#pragma once
#include "Murka.h"
#include "MurkaBasicWidgets.h"

using namespace murka;

#define DOUBLECLICK_TIME 0.2

class MurkaSurface {
public:
    MurkaSurface() {
        
    }
    
    struct GestureContext {
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
        
        if ((c.trackpadGesturePerformed) && (c.mouseScroll.length() > 0)) {
            performingMultitouchZoomNow = true;
        }
        
#ifdef MURKA_OF
        float currentTime = ofGetElapsedTimef(); // time in seconds
#endif
        
        if (c.mouseScroll.length() != 0.0) {
            lastMouseScrollTime = currentTime;
        }
        
        if ((performingMultitouchZoomNow) && ((currentTime - lastMouseScrollTime) > 0.2)) { // if it's more than 0.2 secs till the last scroll info
            performingMultitouchZoomNow = false; // this means the trackpad gesture stopped. Need this due to how trackpad zoom acceleration works on OSX
        }
        
        if (c.mouseReleased[0]) {
            surface.pointerUpCallback(0, transformedCursorPosition);
        }

        if (c.mouseDownPressed[0] && inside) {
            surface.pointerDownCallback(0, transformedCursorPosition);
            
            float time = ofGetElapsedTimef();
            float timeSinceLastClick = time - lastClickTime;
            lastClickTime = time;
            
            if (timeSinceLastClick < DOUBLECLICK_TIME) {
                surface.doubleClickCallback(transformedCursorPosition);
            }
            
            
            if ((!draggingData.panActive) && (!draggingData.internalDragCaptureActive)) {
                draggingData.startX = transformedCursorPosition.x;
                draggingData.startY = transformedCursorPosition.y;
                draggingData.panOffsetStart = panOffset;
                draggingData.screenSpaceStart = c.mousePosition;
                if (surface.clickShouldStartPanning) {
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
            
            if (!c.mouseDown[0]) {
                if (draggingData.panActive) {
//                    ofLog() << "stopping pan.";
                }
                draggingData.panActive = false;
                draggingData.internalDragCaptureActive = false;
                
//                pointerUpCallback(0);
            }
            
            if (draggingData.panActive) {
                panOffset = draggingData.panOffsetStart + draggingData.widgetSpaceDelta;
                limitPan();
            }
            
        }
        
        if ((c.pinchMagnification != 0) && (inside)) {
            zoom(-c.pinchMagnification * 150 * scale, c);
        }
        
        if (((c.mouseScroll.y != 0) ||
            (c.mouseScroll.x != 0)) && inside) {
            // Scrolling somewhere
            
            if (!performingMultitouchZoomNow) { // checking if this was trackpad gesture
//                ofLog() << "no multitouch.";
                if (surface.scrollwheelShouldZoom) {
                    zoom(c.mouseScroll.y * 5, c);
                } else
                if (surface.scrollwheelShouldPan) {
                    panOffset -= MurkaPoint(c.mouseScroll.x, c.mouseScroll.y);
                    limitPan();
                }
            } else { // this was a trackpad swipe gesture, we should def pan then
                panOffset -= MurkaPoint(c.mouseScroll.x, c.mouseScroll.y) * 3;
                limitPan();
            }
        }
         
        
        latestContextDebug = c;
        surface = SurfaceControl();
    }
    
    void zoom(float zoomScale, const MurkaContext & c) {
        auto previousScale = scale;
        
        scale -= zoomScale * zoomSpeed;
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
    }
    
    MurkaContext latestContextDebug;

    // Surface parameters
    
    struct SurfaceControl {
        bool scrollwheelShouldZoom = false;
        bool scrollwheelShouldPan = false;
        bool clickShouldStartPanning = false;
        
        std::function<void(MurkaPoint)> doubleClickCallback = [](MurkaPoint p){};
        std::function<void(int, MurkaPoint)> pointerDownCallback = [](int button, MurkaPoint p){};
        std::function<void(int, MurkaPoint)> pointerUpCallback = [](int button, MurkaPoint p){};
    };
    
    SurfaceControl surface;
    
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
            ofSetColor(80);
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
    
    bool performingMultitouchZoomNow = false;
    
    MurkaPoint panOffset = {0, 0};
    float scale = 1.0;
    
    float zoomSpeed = 0.01;
    
    double lastClickTime = 0; // TODO: this, but for multitouch
    double lastMouseScrollTime = 0;
    
    GestureContext draggingData;
};
