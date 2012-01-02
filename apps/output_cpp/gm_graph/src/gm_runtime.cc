
#include <assert.h>
#include <omp.h>
#include <stdlib.h>
#include "gm_runtime.h"
#include "gm_mem_helper.h"

//===============================
gm_runtime _GM_RT;
gm_mem_helper _GM_MEM;;

//===============================
gm_runtime::gm_runtime() : is_init(false), num_threads(0), random_seeds(NULL)
{
    initialize();
}
gm_runtime::~gm_runtime()
{

}
// Called at each procedure entry
void gm_runtime::initialize()
{
    if (is_init) return;
    is_init = true;
    set_num_threads(omp_get_max_threads());

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
    int old = num_threads;
    omp_set_num_threads(n);
    if (n > old) {
        _GM_MEM.resize(n);
        expand_random_seeds(old, n);
    }
}
int gm_runtime::get_thread_id()
{
    return omp_get_thread_num();
}

void gm_runtime::expand_random_seeds(int old, int n)
{
    unsigned short* old_seeds = random_seeds;
    unsigned short* random_seeds = new unsigned short[n*3];

    // copy old random seeds
    for(int i=0;i<old*3;i++)
        random_seeds[i] = old_seeds[i];

    // new random seeds
    // short[0]   : 0x330E  (arbitary number)
    // short[1~2] : thread id 
    int base = old*3;
    for(int i=old;i<n;i++)
    {
        int base = i*3;
        random_seeds[base + 0] = 0x330E;
        random_seeds[base + 1] = (n & 0xFFFF);
        random_seeds[base + 2] = ((n>>16) & 0xFFFF);
    }

    delete old_seeds;
}

double gm_runtime::uniform(int tid)
{
    double d = erand48(&(random_seeds[tid*3]));
    return d;
}

int gm_runtime::rand(int min, int max, int tid)
{
    int range = (max - min); (assert(range > 0));
    int ret = (int)(uniform(tid) * range) + min;
    return ret;
}
long gm_runtime::rand_long(long max, int tid)
{
    long ret = (long)(uniform(tid) * max); 
    return ret;
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
