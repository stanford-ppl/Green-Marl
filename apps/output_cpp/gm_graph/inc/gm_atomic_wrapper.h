#ifndef GM_ATOMIC_WRAP_H
#define GM_ATOMIC_WRAP_H
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

static inline bool _gm_atomic_compare_and_swap(int32_t *dest, int32_t old_val, int32_t new_val) {
    return _gm_atomic_cas_int32(dest, old_val, new_val);
}

//----------------------------------------------------------------- 
// In some experiments, somehow gcc's builtin CAS failed to bring 
// atomicity for float type (on x86_64);  
// Note that gcc's builtin is only defined for int and long long, but 
// reinterpret_cast did not work correctly.
// We add our own asm implementation for CAS. (This is a work-around te
//----------------------------------------------------------------- 
static inline bool _gm_atomic_compare_and_swap(float *dest, float old_val, float new_val) {
    return _gm_CAS_asm_32(dest, old_val, new_val);
}

static inline bool _gm_atomic_compare_and_swap(bool *dest, bool old_val, bool new_val) {
    return _gm_atomic_cas_bool(dest, old_val, new_val);
}

#if defined(__arch64__) || defined(__x86_64__) || defined(__SUNPRO_CC)

static inline bool _gm_atomic_compare_and_swap(int64_t *dest, int64_t old_val, int64_t new_val)
{   return _gm_atomic_cas_int64(dest, old_val, new_val);}

static inline bool _gm_atomic_compare_and_swap(double *dest, double old_val, double new_val)
{
    return _gm_CAS_asm_64(dest, old_val, new_val);
}

#elif defined(__i386__)

//-------------------------------------------------------------
// 64bit compare-and-swap on 32bit x86 machine
//-------------------------------------------------------------

static inline bool _gm_atomic_compare_and_swap(int64_t *dest, int64_t old_val, int64_t new_val) 
{
    return _gm_CAS_asm_64(dest, old_val, new_val);
}
//    const unsigned long long    oldVal = reinterpret_cast<const unsigned long long>(old_val);
//    const unsigned long long    newVal = reinterpret_cast<const unsigned long long>(new_val);
//    volatile unsigned long long*  ptr  = reinterpret_cast<volatile unsigned long long*>(dest);

//    return _gm_CAS_asm(ptr, oldval, newval);
//}

static inline bool _gm_atomic_compare_and_swap(double *dest, double old_val, double new_val) 
{
//    const unsigned long long    oldVal = reinterpret_cast<const unsigned long long>(old_val);
//    const unsigned long long    newVal = reinterpret_cast<const unsigned long long>(new_val);
//    volatile unsigned long long*  ptr  = reinterpret_cast<volatile unsigned long long*>(dest);
    return _gm_CAS_asm_64(ptr, oldval, newval);
}

#else

#error "Unsupported architecture"

/*
#include "gm_lock.h"

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
*/

#endif // end of else

#endif // end of file
