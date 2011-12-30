
#include <omp.h>
#include "gm_runtime.h"
#include "gm_mem_helper.h"

//===============================
gm_runtime _GM_RT;
gm_mem_helper _GM_MEM;;

//===============================
gm_runtime::gm_runtime() : is_init(false)
{

}
gm_runtime::~gm_runtime()
{

}
// Called at each procedure entry
void gm_runtime::initialize()
{
    if (is_init) return;

    is_init = true;
    num_threads = gm_rt_get_num_threads();
    _GM_MEM.resize(num_threads);

}

bool gm_runtime::is_initialized()
{
    return is_init;
}

int gm_runtime::get_num_threads()
{
    return num_threads = omp_get_max_threads();
}

void gm_runtime::set_num_threads(int n)
{
    omp_set_num_threads(n);
    _GM_MEM.resize(n);
}
int gm_runtime::get_thread_id()
{
    return omp_get_thread_num();
}




#if 0
//---------------------------------------------
// locktable is here only for temporarily
//---------------------------------------------
static const int GM_LOCKTABLE_SZ=128;
omp_lock_t _gm_locktable[GM_LOCKTABLE_SZ];
static bool _gm_locktable_init = false;
void _gm_init_locktable() {
    if (!_gm_locktable_init) {
        for(int i=0;i<GM_LOCKTABLE_SZ;i++)
            omp_init_lock(&_gm_locktable[i]);
    }
}


#define INDEX(P)    (((intptr_t) (P)) % GM_LOCKTABLE_SZ)
void _gm_lock_addr(void*p)
{
    omp_set_lock(&_gm_locktable[INDEX(p)]);
}
void _gm_lock_addrs(void*p[], int cnt)
{
    // bubble sort
    // sort by locktable id
    intptr_t U = (intptr_t) p;
    int n = cnt;
    do {
      int newn = 0;
      for (int i = 0; i < n-1; i++) {
        if (INDEX(p[i]) > INDEX(p[i+1])) {
           void* temp = p[i];
           p[i] = p[i+1];
           p[i+1] = temp;
           newn = i + 1 ;
        }
      }
      n = newn;
  } while (n > 1);

  for(int i=0;i<cnt;i++)
  { if ((i>0) && (INDEX(p[i]) != INDEX(p[i-1]))) {
        omp_set_lock(&_gm_locktable[INDEX(p[i])]);
    }
  }
}
void _gm_unlock_addr(void*p)
{
    omp_unset_lock(&_gm_locktable[INDEX(p)]);
}
void _gm_unlock_addrs(void*p[], int cnt)
{
    // bubble sort
    // sort by locktable id
    intptr_t U = (intptr_t) p;
    int n = cnt;
    do {
      int newn = 0;
      for (int i = 0; i < n-1; i++) {
        if (INDEX(p[i]) > INDEX(p[i+1])) {
           void* temp = p[i];
           p[i] = p[i+1];
           p[i+1] = temp;
           newn = i + 1 ;
        }
      }
      n = newn;
  } while (n > 1);

  for(int i=0;i<cnt;i++)
  { if ((i>0) && (INDEX(p[i]) != INDEX(p[i-1]))) {
        omp_unset_lock(&_gm_locktable[INDEX(p[i])]);
    }
  }
}
#endif

//===============================
