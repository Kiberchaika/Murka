#pragma once
#include "MurkaTypes.h"

namespace murka {

// Handler heirarchy

// Handler is View that also wraps data to control, parameters and results.
// This is made for the user to only subclass a super minimal View object,
// Murka itself manages allocating memory for all of those

struct MurkaViewHandlerInternal {
    void* parametersInternal = nullptr;
    void* resultsInternal = nullptr;
    void* dataToControl = nullptr;
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
    T* widgetObject; // TODO: check if this widgetObject is redundant, since there is "external" version and it's sometimes NULL if debugger is right
    
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
    
} // end of namespace Murka
