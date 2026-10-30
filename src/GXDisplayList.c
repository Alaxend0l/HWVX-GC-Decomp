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
    u8 pad4[0x4F4 - 0x4];
    u32 dirtyState;
} GXData;

extern GXData *gx;

void __GXSetDirtyState(void);
void __GXSendFlushPrim(void);

void GXCallDisplayList(void *list, u32 nbytes) {
    if (gx->dirtyState != 0) {
        __GXSetDirtyState();
    }
    if (*(u32 *)&gx->vNumNot == 0) {
        __GXSendFlushPrim();
    }
    GXWGFifo.u8 = 0x40;
    GXWGFifo.u32 = (u32)list;
    GXWGFifo.u32 = nbytes;
}
