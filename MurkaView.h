#include <functional>
#include "MurkaShapes.h"
#include "MurkaContext.h"


// Data, parameters and the context are what you put inside. Optional return is put into void*.
typedef std::function<void (void* dataToControl,
                            void* parametersObject,
                            void* thisWidgetObject,
                            const MurkaContext & context,
                            void* resultObject)> viewDrawFunction;


// Handler heirarchy

struct MurkaViewHandlerInternal {
//    viewDrawFunction drawFunction;
    void* parametersInternal = NULL;
    void* resultsInternal = NULL;
    void* dataToControl = NULL;
    void* widgetObjectInternal;

    
    bool manuallyControlled = false; // false means immediate mode, true means OOP
    bool wasUsedInLastFrame = true; // if this becomes false, we unallocate it
    
    // Things to get out from a user's perspective
    MurkaShape shape; // optionally this should point to something like a layout generator
    

};

template<typename T>
struct MurkaViewHandler: public MurkaViewHandlerInternal {
    
//    typename T::Results* results;
    typename T::Parameters* tParams;
    T* widgetObject;
    
    typename T::Results* castResultsP() {
        
    }
    
    typename T::Results& getResults() {
        return *castResults(resultsInternal);
    }
    
    typename T::Parameters* castParameters(void* p) {
        return (typename T::Parameters*)p;
    }
    
    typename T::Results* castResults(void *p) {
        return (typename T::Results*)p;
    }
    
    
};

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
    std::vector<MurkaViewHandlerInternal*> children; // the actual MurkaViewHandlers are in the base class, not here.

    bool isChildrenHovered(MurkaContext c) {
        if (!c.isHovered()) {
            return false;
        }
        
        for (auto i: children) {
            auto shape = i->shape;
            shape.position = i->shape.position;
            
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
};

template<class T>
class MurkaViewInterface: public MurkaView {
public:
//    static viewDrawFunction getStaticDrawFunction() {
//        return T::staticDraw();
//    }
    
//    typename T::Results* results(void* resultsObject) {
//        return *((T::Results*)resultObject);
//    }
    
    void* returnNewResultsObject() {
        return new typename T::Results();
    }
    
    void* returnNewWidgetObject() {
        return new T();
    }
};
