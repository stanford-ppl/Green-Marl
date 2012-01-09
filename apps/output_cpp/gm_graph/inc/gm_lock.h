#ifndef GM_LOCK_H
#define GM_LOCK_H
#include <stdint.h>

#if defined (__i386__) || (__x86_64__)

typedef volatile int32_t gm_spinlock_t;

static inline void gm_spinlock_acquire(gm_spinlock_t* ptr)
{
    while (__sync_lock_test_and_set(ptr, 1))
    {
        while(*ptr == 1)
        {
            // pause. (other hyper threads to proceed)
            asm volatile ("pause" ::: "memory");
        }
    }

    // no barrier needed 
    // __sync_lock_test_and_set implies half-barrier such that
    // any read cannot happen before we go out of this loop.
}

static inline void gm_spinlock_release(gm_spinlock_t* ptr)
{
    // half-barrier:
    //   all the writes prior to below is now visible.
    __sync_synchronize(); 

    *ptr = 0;
}

#else

#error "Need x86 (32 or 64bit) architecture"

#endif

#define GM_CACHELINE            16          // 16 * 4 byte
#define GM_SPINLOCK_TAB_ENTRY   64          // 64 entries 
#define GM_SPINLOCK_TAB_SZ      (GM_CACHELINE*GM_SPINLOCK_TAB_ENTRY)

extern uint32_t gm_spinlock_tab_sz;  
extern gm_spinlock_t gm_spinlock_tab[];       

extern void gm_spinlock_table_init(); 

// Spinlock should not be nested.
static inline void gm_spinlock_acquire_for_ptr(void* ptr)
{
    uintptr_t entry_idx = ((uintptr_t) ptr) & (GM_SPINLOCK_TAB_ENTRY -1);
    uintptr_t tab_idx = entry_idx * GM_CACHELINE;
    gm_spinlock_acquire( &gm_spinlock_tab[tab_idx]);
}
static inline void gm_spinlock_release_for_ptr(void* ptr)
{
    uintptr_t entry_idx = ((uintptr_t) ptr) & (GM_SPINLOCK_TAB_ENTRY -1);
    uintptr_t tab_idx = entry_idx * GM_CACHELINE;
    gm_spinlock_release( &gm_spinlock_tab[tab_idx]);
}


#endif
