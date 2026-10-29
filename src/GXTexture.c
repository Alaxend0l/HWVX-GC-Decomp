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

typedef union PPCWGPipe {
    u8 u8;
    u16 u16;
    u32 u32;
    s32 s32;
    f32 f32;
} PPCWGPipe;

volatile PPCWGPipe GXWGFifo : 0xCC008000;

typedef struct GXDataSUTex {
    u16 vNumNot;
    u16 bpSentNot;
    u8 unk4[0xB8 - 0x4];
    u32 suTs0[8];
    u32 suTs1[8];
    u8 unkF8[0x100 - 0xF8];
    u32 tref[8];
    u32 iref;
    u8 unk124[0x204 - 0x124];
    u32 genMode;
    u8 unk208[0x410 - 0x208];
    void *(*texRegionCallback)(GXTexObj *obj, u32 id);
    void *tlutRegionCallback;
    u8 unk418[0x45C - 0x418];
    u32 tImage0[8];
    u32 tMode0[8];
    u32 texmapId[16];
    u32 tcsManEnab;
    u32 tevTcEnab;
} GXDataSUTex;

extern GXDataSUTex *gx;

void __SetSURegs(u32 tmap, u32 tcoord);


void __GetImageTileCount(s32 fmt, u16 wd, u16 ht, u32 *rowTiles, u32 *colTiles, u32 *cmpTiles) {
    u32 texRowShift;
    u32 texColShift;

    switch (fmt) {
    case 0x0:
    case 0x8:
    case 0xE:
    case 0x20:
    case 0x30:
        texRowShift = 3;
        texColShift = 3;
        break;
    case 0x1:
    case 0x2:
    case 0x9:
    case 0x11:
    case 0x22:
    case 0x27:
    case 0x28:
    case 0x29:
    case 0x2A:
    case 0x39:
    case 0x3A:
        texRowShift = 3;
        texColShift = 2;
        break;
    case 0x3:
    case 0x4:
    case 0x5:
    case 0x6:
    case 0xA:
    case 0x13:
    case 0x16:
    case 0x23:
    case 0x2B:
    case 0x2C:
    case 0x3C:
        texRowShift = 2;
        texColShift = 2;
        break;
    default:
        texRowShift = texColShift = 0;
        break;
    }
    if (wd == 0) {
        wd = 1;
    }
    if (ht == 0) {
        ht = 1;
    }
    *rowTiles = (wd + (1 << texRowShift) - 1) >> texRowShift;
    *colTiles = (ht + (1 << texColShift) - 1) >> texColShift;
    *cmpTiles = (fmt == 0x6 || fmt == 0x16) ? 2 : 1;
}

void *GXGetTexObjData(GXTexObj *to) {
    __GXTexObjInt *t = (__GXTexObjInt *)to;

    return (void *)((t->image3 & 0x1FFFFF) << 5);
}

s32 GXGetTexObjFmt(GXTexObj *to) {
    __GXTexObjInt *t = (__GXTexObjInt *)to;

    return t->fmt;
}

void GXLoadTexObjPreLoaded(GXTexObj *obj, void *region, u32 id);
void GXLoadTexObj(GXTexObj *obj, u32 id) {
    void *r;

    r = ((void *(*)(GXTexObj *, u32))gx->texRegionCallback)(obj, id);
    GXLoadTexObjPreLoaded(obj, r, id);
}

void GXInitTlutRegion(GXTlutRegion *region, u32 tmem_addr, u32 tlut_size) {
    __GXTlutRegionInt *t = (__GXTlutRegionInt *)region;

    t->loadTlut1 = 0;
    t->loadTlut1 = (t->loadTlut1 & ~0x3FF) | ((tmem_addr - 0x80000) >> 9);
    t->loadTlut1 = (t->loadTlut1 & ~0x1FFC00) | (tlut_size << 10);
    t->loadTlut1 = (t->loadTlut1 & ~0xFF000000) | (0x65 << 24);
}

void __GXFlushTextureState(void);
void GXInvalidateTexAll(void) {
    u32 reg0;
    u32 reg1;

    reg0 = 0x66001000;
    reg1 = 0x66001100;
    __GXFlushTextureState();
    *(volatile u8 *)0xCC008000 = 0x61;
    *(volatile u32 *)0xCC008000 = reg0;
    *(volatile u8 *)0xCC008000 = 0x61;
    *(volatile u32 *)0xCC008000 = reg1;
    __GXFlushTextureState();
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

