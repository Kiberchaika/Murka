#pragma once
#include <functional>
#include "MurkaShapes.h"
#include "MurkaContext.h"
#include "MurkaViewHandler.h"


// View heirarchy

class MurkaView {
public:
    MurkaView() {
        
    }
    
    ~MurkaView() {
        // TODO: delete children
    }
    
    static viewDrawFunction getDrawFunction(MurkaView* view) {
        return (*view).draw;
    }
    
    
    // This children list contains all the children objects and their respective data
    std::vector<MurkaViewHandlerInternal*> children;
    
    typedef std::tuple<int, void*> imIdentifier;
    std::map<imIdentifier, MurkaViewHandler<MurkaView>*> imChildren; // Immediate mode widget objects that were once drawn inside this widget. We hold their state here in the shape of their MurkaViews.

    bool isChildrenHovered(MurkaContext c) {
        if (!c.isHovered()) {
            return false;
        }
        
        for (auto i: children) {
            auto shape = ((MurkaView*)i->widgetObjectInternal)->shape;
            shape.position = ((MurkaView*)i->widgetObjectInternal)->shape.position;
            
            if (shape.inside(c.eventState.mousePosition)) {
                return true;
            }
        }
        
        ofLog() << "imchildren size:" << imChildren.size();
        
        int index = 0;
        std::map<imIdentifier, MurkaViewHandler<MurkaView>*>::iterator it;
        for (it = imChildren.begin(); it != imChildren.end(); it++) {
            auto shape = ((MurkaView*)it->second->widgetObjectInternal)->shape;
            shape.position = ((MurkaView*)it->second->widgetObjectInternal)->shape.position;
            
            if (shape.inside(c.eventState.mousePosition)) {
                return true;
            }
        }

        return false;
    }

    //////////////////////////////////////////////// The following is to be overriden by children classes
    
    // A parameters initialiser. Gets called if Murka wants widget to allocate memory for its parameters.
    // This is not restricted to anything, but what you allocate here is what you will have to deal with
    // in draw function, and also what the user will use to set the actual parameters.
    
    void* returnNewParametersObject() {
        return new MurkaView();
    }
    
    // Results object is used for internal state management and is crucial to get right.
    void* returnNewResultsObject() {
        return new bool(false);
    }
    
    void* returnNewWidgetObject() {
        return new MurkaView();
    }
    
    viewDrawFunction draw = [](void* dataToControl,
                               void* parametersObject,
                               void* thisWidgetObject,
                               const MurkaContext & context,
                               void* resultObject) {
        ofLog() << "drawing an empty func...";
        return new bool(false);
    };
    
    struct Parameters { };
    struct Results {};
    
    void* parametersInternal;
    void* resultsInternal;
    
    MurkaShape shape;
    
    ///// IM mode helpers
    
    MurkaContext latestContext;
};

template<class T>
class MurkaViewInterface: public MurkaView {
public:
    
    void* returnNewResultsObject() {
        return new typename T::Results();
    }
    
    void* returnNewWidgetObject() {
        return new T();
    }
    
    
    static MurkaViewHandler<T>* getOrCreateImModeWidgetObject(int index, void* data, MurkaView* parentWidget, MurkaShape shape) {
        auto idTuple = std::make_tuple(index, data);
        if (parentWidget->imChildren.find(idTuple) != parentWidget->imChildren.end()) {
            // the widget exists
            ofLog() << "returning the object";
            return (MurkaViewHandler<T>*)parentWidget->imChildren[idTuple];
        } else {
            auto newWidget = new T();
            auto resultsObject = newWidget->returnNewResultsObject();
            newWidget->shape = shape;
            
            MurkaViewHandler<T>* newHandler = new MurkaViewHandler<T>();
            newHandler->resultsInternal = resultsObject;
            newHandler->dataToControl = data;
            newHandler->widgetObjectInternal = newWidget;
            
            ofLog() << "imchildren size was " << parentWidget->imChildren.size();
            parentWidget->imChildren[idTuple] = (MurkaViewHandler<MurkaView> *)newHandler;
            ofLog() << "creating the object..." << " now the imchildren size is " << parentWidget->imChildren.size();
            return newHandler;
        }
    }
    
};
