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

typedef struct GXDataSUTex {
    u8 unk0[0x100];
    u32 tref[8];
    u32 iref;
    u8 unk124[0x204 - 0x124];
    u32 genMode;
    u8 unk208[0x49C - 0x208];
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
