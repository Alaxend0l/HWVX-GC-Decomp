#include "common.h"

typedef struct GXTlutRegion {
    u32 unk0[4];
} GXTlutRegion;

typedef struct GXTexRegion {
    u32 unk0[4];
} GXTexRegion;

typedef struct GXTexObj GXTexObj;

typedef struct GXData {
    u8 unk0[0x208];
    GXTexRegion TexRegions[8];
    GXTexRegion TexRegionsCI[4];
    u32 nextTexRgn;
    u32 nextTexRgnCI;
    GXTlutRegion TlutRegions[20];
} GXData;

extern GXData *gx;

#pragma peephole off
s32 GXGetTexObjFmt(GXTexObj *obj);
GXTexRegion *__GXDefaultTexRegionCallback(GXTexObj *t_obj, s32 id)
{
    s32 fmt;

    fmt = GXGetTexObjFmt(t_obj);
    if (fmt != 8 && fmt != 9 && fmt != 10) {
        return &gx->TexRegions[gx->nextTexRgn++ & 7];
    } else {
        return &gx->TexRegionsCI[gx->nextTexRgnCI++ & 3];
    }
}

GXTlutRegion *__GXDefaultTlutRegionCallback(u32 idx)
{
    if (idx >= 20) {
        return NULL;
    }
    return &gx->TlutRegions[idx];
}
