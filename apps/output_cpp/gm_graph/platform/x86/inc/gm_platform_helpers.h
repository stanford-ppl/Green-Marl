#ifndef GM_PLATFORM_HELPERS
#define GM_PLATFORM_HELPERS

#if defined(__arch64__) || defined(__x86_64__) 
// implementation for 64bit environment
#define _gm_CAS_asm(ptr, oldval, newval) \
    ({ \
      __typeof__(ptr) ___p = (ptr); \
      __typeof__(*___p) ___oldval = (oldval); \
      __typeof__(*___p) ___newval = (newval); \
      register unsigned char ___result; \
      register __typeof__(*___p) ___readval; \
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
      (___result==1); \
    })

#define _gm_CAS_asm_32(ptr, oldval, newval) _gm_CAS_asm(ptr, oldval, newval)
#define _gm_CAS_asm_64(ptr, oldval, newval) _gm_CAS_asm(ptr, oldval, newval)

#elif defined(__i386__)
// implementation for 32bit environment
//
// CAS for 64 bit

static inline bool
_gm_i386_casX(volatile unsigned long long* addr,
     unsigned long expected_high, unsigned long expected_low,
     unsigned long new_high, unsigned long new_low)
{
    char success;
    asm volatile("lock; cmpxchg8b (%6);"
                 "setz %7; "
                 : "=a" (expected_low), "=d" (expected_high)
                 : "0" (expected_low), "1" (expected_high),
                   "c" (new_high), "b" (new_low),
                   "r" (addr), "m" (success)
                 : "cc", "memory");
    return success;
}

static inline bool _gm_i386_casX(volatile unsigned long long* addr,
                        const unsigned long long *oldVal,
                        const unsigned long long *newVal)
{
    unsigned long old_high = *oldVal >> 32, old_low = *oldVal;
    unsigned long new_high = *newVal >> 32, new_low = *newVal;

    return _gm_i386_casX(addr, old_high, old_low, new_high, new_low);
}

#define _gm_CAS_asm_64(dest, old_val, new_val) \
    ({ \
    const unsigned long long    oldVal = reinterpret_cast<const unsigned long long>(old_val); \
    const unsigned long long    newVal = reinterpret_cast<const unsigned long long>(new_val); \
    volatile unsigned long long*  ptr  = reinterpret_cast<volatile unsigned long long*>(dest);\
    _gm_i386_casX(ptr, &oldVal, &newVal); \
    })


static inline unsigned long
_gm_i386_cas(volatile unsigned long* ptr, unsigned long old, unsigned long _new)
{
    unsigned long prev;
    asm volatile("lock;"
                 "cmpxchgl %1, %2;"
                 : "=a"(prev)
                 : "q"(_new), "m"(*ptr), "a"(old)
                 : "memory");
    return prev;
}

#define _gm_CAS_asm_32(dest, old_val, new_val) \
    ({ \
    const unsigned long     oldVal = reinterpret_cast<const unsigned long >(old_val); \
    const unsigned long     newVal = reinterpret_cast<const unsigned long >(new_val); \
    volatile unsigned long*  ptr  = reinterpret_cast<volatile unsigned long*>(dest);\
    _gm_i386_cas(ptr, &oldVal, &newVal); \
    })

#endif // end of 32bit

#define _gm_pause() asm volatile ("pause" ::: "memory")

//-----------------------------------------------------------------
// atomic mapping to GCC
//-----------------------------------------------------------------

#define _gm_atomic_fetch_and_or_char(ptr, val) __sync_fetch_and_or(ptr, val)
#define _gm_atomic_fetch_and_and_char(ptr, val)  __sync_fetch_and_and(ptr, val)
#define _gm_atomic_fetch_and_add_int32(ptr, val) __sync_fetch_and_add(ptr, val)
#define _gm_atomic_fetch_and_add_int64(ptr, val) __sync_fetch_and_add(ptr, val)
#define _gm_atomic_cas_bool(ptr, oldval, newval)  __sync_bool_compare_and_swap(ptr, oldval, newval)
#define _gm_atomic_cas_int32(ptr, oldval, newval)  __sync_bool_compare_and_swap(ptr, oldval, newval)
#define _gm_atomic_cas_int64(ptr, oldval, newval)  __sync_bool_compare_and_swap(ptr, oldval, newval)
#define _gm_atomic_swap_int32(ptr, newval) __sync_lock_test_and_set(ptr, newval)
#define _gm_full_barrier() __sync_synchronize()

#define _gm_atomic_fetch_and_add_edge(ptr, val) __sync_fetch_and_add(ptr, val)
#define _gm_atomic_fetch_and_add_node(ptr, val) __sync_fetch_and_add(ptr, val)

#endif
