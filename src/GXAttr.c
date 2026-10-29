#include "common.h"

typedef struct GXData {
    u8 unk0[0x14];
    u32 vcdLo;
    u32 vcdHi;
    u8 unk1C[0x41C - 0x1C];
    u8 hasNrms;
    u8 hasBiNrms;
} GXData;

extern GXData *gx;


void GXInvalidateVtxCache(void) {
    *(volatile u8 *)0xCC008000 = 0x48;
}
