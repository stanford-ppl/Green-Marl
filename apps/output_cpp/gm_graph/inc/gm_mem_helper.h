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
    gm_mem_helper() : max(0) {}

    virtual ~gm_mem_helper() {
        destroy();
    }

    void resize(int n);

    // save pointer of primitive array type into a thread-private list
    void save(void* ptr, int typeinfo, int thread_id = 0);

    //save pointer of complex type into a thread-private list
    void save(gm_complex_data_type* ptr, int typeinfo, int thread_id = 0);

    // remove pointer of primitive array type and remove it from the list
    void clear(void* ptr, int typeinfo, int thread_id = 0);

    // (called at function exit)
    // remove all the saved pointers
    void cleanup();

  private:
    void destroy() {}

    int max;
    std::vector<std::list<void*> > primitivePointers;
    std::vector<std::list<gm_complex_data_type*> > complexPointers;
};

// defined in gm_runtime.cc
extern gm_mem_helper _GM_MEM;

/*
 * Wrappers to call the gm_mem_helper methods
 *    - Shouldnt they be static members of the gm_mem_helper class?
 */
int64_t* gm_rt_allocate_long(size_t sz, int thread_id = 0);
int32_t* gm_rt_allocate_int(size_t sz, int thread_id = 0);
float* gm_rt_allocate_float(size_t sz, int thread_id = 0);
bool* gm_rt_allocate_bool(size_t sz, int thread_id = 0);
double* gm_rt_allocate_double(size_t sz, int thread_id = 0);
node_t* gm_rt_allocate_node_t(size_t sz, int thread_id = 0);

template<class collectionType, bool lazy>
gm_property_of_collection<collectionType>& gm_rt_allocate_collection(size_t size, int thread_id = 0) {
    gm_property_of_collection<collectionType>* pointer = new gm_property_of_collection_impl<collectionType, lazy>(size);
    _GM_MEM.save(pointer, 0, thread_id);
    return *pointer;
}

void gm_rt_deallocate(void* ptr, int thread_id = 0);
void gm_rt_cleanup();

#endif
