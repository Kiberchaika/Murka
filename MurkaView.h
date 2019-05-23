#include <functional>
#include "MurkaShapes.h"
#include "MurkaContext.h"


// Data, parameters and the context are what you put inside. Optional return is put into void*.
typedef std::function<void (void* dataToControl, void* parametersObject, void* thisWidgetObject, MurkaContext&)> viewDrawFunction;

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
    std::list <std::tuple<MurkaView*, void*>> children; // std::list because we may want to store a pointer to the view
    
    MurkaShape shape; // for the widgets that are instantiated
    
    //////////////////////////////////////////////// The following is to be overriden by children classes
    
    // A parameters initialiser. Gets called if Murka wants widget to allocate memory for its parameters.
    void* returnNewParametersObject() {
        return new MurkaView();
    }
    
    void* returnNewResultsObject() {
        return NULL;
    }
    
    viewDrawFunction draw = [](void* dataToControl, void* parametersObject, void* thisWidgetObject, MurkaContext & context)->void* {
        ofLog() << "drawing an empty func...";
        return new bool(false);
    };
    // draw arguments: data void pointer, parameters void pointer, context object pointer
    
    // create function ("factory"): returns the object initialised with the default parameter set
    
};
