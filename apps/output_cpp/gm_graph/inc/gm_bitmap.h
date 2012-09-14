#ifndef GM_BITMAP_H_
#define GM_BITMAP_H_

#include <stdint.h>
#include "gm_atomic_wrapper.h"

unsigned _gm_get_bit(unsigned char* Bit, int n);

void _gm_set_bit(unsigned char* BitMap, int n);

// true if I'm the one who set the bit
bool _gm_set_bit_atomic(unsigned char* BitMap, int n);

void _gm_clear_bit(unsigned char* BitMap, int n);

bool _gm_clear_bit_atomic(unsigned char* BitMap, int n);

#endif
