#include "common.h"

s32 VISetPostRetraceCallback(s32 cb) {
    extern s32 PostCB;
    s32 OSDisableInterrupts(void);
    void OSRestoreInterrupts(void);
    s32 old;

    old = PostCB;
    OSDisableInterrupts();
    PostCB = cb;
    OSRestoreInterrupts();
    return old;
}

u8 *getTiming(u32 mode) {
    extern u8 timing[0x130];
    u8 *tim = timing;

    switch (mode) {
    case 0:
        return tim;
    case 1:
        return tim + 0x26;
    case 2:
        return tim + 0x4C;
    case 3:
        return tim + 0x72;
    case 20:
        return tim;
    case 21:
        return tim + 0x26;
    case 4:
        return tim + 0x98;
    case 5:
        return tim + 0xBE;
    case 6:
        return tim + 0xE4;
    case 7:
        return tim + 0x10A;
    case 8:
        return tim + 0x4C;
    case 9:
        return tim + 0x72;
    default:
        return NULL;
    }
}

