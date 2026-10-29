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


s32 GXGetTexObjFmt(void *arg0) {
    return M2C_FIELD(arg0, s32 *, 0x14);
}
