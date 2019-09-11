#pragma once
#include <functional>
#include "MurkaTypes.h"
#include "MurkaContext.h"
#include "MurkaViewHandler.h"
#include "Murka.h"
#include "MurkaLinearLayoutGenerator.h"

namespace murka {
    
// Data, parameters and the context are what you put inside. Optional return is put into void*.
typedef std::function<void (void* dataToControl,
                            void* parametersObject,
                            void* thisWidgetObject,
                            MurkaContext & context,
                            void* resultObject)> viewDrawFunction;


// View heirarchy

class View {
public:
    
    MurkaShape latestShapeThatCameFromOutside;
    
    struct MurkaVar {
        enum MurkaVarKind {INT, /* FLOAT, */ DOUBLE, STRING};
        MurkaVarKind kind;
        int intValue = 0;
        double doubleValue = 0;
        std::string stringValue = "";
        
        MurkaVar(int value) { intValue = value; kind = INT; }
        MurkaVar(double value) { doubleValue = value; kind = DOUBLE; }
        MurkaVar(std::string value) { stringValue = value; kind = STRING; }
        
        operator int() {return intValue; }
        operator double() {return doubleValue; }
        operator float() {return float(doubleValue); }
        operator std::string() {return stringValue; }
    };
    
    // TODO : add a flag to a base Murka class to bypass this save/load routine
    
//    virtual std::vector<MurkaVar> saveInternalData(int indexX, int indexY) { return std::vector<MurkaVar>(); }
//    virtual void loadInternalData(std::vector<MurkaVar>) {}
    
    virtual bool wantsClicks() { return true; } // override this if you want to signal to other widgets that you don't want clicks
    
public:
    View() {
        
    }
    
    ~View() {
        // TODO: delete children
    }
    
    static viewDrawFunction getDrawFunction(View* view) {
        return (*view).draw;
    }
    
    
    // This children list contains all the children objects and their respective data
    std::vector<MurkaViewHandlerInternal*> children;
    
    // This tuple is how Murka identifies the IM mode widget object.
    // Int stands for object drawing index, void* is a pointer to data, string is typeid
    typedef std::tuple<int, void*, std::string> imIdentifier;
    std::map<imIdentifier, MurkaViewHandler<View>*> imChildren; // Immediate mode widget objects that were once drawn inside this widget. We hold their state here in the shape of their MurkaViews.
    
    MurkaShape childrenBounds = {std::numeric_limits<float>::max(), std::numeric_limits<float>::max(),
                                 0, 0};
    
    void resetChildrenBounds() {
        childrenBounds = {std::numeric_limits<float>::max(), std::numeric_limits<float>::max(),
            0, 0};
    }

    bool areInteractiveChildrenHovered(MurkaContext c) {
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
    
    viewDrawFunction draw = [](void* dataToControl,
                               void* parametersObject,
                               void* thisWidgetObject,
                            MurkaContext & context,
                               void* resultObject) {
        ofLog() << "drawing an empty func...";
        return new bool(false);
    };
    
    struct Parameters { };
    struct Results {};
    
    void* parametersInternal;
    void* resultsInternal;
    
    MurkaShape shape = {0,0, 1, 2};
    
    MurkaLinearLayoutGenerator layoutGenerator;
    
    MurkaContext latestContext;
};

template<class T>
class MurkaViewInterface: public View {
public:
    
    void* returnNewResultsObject() {
        return new typename T::Results();
    }
    
    void* returnNewWidgetObject() {
        return new T();
    }
    
    static MurkaViewHandler<T>* getOrCreateImModeWidgetObject(int index, void* data, View* parentWidget, MurkaShape shape) {
        
//        ofLog() << typeid(T).name();
//        ofLog() << "parent:" << typeid(parentWidget).name();
        
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

            // Updating children bounds
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
            return newHandler;
        }
    }
    
    static MurkaViewInterface<T>::Results imDrawST2(MurkaContext &c, void* dataToControl, MurkaViewInterface<T>::Parameters parameters, MurkaShape shape) {
    }

};

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
    c.transformTheRenderIntoThisContextShape();
    widgetObject->layoutGenerator.restart(((View*)widgetHandler->widgetObjectInternal)->shape/*,  widgetObject->childrenBounds */);
        widgetObject->draw(NULL, &parameters, widgetObject, c, &results);
    
    /*
    //DEBUG - drawing the children frame that we had at the last frame end
    ofSetColor(255, 100, 0);
        ofNoFill();

    ofDrawRectangle(((View*)c.murkaView)->childrenBounds.position.x, ((View*)c.murkaView)->childrenBounds.position.y, ((View*)c.murkaView)->childrenBounds.size.x, ((View*)c.murkaView)->childrenBounds.size.y);
        ofFill();
    //////
     */

    widgetObject->resetChildrenBounds();

    
    c.transformTheRenderBackFromThisContextShape();
    c.popContext();
    
    return results;
}

template<typename T>
typename T::Results drawWidget(MurkaContext &c, void* dataToControl, typename T::Parameters parameters, MurkaShape shape) {
    

    int counter = c.getImCounter();
    
    auto mView = (View*)c.murkaView;
    auto widgetHandler = T::getOrCreateImModeWidgetObject(counter, NULL, (View*)c.murkaView, shape);
    auto widgetObject = (View*)widgetHandler->widgetObjectInternal;
    
    
    typename T::Results results;
    
    c.pushContext(widgetHandler);
    c.transformTheRenderIntoThisContextShape();
    
    widgetObject->layoutGenerator.restart(shape /*, widgetObject->childrenBounds*/);
    widgetObject->draw(dataToControl, &parameters, widgetObject, c, &results);
    
    //DEBUG
    /*
    ofSetColor(255, 100, 0);
        ofNoFill();
    ofDrawRectangle(((View*)c.murkaView)->childrenBounds.position.x, ((View*)c.murkaView)->childrenBounds.position.y, ((View*)c.murkaView)->childrenBounds.size.x, ((View*)c.murkaView)->childrenBounds.size.y);
        ofFill();
     */
    //////

    widgetObject->resetChildrenBounds();

    
    c.transformTheRenderBackFromThisContextShape();
    c.popContext();
    

    return results;

}
    
// Immediate mode automatic layout

template<typename T>
typename T::Results drawWidget(MurkaContext &c, void* dataToControl, typename T::Parameters parameters) {
    
    int counter = c.getImCounter();
    
    auto parentMView = (View*)c.murkaView;
    
    auto shapeOffering = parentMView->layoutGenerator.getNextShapeOffering();
    
    auto widgetHandler = T::getOrCreateImModeWidgetObject(counter, NULL, (View*)c.murkaView, shapeOffering);
    auto widgetObject = (View*)widgetHandler->widgetObjectInternal;
    
    typename T::Results results;
    
    c.pushContext(widgetHandler);
    c.transformTheRenderIntoThisContextShape();
    
    widgetObject->layoutGenerator.restart(shapeOffering /*, widgetObject->childrenBounds*/);
    widgetObject->draw(dataToControl, &parameters, widgetObject, c, &results);
    
    c.transformTheRenderBackFromThisContextShape();
    c.popContext();
    
    return results;
}
    
template<typename T>
typename T::Results drawWidget(MurkaContext &c, typename T::Parameters parameters) {
    
    int counter = c.getImCounter();
    
    auto parentMView = (View*)c.murkaView;
    
    auto shapeOffering = parentMView->layoutGenerator.getNextShapeOffering();
    
    auto widgetHandler = T::getOrCreateImModeWidgetObject(counter, NULL, (View*)c.murkaView, shapeOffering);
    auto widgetObject = (View*)widgetHandler->widgetObjectInternal;
    
    typename T::Results results;
    
    c.pushContext(widgetHandler);
    c.transformTheRenderIntoThisContextShape();
    
    widgetObject->layoutGenerator.restart(shapeOffering /*, widgetObject->childrenBounds*/);
    widgetObject->draw(NULL, &parameters, widgetObject, c, &results);
    
    c.transformTheRenderBackFromThisContextShape();
    c.popContext();
    
    return results;
}
 
}
