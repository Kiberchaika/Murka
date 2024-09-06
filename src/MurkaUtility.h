#pragma once

#include <cmath>
#include <limits>

#ifndef FLT_EPSILON
#include <cfloat> // TODO: decide if its better to just directly define FLT_EPSILON
#endif

namespace murka {

// if T != int
// TODO: finish this template class and replace the below function
//template <typename T, std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
//static bool IsApproximatelyEqual(T a, T b, T epsilon = std::numeric_limits<T>::epsilon) {
//
//    const T absA = std::fabs(a);
//    const T absB = std::fabs(b);
//    const T diff = std::fabs(a - b);
//
//    if (a == b) { // shortcut, handles infinities, ignore the warning for the comparison
//        return true;
//    } else if (a == 0 || b == 0 || (absA + absB < std::numeric_limits<T>::min())) { // ignore the warnings for comparisons
//        // a or b is zero or both are extremely close to it
//        // relative error is less meaningful here
//        return diff < (epsilon * std::numeric_limits<T>::min());
//    } else {
//        // use relative error
//        return diff / std::min((absA + absB), std::numeric_limits<T>::max()) < epsilon;
//    }
//}

static bool IsApproximatelyEqual(float a, float b) {

    if (a == b) { // shortcut, handles infinities, ignore the warning for the comparison
        return true;
    }

    float tolerance = FLT_EPSILON * std::abs(a);
    if (tolerance < FLT_EPSILON) {
        tolerance = FLT_EPSILON;
    }

    return std::abs(a - b) < tolerance;
}

} // namespace murka
