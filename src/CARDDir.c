#include "common.h"

typedef void (*CARDCallback)(s32 chan, s32 result);

typedef struct CARDControl {
    s32 attached;
    s32 result;
    u16 size;
    u8 unkA[0xC - 0xA];
    s32 sectorSize;
    u16 cBlock;
    u8 unk12[0x14 - 0x12];
    s32 latency;
    u8 unk18[0x24 - 0x18];
    s32 mountStep;
    s32 formatStep;
    u8 unk2C[0x80 - 0x2C];
    void *workArea;
    u8 *currentDir;
    void *buffer;
    u8 unk8C[0x94 - 0x8C];
    u8 cmd[9];
    s32 cmdlen;
    s32 mode;
    s32 retry;
    u32 repeat;
    void *addr;
    s32 offset;
    u8 unkB8[0xBE - 0xB8];
    u16 startBlock;
    struct CARDFileInfo *fileInfo;
    CARDCallback extCallback;
    CARDCallback txCallback;
    CARDCallback exiCallback;
    CARDCallback apiCallback;
    CARDCallback xferCallback;
    CARDCallback eraseCallback;
    CARDCallback unlockCallback;
    u8 alarm[0x28];
    u8 unk108[0x110 - 0x108];
} CARDControl;

extern CARDControl __CARDBlock[2];

void *memcpy(void *dst, const void *src, u32 n);
s32 __CARDWrite(s32 chan, u32 addr, s32 length, void *dst, CARDCallback callback);
s32 __CARDPutControlBlock(CARDControl *card, s32 result);

u8 *__CARDGetDirBlock(CARDControl *card) {
    return card->currentDir;
}

static void WriteCallback(s32 chan, s32 result) {
    CARDControl *card;
    CARDCallback callback;
    u8 *dir0;
    u8 *dir1;

    card = &__CARDBlock[chan];
    if (result >= 0) {
        dir0 = (u8 *)card->workArea + 0x2000;
        dir1 = (u8 *)card->workArea + 0x4000;
        if (card->currentDir == dir0) {
            card->currentDir = dir1;
            memcpy(dir1, dir0, 0x2000);
        } else {
            card->currentDir = dir0;
            memcpy(dir0, dir1, 0x2000);
        }
    }
    if (card->apiCallback == NULL) {
        __CARDPutControlBlock(card, result);
    }
    callback = card->eraseCallback;
    if (callback) {
        card->eraseCallback = NULL;
        callback(chan, result);
    }
}

void WriteCallback(s32 chan, s32 result);
s32 __CARDWrite(s32 chan, u32 addr, s32 length, void *dst, CARDCallback callback);
s32 __CARDPutControlBlock(CARDControl *card, s32 result);
void *memcpy(void *dst, const void *src, u32 n);
static void EraseCallback(s32 chan, s32 result) {
    CARDControl *card;
    CARDCallback callback;
    u8 *dir;
    u32 addr;

    card = &__CARDBlock[chan];
    if (result < 0) {
        goto error;
    }
    dir = __CARDGetDirBlock(card);
    addr = ((u32)dir - (u32)card->workArea) / 0x2000 * card->sectorSize;
    result = __CARDWrite(chan, addr, 0x2000, dir, WriteCallback);
    if (result < 0) {
        goto error;
    }
    return;

error:
    if (card->apiCallback == NULL) {
        __CARDPutControlBlock(card, result);
    }
    callback = card->eraseCallback;
    if (callback) {
        card->eraseCallback = NULL;
        callback(chan, result);
    }
}

