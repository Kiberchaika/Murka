#pragma once

namespace murka {

typedef std::tuple<int, std::string> imIdentifier_NEW;

class ViewBase_NEW {
public:
    std::map<imIdentifier_NEW, ViewBase_NEW*> imChildren_NEW;
    
    MurkaShape childrenBounds = {std::numeric_limits<float>::max(), std::numeric_limits<float>::max(),
                                 0, 0};
    
    void resetChildrenBounds() {
        childrenBounds = {std::numeric_limits<float>::max(), std::numeric_limits<float>::max(),
            0, 0};
    }

    MurkaShape shape = {0, 0, 1, 2};
    
};

}
