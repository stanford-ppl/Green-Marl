#ifndef GM_LIMITS_H_
#define GM_LIMITS_H_

#include <limits.h>
#include <math.h>
#include <float.h>

template<class Type>
Type gm_get_min();

template<class Type>
Type gm_get_max();

template<>
inline int32_t gm_get_min() {
    return INT_MIN;
}

template<>
inline int32_t gm_get_max() {
    return INT_MAX;
}

template<>
inline int64_t gm_get_min() {
    return LONG_MIN;
}

template<>
inline int64_t gm_get_max() {
    return LONG_MAX;
}

template<>
inline double gm_get_min() {
    return -DBL_MAX;
}

template<>
inline double gm_get_max() {
    return DBL_MAX;
}

template<>
inline float gm_get_min() {
    return -FLT_MAX;
}

template<>
inline float gm_get_max() {
    return FLT_MAX;
}


#endif /* GM_LIMITS_H_ */
