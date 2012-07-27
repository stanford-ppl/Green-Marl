#ifndef GM_PLATFORM_HELPERS
#define GM_PLATFORM_HELPERS

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
      ___result; \
    })

#define _gm_pause() asm volatile ("pause" ::: "memory")

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
