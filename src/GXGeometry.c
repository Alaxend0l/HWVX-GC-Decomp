#include "common.h"

typedef union PPCWGPipe {
    u8 u8;
    u16 u16;
    u32 u32;
    s32 s32;
    f32 f32;
} PPCWGPipe;

volatile PPCWGPipe GXWGFifo : 0xCC008000;

typedef struct GXData {
    u16 vNumNot;
    u16 bpSentNot;
    u8 pad4[0x204 - 0x4];
    u32 genMode;
    u8 pad208[0x4F4 - 0x208];
    u32 dirtyState;
} GXData;

extern GXData *gx;

#pragma push
#pragma peephole on
void GXSetCullMode(s32 mode) {
    s32 hwMode;

    switch (mode) {
    case 1:
        hwMode = 2;
        break;
    case 2:
        hwMode = 1;
        break;
    default:
        hwMode = mode;
        break;
    }
    gx->genMode = (gx->genMode & ~0xC000) | (hwMode << 14);
    gx->dirtyState |= 4;
}

void GXSetCoPlanar(u8 enable) {
    gx->genMode = (gx->genMode & ~0x80000) | ((u32)enable << 19);
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = 0xFE080000;
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = gx->genMode;
}
#pragma pop

void __GXSetGenMode(void) {
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = gx->genMode;
    gx->bpSentNot = 0;
}
