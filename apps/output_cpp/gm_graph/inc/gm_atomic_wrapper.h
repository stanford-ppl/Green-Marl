#ifndef GM_ATOMIC_WRAP_H_
#define GM_ATOMIC_WRAP_H_
#include <stdlib.h>
#include <stdint.h>

#include "gm_internal.h"

// The following check is required because we use __compare_and_swap
// which is a gcc-specific builtin 
#if !defined(__GNUC__) && !defined(__SUNPRO_CC)
#error "GM_Graph library requires gcc or oracle studio for now" 
#endif // no GNUC

//---------------------------------------------------------
// A thin layer of atomic operations
//---------------------------------------------------------
//  bool _gm_atomic_compare_and_swap(T* dest, T old, T new): 
//       compare and swap; return true if successful
//       T should be defined at least for following types:
//          int32_t, int64_t, double, float
//
// The following atomic operations are not used at this point and will
// be defined as needed.
//
//  unsigned char _gm_atomic_or(unsigned char* dest, unsigned char byte_val)
//       atomic or; returns old_value
//  unsigned char _gm_atomic_or(unsigned char* dest, unsigned char byte_val)
//       atomic and; returns old_value
//  T _gm_atomic_add(T* dest, T increment)
//       atomically add; returns old value
//       T should be at least:
//          (u)int32_t, (u)int16_t, (u)int8_t
//---------------------------------------------------------


//-----------------------------------------------------------------------
// This layer simply remames _gm_atomic_compare_and_swap into gm_atomic_cas_<type>
// : The actual implementation of gm_atomic_cas_<type> is implemented in platform*.h
//-----------------------------------------------------------------------
static inline bool _gm_atomic_compare_and_swap(int32_t *dest, int32_t old_val, int32_t new_val) {
    return _gm_atomic_cas_int32(dest, old_val, new_val);
}
static inline bool _gm_atomic_compare_and_swap(int64_t *dest, int64_t old_val, int64_t new_val) {
    return _gm_atomic_cas_int64(dest, old_val, new_val);
}
static inline bool _gm_atomic_compare_and_swap(float *dest, float old_val, float new_val) {
    return _gm_atomic_cas_float(dest, old_val, new_val);
}
static inline bool _gm_atomic_compare_and_swap(double *dest, double old_val, double new_val)
{
    return _gm_atomic_cas_double(dest, old_val, new_val);
}

#endif // end of file
