#pragma once

#if defined(MURKA_OF)

class MurTexture {
public:
    MurTexture(){}
    
    ofTexture internal;
};

#elif defined(MURKA_JUCE)

#include "MurImage.h"

class MurTexture : public MurImage {
public:
	MurTexture(){};
};

#else // Default version
#endif
