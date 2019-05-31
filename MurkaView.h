#include <functional>
#include "MurkaShapes.h"
#include "MurkaContext.h"


// Data, parameters and the context are what you put inside. Optional return is put into void*.
typedef std::function<void (void* dataToControl,
                            void* parametersObject,
                            void* thisWidgetObject,
                            MurkaContext & context,
                            void* resultObject)> viewDrawFunction;

struct MurkaViewHandler {
    viewDrawFunction drawFunction;
    void* result = NULL;
    MurkaShape shape; // optionally this should point to something like a layout generator
    void* parametersObject = NULL;
    void* dataToControl = NULL;
    void* widgetObject;
    
    bool manuallyControlled = false; // false means immediate mode, true means OOP
    bool wasUsedInLastFrame = true; // if this becomes false, we unallocate it
};

//template<class T>
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
    
//    static viewDrawFunction getStaticDrawFunction() {
//        return T::staticDraw();
//    }
    
    
    // This children list contains all the children objects and their respective data
//    std::list <std::tuple<MurkaView*, void*>> children; // std::list because we may want to store a pointer to the view
    std::vector<MurkaViewHandler*> children; // the actual MurkaViewHandlers are in the base class, not here.
    
//    MurkaShape shape; // for the widgets that are instantiated
    
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
                               MurkaContext & context,
                               void* resultObject) {
        ofLog() << "drawing an empty func...";
        return new bool(false);
    };
    // draw arguments: data void pointer, parameters void pointer, context object pointer
    
    // create function ("factory"): returns the object initialised with the default parameter set
    
};
