#include "gm_runtime.h"

//===============================
gm_mem_helper _GM_MEM;
;
//GM_MEM should be initialized first
gm_runtime _GM_RT;

//===============================
gm_runtime::gm_runtime() :
        is_init(false), num_threads(0), random_seeds(NULL) {

    gm_spinlock_table_init();
    initialize();
}

gm_runtime::~gm_runtime() {
    delete[] random_seeds;
}

// Called at each procedure entry
void gm_runtime::initialize() {

    if ((is_init == false) || (omp_get_max_threads() > num_threads)) {
        set_num_threads (omp_get_max_threads());
        is_init = true;
    }

}

bool gm_runtime::is_initialized() {
    return is_init;
}

int gm_runtime::get_num_threads() {
    return num_threads = omp_get_max_threads();
}

void gm_runtime::set_num_threads(int n) {
    if ((is_init == false) || (n > num_threads)) {
        int old = num_threads;
        _GM_MEM.resize(n);
        expand_random_seeds(old, n);

        // initialize threads by generationg random numbers
        #pragma omp parallel
        {
            uniform(gm_rt_thread_id());
        }
    }

    num_threads = n;
    omp_set_num_threads(n);
}

int gm_runtime::get_thread_id() {
    return omp_get_thread_num();
}

void gm_runtime::expand_random_seeds(int old, int n) {
    if (is_init && (n <= old)) return;

    unsigned short* old_seeds = random_seeds;
    random_seeds = new unsigned short[n * 3];

    if (!is_init) old = 0;

    // copy old random seeds
    for (int i = 0; i < old * 3; i++) {
        random_seeds[i] = old_seeds[i];
    }

    // new random seeds
    // short[0]   : 0x330E  (arbitary number)
    // short[1~2] : thread id 
    for (int i = old; i < n; i++) {
        int base = i * 3;
        random_seeds[base + 0] = (unsigned short) 0x330E;
        random_seeds[base + 1] = (unsigned short) (n & 0xFFFF);
        random_seeds[base + 2] = (unsigned short) ((n >> 16) & 0xFFFF);
        assert((base + 2) < n * 3);
    }

    delete[] old_seeds;
}

double gm_runtime::uniform(int tid) {
    double d = erand48(&(random_seeds[tid * 3]));
    return d;
}

int gm_runtime::rand(int min, int max, int tid) {
    int range = (max - min);
    (assert(range > 0));
    int ret = (int) (uniform(tid) * range) + min;
    return ret;
}

long gm_runtime::rand_long(long max, int tid) {
    long ret = (long) (uniform(tid) * max);
    return ret;
}

/*
 * Wrappers to call the gm_runtime methods
 */
void gm_rt_initialize() {
    _GM_RT.initialize();
}

bool gm_rt_is_initialized() {
    return _GM_RT.is_initialized();
}

int gm_rt_get_num_threads() {
    return _GM_RT.get_num_threads();
}

void gm_rt_set_num_threads(int n) {
    _GM_RT.set_num_threads(n);
}

int gm_rt_thread_id() {
    return _GM_RT.get_thread_id();
}

double gm_rt_uniform(int tid) {
    return _GM_RT.uniform(tid);
}

int gm_rt_rand(int min, int max, int tid) {
    return _GM_RT.rand(min, max, tid);
}

long gm_rt_rand_long(long max, int tid) {
    return _GM_RT.rand_long(max, tid);
}
