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
    void *offset;              // 0xB4
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

typedef struct CARDDir {
    u8 gameName[4];   // 0x00
    u8 company[2];    // 0x04
    u8 _padding0;     // 0x06
    u8 bannerFormat;  // 0x07
    u8 fileName[32];  // 0x08
    u32 time;         // 0x28
    u32 iconAddr;     // 0x2C
    u16 iconFormat;   // 0x30
    u16 iconSpeed;    // 0x32
    u8 permission;    // 0x34
    u8 copyTimes;     // 0x35
    u16 startBlock;   // 0x36
    u16 length;       // 0x38
    u8 _padding1[2];  // 0x3A
    u32 commentAddr;  // 0x3C
} CARDDir;

s32 __CARDGetControlBlock(s32 chan, CARDControl **pcard);
CARDDir *__CARDGetDirBlock(CARDControl *card);

u16 *__CARDGetFatBlock(CARDControl *card);
s32 __CARDPutControlBlock(CARDControl *card, s32 result);
s32 __CARDRead(s32 chan, u32 addr, s32 length, void *dst, CARDCallback callback);



typedef void (*CARDCallback)(s32 chan, s32 result);
#pragma push
#pragma use_lmw_stmw on
s32 __CARDSeek(CARDFileInfo *fileInfo, s32 length, s32 offset, CARDControl **pcard)
{
    CARDControl *card;
    CARDDir *dir;
    CARDDir *ent;
    s32 result;
    u16 *fat;

    result = __CARDGetControlBlock(fileInfo->chan, &card);
    if (result < 0) {
        return result;
    }
    if (!(5 <= fileInfo->iBlock && fileInfo->iBlock < card->cBlock) || card->cBlock * card->sectorSize <= fileInfo->offset) {
        return __CARDPutControlBlock(card, -128);
    }

    dir = __CARDGetDirBlock(card);
    ent = &dir[fileInfo->fileNo];
    if (ent->length * card->sectorSize <= offset || ent->length * card->sectorSize < offset + length) {
        return __CARDPutControlBlock(card, -11);
    }

    card->fileInfo = fileInfo;
    fileInfo->length = length;
    if (offset < fileInfo->offset) {
        fileInfo->offset = 0;
        fileInfo->iBlock = ent->startBlock;
        if (!(5 <= fileInfo->iBlock && fileInfo->iBlock < card->cBlock)) {
            return __CARDPutControlBlock(card, -6);
        }
    }
    fat = __CARDGetFatBlock(card);
    while (fileInfo->offset < ((u32)offset & ~(card->sectorSize - 1))) {
        fileInfo->offset += card->sectorSize;
        fileInfo->iBlock = fat[fileInfo->iBlock];
        if (!(5 <= fileInfo->iBlock && fileInfo->iBlock < card->cBlock)) {
            return __CARDPutControlBlock(card, -6);
        }
    }

    fileInfo->offset = offset;
    *pcard = card;
    return 0;
}

static void ReadCallback(s32 chan, s32 result)
{
    CARDControl *card;
    s32 length;
    CARDFileInfo *fileInfo;
    u16 *fat;
    CARDCallback callback;

    card = &__CARDBlock[chan];
    if (result < 0) {
        goto error;
    }

    fileInfo = card->fileInfo;
    if (fileInfo->length < 0) {
        result = -14;
        goto error;
    }

    length = (s32)(((u32)(fileInfo->offset + card->sectorSize)) & ~(card->sectorSize - 1)) - fileInfo->offset;
    fileInfo->length -= length;
    if (fileInfo->length <= 0) {
        goto error;
    }

    fat = __CARDGetFatBlock(card);
    fileInfo->offset += length;
    fileInfo->iBlock = fat[fileInfo->iBlock];
    if (fileInfo->iBlock < 5 || fileInfo->iBlock >= card->cBlock) {
        result = -6;
        goto error;
    }
    result = __CARDRead(chan, card->sectorSize * fileInfo->iBlock,
                        (fileInfo->length < card->sectorSize) ? fileInfo->length : card->sectorSize,
                        card->offset, ReadCallback);
    if (result < 0) {
        goto error;
    }

    return;

error:
    callback = card->apiCallback;
    card->apiCallback = 0;
    __CARDPutControlBlock(card, result);
    callback(chan, result);
}

s32 __CARDAccess(CARDControl *card, CARDDir *ent);
s32 __CARDIsPublic(CARDDir *ent);
void DCInvalidateRange(void *addr, u32 nBytes);
void __CARDDefaultApiCallback(s32 chan, s32 result);
#pragma push
#pragma use_lmw_stmw on
s32 CARDReadAsync(CARDFileInfo *fileInfo, void *buf, s32 length, s32 offset, CARDCallback callback)
{
    CARDControl *card;
    s32 result;
    CARDDir *dir;
    CARDDir *ent;

    if ((offset & 0x1FF) || (length & 0x1FF)) {
        return -128;
    }
    result = __CARDSeek(fileInfo, length, offset, &card);
    if (result < 0) {
        return result;
    }

    dir = __CARDGetDirBlock(card);
    ent = &dir[fileInfo->fileNo];
    result = __CARDAccess(card, ent);
    if (result == -10) {
        result = __CARDIsPublic(ent);
    }
    if (result < 0) {
        return __CARDPutControlBlock(card, result);
    }

    DCInvalidateRange(buf, (u32)length);
    card->apiCallback = callback ? callback : __CARDDefaultApiCallback;

    offset = (s32)(fileInfo->offset & (card->sectorSize - 1));
    length = (length < card->sectorSize - offset) ? length : card->sectorSize - offset;
    result = __CARDRead(fileInfo->chan, card->sectorSize * fileInfo->iBlock + offset, length, buf, ReadCallback);
    if (result < 0) {
        __CARDPutControlBlock(card, result);
    }
    return result;
}
#pragma pop

#pragma pop

s32 CARDRead(CARDFileInfo *fileInfo, void *buf, s32 length, s32 offset)
{
    s32 CARDReadAsync(CARDFileInfo *fileInfo, void *buf, s32 length, s32 offset, CARDCallback callback);
    s32 __CARDSync(s32 chan);
    void __CARDSyncCallback(s32 chan, s32 result);
    s32 result;

    result = CARDReadAsync(fileInfo, buf, length, offset, __CARDSyncCallback);
    if (result < 0) {
        return result;
    }
    return __CARDSync(fileInfo->chan);
}
