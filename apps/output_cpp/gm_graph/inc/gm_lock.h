#ifndef GM_LOCK_H_
#define GM_LOCK_H_
#include <stdint.h>
#include "gm_graph.h"
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

typedef volatile int32_t gm_spinlock_t;

// return true if quired
static inline bool gm_spinlock_try_acquire(gm_spinlock_t* ptr) {
    int ret = _gm_atomic_swap_int32(ptr, 1);
    return (ret == 0); // 0 means success
}

static inline void gm_spinlock_acquire(gm_spinlock_t* ptr) {
    while (_gm_atomic_swap_int32(ptr, 1)) {
        while (*ptr == 1)            // spin on local cache, until the pointer is updated by remote cpu
        {
            // pause (let other hw-threads to proceed)
            _gm_pause();
        }
    }

    // no barrier needed 
    // __sync_lock_test_and_set implies half-barrier such that
    // any following reads cannot happen (before we go out of the above loop).
}

static inline void gm_spinlock_release(gm_spinlock_t* ptr) {
    _gm_full_barrier();
    // half-barrier:
    // make sure that all the prior writes are now visible to everybody else in the world

    *ptr = 0;
}

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
static void gm_spinlock_acquire_for_ptr(void* ptr) {
    uint32_t entry_idx = (uint32_t)(((uintptr_t) ptr) & (GM_SPINLOCK_TAB_ENTRY - 1));
    uint32_t tab_idx = entry_idx * GM_CACHELINE;
    gm_spinlock_acquire(&gm_spinlock_tab[tab_idx]);
}
static void gm_spinlock_release_for_ptr(void* ptr) {
    uint32_t entry_idx = (uint32_t)(((uintptr_t) ptr) & (GM_SPINLOCK_TAB_ENTRY - 1));
    uint32_t tab_idx = entry_idx * GM_CACHELINE;
    gm_spinlock_release(&gm_spinlock_tab[tab_idx]);
}
static void gm_spinlock_acquire_for_node(node_t ptr) {
    uint32_t entry_idx = (uint32_t)(ptr & (GM_SPINLOCK_TAB_ENTRY - 1));
    uint32_t tab_idx = entry_idx * GM_CACHELINE;
    gm_spinlock_acquire(&gm_spinlock_tab[tab_idx]);
}
static void gm_spinlock_release_for_node(node_t ptr) {
    uint32_t entry_idx = (uint32_t)(ptr & (GM_SPINLOCK_TAB_ENTRY - 1));
    uint32_t tab_idx = entry_idx * GM_CACHELINE;
    gm_spinlock_release(&gm_spinlock_tab[tab_idx]);
}
static void gm_spinlock_acquire_for_edge(edge_t ptr) {
    uint32_t entry_idx = (uint32_t)(ptr & (GM_SPINLOCK_TAB_ENTRY - 1));
    uint32_t tab_idx = entry_idx * GM_CACHELINE;
    gm_spinlock_acquire(&gm_spinlock_tab[tab_idx]);
}
static void gm_spinlock_release_for_edge(edge_t ptr) {
    uint32_t entry_idx = (uint32_t)(ptr & (GM_SPINLOCK_TAB_ENTRY - 1));
    uint32_t tab_idx = entry_idx * GM_CACHELINE;
    gm_spinlock_release(&gm_spinlock_tab[tab_idx]);
}

#endif
