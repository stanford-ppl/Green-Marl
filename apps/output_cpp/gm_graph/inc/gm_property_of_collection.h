#ifndef GM_TESTSET_H
#define GM_TESTSET_H

#include <stdio.h>
#include <omp.h>
#include "gm_set.h"

#ifdef __APPLE__
	#include <libkern/OSAtomic.h>

	#define Spinlock 	OSSpinLock
	#define lock(X) 	OSSpinLockLock(&X)
	#define unlock(X)	OSSpinLockUnlock(&X)
	#define init(X)		;
	#define destroy(X)	;
#else
	#include <pthread.h>

	#define Spinlock 	pthread_spinlock_t
	#define lock(X) 	pthread_spin_lock(&X)
	#define unlock(X)	pthread_spin_unlock(&X)
	#define init(X)		pthread_spin_init(&X, 0)
	#define destroy(X)	pthread_spin_destroy(&X)
#endif

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
    T** data;Spinlock* locks;
    int size;

    inline void lazyInit(int index) {
        lock(locks[index]);
        if (data[index] == NULL) data[index] = new T(size);
        unlock(locks[index]);
    }

public:

    gm_property_of_collection_impl(int size) :
            size(size), locks(NULL) {

        data = new T*[size];
        #pragma omp parallel for
        for (int i = 0; i < size; i++) {
            if (lazy) {
                data[i] = NULL;
                init(locks[i]);
            } else {
                data[i] = new T(size);
            }
        }
    }

    ~gm_property_of_collection_impl() {
        for (int i = 0; i < size; i++) {
            if (lazy) destroy(locks[i]);
            delete data[i];
        }
        delete[] data;
        if (lazy) delete[] locks;
    }

    T& operator[](int index) {
        if (lazy && data[index] == NULL) lazyInit(index);
        return *data[index];
    }
};

#undef Spinlock
#undef lock
#undef unlock
#undef init
#undef destroy

#endif
