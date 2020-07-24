#pragma once

#ifdef MURKA_OF

class MurTexture {
public:
    MurTexture(){};
    
    ofTexture internal;
};

#else // Default version

class MurTexture {
public:
    MurImage(){};
};

#endif
