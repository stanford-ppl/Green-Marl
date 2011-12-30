
#ifndef GM_MEM_HELPER_H
#define GM_MEM_HELPER_H

#include <stdint.h>
#include <list>
//---------------------------------------------------------
// A thin runtime for object deletion management
//---------------------------------------------------------
class gm_mem_helper 
{
public:
    gm_mem_helper(): memptrs(NULL), max(0) {
    }

    virtual ~gm_mem_helper() {
        destroy();
    }

    void resize(int n) 
    {
        if (n <= max) return;

        destroy();
        
        max = n;
        memptrs = new std::list<void* >* [max];
        for(int i=0;i<max;i++) {
            memptrs[i] = new std::list<void*>();
        }
    }

private:
    void destroy()
    {
        for(int i=0;i<max;i++)
        {
            delete memptrs[i];
        }
        delete [] memptrs;
    }

public:
    // save pointer of primitive array type into a thread-private list
    void save(void* ptr, int typeinfo, int thread_id=0) {
        std::list<void*>* L = memptrs[thread_id];
        L->push_back(ptr);
    }

    // remove pointer of primitive array type and remove it from the list
    void clear(void* ptr, int typeinfo, int thread_id=0) {
        std::list<void*>* L = memptrs[thread_id];
        std::list<void*>::iterator i;
        for(i=L->begin(); i!= L->end(); i++) {
            if (*i == ptr) {
                // primitive aray. 
                // This will work for primitive types.
                float* P = (float*) *i;
                delete [] P;

                L->erase(i);
                return;
            }
        }
    }

    // (called at function exit)
    // remove all the saved pointers
    void cleanup() {
        // remove every thread'
        for(int p=0;p<max;p++)  {
            std::list<void*>* L = memptrs[p];
            std::list<void*>::iterator i;
            for(i=L->begin(); i!= L->end(); i++) {
                //delete [] *i;
                float* P = (float*) *i;
                delete [] P;
            }
            L->clear();
        }
    }

private:
    std::list<void* >** memptrs;
    int max;
};

// defined in gm_runtime.cc
extern gm_mem_helper _GM_MEM;

static inline int64_t* gm_rt_allocate_long  (size_t sz, int thread_id=0) 
{ 
    int64_t* ptr = new int64_t[sz];
    _GM_MEM.save(ptr, 0, thread_id); 
    return ptr;
}

static inline int32_t* gm_rt_allocate_int   (size_t sz, int thread_id=0) 
{
    int32_t* ptr = new int32_t[sz];
    _GM_MEM.save(ptr, 0, thread_id); 
    return ptr;
}

static inline float*   gm_rt_allocate_float (size_t sz, int thread_id=0) 
{
    float* ptr = new float[sz];
    _GM_MEM.save(ptr, 0, thread_id); 
    return ptr;
}

static inline double*  gm_rt_allocate_double(size_t sz, int thread_id=0)
{ 
    double* ptr = new double[sz];
    _GM_MEM.save(ptr, 0, thread_id); 
    return ptr;
}

static inline void gm_rt_deallocate(void* ptr, int thread_id=0) 
{ 
    _GM_MEM.clear(ptr, 0, thread_id); 
}
static inline void gm_rt_cleanup() 
{ 
    _GM_MEM.cleanup(); 
}


#endif
