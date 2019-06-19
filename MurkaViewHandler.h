#pragma once
#include "MurkaShapes.h"

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
//    MurkaShape shape; // optionally this should point to something like a layout generator
    
    
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
