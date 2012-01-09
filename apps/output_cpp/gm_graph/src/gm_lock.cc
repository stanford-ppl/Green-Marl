#include "gm_lock.h"

gm_spinlock_t gm_spinlock_tab[GM_SPINLOCK_TAB_SZ];
void gm_spinlock_table_init()
{
    for(int i=0;i<GM_SPINLOCK_TAB_SZ;i++)
        gm_spinlock_tab[i] = 0;
}
