#pragma once

// TODO: add speed
// TODO: add easings from this site by converting them to functions of input
// https://easings.net/#

class MurkaAnimator {
    int AIndex = 0;
    
    struct AnimationData {
        double target = 0;
        double value = 0;
        
        AnimationData() {}
        AnimationData(double t, double v) { target = t; value = v; }
    };

public:
    std::map<int, /* double */ AnimationData> animationsMap;
    
    void animationRestart() {
        AIndex = 0;
    }
    
    // TODO: iterate by time, not just each time you call this function
    template<typename NUMBER_OR_BOOL>
    double A(NUMBER_OR_BOOL X) {
        if ((animationsMap.count(AIndex + 1)) == 1) {
            // found this one
            animationsMap[AIndex + 1].target = double(X);
            if (animationsMap[AIndex + 1].target > animationsMap[AIndex + 1].value) {
                animationsMap[AIndex + 1].value += (animationsMap[AIndex + 1].target - animationsMap[AIndex + 1].value) / 10;
            } else {
                animationsMap[AIndex + 1].value -= (animationsMap[AIndex + 1].value - animationsMap[AIndex + 1].target) / 10;
            }
        } else {
            animationsMap[AIndex + 1] = AnimationData(double(X), double(X));
        }

        AIndex++;
        return animationsMap[AIndex].value;
    };
    
    /* // boolean non-struct version
    double A(bool X) {
        if ((animationsMap.count(AIndex + 1)) == 1) {
            // found this one
            if (X) animationsMap[AIndex + 1] += (1.0 - animationsMap[AIndex + 1]) / 10.;
                else animationsMap[AIndex + 1] -= animationsMap[AIndex + 1] / 20.;
        } else {
            // not found, creating
            animationsMap[AIndex + 1] = double(X);
            ofLog() << "c!...";
        }
        
        AIndex++;
        return animationsMap[AIndex];
    }
     */

};
