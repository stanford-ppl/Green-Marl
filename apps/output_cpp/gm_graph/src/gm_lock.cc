#include "gm_lock.h"

bool gm_spinlock_try_acquire(gm_spinlock_t* ptr) {
    int ret = _gm_atomic_swap_int32(ptr, 1);
    return (ret == 0); // 0 means success
}

void gm_spinlock_acquire(gm_spinlock_t* ptr) {
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

void gm_spinlock_release(gm_spinlock_t* ptr) {
    _gm_full_barrier();
    // half-barrier:
    // make sure that all the prior writes are now visible to everybody else in the world

    *ptr = 0;
}

gm_spinlock_t gm_spinlock_tab[GM_SPINLOCK_TAB_SZ];
void gm_spinlock_table_init() {
    for (int i = 0; i < GM_SPINLOCK_TAB_SZ; i++)
        gm_spinlock_tab[i] = 0;
}

void gm_spinlock_acquire_for_ptr(void* ptr) {
    uint32_t entry_idx = (uint32_t)(((uintptr_t) ptr) & (GM_SPINLOCK_TAB_ENTRY - 1));
    uint32_t tab_idx = entry_idx * GM_CACHELINE;
    gm_spinlock_acquire(&gm_spinlock_tab[tab_idx]);
}

void gm_spinlock_release_for_ptr(void* ptr) {
    uint32_t entry_idx = (uint32_t)(((uintptr_t) ptr) & (GM_SPINLOCK_TAB_ENTRY - 1));
    uint32_t tab_idx = entry_idx * GM_CACHELINE;
    gm_spinlock_release(&gm_spinlock_tab[tab_idx]);
}

void gm_spinlock_acquire_for_node(node_t ptr) {
    uint32_t entry_idx = (uint32_t)(ptr & (GM_SPINLOCK_TAB_ENTRY - 1));
    uint32_t tab_idx = entry_idx * GM_CACHELINE;
    gm_spinlock_acquire(&gm_spinlock_tab[tab_idx]);
}

void gm_spinlock_release_for_node(node_t ptr) {
    uint32_t entry_idx = (uint32_t)(ptr & (GM_SPINLOCK_TAB_ENTRY - 1));
    uint32_t tab_idx = entry_idx * GM_CACHELINE;
    gm_spinlock_release(&gm_spinlock_tab[tab_idx]);
}

void gm_spinlock_acquire_for_edge(edge_t ptr) {
    uint32_t entry_idx = (uint32_t)(ptr & (GM_SPINLOCK_TAB_ENTRY - 1));
    uint32_t tab_idx = entry_idx * GM_CACHELINE;
    gm_spinlock_acquire(&gm_spinlock_tab[tab_idx]);
}

void gm_spinlock_release_for_edge(edge_t ptr) {
    uint32_t entry_idx = (uint32_t)(ptr & (GM_SPINLOCK_TAB_ENTRY - 1));
    uint32_t tab_idx = entry_idx * GM_CACHELINE;
    gm_spinlock_release(&gm_spinlock_tab[tab_idx]);
}
