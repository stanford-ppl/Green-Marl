#ifndef GM_ATOMIC_OPERATIONS_H_
#define GM_ATOMIC_OPERATIONS_H_

template<typename T>
void ATOMIC_ADD(T* target, T value) {

    if (value == 0) return;

    T oldValue, newValue;
    do {
        oldValue = *target;
        newValue = oldValue + value;
    } while (_gm_atomic_compare_and_swap(target, oldValue, newValue) == false);
}

template<typename T>
void ATOMIC_MULT(T* target, T value) {

    if (value == 1) return;

    T oldValue, newValue;
    do {
        oldValue = *target;
        newValue = oldValue * value;
    } while (_gm_atomic_compare_and_swap(target, oldValue, newValue) == false);
}

template<typename T>
void ATOMIC_MIN(T* target, T value) {
    T oldValue, newValue;
    do {
        oldValue = *target;
        newValue = std::min(oldValue, value);
        if (oldValue == newValue) return;
    } while (_gm_atomic_compare_and_swap(target, oldValue, newValue) == false);
}

template<typename T>
void ATOMIC_MAX(T* target, T value) {
    T oldValue, newValue;
    do {
        oldValue = *target;
        newValue = std::max(oldValue, value);
        if (oldValue == newValue) return;
    } while (_gm_atomic_compare_and_swap(target, oldValue, newValue) == false);
}

void ATOMIC_AND(bool* target, bool value);
void ATOMIC_OR(bool* target, bool value);

#endif /* GM_ATOMIC_OPERATIONS_H_ */
