#include <assert.h>
#include "gm_mem_helper.h"

void gm_mem_helper::resize(int n) {
    if ((max > 0) && (n <= max)) return;
    primitivePointers.resize(n);
    complexPointers.resize(n);
    max = n;
}

void gm_mem_helper::save(void* ptr, int typeinfo, int thread_id) {
    primitivePointers[thread_id].push_back(ptr);
}

void gm_mem_helper::save(gm_complex_data_type* ptr, int typeinfo, int thread_id) {
    complexPointers[thread_id].push_back(ptr);
}

void gm_mem_helper::clear(void* ptr, int typeinfo, int thread_id) {
    std::list<void*>& L = primitivePointers[thread_id];
    std::list<void*>::iterator i;
    for (i = L.begin(); i != L.end(); i++) {
        if (*i == ptr) {
            // primitive aray.
            // This will work for primitive types.
            float* P = (float*) *i;
            delete[] P;

            L.erase(i);
            return;
        }
    }

    std::list<gm_complex_data_type*>& L_c = complexPointers[thread_id];
    std::list<gm_complex_data_type*>::iterator i_c;
    for (i_c = L_c.begin(); i_c != L_c.end(); i_c++) {
        if (*i_c == ptr) {
            //complex type
            delete[] *i_c;

            L_c.erase(i_c);
            return;
        }
    }
}

void gm_mem_helper::cleanup() {
    // remove every thread'
    for (int p = 0; p < max; p++) {
        std::list<void*>& L = primitivePointers[p];
        std::list<void*>::iterator i;
        for (i = L.begin(); i != L.end(); i++) {
            float* P = (float*) *i;
            delete[] P;
        }
        L.clear();

        std::list<gm_complex_data_type*>& L_c = complexPointers[p];
        std::list<gm_complex_data_type*>::iterator i_c;
        for (i_c = L_c.begin(); i_c != L_c.end(); i_c++) {
            delete *i_c;
        }
        L_c.clear();
    }
}

/*
 * Wrappers to call the gm_mem_helper methods
 */
int64_t* gm_rt_allocate_long(size_t sz, int thread_id) {
    int64_t* ptr = new int64_t[sz];
    _GM_MEM.save(ptr, 0, thread_id);
    return ptr;
}

int32_t* gm_rt_allocate_int(size_t sz, int thread_id) {
    int32_t* ptr = new int32_t[sz];
    _GM_MEM.save(ptr, 0, thread_id);
    return ptr;
}

float* gm_rt_allocate_float(size_t sz, int thread_id) {
    float* ptr = new float[sz];
    _GM_MEM.save(ptr, 0, thread_id);
    return ptr;
}

bool* gm_rt_allocate_bool(size_t sz, int thread_id) {
    bool* ptr = new bool[sz];
    assert(ptr != NULL);
    _GM_MEM.save(ptr, 0, thread_id);
    return ptr;
}

double* gm_rt_allocate_double(size_t sz, int thread_id) {
    double* ptr = new double[sz];
    _GM_MEM.save(ptr, 0, thread_id);
    return ptr;
}

node_t* gm_rt_allocate_node_t(size_t sz, int thread_id) {
    node_t* ptr = new node_t[sz];
    _GM_MEM.save(ptr, 0, thread_id);
    return ptr;
}
edge_t* gm_rt_allocate_edge_t(size_t sz, int thread_id) {
    node_t* ptr = new edge_t[sz];
    _GM_MEM.save(ptr, 0, thread_id);
    return ptr;
}

void gm_rt_deallocate(void* ptr, int thread_id) {
    _GM_MEM.clear(ptr, 0, thread_id);
}

void gm_rt_cleanup() {
    _GM_MEM.cleanup();
}
