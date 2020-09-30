#pragma once

#include "MurkaTypes.h"
#include "MurkaContext.h"
#include "MurkaViewHandler.h"
#include "Murka.h"
#include "MurkaLinearLayoutGenerator.h"
#include "MurkaAnimator.h"
#include "MosaicLayout.h"

namespace murka {
    
// Data, parameters and the context are what you put inside. Optional return is put into void*.
typedef std::function<void (void* dataToControl,
                            void* parametersObject,
                            void* thisWidgetObject,
                            MurkaContext & context,
                            void* resultObject)> viewDrawFunction;

#define MURKA_VIEW_DRAW_FUNCTION void draw(void* dataToControl, void* parametersObject, void* thisWidgetObject, MurkaContext & context, void* resultObject)

// // View heirarchy

// View is non-template class that holds everything that any widget should subclass (wantsClicks) and hold.
// MurkaViewInterface class is a templated class that wraps View and adds some functions needed for Murka
// to allocate resources for this widget.

class View: public MurkaAnimator {
public:
    
    // This variable is needed to support immediate mode widgets resizing themselves while also receiving sizes from outside.
    MurkaShape latestShapeThatCameFromOutside;
    
    
    virtual bool wantsClicks() { return true; } // override this if you want to signal to other widgets that you don't want clicks
    
public:
    View() {
        
    }
    
    ~View() {
        // TODO: delete children
    }
    
    
    // This children list contains all the children objects and their respective data
    std::vector<MurkaViewHandlerInternal*> children;
    
    // This tuple is how Murka identifies the IM mode widget object.
    // Int stands for object drawing index, void* is a pointer to data, string is typeid
    typedef std::tuple<int, void*, std::string> imIdentifier;
    std::map<imIdentifier, MurkaViewHandler<View>*> imChildren; // Immediate mode widget objects that were once drawn inside this widget. We hold their state here in the shape of their MurkaViews.
    
    MurkaViewHandlerInternal* latestDrawnIMModeWidgetObjectHandler;

    
    void clearChildren() {
        for (auto &i: children) {
            ((View*)i->widgetObjectInternal)->clearChildren();
            delete i;
        }
        children.clear();
    }
    
    MurkaShape childrenBounds = {std::numeric_limits<float>::max(), std::numeric_limits<float>::max(),
                                 0, 0};
    
    void resetChildrenBounds() {
        childrenBounds = {std::numeric_limits<float>::max(), std::numeric_limits<float>::max(),
            0, 0};
    }
    
    int hoverIndexCache = 0;
    
    bool hasMouseFocus(MurkaContext & context) {
        bool inside = context.isHovered() * !areInteractiveChildrenHovered(context);
        
        bool pass = false;
        if (inside && wantsClicks()) {
            auto itr = context.iterateHoverIndex();
            if (hoverIndexCache != context.getMaxHoverIndex()) pass = false;
                else pass = true;
                
            hoverIndexCache = itr;
        }
        
        return pass;
    }


    bool areInteractiveChildrenHovered(MurkaContext & c) {
        if (!c.isHovered()) {
            return false;
        }
        
        for (auto i: children) {
            auto shape = ((View*)i->widgetObjectInternal)->shape;
            shape.position = ((View*)i->widgetObjectInternal)->shape.position;
            
            if ((shape.inside(c.mousePosition)) && (((View*)i->widgetObjectInternal)->wantsClicks())) {
                return true;
            }
        }
        
        
        int index = 0;
        std::map<imIdentifier, MurkaViewHandler<View>*>::iterator it;
        for (it = imChildren.begin(); it != imChildren.end(); it++) {
            auto shape = ((View*)it->second->widgetObjectInternal)->shape;
            shape.position = ((View*)it->second->widgetObjectInternal)->shape.position;
            
            if ((shape.inside(c.mousePosition)) && (((View*)it->second->widgetObjectInternal)->wantsClicks())) {
                return true;
            }
        }
        
        return false;
    }

    bool areChildrenHovered(MurkaContext c) {
        if (!c.isHovered()) {
            return false;
        }
        
        for (auto i: children) {
            auto shape = ((View*)i->widgetObjectInternal)->shape;
            shape.position = ((View*)i->widgetObjectInternal)->shape.position;
            
            if (shape.inside(c.mousePosition)) {
                return true;
            }
        }
        
        
        int index = 0;
        std::map<imIdentifier, MurkaViewHandler<View>*>::iterator it;
        for (it = imChildren.begin(); it != imChildren.end(); it++) {
            auto shape = ((View*)it->second->widgetObjectInternal)->shape;
            shape.position = ((View*)it->second->widgetObjectInternal)->shape.position;
            
            if (shape.inside(c.mousePosition)) {
                return true;
            }
        }

        return false;
    }

    //////////////////////////////////////////////// The following is to be overriden by children classes
    
    // A parameters initialiser, meant to be overriden. Gets called if Murka wants widget to allocate memory for its parameters.
    void* returnNewParametersObject() {
        return new View();
    }
    
    void* returnNewResultsObject() {
        return new bool(false);
    }
    
    void* returnNewWidgetObject() {
        return new View();
    }
    
//    viewDrawFunction draw = [](void* dataToControl,
//                               void* parametersObject,
//                               void* thisWidgetObject,
//                            MurkaContext & context,
//                               void* resultObject) {
//        ofLog() << "drawing an empty func...";
//        return new bool(false);
//    };
    
    virtual MURKA_VIEW_DRAW_FUNCTION {
        ofLog() << "drawing an empty func...";
    }
    
    struct Parameters {};
    struct Results {};
    
    void* parametersInternal;
    void* resultsInternal;
    std::string dataTypeName = "";
    
    MurkaShape shape = {0, 0, 1, 2};
    
    MurkaLinearLayoutGenerator linearLayout;
    MosaicLayout mosaicLayout;
    
    MurkaContext latestContext;
};

// MurkaViewInterface is used to add functions that automatically create resources and widget objects for any widget T.
// It's meant to be used in a child class definition like this:
//
// class Foo : public MurkaViewInterface<Foo>

template<class T>
class MurkaViewInterface: public View {
public:
    
    void* returnNewResultsObject() {
        return new typename T::Results();
    }
    
    void* returnNewWidgetObject() {
        return new T();
    }
    
    
    static MurkaViewHandler<T>* getLatestDrawn(View* parentWidget) {
        return (MurkaViewHandler<T>*)parentWidget->latestDrawnIMModeWidgetObjectHandler;
    }
    
    static MurkaViewHandler<T>* getOrCreateImModeWidgetObject(int index, void* data, View* parentWidget, MurkaShape shape) {
        
        auto idTuple = std::make_tuple(index, data, typeid(T).name());
        if (parentWidget->imChildren.find(idTuple) != parentWidget->imChildren.end()) {
            // the widget exists
            
//            ofLog() << "returning the object";
            
            auto imChild = ((MurkaViewHandler<T>*)parentWidget->imChildren[idTuple]);
            auto widget = (T*)imChild->widgetObjectInternal;
            
            if (widget->latestShapeThatCameFromOutside != shape) {
                // updating shape cause something else came from outside
                
                widget->latestShapeThatCameFromOutside = shape;
                widget->shape = shape;
            }

            // Updating children bounds
            if (parentWidget->childrenBounds.position.x > widget->shape.position.x) {
                parentWidget->childrenBounds.position.x = widget->shape.position.x;
//                ofLog() << "updated cb x (2) to " << shape.position.x;
            }
            if (parentWidget->childrenBounds.position.y > widget->shape.position.y) {
                parentWidget->childrenBounds.position.y = widget->shape.position.y;
//                ofLog() << "updated cb y (2) to " << shape.position.y;
            }
            if ((parentWidget->childrenBounds.size.x + parentWidget->childrenBounds.position.x) < (widget->shape.position.x + widget->shape.size.x)) {
                parentWidget->childrenBounds.size.x = widget->shape.position.x + widget->shape.size.x - parentWidget->childrenBounds.position.x;
//                ofLog() << "updated cb sx (2) to " << parentWidget->childrenBounds.size.x;
            }
            
            if ((parentWidget->childrenBounds.size.y + parentWidget->childrenBounds.position.y) < widget->shape.position.y + widget->shape.size.y) {
                parentWidget->childrenBounds.size.y = widget->shape.position.y + widget->shape.size.y - parentWidget->childrenBounds.position.y;
            }

            parentWidget->latestDrawnIMModeWidgetObjectHandler = (MurkaViewHandler<T>*)parentWidget->imChildren[idTuple];
            return (MurkaViewHandler<T>*)parentWidget->imChildren[idTuple];
        } else {
            auto newWidget = new T();
            auto resultsObject = newWidget->returnNewResultsObject();
            newWidget->shape = shape;
            newWidget->latestShapeThatCameFromOutside = shape;
            
            MurkaViewHandler<T>* newHandler = new MurkaViewHandler<T>();
            newHandler->resultsInternal = resultsObject;
            newHandler->dataToControl = data;
            newHandler->widgetObjectInternal = newWidget;
            
            parentWidget->imChildren[idTuple] = (MurkaViewHandler<View> *)newHandler;

            // Updating children bounds (this is needed for automatic scrolling and things like that -
            // basically informing parent widget of how much space does its children occupy)
            if (parentWidget->childrenBounds.position.x > shape.position.x) {
                parentWidget->childrenBounds.position.x = shape.position.x;
//                ofLog() << "updated cb x (1) to " << shape.position.x;
            }
            if (parentWidget->childrenBounds.position.y > shape.position.y) {
                parentWidget->childrenBounds.position.y = shape.position.y;
//                ofLog() << "updated cb y (1) to " << shape.position.y;
            }
            if ((parentWidget->childrenBounds.size.x + parentWidget->childrenBounds.position.x) < (shape.position.x + shape.size.x)) {
                parentWidget->childrenBounds.size.x = shape.position.x + shape.size.x - parentWidget->childrenBounds.position.x;
//                ofLog() << "updated cb sx (2) to " << parentWidget->childrenBounds.size.x;
            }
            
            if ((parentWidget->childrenBounds.size.y + parentWidget->childrenBounds.position.y) < (shape.position.y + shape.size.y)) {
                parentWidget->childrenBounds.size.y = shape.position.y + shape.size.y - parentWidget->childrenBounds.position.y;
            }
            
            parentWidget->latestDrawnIMModeWidgetObjectHandler = newHandler;
            return newHandler;
        }
    }
    
    static MurkaViewInterface<T>::Results imDrawST2(MurkaContext &c, void* dataToControl, MurkaViewInterface<T>::Parameters parameters, MurkaShape shape) {
    }

};

// New API

template<typename T>
T& drawWidget(MurkaContext &c, MurkaShape shape) {

    //        auto context = &(m.currentContext);
    int counter = c.getImCounter();

    
    auto parentView = (View*)c.murkaView;
    auto widgetHandler = T::getOrCreateImModeWidgetObject(counter, NULL, parentView, shape);
    auto widgetObject = (View*)widgetHandler->widgetObjectInternal;
    
    
    typename T::Results results = typename T::Results();
    
    typename T::Parameters p();
    
    c.pushContext(widgetHandler);
    if (c.transformTheRenderIntoThisContextShape(c.overlayHolder->disableViewportCrop)) {
        widgetObject->linearLayout.restart(((View*)widgetHandler->widgetObjectInternal)->shape);
            widgetObject->draw(NULL, &p, widgetObject, c, &results);
        widgetObject->animationRestart();
        widgetObject->mosaicLayout.restart();
        
        /*
        //DEBUG - drawing the children frame that we had at the last frame end
        ofSetColor(255, 100, 0);
            ofNoFill();

        ofDrawRectangle(((View*)c.murkaView)->childrenBounds.position.x, ((View*)c.murkaView)->childrenBounds.position.y, ((View*)c.murkaView)->childrenBounds.size.x, ((View*)c.murkaView)->childrenBounds.size.y);
            ofFill();
        //////
         */

        
        c.transformTheRenderBackFromThisContextShape();
    }
    c.popContext();
        
    widgetObject->resetChildrenBounds();
        

    return *widgetObject;
}


// Immediate mode custom layout
    
template<typename T>
typename T::Results drawWidget(MurkaContext &c, typename T::Parameters parameters, MurkaShape shape) {

    //        auto context = &(m.currentContext);
    int counter = c.getImCounter();

    
    auto parentView = (View*)c.murkaView;
    auto widgetHandler = T::getOrCreateImModeWidgetObject(counter, NULL, parentView, shape);
    auto widgetObject = (View*)widgetHandler->widgetObjectInternal;
    
    
    typename T::Results results = typename T::Results();
    
    c.pushContext(widgetHandler);
    if (c.transformTheRenderIntoThisContextShape(c.overlayHolder->disableViewportCrop)) {
        widgetObject->linearLayout.restart(((View*)widgetHandler->widgetObjectInternal)->shape);
            widgetObject->draw(NULL, &parameters, widgetObject, c, &results);
        widgetObject->animationRestart();
        widgetObject->mosaicLayout.restart();
        
        /*
        //DEBUG - drawing the children frame that we had at the last frame end
        ofSetColor(255, 100, 0);
            ofNoFill();

        ofDrawRectangle(((View*)c.murkaView)->childrenBounds.position.x, ((View*)c.murkaView)->childrenBounds.position.y, ((View*)c.murkaView)->childrenBounds.size.x, ((View*)c.murkaView)->childrenBounds.size.y);
            ofFill();
        //////
         */

        
        c.transformTheRenderBackFromThisContextShape();
    }
    c.popContext();
        
    widgetObject->resetChildrenBounds();
        

    return results;
}

template<typename T, class B>
typename T::Results drawWidget(MurkaContext &c, B* dataToControl, typename T::Parameters parameters, MurkaShape shape) {

    int counter = c.getImCounter();
    
    auto mView = (View*)c.murkaView;
    auto widgetHandler = T::getOrCreateImModeWidgetObject(counter, NULL, (View*)c.murkaView, shape);
    auto widgetObject = (View*)widgetHandler->widgetObjectInternal;
    
    widgetObject->dataTypeName = typeid(dataToControl).name();
    
    typename T::Results results = typename T::Results();
    
    c.pushContext(widgetHandler);
    if (c.transformTheRenderIntoThisContextShape(c.overlayHolder->disableViewportCrop)) {
    
        widgetObject->linearLayout.restart(shape);
        widgetObject->animationRestart();
        widgetObject->mosaicLayout.restart();
        widgetObject->draw(dataToControl, &parameters, widgetObject, c, &results);
        
        //DEBUG
        /*
        ofSetColor(255, 100, 0);
            ofNoFill();
        ofDrawRectangle(((View*)c.murkaView)->childrenBounds.position.x, ((View*)c.murkaView)->childrenBounds.position.y, ((View*)c.murkaView)->childrenBounds.size.x, ((View*)c.murkaView)->childrenBounds.size.y);
            ofFill();
         */
        //////
        
        c.transformTheRenderBackFromThisContextShape();
    }
    c.popContext();

    widgetObject->resetChildrenBounds();
    

    return results;

}
    
// Immediate mode automatic layout

template<typename T, class B>
typename T::Results drawWidget(MurkaContext &c, B* dataToControl, typename T::Parameters parameters) {
    
    int counter = c.getImCounter();
    
    auto parentMView = (View*)c.murkaView;
    
    auto shapeOffering = parentMView->linearLayout.getNextShapeOffering();
    
    auto widgetHandler = T::getOrCreateImModeWidgetObject(counter, NULL, (View*)c.murkaView, shapeOffering);
    auto widgetObject = (View*)widgetHandler->widgetObjectInternal;

    widgetObject->dataTypeName = typeid(dataToControl).name();

    typename T::Results results;
    
    c.pushContext(widgetHandler);
    if (c.transformTheRenderIntoThisContextShape(c.overlayHolder->disableViewportCrop)) {
        
        widgetObject->linearLayout.restart(shapeOffering);
        widgetObject->animationRestart();
        widgetObject->mosaicLayout.restart();
        widgetObject->draw(dataToControl, &parameters, widgetObject, c, &results);
        
        c.transformTheRenderBackFromThisContextShape();
    }
    c.popContext();
    
    widgetObject->resetChildrenBounds();
    
    return results;
}
    
template<typename T>
typename T::Results drawWidget(MurkaContext &c, typename T::Parameters parameters) {
    
    int counter = c.getImCounter();
    
    auto parentMView = (View*)c.murkaView;
    
    auto shapeOffering = parentMView->linearLayout.getNextShapeOffering();
    
    auto widgetHandler = T::getOrCreateImModeWidgetObject(counter, NULL, (View*)c.murkaView, shapeOffering);
    auto widgetObject = (View*)widgetHandler->widgetObjectInternal;
    
    typename T::Results results;
    
    c.pushContext(widgetHandler);
    if (c.transformTheRenderIntoThisContextShape(c.overlayHolder->disableViewportCrop)) {
        
        widgetObject->linearLayout.restart(shapeOffering);
        widgetObject->animationRestart();
        widgetObject->mosaicLayout.restart();
        widgetObject->draw(NULL, &parameters, widgetObject, c, &results);
        
        c.transformTheRenderBackFromThisContextShape();
    }
    c.popContext();
    
    widgetObject->resetChildrenBounds();
    
    return results;
}

// Get the latest drawn immediate mode widget

template<typename T>
T* getLatestDrawnWidget(MurkaContext &c) {
//    auto widgetHandler = T::getOrCreateImModeWidgetObject(0, new View(), new View(), MurkaShape());
    
    auto widgetHandler = T::getLatestDrawn((View*)c.murkaView);
//
    auto widgetObject = (T*)widgetHandler->widgetObjectInternal;
//
    return widgetObject;
}

 
}
