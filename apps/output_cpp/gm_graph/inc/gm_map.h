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

#include <limits.h>

template<class Type>
Type gm_get_min();

template<class Type>
Type gm_get_max();

template<>
inline int32_t gm_get_min() {
    return INT_MIN;
}

template<>
inline int32_t gm_get_max() {
    return INT_MAX;
}

template<>
inline int64_t gm_get_min() {
    return LONG_MIN;
}

template<>
inline int64_t gm_get_max() {
    return LONG_MAX;
}

template<class Key, class Value, Value defaultValue>
class gm_map_impl<Key, Value, false, defaultValue> : public gm_map<Key, Value>
{
private:
    const int size_;
    Value* const data;
    bool * const valid;

    template<class Function>
    Value getValue_generic(Function func, Value initialValue) {
        assert(size() > 0);

        Value value = initialValue;
        #pragma omp parallel
        {
            Value value_private = value;

            #pragma omp for nowait
            for (Key i = 0; i < size(); i++) {
                if (valid[i]) value_private = func(value_private, data[i]);
            }
            // reduction
            {
                Value value_old;
                Value value_new;
                do {
                    value_old = value;
                    value_new = func(value_old, value_private);
                    if (value_old == value_new) break;
                } while (_gm_atomic_compare_and_swap(&(value), value_old, value_new) == false);
            }
        }
        return value;
    }

public:
    gm_map_impl(int size) :
            size_(size), data(new Value[size]), valid(new bool[size]) {
        #pragma omp parallel for
        for (int i = 0; i < size; i++) {
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

        Value maxValue = gm_get_min<Value>();
        Key key = 0;
        #pragma omp parallel
        {
            Value maxValue_private = maxValue;
            Key key_private = key;

            #pragma omp for nowait
            for (Key i = 0; i < size(); i++) {
                if (valid[i] && maxValue_private < data[i]) {
                    maxValue_private = data[i];
                    key_private = i;
                }
            }
            // reduction
            {
                Value maxValue_old;
                Value maxValue_new;
                Key key_old;
                Key key_new;
                do {
                    key_old = key;
                    maxValue_old = maxValue;
                    if(maxValue_old < maxValue_private) {
                        maxValue_new = maxValue_private;
                        key_new = key_private;
                    } else {
                        break;
                    }
                } while (!_gm_atomic_compare_and_swap(&(maxValue), maxValue_old, maxValue_new) || !_gm_atomic_compare_and_swap(&(key), key_old, key_new));
            }
        }
        return key;
    }

    Key getMinKey() {
        assert(size() > 0);

        Value minValue = gm_get_max<Value>();
        Key key = 0;
        #pragma omp parallel
        {
            Value minValue_private = minValue;
            Key key_private = key;

            #pragma omp for nowait
            for (Key i = 0; i < size(); i++) {
                if (valid[i] && minValue_private > data[i]) {
                    minValue_private = data[i];
                    key_private = i;
                }
            }
            // reduction
            {
                Value minValue_old;
                Value minValue_new;
                Key key_old;
                Key key_new;
                do {
                    key_old = key;
                    minValue_old = minValue;
                    if(minValue_old > minValue_private) {
                        minValue_new = minValue_private;
                        key_new = key_private;
                    } else {
                        break;
                    }
                } while (!_gm_atomic_compare_and_swap(&(minValue), minValue_old, minValue_new) || !_gm_atomic_compare_and_swap(&(key), key_old, key_new));
            }
        }
        printf("minKey: %d\n", key);
        return key;
    }

    Value getMaxValue() {
        return getValue_generic(std::max<Value>, gm_get_min<Value>());
    }

    Value getMinValue() {
        return getValue_generic(std::min<Value>, gm_get_max<Value>());
    }

    size_t size() {
        return size_;
    }

};

#endif /* GM_MAP_H_ */
