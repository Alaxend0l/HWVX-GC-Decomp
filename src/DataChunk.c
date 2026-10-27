#include "common.h"


void UpdatePointers(u32 *offsets, u32 count, u8 *base) {
    u32 i;

    for (i = 0; i < count; i++) {
        *(u32 *)(base + offsets[i]) += (u32)base;
    }
}
