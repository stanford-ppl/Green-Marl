
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

static inline bool _gm_atomic_compare_and_swap(double  *dest, double old_valp,  double new_valp) 
{   
    uint64_t* D = reinterpret_cast<uint64_t*> (dest); 
    uint64_t* O = reinterpret_cast<uint64_t*> (&old_valp); 
    uint64_t* P = reinterpret_cast<uint64_t*> (&new_valp); 
    return __sync_bool_compare_and_swap(D, *O, *P);
}

static inline bool _gm_atomic_compare_and_swap(float   *dest, float  old_valp,  float new_valp) 
{
    uint32_t* D = reinterpret_cast<uint32_t*> (dest); 
    uint32_t* O = reinterpret_cast<uint32_t*> (&old_valp); 
    uint32_t* P = reinterpret_cast<uint32_t*> (&new_valp); 
    return __sync_bool_compare_and_swap(D, *O, *P);
}

#else // no GNUC

#error "GM_Graph library requires gcc for now" 

#endif // no GNUC

#endif
