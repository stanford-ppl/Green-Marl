
#ifndef GM_ATOMIC_WRAP_H
#define GM_ATOMIC_WRAP_H
#include <stdlib.h>
#include <stdint.h>

//---------------------------------------------------------
// A thin layer of atomic operations
//---------------------------------------------------------
//  bool _gm_atomic_compare_and_swap(T* dest, T old, T new): 
//       compare and swap; return true if successful
//       T should be defined at least for following types:
//          int32_t, int64_t, double, float
//  unsigned char _gm_atomic_or(unsigned char* dest, unsigned char byte_val)
//       atomic or; returns old_value
//  unsigned char _gm_atomic_or(unsigned char* dest, unsigned char byte_val)
//       atomic and; returns old_value
//  T _gm_atomic_add(T* dest, T increment)
//       atomically add; returns old value
//       T should be at least:
//          (u)int32_t, (u)int16_t, (u)int8_t
//---------------------------------------------------------

#if __GNUC__

#define _gm_atomic_add __sync_fetch_and_add
#define _gm_atomic_or  __sync_fetch_and_or
#define _gm_atomic_and __sync_fetch_and_and

static inline bool _gm_atomic_compare_and_swap(int32_t *dest, int32_t old_val, int32_t new_val)
{ return __sync_bool_compare_and_swap(dest, old_val, new_val);}
static inline bool _gm_atomic_compare_and_swap(int64_t *dest, int64_t old_val, int64_t new_val)
{ return __sync_bool_compare_and_swap(dest, old_val, new_val);}

// for x86 only
# if (__i386__ || __x86_64__)
#define _gm_atomic_compare_and_swap_asm(ptr, oldval, newval) \
    ({ \
      typeof(ptr) ___p = (ptr); \
      typeof(*___p) ___oldval = (oldval); \
      typeof(*___p) ___newval = (newval); \
      register unsigned char ___result; \
      register typeof(*___p) ___readval; \
      if (sizeof(*___p) == 4) { \
        __asm__ __volatile__ ("lock; cmpxchgl %3,%1; sete %0" \
                              : "=q"(___result), "=m"(*___p), "=a"(___readval) \
                              : "r"(___newval), "m"(*___p), "2"(___oldval) \
                              : "memory"); \
      } else if (sizeof(*___p) == 8) { \
        __asm__ __volatile__ ("lock; cmpxchgq %3,%1; sete %0" \
                              : "=q"(___result), "=m"(*___p), "=a"(___readval) \
                              : "r"(___newval), "m"(*___p), "2"(___oldval) \
                              : "memory"); \
      } else { \
        abort(); \
      } \
      ___result; \
    })
static inline bool _gm_atomic_compare_and_swap(double  *dest, double old_val,  double new_val) 
{ return _gm_atomic_compare_and_swap_asm(dest, old_val, new_val); }
static inline bool _gm_atomic_compare_and_swap(float  *dest, float old_val,  float new_val) 
{ return _gm_atomic_compare_and_swap_asm(dest, old_val, new_val); }

# else  // no x86
# error "Graph library requires x86 for now."
//   The following gcc exptension does not work correctly with -O2 or -O3
//   I do not know why.
/*
static inline bool _gm_atomic_compare_and_swap(double  *dest, double* old_valp,  double* new_valp) 
{   
    uint64_t* D = reinterpret_cast<uint64_t*> (dest); 
    uint64_t* O = reinterpret_cast<uint64_t*> (old_valp); 
    uint64_t* P = reinterpret_cast<uint64_t*> (new_valp); 
    return __sync_bool_compare_and_swap(D, *O, *P);
}

static inline bool _gm_atomic_compare_and_swap(float   *dest, float*  old_valp,  float* new_valp) 
{
    uint32_t* D = reinterpret_cast<uint32_t*> (dest); 
    uint32_t* O = reinterpret_cast<uint32_t*> (old_valp); 
    uint32_t* P = reinterpret_cast<uint32_t*> (new_valp); 
    return __sync_bool_compare_and_swap(D, *O, *P);
}
    */
# endif // no x86

#else // no GNUC
#error "Graph library requires gcc for now" 

#endif // no GNUC

#endif
