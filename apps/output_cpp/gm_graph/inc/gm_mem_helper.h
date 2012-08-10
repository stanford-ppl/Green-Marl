#ifndef GM_MEM_HELPER_H
#define GM_MEM_HELPER_H

#include <stdint.h>
#include <vector>
#include <list>
#include "gm_internal.h"
#include "gm_property_of_collection.h"

//---------------------------------------------------------
// A thin runtime for object deletion management
//---------------------------------------------------------
class gm_mem_helper
{
public:
    gm_mem_helper() :
            max(0) {
    }

    virtual ~gm_mem_helper() {
        destroy();
    }

    void resize(int n) {
        if ((max > 0) && (n <= max)) return;
        primitivePointers.resize(n);
        complexPointers.resize(n);
        max = n;
    }

private:
    void destroy() {
    }

public:
    // save pointer of primitive array type into a thread-private list
    void save(void* ptr, int typeinfo, int thread_id = 0) {
        primitivePointers[thread_id].push_back(ptr);
    }

    //save pointer of complex type into a thread-private list
    void save(gm_complex_data_type* ptr, int typeinfo, int thread_id = 0) {
        complexPointers[thread_id].push_back(ptr);
    }

    // remove pointer of primitive array type and remove it from the list
    void clear(void* ptr, int typeinfo, int thread_id = 0) {
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

    // (called at function exit)
    // remove all the saved pointers
    void cleanup() {
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

private:
    int max;
    std::vector<std::list<void*> > primitivePointers;
    std::vector<std::list<gm_complex_data_type*> > complexPointers;
};

// defined in gm_runtime.cc
extern gm_mem_helper _GM_MEM;

static inline int64_t* gm_rt_allocate_long(size_t sz, int thread_id = 0) {
    int64_t* ptr = new int64_t[sz];
    _GM_MEM.save(ptr, 0, thread_id);
    return ptr;
}

static inline int32_t* gm_rt_allocate_int(size_t sz, int thread_id = 0) {
    int32_t* ptr = new int32_t[sz];
    _GM_MEM.save(ptr, 0, thread_id);
    return ptr;
}

static inline float* gm_rt_allocate_float(size_t sz, int thread_id = 0) {
    float* ptr = new float[sz];
    _GM_MEM.save(ptr, 0, thread_id);
    return ptr;
}
static inline bool* gm_rt_allocate_bool(size_t sz, int thread_id = 0) {
    bool* ptr = new bool[sz];
    assert(ptr != NULL);
    _GM_MEM.save(ptr, 0, thread_id);
    return ptr;
}

static inline double* gm_rt_allocate_double(size_t sz, int thread_id = 0) {
    double* ptr = new double[sz];
    _GM_MEM.save(ptr, 0, thread_id);
    return ptr;
}
static inline node_t* gm_rt_allocate_node_t(size_t sz, int thread_id = 0) {
    node_t* ptr = new node_t[sz];
    _GM_MEM.save(ptr, 0, thread_id);
    return ptr;
}

template<class collectionType, bool lazy>
static inline gm_property_of_collection<collectionType>& gm_rt_allocate_collection(size_t size, int thread_id = 0) {
    gm_property_of_collection<collectionType>* pointer = new gm_property_of_collection_impl<collectionType, lazy>(size);
    _GM_MEM.save(pointer, 0, thread_id);
    return *pointer;
}

static inline void gm_rt_deallocate(void* ptr, int thread_id = 0) {
    _GM_MEM.clear(ptr, 0, thread_id);
}
static inline void gm_rt_cleanup() {
    _GM_MEM.cleanup();
}

#endif
