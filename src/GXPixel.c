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
    u8 pad6[0x1DC - 0x6];
    u32 peCtrl;
    u8 pad1E0[0x4F1 - 0x1E0];
    u8 dlSaveContext;
    u8 pad4F2[2];
    u32 dirtyState;
} GXData;

extern GXData *gx;
