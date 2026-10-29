#include "common.h"

typedef void (*CARDCallback)(s32 chan, s32 result);

typedef struct CARDControl {
    s32 attached;              // 0x00
    s32 result;                // 0x04
    u16 size;                  // 0x08
    u8  unkA[0xC - 0xA];
    s32 sectorSize;            // 0x0C
    u16 cBlock;                // 0x10
    u8  unk12[0x14 - 0x12];
    s32 latency;               // 0x14
    u8  unk18[0x24 - 0x18];
    s32 mountStep;             // 0x24
    s32 formatStep;            // 0x28
    u8  unk2C[0x80 - 0x2C];
    void *workArea;            // 0x80
    u8 *currentDir;            // 0x84
    void *buffer;              // 0x88
    u8  unk8C[0x94 - 0x8C];
    u8 cmd[9];                 // 0x94
    s32 cmdlen;                // 0xA0 (aligned)
    s32 mode;                  // 0xA4
    s32 retry;                 // 0xA8
    u32 repeat;                // 0xAC
    void *addr;                // 0xB0
    s32 offset;                // 0xB4
    u8 unkB8[0xBE - 0xB8];
    u16 startBlock;            // 0xBE
    CARDFileInfo *fileInfo;      // 0xC0
    CARDCallback extCallback;    // 0xC4
    CARDCallback txCallback;     // 0xC8
    CARDCallback exiCallback;    // 0xCC
    CARDCallback apiCallback;    // 0xD0
    CARDCallback xferCallback;   // 0xD4
    CARDCallback eraseCallback;  // 0xD8
    CARDCallback unlockCallback; // 0xDC
    u8 alarm[0x28];            // 0xE0
    u8 unk108[0x110 - 0x108];
} CARDControl;

extern CARDControl __CARDBlock[2];

s32 __CARDWrite(s32 chan, u32 addr, s32 length, void *dst, void *callback);
s32 __CARDPutControlBlock(void *card, s32 result);
static void WriteCallback(s32 chan, s32 result);

u8 *__CARDGetDirBlock(CARDControl *card);
u16 *__CARDGetFatBlock(CARDControl *card);
s32 __CARDUpdateDir(s32 chan, CARDCallback callback);
s32 __CARDEraseSector(s32 chan, u32 addr, CARDCallback callback);
s64 OSGetTime(void);
void EraseCallback(s32 chan, s32 result);
static void WriteCallback(s32 chan, s32 result) {
    CARDControl *card;
    CARDCallback callback;
    u16 *fat;
    u8 *dir;
    u8 *ent;
    CARDFileInfo *fileInfo;

    card = &__CARDBlock[chan];
    if (result < 0) {
        goto error;
    }
    fileInfo = card->fileInfo;
    if (fileInfo->length < 0) {
        result = -14;
        goto error;
    }
    fileInfo->length -= card->sectorSize;
    if (fileInfo->length <= 0) {
        dir = __CARDGetDirBlock(card);
        ent = dir + fileInfo->fileNo * 0x40;
        *(u32 *)(ent + 0x28) = (u32)(OSGetTime() / (*(u32 *)0x800000F8 / 4));
        callback = card->apiCallback;
        card->apiCallback = NULL;
        result = __CARDUpdateDir(chan, callback);
    } else {
        fat = __CARDGetFatBlock(card);
        fileInfo->offset += card->sectorSize;
        fileInfo->iBlock = fat[fileInfo->iBlock];
        if (fileInfo->iBlock < 5 || fileInfo->iBlock >= card->cBlock) {
            result = -6;
            goto error;
        }
        result = __CARDEraseSector(chan, card->sectorSize * fileInfo->iBlock, EraseCallback);
    }
    if (result < 0) {
    error:
        callback = card->apiCallback;
        card->apiCallback = NULL;
        __CARDPutControlBlock(card, result);
        callback(chan, result);
    }
}

void EraseCallback(s32 chan, s32 result) {
    CARDControl *card;
    CARDCallback callback;

    card = &__CARDBlock[chan];
    if (result >= 0) {
        result = __CARDWrite(chan, card->sectorSize * card->fileInfo->iBlock, card->sectorSize, (void *)card->offset, WriteCallback);
        if (result >= 0) {
            return;
        }
    }
    callback = card->apiCallback;
    card->apiCallback = NULL;
    __CARDPutControlBlock(card, result);
    callback(chan, result);
}

void CARDWrite(CARDFileInfo *fileInfo, void *buf, s32 length, s32 offset)
{
    s32 CARDWriteAsync(CARDFileInfo *fileInfo, void *buf, s32 length, s32 offset, CARDCallback callback);
    s32 __CARDSync(s32 chan);
    void __CARDSyncCallback(s32 chan, s32 result);
    s32 result;

    result = CARDWriteAsync(fileInfo, buf, length, offset, __CARDSyncCallback);
    if (result >= 0) {
        __CARDSync(fileInfo->chan);
    }
    else {
    }
}

