#pragma once

#include "MurkaTypes.h"
#include "MurkaContext.h"
#include "MurkaView.h"
#include "MurkaInputEventsRegister.h"
#include "MurkaLinearLayoutGenerator.h"

using namespace murka;

class LambdaButton : public MurkaViewInterface<LambdaButton> {
public:
    MURKA_VIEW_DRAW_FUNCTION  {

                auto params = (Parameters*)parametersObject;
                auto &castedResults = *(castResults(resultObject)); 

                LambdaButton* thisWidget = (LambdaButton*)thisWidgetObject;

                bool inside = context.isHovered() * !areChildrenHovered(context);

                castedResults = params->lambda(context, this);

    }
    
    bool wantsClicks() override {
        return true;
    }
    
    // The results type, you also need to define it even if it's nothing.
    typedef bool Results;


    
    Results* castResults(void* results) {
        auto resultsPointer = (Results*)results;
        return resultsPointer;
    }


    struct Parameters {
        function<bool(MurkaContext, LambdaButton*)> lambda;

        Parameters() {}
        Parameters(function<bool(MurkaContext, LambdaButton*)> Lambda) { lambda = Lambda; } // a convenience initializer
    };

};
