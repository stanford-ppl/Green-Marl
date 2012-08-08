#ifndef GM_ATOMIC_WRAP_H_
#define GM_ATOMIC_WRAP_H_
#include <stdlib.h>
#include <stdint.h>
#if defined(__x86_64__) || defined(__i386__)
  #include "../platform/x86/inc/gm_platform_helpers.h"
#else
  #if defined(__sparc)
    #if defined (__ORACLE__)
      #include "../platform/sparc/inc/gm_platform_helpers.h"
    #endif
  #else
    #error "We need x86 (32bit or 64bit) or Sparc environment"
  #endif
#endif
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

#if !defined(__GNUC__) && !defined(__SUNPRO_CC)
#error "GM_Graph library requires gcc or oracle studio for now" 
#endif // no GNUC

static inline bool _gm_atomic_compare_and_swap(int32_t *dest, int32_t old_val, int32_t new_val) {
    return _gm_atomic_cas_int32(dest, old_val, new_val);
}

// It is not possible to use gcc's inherent CAS for double and float
// (Reinterpret_cast does not work here)
static inline bool _gm_atomic_compare_and_swap(float *dest, float old_val, float new_val) {
    unsigned char c = _gm_CAS_asm(dest, old_val, new_val);
    return (c == 1);
}

static inline bool _gm_atomic_compare_and_swap(bool *dest, bool old_val, bool new_val) {
    return _gm_atomic_cas_bool(dest, old_val, new_val);
}

#if defined(__arch64__) || defined(__x86_64__) || defined(__SUNPRO_CC)

static inline bool _gm_atomic_compare_and_swap(int64_t *dest, int64_t old_val, int64_t new_val)
{   return _gm_atomic_cas_int64(dest, old_val, new_val);}

static inline bool _gm_atomic_compare_and_swap(double *dest, double old_val, double new_val)
{
    unsigned char c = _gm_CAS_asm(dest, old_val, new_val);
    return (c==1);
}

#else

#include "gm_lock.h"

static void gm_spinlock_acquire_for_ptr(void* ptr);
static void gm_spinlock_release_for_ptr(void* ptr);

static inline bool _gm_atomic_compare_and_swap(int64_t *dest, int64_t old_val, int64_t new_val) {
#warning "atomic operation performance for 64bit data can be slow on 32-bit environment. (Consider using 64-bit environment.)"

    bool ret = false;
    if (*dest != old_val) return false;

    gm_spinlock_acquire_for_ptr(dest);

    if (*dest == old_val) {
        *dest = new_val;
        ret = true;
    }

    gm_spinlock_release_for_ptr(dest);
    return ret;
}

static inline bool _gm_atomic_compare_and_swap(double *dest, double old_val, double new_val) {

    bool ret = false;
    if (*dest != old_val) return false;

    gm_spinlock_acquire_for_ptr(dest);

    if (*dest == old_val) {
        *dest = new_val;
        ret = true;
    }

    gm_spinlock_release_for_ptr(dest);
    return ret;
}

#endif

#endif // end of file
