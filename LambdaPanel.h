#pragma once

#include "MurkaTypes.h"
#include "MurkaContext.h"
#include "MurkaView.h"
#include "MurkaInputEventsRegister.h"
#include "MurkaAssets.h"
#include "MurkaLinearLayoutGenerator.h"

using namespace murka;

class LambdaPanel : public MurkaViewInterface<LambdaPanel> {
public:
    MURKA_VIEW_DRAW_FUNCTION  {
        
                auto params = (Parameters*)parametersObject;
                auto &castedResults = *(castResults(resultObject));

                params->lambda(context);
    };
    
    bool wantsClicks() override {
        return false;
    }
    
    // The results type, you also need to define it even if it's nothing.
    typedef bool Results;

    
    Results* castResults(void* results) {
        auto resultsPointer = (Results*)results;
        return resultsPointer;
    }


    struct Parameters {
        function<void(MurkaContext&)> lambda;

        Parameters() {}
        Parameters(function<void(MurkaContext&)> Lambda) { lambda = Lambda; } // a convenience initializer
    };

};
