#pragma once

#include "ViewBase.h"
#include "MurkaTypes.h"
#include "MurkaContext.h"
#include "MurkaViewHandler.h"
#include "Murka.h"
#include "MurkaLinearLayoutGenerator.h"
#include "MurkaAnimator.h"
#include "MurkaChildContextHolder.h"

namespace murka {
    
// Data, parameters and the context are what you put inside. Optional return is put into void*.
typedef std::function<void (void* dataToControl,
                            void* parametersObject,
                            void* thisWidgetObject,
                            MurkaContextBase & context,
                            void* resultObject)> viewDrawFunction;

#define MURKA_VIEW_DRAW_FUNCTION void draw(void* dataToControl, void* parametersObject, void* thisWidgetObject, murka::MurkaContext & context, void* resultObject)

template<typename T>
class View_NEW: public ViewBase_NEW {
public:
    
    // This variable is needed to support immediate mode widgets resizing themselves while also receiving sizes from outside.
    MurkaShape latestShapeThatCameFromOutside;
    
    
    virtual bool wantsClicks() { return true; } // override this if you want to signal to other widgets that you don't want clicks
    
public:
    View_NEW() {
    }
    
    // Copy constructor to warn against copying views
    View_NEW(const View_NEW &p1) {
        /// WARNING!!
        /// Views SHOULD NOT be copied! If you acquire a View from a draw function,
        /// make sure you use an lvalue reference:
        /// auto & view = m.draw<WidgetType>({0, 0, 100, 100});
        throw;
    }

    T& commit() {
        defferedViewDrawFunc();
        return *(static_cast<T*>(this));
    }
    
    ~View_NEW() {
        // TODO: delete children
    }
    
    // This children list contains all the children objects and their respective data
    std::vector<MurkaViewHandlerInternal*> children;
    
    // This tuple is how Murka identifies the IM mode widget object.
    // Int stands for object drawing index, void* is a pointer to data, string is typeid
    typedef std::tuple<int, void*, std::string> imIdentifier;
    

    /*
    void clearChildren() {
        for (auto &i: children) {
            ((View*)i->widgetObjectInternal)->clearChildren();
            delete i;
        }
        children.clear();
    }
     */
    
    int hoverIndexCache = 0;
    
    bool hasMouseFocus(MurkaChildContextHolder& m) {
        bool inside = m.latestChildContext.isHovered() * !areInteractiveChildrenHovered(m.latestChildContext);
        
        bool pass = false;
        if (inside && wantsClicks()) {
            auto itr = m.latestChildContext.iterateHoverIndex();
            if (hoverIndexCache != m.latestChildContext.getMaxHoverIndex()) pass = false;
                else pass = true;
                
            hoverIndexCache = itr;
        }
        
        return pass;
    }
    
    bool areInteractiveChildrenHovered(MurkaChildContextHolder& m) {
        if (!m.latestChildContext.isHovered()) {
            return false;
        }
        
        /*
        for (auto i: children) {
            auto shape = ((View*)i->widgetObjectInternal)->shape;
            shape.position = ((View*)i->widgetObjectInternal)->shape.position;
            
            if ((shape.inside(c.mousePosition)) && (((View*)i->widgetObjectInternal)->wantsClicks())) {
                return true;
            }
        }
        */
        
        int index = 0;
//        typename std::map<imIdentifier, MurkaViewHandler<View_NEW>*>::iterator it;
        for (auto it = imChildren_NEW.begin(); it != imChildren_NEW.end(); it++) {
            auto shape = ((View_NEW*)it->second)->shape;
            shape.position = ((View_NEW*)it->second)->shape.position;
            
            if ((shape.inside(m.latestChildContext.mousePosition)) && (((View_NEW*)it->second)->wantsClicks())) {
                return true;
            }
        }
        
        return false;
    }


    bool areInteractiveChildrenHovered(const MurkaContextBase& c) {
        if (!c.isHovered()) {
            return false;
        }
        
        /*
        for (auto i: children) {
            auto shape = ((View*)i->widgetObjectInternal)->shape;
            shape.position = ((View*)i->widgetObjectInternal)->shape.position;
            
            if ((shape.inside(c.mousePosition)) && (((View*)i->widgetObjectInternal)->wantsClicks())) {
                return true;
            }
        }
        */
        
        int index = 0;
//        typename std::map<imIdentifier, MurkaViewHandler<View_NEW>*>::iterator it;
        for (auto it = imChildren_NEW.begin(); it != imChildren_NEW.end(); it++) {
            auto shape = ((View_NEW*)it->second)->shape;
            shape.position = ((View_NEW*)it->second)->shape.position;
            
            if ((shape.inside(c.mousePosition)) && (((View_NEW*)it->second)->wantsClicks())) {
                return true;
            }
        }
        
        return false;
    }

    bool areChildrenHovered(const MurkaContextBase & c) {
        if (!c.isHovered()) {
            return false;
        }
        
        int index = 0;
//        typename std::map<imIdentifier, MurkaViewHandler<View_NEW>*>::iterator it;
        for (auto it = imChildren_NEW.begin(); it != imChildren_NEW.end(); it++) {
            auto shape = ((View_NEW*)it->second)->shape;
            shape.position = ((View_NEW*)it->second)->shape.position;
            
            if (shape.inside(c.mousePosition)) {
                return true;
            }
        }

        return false;
    }

    
    void* returnNewResultsObject() {
        return new bool(false);
    }
    
    void* returnNewWidgetObject() {
        return new View_NEW();
    }
    
    /*
    virtual MURKA_VIEW_DRAW_FUNCTION {
        ofLog() << "drawing an empty func...";
    }
     */
    
    
    struct Parameters {};
    struct Results {};
    
    std::string dataTypeName = "";
    
    // Utility functions for those calling from outside
    
    static T* getOrCreateImModeWidgetObject_NEW(int index, ViewBase_NEW* parentWidget, MurkaShape shape) {
        
        auto idTuple = std::make_tuple(index, typeid(T).name());
        if (parentWidget->imChildren_NEW.find(idTuple) != parentWidget->imChildren_NEW.end()) {
            // the widget exists
            
//            ofLog() << "returning the object";
            
            auto imChild = ((T*)parentWidget->imChildren_NEW[idTuple]);
            auto widget = imChild;
            
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

            return (T*)parentWidget->imChildren_NEW[idTuple];
        } else {
            // Creating new widget
            
            auto newWidget = new T();
            newWidget->shape = shape;
            newWidget->latestShapeThatCameFromOutside = shape;
            
            
            std::cout << "creating a new object";
            
            int z = parentWidget->imChildren_NEW.size();
            
            if (z == 0) {
                // This doesn't work implicitly on Mac so we're remaking the map
//                parentWidget->imChildren_NEW = std::map<imIdentifier_NEW, ViewBase_NEW*>();
            }
            
            parentWidget->imChildren_NEW.insert(std::pair<imIdentifier_NEW, ViewBase_NEW*>(idTuple, newWidget));
//            parentWidget->imChildren_NEW[idTuple] = new ViewBase_NEW();

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
            
            return newWidget;
        }
    }
    
    
    MurkaContext latestContext;

};

 
}
