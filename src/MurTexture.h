#pragma once

#include "MurImage.h"

namespace murka {

#if defined(MURKA_OF)

class MurTexture {
public:
    MurTexture(){}
    
    ofTexture internal;
};

#elif defined(MURKA_JUCE)

class MurTexture : public MurImage {
public:
    MurTexture(){}
};

#else // Default version
#endif

}
