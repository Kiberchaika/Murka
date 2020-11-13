#pragma once

#include "MurkaContextBase.h"
#include "MurkaLinearLayoutGenerator.h"
#include "MosaicLayout.h"


#define MURKA_VIEW_DRAW_FUNCTION_NEW void draw(MurkaContextBase & c)



namespace murka {

typedef std::tuple<int, std::string> imIdentifier_NEW;

class ViewBase_NEW: public MurkaAnimator {
public:
    std::map<imIdentifier_NEW, ViewBase_NEW*> imChildren_NEW = std::map<imIdentifier_NEW, ViewBase_NEW*>();
    
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
