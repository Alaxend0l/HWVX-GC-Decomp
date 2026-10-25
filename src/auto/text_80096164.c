/* Functions from 0x80096164 to 0x800967B0, found by decomp-toolkit. */
#include "common.h"

s32 __shl2i(s32 arg0, u32 arg1, s32 arg2) {
    return (arg0 << arg2) | (arg1 >> (0x20 - arg2)) | (arg1 << (arg2 - 0x20));
}

u64 __shr2u(u32 hi, u32 lo, s32 n) {
    u32 rlo = (lo >> n) | (hi << (32 - n)) | (hi >> (n - 32));
    u32 rhi = hi >> n;
    return ((u64)rhi << 32) | rlo;
}
