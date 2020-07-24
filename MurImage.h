#pragma once

#ifdef MURKA_OF

class MurImage {
public:
    MurImage(){};
    
    ofImage internal;
    
};

#else // Default version

class MurImage {
public:
    MurImage(){};
};

#endif
