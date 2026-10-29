#include "common.h"

typedef void (*CARDCallback)(s32 chan, s32 result);

typedef struct CARDControl {
    s32 attached;              // 0x00
    s32 result;                // 0x04
    u8  unk8[0xAC - 0x8];
    u32 repeat;                // 0xAC
    void *addr;                // 0xB0
    s32 offset;                // 0xB4
    u8 unkB8[0xD0 - 0xB8];
    CARDCallback apiCallback;    // 0xD0
    CARDCallback xferCallback;   // 0xD4
    CARDCallback eraseCallback;  // 0xD8
    CARDCallback unlockCallback; // 0xDC
    u8 alarm[0x28];            // 0xE0
    u8 unk108[0x110 - 0x108];
} CARDControl;

extern CARDControl __CARDBlock[2];

s32 __CARDReadSegment(s32 chan, CARDCallback callback);
void BlockReadCallback(s32 chan, s32 result);

#pragma peephole on
s32 __CARDRead(s32 chan, u32 addr, s32 length, void *dst, CARDCallback callback)
{
    CARDControl *card;

    card = &__CARDBlock[chan];
    if (!card->attached) {
        return -3;
    }
    card->xferCallback = callback;
    card->repeat = (u32)length / 512;
    card->addr = (void *)addr;
    card->offset = (s32)dst;
    return __CARDReadSegment(chan, BlockReadCallback);
}
