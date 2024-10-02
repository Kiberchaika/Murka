#pragma once

#include "MurkaTypes.h"

namespace murka {

class MurkaOverlayHolder {
public:
    struct OverlayData {
        std::function<void()> func = []{};
        void* sourcePointer = nullptr;
        
        OverlayData(std::function<void()> f, void* o) {
            func = f;
            sourcePointer = o;
        }
    };

    std::vector<OverlayData> overlays;
    
    bool disableViewportCrop = false;
    
    void addOverlay(std::function<void()> func, void* sourcePointer) {
        overlays.push_back({func, sourcePointer});
    }
    
    MurkaOverlayHolder() {
        overlays.clear();
    }

    ~MurkaOverlayHolder() {
        // TODO: delete objects too
        overlays.clear();
    }
};

} // end of namespace Murka
