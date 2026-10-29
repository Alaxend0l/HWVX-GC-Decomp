#include "common.h"

typedef struct GXData {
    u8 unk0[0x14];
    u32 vcdLo;
    u32 vcdHi;
    u8 unk1C[0x204 - 0x1C];
    u32 genMode;
    u8 unk208[0x41C - 0x208];
    u8 hasNrms;
    u8 hasBiNrms;
    u8 unk41E[0x4F4 - 0x41E];
    u32 dirtyState;
} GXData;

extern GXData *gx;

typedef union PPCWGPipe {
    u8 u8;
    u16 u16;
    u32 u32;
    s32 s32;
    f32 f32;
} PPCWGPipe;

volatile PPCWGPipe GXWGFifo : 0xCC008000;


void GXGetVtxDesc(s32 attr, s32 *type) {
    s32 cpType;

    switch (attr) {
    case 0:
        cpType = gx->vcdLo & 1;
        break;
    case 1:
        cpType = (gx->vcdLo >> 1) & 1;
        break;
    case 2:
        cpType = (gx->vcdLo >> 2) & 1;
        break;
    case 3:
        cpType = (gx->vcdLo >> 3) & 1;
        break;
    case 4:
        cpType = (gx->vcdLo >> 4) & 1;
        break;
    case 5:
        cpType = (gx->vcdLo >> 5) & 1;
        break;
    case 6:
        cpType = (gx->vcdLo >> 6) & 1;
        break;
    case 7:
        cpType = (gx->vcdLo >> 7) & 1;
        break;
    case 8:
        cpType = (gx->vcdLo >> 8) & 1;
        break;
    case 9:
        cpType = (gx->vcdLo >> 9) & 3;
        break;
    case 10:
        if (gx->hasNrms) {
            cpType = (gx->vcdLo >> 11) & 3;
        } else {
            cpType = 0;
        }
        break;
    case 25:
        if (gx->hasBiNrms) {
            cpType = (gx->vcdLo >> 11) & 3;
        } else {
            cpType = 0;
        }
        break;
    case 11:
        cpType = (gx->vcdLo >> 13) & 3;
        break;
    case 12:
        cpType = (gx->vcdLo >> 15) & 3;
        break;
    case 13:
        cpType = gx->vcdHi & 3;
        break;
    case 14:
        cpType = (gx->vcdHi >> 2) & 3;
        break;
    case 15:
        cpType = (gx->vcdHi >> 4) & 3;
        break;
    case 16:
        cpType = (gx->vcdHi >> 6) & 3;
        break;
    case 17:
        cpType = (gx->vcdHi >> 8) & 3;
        break;
    case 18:
        cpType = (gx->vcdHi >> 10) & 3;
        break;
    case 19:
        cpType = (gx->vcdHi >> 12) & 3;
        break;
    case 20:
        cpType = (gx->vcdHi >> 14) & 3;
        break;
    default:
        cpType = 0;
        break;
    }
    *type = cpType;
}

void GXInvalidateVtxCache(void) {
    *(volatile u8 *)0xCC008000 = 0x48;
}
