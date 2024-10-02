#pragma once

#include "MurkaTypes.h"
#include "MurkaContext.h"
#include "MurkaView.h"
#include "MurkaInputEventsRegister.h"
#include "MurkaLinearLayoutGenerator.h"

namespace murka {

class LambdaPanel : public View<LambdaPanel> {
public:
	
    void internalDraw(const murka::MurkaContextBase & c) {
                lambdaFunc(c);
    }
    
    bool wantsClicks() override {
        return false;
    }
    
    MURKA_PARAMETER(LambdaPanel, // class name
                    function<void(const MurkaContextBase&)>, // parameter type
                    lambdaFunc, // parameter variable name
                    lambda, // setter
                    [](const MurkaContextBase&){} // default
    )
};
} // end of namespace Murka
