#ifndef GM_MAP_H_
#define GM_MAP_H_

#include <map>
#include "gm_graph_typedef.h"

using namespace std;

/*don't trust Eclipse - trust your compiler */

//map-interface
template<class Key, class Value>
class gm_map
{
public:
    virtual ~gm_map() {
    }
    ;

    virtual bool hasKey(const Key key) = 0;

    /**
     * Return the value that has been set for the key - if none has been specified, the defaultValue is returned
     */
    virtual Value getValue(const Key key) = 0;
    virtual void setValue(const Key key, Value value) = 0;

    /**
     * Returns true if the key corresponds to the highest value in the map.
     * If there has no value been set in the map, false is returned
     */
    virtual bool hasMaxValue(const Key key) = 0;

    /**
     * Returns true if the key corresponds to the lowest value in the map.
     * If there has no value been set in the map, false is returned
     */
    virtual bool hasMinValue(const Key key) = 0;

    /**
     * Returns the key that corresponds to the highest value in the map.
     * If there has no value been set in the map, the behavior is unspecified.
     */
    virtual Key getMaxKey() = 0;

    /**
     * Returns the key that corresponds to the lowest value in the map.
     * If there has no value been set in the map, the behavior is unspecified.
     */
    virtual Key getMinKey() = 0;

    /**
     * Returns the highest value in the map.
     * If there has no value been set in the map, the behavior is unspecified.
     */
    virtual Value getMaxValue() = 0;

    /**
     * Returns the lowest value in the map.
     * If there has no value been set in the map, the behavior is unspecified.
     */
    virtual Value getMinValue() = 0;

    virtual size_t size() = 0;
};

template<class Key, class Value, bool, Value defaultValue>
class gm_map_impl: public gm_map<Key, Value>
{
};

template<class Key, class Value, Value defaultValue>
class gm_map_impl<Key, Value, true, defaultValue> : public gm_map<Key, Value>
{
private:
    map<Key, Value> data;
    typedef typename map<Key, Value>::iterator Iterator;

public:
    ~gm_map_impl() {
    }

    bool hasKey(const Key key) {
        return data.find(key) != data.end();
    }

    Value getValue(const Key key) {
        if (hasKey(key))
            return data[key];
        else
            return defaultValue;
    }

    void setValue(const Key key, Value value) {
        data[key] = value;
    }

    bool hasMaxValue(const Key key) {
        if (size() == 0 || !hasKey(key)) return false;
        Value value = data[key];
        for (Iterator iter = data.begin(); iter != data.end(); iter++)
            if (value < iter->second) return false;
        return true;
    }

    bool hasMinValue(const Key key) {
        if (size() == 0 || !hasKey(key)) return false;
        Value value = data[key];
        for (Iterator iter = data.begin(); iter != data.end(); iter++)
            if (value > iter->second) return false;
        return true;
    }

    Key getMaxKey() {
        assert(size() > 0);
        Iterator iter = data.begin();
        Key key = iter->first;
        Value value = iter->second;
        for (iter++; iter != data.end(); iter++)
            if (value < iter->second) {
                key = iter->first;
                value = iter->second;
            }
        return key;
    }

    Key getMinKey() {
        assert(size() > 0);
        Iterator iter = data.begin();
        Key key = iter->first;
        Value value = iter->second;
        for (iter++; iter != data.end(); iter++)
            if (value > iter->second) {
                key = iter->first;
                value = iter->second;
            }
        return key;
    }

    Value getMaxValue() {
        assert(size() > 0);
        Iterator iter = data.begin();
        Value value = iter->second;
        for (iter++; iter != data.end(); iter++)
            if (value < iter->second) {
                value = iter->second;
            }
        return value;
    }

    Value getMinValue() {
        assert(size() > 0);
        Iterator iter = data.begin();
        Value value = iter->second;
        for (iter++; iter != data.end(); iter++)
            if (value > iter->second) {
                value = iter->second;
            }
        return value;
    }

    size_t size() {
        return data.size();
    }

};

template<class Key, class Value, Value defaultValue>
class gm_map_impl<Key, Value, false, defaultValue> : public gm_map<Key, Value>
{
private:
    const int size_;
    Value* const data;
    bool * const valid;

public:
    gm_map_impl(int size) :
            size_(size), data(new Value[size]), valid(new bool[size]) {
#pragma omp parallel for
        for (int i = 0; i < size; i++) {
            data[i] = defaultValue;
            valid[i] = false;
        }
    }

    ~gm_map_impl() {
        delete[] data;
        delete[] valid;
    }

    bool hasKey(const Key key) {
        return valid[key];
    }

    Value getValue(const Key key) {
        if (hasKey(key))
            return data[key];
        else
            return defaultValue;
    }

    void setValue(const Key key, Value value) {
        data[key] = value;
        valid[key] = true;
    }

    bool hasMaxValue(const Key key) {
        if (size() == 0 || !hasKey(key)) return false;
        Value value = data[key];
        bool result = true;
        #pragma omp parallel for
        for (int i = 0; i < size(); i++)
            if (valid[i] && value < data[i]) result = false;
        return result;
    }

    bool hasMinValue(const Key key) {
        if (size() == 0 || !hasKey(key)) return false;
        Value value = data[key];
        bool result = true;
        #pragma omp parallel for
        for (int i = 0; i < size(); i++)
            if (valid[i] && value > data[i]) result = false;
        return result;
    }

    Key getMaxKey() {
        assert(size() > 0);
        Value* value;
        Key key = 0;
        while (!valid[key])
            key++;
        value = data + key;
        //#pragma omp parallel for
        for (Key i = key + 1; i < size(); i++) {
            if (valid[i] && *value < data[i]) {
                key = i;
                value = data + i;
            }
        }
        return key;
    }

    Key getMinKey() {
        assert(size() > 0);
        Value value;
        Key key = 0;
        while (!valid[key])
            key++;
        value = data[key];
        //#pragma omp parallel for
        for (Key i = key + 1; i < size(); i++) {
            if (valid[i] && value > data[i]) {
                key = i;
                value = data[i];
            }
        }
        return key;
    }

    Value getMaxValue() {
        assert(size() > 0);
        Value value;
        Key key = 0;
        while (!valid[key])
            key++;
        value = data[key];


//            #pragma omp parallel for
//            for (Key i = key + 1; i < size(); i++) {
//                if (valid[i] && value < data[i]) {
//                    #pragma omp critical
//                    if (value < data[i]) value = data[i];
//                }
//            }

            for (Key i = key + 1; i < size(); i++) {
                if (valid[i] && value < data[i]) value = data[i];
            }


        return value;
    }

    Value getMinValue() {
        assert(size() > 0);
        Value value;
        Key key = 0;
        while (!valid[key])
            key++;
        value = data[key];
        //#pragma omp parallel for
        for (Key i = key + 1; i < size(); i++)
            if (valid[i] && value > data[i]) value = data[i];
        return value;
    }

    size_t size() {
        return size_;
    }

};

#endif /* GM_MAP_H_ */
