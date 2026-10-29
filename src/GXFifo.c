#include "common.h"

typedef struct __GXFifoObj {
    u8 *base;
    u8 *top;
    u32 size;
    u32 hiWatermark;
    u32 loWatermark;
    void *rdPtr;
    void *wrPtr;
    s32 count;
    u8 bind_cpu;
    u8 bind_gp;
} __GXFifoObj;

typedef struct GXFifoObj {
    u8 pad[128];
} GXFifoObj;
