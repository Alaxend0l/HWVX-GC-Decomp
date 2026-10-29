#include "common.h"

typedef struct __GXTexObjInt {
    u32 mode0;
    u32 mode1;
    u32 image0;
    u32 image3;
    void *userData;
    s32 fmt;
    u32 tlutName;
    u16 loadCnt;
    u8 loadFmt;
    u8 flags;
} __GXTexObjInt;

typedef struct GXTexObj {
    u32 dummy[8];
} GXTexObj;

typedef struct __GXTlutRegionInt {
    u32 loadTlut1;
    u32 tlutObj[2];
    u32 unkC;
} __GXTlutRegionInt;

typedef struct GXTlutRegion {
    u32 dummy[4];
} GXTlutRegion;

typedef struct GXDataSUTex {
    u8 unk0[0x100];
    u32 tref[8];
    u32 iref;
    u8 unk124[0x204 - 0x124];
    u32 genMode;
    u8 unk208[0x410 - 0x208];
    void *texRegionCallback;
    void *tlutRegionCallback;
    u8 unk418[0x49C - 0x418];
    u32 texmapId[16];
    u32 tcsManEnab;
    u32 tevTcEnab;
} GXDataSUTex;

extern GXDataSUTex *gx;

void __SetSURegs(u32 tmap, u32 tcoord);


void *GXGetTexObjData(GXTexObj *to) {
    __GXTexObjInt *t = (__GXTexObjInt *)to;

    return (void *)((t->image3 & 0x1FFFFF) << 5);
}

s32 GXGetTexObjFmt(GXTexObj *to) {
    __GXTexObjInt *t = (__GXTexObjInt *)to;

    return t->fmt;
}

void *GXSetTexRegionCallback(void *f) {
    void *oldcb = gx->texRegionCallback;

    gx->texRegionCallback = f;
    return oldcb;
}

void *GXSetTlutRegionCallback(void *f) {
    void *oldcb = gx->tlutRegionCallback;

    gx->tlutRegionCallback = f;
    return oldcb;
}

void __GXSetSUTexRegs(void) {
    u32 nStages;
    u32 nIndStages;
    u32 i;
    u32 map;
    u32 tmap;
    u32 coord;
    u32 *ptref;

    if (gx->tcsManEnab != 0xFF) {
        nStages = ((gx->genMode >> 10) & 0xF) + 1;
        nIndStages = (gx->genMode >> 16) & 7;
        for (i = 0; i < nIndStages; i++) {
            switch (i) {
            case 0:
                tmap = gx->iref & 7;
                coord = (gx->iref >> 3) & 7;
                break;
            case 1:
                tmap = (gx->iref >> 6) & 7;
                coord = (gx->iref >> 9) & 7;
                break;
            case 2:
                tmap = (gx->iref >> 12) & 7;
                coord = (gx->iref >> 15) & 7;
                break;
            case 3:
                tmap = (gx->iref >> 18) & 7;
                coord = (gx->iref >> 21) & 7;
                break;
            }
            if (!(gx->tcsManEnab & (1 << coord))) {
                __SetSURegs(tmap, coord);
            }
        }
        for (i = 0; i < nStages; i++) {
            ptref = &gx->tref[i / 2];
            map = gx->texmapId[i];
            tmap = map & ~0x100;
            if (i & 1) {
                coord = (*ptref >> 15) & 7;
            } else {
                coord = (*ptref >> 3) & 7;
            }
            if (tmap != 0xFF && !(gx->tcsManEnab & (1 << coord)) && (gx->tevTcEnab & (1 << i))) {
                __SetSURegs(tmap, coord);
            }
        }
    }
}

