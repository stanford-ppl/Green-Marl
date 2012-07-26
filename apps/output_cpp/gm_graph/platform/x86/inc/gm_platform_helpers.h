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

#endif
