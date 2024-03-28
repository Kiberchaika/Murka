#pragma once

#include "ViewBase.h"
#include "MurkaTypes.h"
#include "MurkaContext.h"
#include "MurkaViewHandler.h"
#include "Murka.h"
#include "MurkaLinearLayoutGenerator.h"
#include "MurkaAnimator.h"
#include "MurkaChildContextHolder.h"
#include "MurkaViewEventsContainer.h"

namespace murka {
    
template<typename T>
class View: public ViewBase, public MurkaViewEventsContainer {
public:
    
    // This variable is needed to support immediate mode widgets resizing themselves while also receiving sizes from outside.
    MurkaShape latestShapeThatCameFromOutside;
    
    virtual bool wantsClicks() { return true; } // override this if you want to signal to other widgets that you don't want clicks
    
    MurkaContext currentContext = MurkaContext();
    
    bool currentlyHasMouseFocus = false;
    
    // This function is used to prevent hover if one of the children is hovered.
    bool hasMouseFocus(MurkaChildContextHolder& m) {
        bool pass = false;
        if (inside() && wantsClicks()) {
            auto itr = m.latestChildContext.iterateHoverIndex();
            if (hoverIndexCache != m.latestChildContext.getMaxHoverIndex()) pass = false;
                else pass = true;
                
            hoverIndexCache = itr;
        }
        
        return pass;
    }

    // This function is used to prevent hover if one of the children is hovered.
    bool hasMouseFocus(MurkaContext& c) {
        bool pass = false;
        if (isHovered() && wantsClicks()) {
            auto itr = c.iterateHoverIndex();
            if (hoverIndexCache != c.getMaxHoverIndex()) pass = false;
                else pass = true;
                
            hoverIndexCache = itr;
        }
        
        return pass;
    }
    
public:
    View() {
    }
    
    // Copy constructor to warn against copying views
    View(const View &p1) {
        /// WARNING!!
        /// Views SHOULD NOT be copied! If you acquire a View from a draw function,
        /// make sure you use an lvalue reference:
        /// auto & view = m.draw<WidgetType>({0, 0, 100, 100});
        throw;
    }

    T& draw() {
        currentlyHasMouseFocus = hasMouseFocus(currentContext);
        
        defferedViewDrawFunc();
        return *(static_cast<T*>(this));
    }
    
    virtual ~View()  {
        // delete children
        for (auto it = imChildren.begin(); it != imChildren.end(); it++) {
            delete ((View*)it->second);
        }
    }
    
    int hoverIndexCache = 0;
    
    void registerCurrentContext(MurkaContext c) {
        currentContext = c;
    }
    
    MurkaPoint getSize() {
        return currentContext.getSize();
    }

    MurkaPoint getAbsoluteViewPosition() {
        return currentContext.currentViewShape.position;
    }

//    void addOverlay(std::function<void()> func, void* object) {
//        currentContext.overlayHolder->addOverlay(func, object);
//    }
    



    bool areInteractiveChildrenHovered() {
        if (!currentContext.isHovered()) {
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
        for (auto it = imChildren.begin(); it != imChildren.end(); it++) {
            auto shape = ((View*)it->second)->shape;
            shape.position = ((View*)it->second)->shape.position;
            
            if ((shape.inside(currentContext.mousePosition)) && (((View*)it->second)->wantsClicks())) {
                return true;
            }
        }
        
        return false;
    }
    
    bool isHovered() {
        return currentContext.isHovered();
    }
    
    bool inside(bool includingAllChildren = false,
                bool includingAllChildrenButInteractive = true) {
        bool result = false;
        if (!includingAllChildren && !includingAllChildrenButInteractive) {
            result = currentContext.isHovered() * !areChildrenHovered();
        } else if (includingAllChildren) {
            result = currentContext.isHovered();
        } else if (includingAllChildrenButInteractive) {
            auto currentContextHovered = currentContext.isHovered();
            result = !areInteractiveChildrenHovered() + currentContext.isHovered();
        }
        
        if (wantsClicks()) result *= currentlyHasMouseFocus;
        
        return result;
    }
    
    void claimKeyboardFocus(void* view = nullptr) {
        if (view == nullptr) view = this;
        currentContext.claimKeyboardFocus(view);
    }
    
    void resetKeyboardFocus() {
        currentContext.resetKeyboardFocus(this);
    }

    bool areChildrenHovered() {
        if (!currentContext.isHovered()) {
            return false;
        }
        
        int index = 0;
//        typename std::map<imIdentifier, MurkaViewHandler<View_NEW>*>::iterator it;
        for (auto it = imChildren.begin(); it != imChildren.end(); it++) {
            auto shape = ((View*)it->second)->shape;
            shape.position = ((View*)it->second)->shape.position;
            
            if (shape.inside(currentContext.mousePosition)) {
                return true;
            }
        }

        return false;
    }

    
    void* returnNewResultsObject() {
        return new bool(false);
    }
    
    void* returnNewWidgetObject() {
        return new View();
    }
    
    struct Parameters {};
    struct Results {};
    
    std::string dataTypeName = "";
    
    // Utility functions for those calling from outside
    
    static T* getOrCreateImModeWidgetObject_NEW(int index, ViewBase* parentWidget, MurkaShape shape) {
        
        auto idTuple = std::make_tuple(index, typeid(T).name());
        if (parentWidget->imChildren.find(idTuple) != parentWidget->imChildren.end()) {
            // the widget exists
                        
            auto imChild = ((T*)parentWidget->imChildren[idTuple]);
            auto widget = imChild;
            
            if (widget->latestShapeThatCameFromOutside != shape) {
                // updating shape cause something else came from outside
                
                widget->latestShapeThatCameFromOutside = shape;
                widget->shape = shape;
            }

            // Updating children bounds
            if (parentWidget->childrenBounds.position.x > widget->shape.position.x) {
                parentWidget->childrenBounds.position.x = widget->shape.position.x;
            }
            if (parentWidget->childrenBounds.position.y > widget->shape.position.y) {
                parentWidget->childrenBounds.position.y = widget->shape.position.y;
            }
            if ((parentWidget->childrenBounds.size.x + parentWidget->childrenBounds.position.x) < (widget->shape.position.x + widget->shape.size.x)) {
                parentWidget->childrenBounds.size.x = widget->shape.position.x + widget->shape.size.x - parentWidget->childrenBounds.position.x;
            }
            
            if ((parentWidget->childrenBounds.size.y + parentWidget->childrenBounds.position.y) < widget->shape.position.y + widget->shape.size.y) {
                parentWidget->childrenBounds.size.y = widget->shape.position.y + widget->shape.size.y - parentWidget->childrenBounds.position.y;
            }

            return (T*)parentWidget->imChildren[idTuple];
        } else {
            // Creating new widget
            
            auto newWidget = new T();
            newWidget->shape = shape;
            newWidget->latestShapeThatCameFromOutside = shape;
            
#ifdef MURKA_DEBUG
            std::cout << "creating a new object" << std::endl;
#endif
            int z = parentWidget->imChildren.size();
            
            if (z == 0) {
                // This doesn't work implicitly on Mac so we're remaking the map
//                parentWidget->imChildren_NEW = std::map<imIdentifier_NEW, ViewBase_NEW*>();
            }
            
            parentWidget->imChildren.insert(std::pair<imIdentifier, ViewBase*>(idTuple, newWidget));
//            parentWidget->imChildren_NEW[idTuple] = new ViewBase_NEW();

            // Updating children bounds (this is needed for automatic scrolling and things like that -
            // basically informing parent widget of how much space does its children occupy)
            if (parentWidget->childrenBounds.position.x > shape.position.x) {
                parentWidget->childrenBounds.position.x = shape.position.x;
            }
            if (parentWidget->childrenBounds.position.y > shape.position.y) {
                parentWidget->childrenBounds.position.y = shape.position.y;
            }
            if ((parentWidget->childrenBounds.size.x + parentWidget->childrenBounds.position.x) < (shape.position.x + shape.size.x)) {
                parentWidget->childrenBounds.size.x = shape.position.x + shape.size.x - parentWidget->childrenBounds.position.x;
            }
            
            if ((parentWidget->childrenBounds.size.y + parentWidget->childrenBounds.position.y) < (shape.position.y + shape.size.y)) {
                parentWidget->childrenBounds.size.y = shape.position.y + shape.size.y - parentWidget->childrenBounds.position.y;
            }
            
            return newWidget;
        }
    }
//    MurkaContext latestContext;
};
}
