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
    u16 vNum;
    u16 vLim;
    u8 pad8[0x7C - 0x8];
    u32 lpSize;
    u8 pad80[0xB8 - 0x80];
    u32 suTs0[8];
    u8 padD8[0x204 - 0xD8];
    u32 genMode;
    u8 pad208[0x4F4 - 0x208];
    u32 dirtyState;
} GXData;

extern GXData *gx;

#pragma push
#pragma peephole on
#pragma peephole on
void __GXSetSUTexRegs(void);
void __GXUpdateBPMask(void);
void __GXSetGenMode(void);
void __GXSetVCD(void);
void __GXSetVAT(void);
void __GXCalculateVLim(void);
void __GXSetDirtyState(void) {
    if (gx->dirtyState & 1) {
        __GXSetSUTexRegs();
    }
    if (gx->dirtyState & 2) {
        __GXUpdateBPMask();
    }
    if (gx->dirtyState & 4) {
        __GXSetGenMode();
    }
    if (gx->dirtyState & 8) {
        __GXSetVCD();
    }
    if (gx->dirtyState & 0x10) {
        __GXSetVAT();
    }
    if (gx->dirtyState & 0x18) {
        __GXCalculateVLim();
    }
    gx->dirtyState = 0;
}

void __GXSendFlushPrim(void) {
    u32 i;
    u32 numD = gx->vNum * gx->vLim;

    GXWGFifo.u8 = 0x98;
    GXWGFifo.u16 = gx->vNum;
    for (i = 0; i < numD; i += 4) {
        GXWGFifo.u32 = 0;
    }
    gx->bpSentNot = 1;
}

void GXSetLineWidth(u8 width, u32 texOffsets) {
    gx->lpSize = (gx->lpSize & ~0xFF) | width;
    gx->lpSize = (gx->lpSize & ~0x70000) | (texOffsets << 16);
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = gx->lpSize;
    gx->bpSentNot = 0;
}

void GXSetPointSize(u8 pointSize, u32 texOffsets) {
    gx->lpSize = (gx->lpSize & ~0xFF00) | ((u32)pointSize << 8);
    gx->lpSize = (gx->lpSize & ~0x380000) | (texOffsets << 19);
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = gx->lpSize;
    gx->bpSentNot = 0;
}

void GXEnableTexOffsets(s32 coord, u8 line_enable, u8 point_enable) {
    gx->suTs0[coord] = (gx->suTs0[coord] & ~0x40000) | ((u32)line_enable << 18);
    gx->suTs0[coord] = (gx->suTs0[coord] & ~0x80000) | ((u32)point_enable << 19);
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = gx->suTs0[coord];
    gx->bpSentNot = 0;
}

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
