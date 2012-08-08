#ifndef GM_RUNTIME_H_
#define GM_RUNTIME_H_
#include <assert.h>
#include <omp.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include "gm_mem_helper.h"

//----------------------------------------------------
// A thin layer over openmp 
//----------------------------------------------------
// Thin layer over open-mp.
//----------------------------------------------------

class gm_runtime
{
public:
    gm_runtime();
    virtual ~gm_runtime();

    void initialize();
    bool is_initialized();
    int get_num_threads();
    void set_num_threads(int n);
    int get_thread_id();

    double uniform(int tid = 0);                     // returns [0,1)
    long rand_long(long max, int tid = 0);           // returns [0, max-1]   : for large numbers e.g. nodes
    int rand(int min, int max, int tid = 0);   // returns [min, max-1]

private:
    void expand_random_seeds(int old, int n);
    bool is_init;
    int num_threads;
    unsigned short* random_seeds;

};

extern gm_runtime _GM_RT;

static inline void gm_rt_initialize() {
    _GM_RT.initialize();
}
static inline bool gm_rt_is_initialized() {
    return _GM_RT.is_initialized();
}

static inline int gm_rt_get_num_threads() {
    return _GM_RT.get_num_threads();
}
static inline void gm_rt_set_num_threads(int n) {
    _GM_RT.set_num_threads(n);
}
static inline int gm_rt_thread_id() {
    return _GM_RT.get_thread_id();
}

static inline double gm_rt_uniform(int tid = 0) {
    return _GM_RT.uniform(tid);
}
static inline int gm_rt_rand(int min, int max, int tid = 0) {
    return _GM_RT.rand(min, max, tid);
}
static inline long gm_rt_rand_long(long max, int tid = 0) {
    return _GM_RT.rand_long(max, tid);
}

extern int dummy[];

#endif
