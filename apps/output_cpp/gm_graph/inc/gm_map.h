#ifndef GM_MAP_H_
#define GM_MAP_H_

#include <map>
#include "gm_graph_typedef.h"
#include "gm_limits.h"
#include "gm_lock.h"

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

    virtual void clear() = 0;

};

template<class Key, class Value, Value defaultValue>
class gm_map_small : public gm_map<Key, Value>
{
private:
    map<Key, Value> data;
    gm_spinlock_t lock;
    typedef typename map<Key, Value>::iterator Iterator;

public:
    gm_map_small() : lock(0) {

    }

    ~gm_map_small() {
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
        gm_spinlock_acquire(&lock);
        data[key] = value;
        gm_spinlock_release(&lock);
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

    void clear() {
        data.clear();
    }

};


template<class Key, class Value, Value defaultValue>
class gm_map_large : public gm_map<Key, Value>
{
private:
    const size_t size_;
    Value* const data;
    bool * const valid;

    template<class Function>
    inline Value getValue_generic(Function func, const Value initialValue) {
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

    template<class Function>
    inline Key getKey_generic(Function compare, const Value initialValue) {

        assert(size() > 0);

        Value value = initialValue;
        Key key = 0;

        #pragma omp parallel
        {
            Value value_private = value;
            Key key_private = key;

            #pragma omp for nowait
            for (Key i = 0; i < size(); i++) {
                if (valid[i] && compare(data[i], value_private)) {
                    value_private = data[i];
                    key_private = i;
                }
            }
            // reduction
            if(compare(value_private, value)) {
                #pragma omp critical
                {
                    if(compare(value_private, value)) {
                        value = value_private;
                        key = key_private;
                    }
                }
            }
        }
        return key;
    }

    template<class Function>
    inline bool hasKey_generic(Function compare, const Key key) {
        if (size() == 0 || !hasKey(key)) return false;
        Value value = data[key];
        bool result = true;
        #pragma omp parallel for
        for (int i = 0; i < size(); i++)
            if (valid[i] && compare(data[i], value)) result = false;
        return result;
    }

    static bool compare_smaller(Value a, Value b) {
        return a < b;
    }

    static bool compare_greater(Value a, Value b) {
        return a > b;
    }

public:
    gm_map_large(size_t size) :
            size_(size), data(new Value[size]), valid(new bool[size]) {
        #pragma omp parallel for
        for (int i = 0; i < size; i++) {
            valid[i] = false;
        }
    }

    ~gm_map_large() {
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
        return hasKey_generic(&compare_greater, key);
    }

    bool hasMinValue(const Key key) {
        return hasKey_generic(&compare_smaller, key);
    }

    Key getMaxKey() {
        return getKey_generic(&compare_greater, gm_get_min<Value>());
    }

    Key getMinKey() {
        return getKey_generic(&compare_smaller, gm_get_max<Value>());
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

    void clear() {
        #pragma omp parallel for
        for (Key key = 0; key < size(); key++) {
            valid[key] = false;
        }
    }

};

#endif /* GM_MAP_H_ */
