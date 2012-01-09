
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

#ifdef __x86_64__

static inline bool _gm_atomic_compare_and_swap(int32_t *dest, int32_t old_val, int32_t new_val)
{ return __sync_bool_compare_and_swap(dest, old_val, new_val);}
static inline bool _gm_atomic_compare_and_swap(int64_t *dest, int64_t old_val, int64_t new_val)
{ return __sync_bool_compare_and_swap(dest, old_val, new_val);}

// It is not possible to use gcc's inherent CAS for double and float
// (Reinterpret_cast does not work here)
#  define __asm_sync_bool_compare_and_swap(ptr,oldval,newval) \
    ({ \
      typeof(ptr) __p = (ptr); \
      typeof(*__p) __oldval = (oldval); \
      typeof(*__p) __newval = (newval); \
      register unsigned char __result; \
      register typeof(*__p) __readval; \
      if (sizeof(*__p) == 4) { \
        __asm__ __volatile__ ("lock; cmpxchgl %3,%1; sete %0" \
                              : "=q"(__result), "=m"(*__p), "=a"(__readval) \
                              : "r"(__newval), "m"(*__p), "2"(__oldval) \
                              : "memory"); \
      } else if (sizeof(*__p) == 8) { \
        __asm__ __volatile__ ("lock; cmpxchgq %3,%1; sete %0" \
                              : "=q"(__result), "=m"(*__p), "=a"(__readval) \
                              : "r"(__newval), "m"(*__p), "2"(__oldval) \
                              : "memory"); \
      } else { \
        abort(); \
      } \
      __result; \
    })
static inline bool _gm_atomic_compare_and_swap(float *dest, float old_val, float new_val)
{ unsigned char  c = __asm_sync_bool_compare_and_swap(dest, old_val, new_val);
  return (c==1);
}

static inline bool _gm_atomic_compare_and_swap(double *dest, double old_val, double new_val)
{ unsigned char c =  __asm_sync_bool_compare_and_swap(dest, old_val, new_val);
  return (c==1);
}

#endif

#if __i386__

#error "You should compile with -m64 flag. (x86_64 architecture required)" 

#endif



#else // no GNUC

#error "GM_Graph library requires gcc for now" 

#endif // no GNUC

#endif
