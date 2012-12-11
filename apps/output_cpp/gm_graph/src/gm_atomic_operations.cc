#include "gm_atomic_wrapper.h"

void ATOMIC_AND(bool* target, bool value) {
    if ((!value) || (*target)) *target = false;
}

void ATOMIC_OR(bool* target, bool value) {
    if ((value) && !(*target)) *target = true;
}
