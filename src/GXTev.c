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

void GXSetTevOrder(s32 stage, s32 coord, s32 map, s32 color) {
    static s32 c2r[] = { 0, 1, 0, 1, 0, 1, 7, 5, 6 };
    u32 *ptref;
    u32 tmap;
    u32 tcoord;

    ptref = &gx->tref[stage / 2];
    gx->texmapId[stage] = map;
    tmap = map & ~0x100;
    tmap = (tmap >= 8) ? 0 : tmap;
    if (coord >= 8) {
        tcoord = 0;
        gx->tevTcEnab = gx->tevTcEnab & ~(1 << stage);
    } else {
        tcoord = coord;
        gx->tevTcEnab = gx->tevTcEnab | (1 << stage);
    }
    if (stage & 1) {
        *ptref = (*ptref & ~0x7000) | (tmap << 12);
        *ptref = (*ptref & ~0x38000) | (tcoord << 15);
        *ptref = (*ptref & ~0x380000) | (((color == 0xFF) ? 7 : c2r[color]) << 19);
        *ptref = (*ptref & ~0x40000) | ((map != 0xFF && !(map & 0x100)) << 18);
    } else {
        *ptref = (*ptref & ~0x7) | tmap;
        *ptref = (*ptref & ~0x38) | (tcoord << 3);
        *ptref = (*ptref & ~0x380) | (((color == 0xFF) ? 7 : c2r[color]) << 7);
        *ptref = (*ptref & ~0x40) | ((map != 0xFF && !(map & 0x100)) << 6);
    }
    GXWGFifo.u8 = 0x61;
    GXWGFifo.u32 = *ptref;
    gx->bpSentNot = 0;
    gx->dirtyState |= 1;
}
