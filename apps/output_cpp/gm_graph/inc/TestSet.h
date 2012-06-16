#ifndef GM_TESTSET_H
#define GM_TESTSET_H

#include <stdio.h>
#include <omp.h>
#include <pthread.h>
#include "gm_set.h"

#define Spinlock 	pthread_spinlock_t
#define lock(X) 	pthread_spin_lock(&X)
#define unlock(X)	pthread_spin_unlock(&X)
#define init(X)		pthread_spin_init(&X, 0)
#define destroy(X)	pthread_spin_destroy(&X)

template<class T>
class gm_property_of_collection {

public:
	virtual T& operator[](int index) = 0;

};

template<class T, bool lazy>
class TestSet : public  gm_property_of_collection<T> {

private:
	T** data;
	Spinlock* locks;
	int size;

	inline void lazyInit(int index) {
		lock(locks[index]);
		if (data[index] == NULL)
			data[index] = new T(size);
		unlock(locks[index]);
	}

public:

	TestSet(int size) : size(size) {
		data = new T*[size];
		if (lazy) {
			locks = new Spinlock[size];
			#pragma omp parallel for
			for (int i = 0; i < size; i++) {
				data[i] = NULL;
				init(locks[i]);
			}
		} else {
			#pragma omp parallel for
			for (int i = 0; i < size; i++)
				data[i] = new T(size);
		}
	}

	~TestSet() {
		for(int i = 0; i < size; i++) {
			destroy(locks[i]);
			delete data[i];
		}
		delete[] data;
		if(lazy) delete[] locks;
	}

	inline T& operator[](int index) {
		if (lazy)
			if (data[index] == NULL)
				lazyInit(index);
		return *data[index];
	}
};

#undef Spinlock
#undef lock
#undef unlock
#undef init
#undef destroy

#endif
