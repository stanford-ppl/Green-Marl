#ifndef GM_BITMAP_H
#define GM_BITMAP_H

#include <stdint.h>
#include "gm_atomic_wrapper.h"

static inline unsigned _gm_get_bit(unsigned char* Bit, int n) {
    int bit_pos = n / 8;
    int bit_loc = n % 8;
    unsigned val = (Bit[bit_pos] >> bit_loc) & 0x01;
    return val;
}

static inline void _gm_set_bit(unsigned char* BitMap, int n) {
    int bit_pos = n / 8;
    int bit_loc = n % 8;
    unsigned char or_val = 0x1 << bit_loc;
    unsigned char org_val = BitMap[bit_pos];
    unsigned char new_val = or_val | org_val;
    BitMap[bit_pos] = new_val;
}

// true if I'm the one who set the bit
static inline bool _gm_set_bit_atomic(unsigned char* BitMap, int n) {
    int bit_pos = n / 8;
    int bit_loc = n % 8;
    unsigned char or_val = 0x1 << bit_loc;
    unsigned char old_val = _gm_atomic_or(&BitMap[bit_pos], or_val);
    if (((old_val >> bit_loc) & 0x01) == 0) return true;
    return false;
}

static inline void _gm_clear_bit(unsigned char* BitMap, int n) {
    int bit_pos = n / 8;
    int bit_loc = n % 8;
    unsigned char and_val = ~(0x1 << bit_loc);
    unsigned char org_val = BitMap[bit_pos];
    unsigned char new_val = org_val & and_val;
    BitMap[bit_pos] = new_val;
}

static inline bool _gm_clear_bit_atomic(unsigned char* BitMap, int n) {
    int bit_pos = n / 8;
    int bit_loc = n % 8;
    unsigned char and_val = ~(0x1 << bit_loc);
    unsigned char old_val = _gm_atomic_and(&BitMap[bit_pos], and_val);
    if (((old_val >> bit_loc) & 0x01) == 1) return true; // Am I the one who cleared the bit?
    return false;
}

#endif
