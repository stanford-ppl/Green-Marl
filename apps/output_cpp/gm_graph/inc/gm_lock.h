#ifndef GM_LOCK_H_
#define GM_LOCK_H_
#include <stdint.h>
#include "gm_internal.h"

typedef volatile int32_t gm_spinlock_t;

// return true if quired
bool gm_spinlock_try_acquire(gm_spinlock_t* ptr);
void gm_spinlock_acquire(gm_spinlock_t* ptr);
void gm_spinlock_release(gm_spinlock_t* ptr);

// Use small lock-table
//  - the application will keep spin-lock region very small
//  - there will not be too many # of threads
//  - It is better to keep lock table in cache. 
#define GM_CACHELINE            16          // 16 * 4 byte
#define GM_SPINLOCK_TAB_ENTRY   512         // small # of entries only
#define GM_SPINLOCK_TAB_SZ      (GM_CACHELINE*GM_SPINLOCK_TAB_ENTRY)

extern uint32_t gm_spinlock_tab_sz;
extern gm_spinlock_t gm_spinlock_tab[];
void gm_spinlock_table_init();

// [Assumption] spinlock should not be nested.
void gm_spinlock_acquire_for_ptr(void* ptr);
void gm_spinlock_release_for_ptr(void* ptr);
void gm_spinlock_acquire_for_node(node_t ptr);
void gm_spinlock_release_for_node(node_t ptr);
void gm_spinlock_acquire_for_edge(edge_t ptr);
void gm_spinlock_release_for_edge(edge_t ptr);

#endif
