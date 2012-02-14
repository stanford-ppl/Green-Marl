#ifndef GM_LOCK_H
#define GM_LOCK_H
#include <stdint.h>
#include "gm_graph.h"

#if defined (__i386__) || (__x86_64__)

typedef volatile int32_t gm_spinlock_t;

static inline void gm_spinlock_acquire(gm_spinlock_t* ptr)
{
    while (__sync_lock_test_and_set(ptr, 1))
    {
        while(*ptr == 1)            // spin on local cache, until the pointer is updated by remote cpu
        {
            // pause (let other hw-threads to proceed)
            asm volatile ("pause" ::: "memory");
        }
    }

    // no barrier needed 
    // __sync_lock_test_and_set implies half-barrier such that
    // any following reads cannot happen (before we go out of the above loop).
}

static inline void gm_spinlock_release(gm_spinlock_t* ptr)
{
    __sync_synchronize(); 
    // half-barrier:
    // make sure that all the prior writes are now visible to everybody else in the world

    *ptr = 0;
}

#else

#error "Need x86 (32 or 64bit) architecture"

#endif


// Use small lock-table
//  - the application will keep spin-lock region very small
//  - there will not be too many # of threads
//  - It is better to keep lock table in cache. 
#define GM_CACHELINE            16          // 16 * 4 byte
#define GM_SPINLOCK_TAB_ENTRY   128         // small # of entries only
#define GM_SPINLOCK_TAB_SZ      (GM_CACHELINE*GM_SPINLOCK_TAB_ENTRY)

extern uint32_t gm_spinlock_tab_sz;  
extern gm_spinlock_t gm_spinlock_tab[];       

extern void gm_spinlock_table_init(); 

// [Assumption] spinlock should not be nested.
static inline void gm_spinlock_acquire_for_ptr(void* ptr)
{
    uint32_t entry_idx = (uint32_t) (((uintptr_t) ptr) & (GM_SPINLOCK_TAB_ENTRY -1));
    uint32_t tab_idx = entry_idx * GM_CACHELINE;
    gm_spinlock_acquire( &gm_spinlock_tab[tab_idx]);
}
static inline void gm_spinlock_release_for_ptr(void* ptr)
{
    uint32_t entry_idx = (uint32_t) (((uintptr_t) ptr) & (GM_SPINLOCK_TAB_ENTRY -1));
    uint32_t tab_idx = entry_idx * GM_CACHELINE;
    gm_spinlock_release( &gm_spinlock_tab[tab_idx]);
}
static inline void gm_spinlock_acquire_for_node(node_t ptr)
{
    uint32_t entry_idx = (uint32_t) (ptr & (GM_SPINLOCK_TAB_ENTRY -1));
    uint32_t tab_idx = entry_idx * GM_CACHELINE;
    gm_spinlock_acquire( &gm_spinlock_tab[tab_idx]);
}
static inline void gm_spinlock_release_for_node(node_t ptr)
{
    uint32_t entry_idx = (uint32_t) (ptr & (GM_SPINLOCK_TAB_ENTRY -1));
    uint32_t tab_idx = entry_idx * GM_CACHELINE;
    gm_spinlock_release( &gm_spinlock_tab[tab_idx]);
}
static inline void gm_spinlock_acquire_for_edge(edge_t ptr)
{
    uint32_t entry_idx = (uint32_t) (ptr & (GM_SPINLOCK_TAB_ENTRY -1));
    uint32_t tab_idx = entry_idx * GM_CACHELINE;
    gm_spinlock_acquire( &gm_spinlock_tab[tab_idx]);
}
static inline void gm_spinlock_release_for_edge(edge_t ptr)
{
    uint32_t entry_idx = (uint32_t) (ptr & (GM_SPINLOCK_TAB_ENTRY -1));
    uint32_t tab_idx = entry_idx * GM_CACHELINE;
    gm_spinlock_release( &gm_spinlock_tab[tab_idx]);
}


#endif
