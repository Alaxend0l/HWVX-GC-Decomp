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
    u8 pad4[0x100 - 0x4];
    u32 tref[8];
    u8 pad120[0x130 - 0x120];
    u32 tevc[16];
    u32 teva[16];
    u32 tevKsel[8];
    u8 pad1D0[0x49C - 0x1D0];
    u32 texmapId[16];
    u8 pad4DC[0x4E0 - 0x4DC];
    u32 tevTcEnab;
    u8 pad4E4[0x4F4 - 0x4E4];
    u32 dirtyState;
} GXData;

extern GXData *gx;
