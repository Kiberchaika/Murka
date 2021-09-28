#pragma once

#include "MurkaContextBase.h"
#include "MurkaLinearLayoutGenerator.h"
#include "MosaicLayout.h"
#include "MurkaAnimator.h"
#include "parallel_hashmap/phmap.h"

#define MURKA_PARAMETER(typename, parameterType, parameterName, getterName, setterName, default) \
parameterType parameterName = default; \
typename & setterName(parameterType parameterName_) { \
    parameterName = parameterName_; return *this; } \
parameterType getterName() { return parameterName; }

#define MURKA_CALLBACK(typename, callbackVariable, setter) \
std::function<void(typename &)> callbackVariable = [](typename&) {}; \
typename & setter(std::function<void(typename &)> callback) { \
    callbackVariable = callback; \
    return *this; \
}

#define MURKA_CALLBACK_1ARGUMENT(typename, callbackVariable, setter, argument_type) \
std::function<void(typename &, argument_type)> callbackVariable = [](typename&, argument_type) {}; \
typename & setter(std::function<void(typename &, argument_type)> callback) { \
    callbackVariable = callback; \
    return *this; \
}

namespace murka {

typedef std::tuple<int, std::string> imIdentifier_NEW;

class ViewBase_NEW: public MurkaAnimator {
public:
    phmap::flat_hash_map<imIdentifier_NEW, ViewBase_NEW*> imChildren_NEW = phmap::flat_hash_map<imIdentifier_NEW, ViewBase_NEW*>();
    
    ViewBase_NEW() {
    }
    
    MurkaShape childrenBounds = {std::numeric_limits<float>::max(), std::numeric_limits<float>::max(),
                                 0, 0};
    
    void resetChildrenBounds() {
        childrenBounds = {std::numeric_limits<float>::max(), std::numeric_limits<float>::max(),
            0, 0};
    }

    MurkaShape shape = {0, 0, 1, 2};
    
//    virtual MURKA_VIEW_DRAW_FUNCTION_NEW {
//        ofLog() << "Base drawing, not derived...";
//    };
    
    MurkaLinearLayoutGenerator linearLayout;
    MosaicLayout mosaicLayout;

};

}
