#include "MurkaTypes.h"
#include <iostream>

std::ostream& operator<<(std::ostream& os, const MurkaPoint& p)
{
    os << p.x << '/' << p.y;
    return os;
}
