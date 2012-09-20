#ifndef GM_TESTSET_H
#define GM_TESTSET_H

#include <stdio.h>
#include <omp.h>
#include "gm_set.h"
#include "gm_lock.h"

class gm_complex_data_type
{
  public:
    virtual ~gm_complex_data_type() {
    }

};

template<class T>
class gm_property_of_collection: public gm_complex_data_type
{
  public:
    virtual T& operator[](int index) = 0;

    virtual ~gm_property_of_collection() {
    }

};

template<class T, bool lazy>
class gm_property_of_collection_impl: public gm_property_of_collection<T>
{
  private:
    T** data;
    gm_spinlock_t* locks;
    int size;

    inline void lazyInit(int index) {
        gm_spinlock_acquire(locks + index);
        if (data[index] == NULL) data[index] = new T(size);
        gm_spinlock_release(locks + index);
    }

  public:
    gm_property_of_collection_impl(int size) :
            size(size), locks(NULL) {

        data = new T*[size];
        if(lazy) locks = new gm_spinlock_t[size];

        #pragma omp parallel for
        for (int i = 0; i < size; i++) {
            if (lazy) {
                data[i] = NULL;
                locks[i] = 0;
            } else {
                data[i] = new T(size);
            }
        }
    }

    ~gm_property_of_collection_impl() {
        for (int i = 0; i < size; i++)
            delete data[i];
        delete[] data;
        if (lazy) delete[] locks;
    }

    T& operator[](int index) {
        if (lazy && data[index] == NULL) lazyInit(index);
        return *data[index];
    }
};

#endif
