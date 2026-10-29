#include "common.h"

typedef void (*CARDCallback)(s32 chan, s32 result);

typedef struct CARDControl {
    s32 attached;              // 0x00
    s32 result;                // 0x04
    u8  unk8[0xAC - 0x8];
    s32 repeat;                // 0xAC
    void *addr;                // 0xB0
    s32 offset;                // 0xB4
    s32 xferred;               // 0xB8
    u8 unkBC[0xD0 - 0xBC];
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
void __CARDPutControlBlock(CARDControl *card, s32 result);
void BlockReadCallback(s32 chan, s32 result)
{
    CARDControl *card;
    CARDCallback callback;

    card = &__CARDBlock[chan];
    if (result < 0) {
        goto error;
    }

    card->xferred += 512;
    card->addr = (u8 *)card->addr + 512;
    card->offset += 512;
    if (--card->repeat <= 0) {
        goto error;
    }

    result = __CARDReadSegment(chan, BlockReadCallback);
    if (result < 0) {
        goto error;
    }
    return;

error:
    if (card->apiCallback == 0) {
        __CARDPutControlBlock(card, result);
    }
    callback = card->xferCallback;
    if (callback) {
        card->xferCallback = 0;
        callback(chan, result);
    }
}

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

s32 __CARDWritePage(s32 chan, CARDCallback callback);
void BlockWriteCallback(s32 chan, s32 result);
s32 __CARDWrite(s32 chan, u32 addr, s32 length, void *dst, CARDCallback callback)
{
    CARDControl *card;

    card = &__CARDBlock[chan];
    if (!card->attached) {
        return -3;
    }
    card->xferCallback = callback;
    card->repeat = (u32)length / 128;
    card->addr = (void *)addr;
    card->offset = (s32)dst;
    return __CARDWritePage(chan, BlockWriteCallback);
}

