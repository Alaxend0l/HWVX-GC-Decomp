#include "common.h"


s32 __count_trailing_zerol(u32 x) {
    s32 result = 0;
    s32 bits_not_checked = sizeof(u32) * 8;
    s32 n = bits_not_checked / 2;
    s32 mask_size = n;
    u32 mask = (~0UL) >> (bits_not_checked - n);

    while (bits_not_checked) {
        if (!(x & mask)) {
            result += mask_size;
            x >>= mask_size;
            bits_not_checked -= mask_size;
        } else if (mask == 1) {
            break;
        }
        if (n > 1) {
            n /= 2;
        }
        if (mask > 1) {
            mask >>= n;
            mask_size -= n;
        }
    }
    return result;
}
