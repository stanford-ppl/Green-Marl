#ifndef GM_MAP_H_
#define GM_MAP_H_

#include <map>
#include "gm_graph_typedef.h"

using namespace std;

//map-interface
template<class Key, class Value>
class gm_map
{
public:
    virtual ~gm_map() {
    };

    virtual Value getValue(const Key key) = 0;
    virtual void setValue(const Key key, Value value) = 0;

    virtual bool hasMaxValue(const Key key) = 0;
    virtual bool hasMinValue(const Key key) = 0;
    virtual Key getMaxKey() = 0;
    virtual Key getMinKey() = 0;
};


template<class Key, class Value, bool>
class gm_map_impl: public gm_map<Key, Value>
{
};


template<class Key, class Value>
class gm_map_impl<Key, Value, true> : public gm_map<Key, Value>
{
private:
    map<Key, Value> data;
    typedef typename map<Key, Value>::iterator Iterator;

public:
    ~gm_map_impl() {
    }

    gm_map_impl() {
    }

    Value getValue(const Key key) {
        return data[key];
    }

    void setValue(const Key key, Value value) {
        data[key] = value;
    }

    bool hasMaxValue(const Key key) {
        Value value = getValue(key);
        for (Iterator iter = data.begin(); iter != data.end(); iter++)
            if (value < (*iter).second) return false;
        return true;
    }

    bool hasMinValue(const Key key) {
        Value value = getValue(key);
        for (Iterator iter = data.begin(); iter != data.end(); iter++)
            if (value > (*iter).second) return false;
        return true;
    }

    Key getMaxKey() {
        assert(data.size() > 0);
        Value value = (*data.begin()).second;
        Iterator iter = data.begin();
        for (iter++; iter != data.end(); iter++)
            if (value < (*iter).second) value = (*iter).second;
        return value;
    }

    Key getMinKey() {
        assert(data.size() > 0);
        Value value = (*data.begin()).second;
        Iterator iter = data.begin();
        for (iter++; iter != data.end(); iter++)
            if (value > (*iter).second) value = (*iter).second;
        return value;
    }
};


template<class Key, class Value>
class gm_map_impl<Key, Value, false> : public gm_map<Key, Value>
{
private:
    int size;
    Value* data;

public:
    gm_map_impl(int size) : size(size) {
        data = new Value[size];
        #pragma omp parallel for
        for(int i = 0; i < size; i++)
            data[i] = 0;
    }

    ~gm_map_impl() {
        delete[] data;
    }

    Value getValue(const Key key) {
        return data[key];
    }

    void setValue(const Key key, Value value) {
        data[key] = value;
    }

    bool hasMaxValue(const Key key) {
        Value value = getValue(key);
        bool result = true;
        #pragma omp parallel for
        for(int i = 0; i < size; i++)
            if(value < data[i]) result = false;
        return result;
    }

    bool hasMinValue(const Key key) {
        Value value = getValue(key);
        bool result = true;
        #pragma omp parallel for
        for(int i = 0; i < size; i++)
            if(value > data[i]) result = false;
        return result;
    }

    Key getMaxKey() {
        Value value = data[0];
        bool result = true;
        #pragma omp parallel for
        for(int i = 1; i < size; i++)
            if(value < data[i]) result = false;
        return result;
    }

    Key getMinKey() {
        Value value = data[0];
        bool result = true;
        #pragma omp parallel for
        for(int i = 1; i < size; i++)
            if(value < data[i]) result = false;
        return result;
    }

};

#endif /* GM_MAP_H_ */
