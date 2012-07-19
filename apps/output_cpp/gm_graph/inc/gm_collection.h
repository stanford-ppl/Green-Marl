#ifndef GM_COLLECTION_H_
#define GM_COLLECTION_H_

#include <stdio.h>
#include <list>

using namespace std;

template<class T>
class gm_collection
{

private:
    list<T> data;

public:
    gm_collection(int maxNumberThreads = 16) {
    }

    void push_back(T e) {
        data.push_back(e);
    }

    void push_front(T e) {
        data.push_front(e);
    }

    void push_back_par(T e, int tid) {
        assert(false); //TODO
    }

    void push_front_par(T e, int tid) {
        assert(false); //TODO
    }

    void pop_back() {
        data.pop_back();
    }

    void pop_front() {
        data.pop_front();
    }

    void clear() {
        data.clear();
    }

    int get_size() {
        return data.size();
    }

};

#endif /* GM_COLLECTION_H_ */
