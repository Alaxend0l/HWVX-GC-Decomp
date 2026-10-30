#include "common.h"

typedef void (*CARDCallback)(s32 chan, s32 result);

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

typedef struct CARDControl {
    u8 unk0[0xC];
    s32 sectorSize;              // 0x0C
    u8 unk10[0xBC - 0x10];
    u16 freeNo;                  // 0xBC
    u16 startBlock;              // 0xBE
    CARDFileInfo *fileInfo;      // 0xC0
    CARDCallback extCallback;    // 0xC4
    CARDCallback txCallback;     // 0xC8
    CARDCallback exiCallback;    // 0xCC
    CARDCallback apiCallback;    // 0xD0
    u8 unkD4[0x10C - 0xD4];
    u8 *diskID;                  // 0x10C
} CARDControl;

extern CARDControl __CARDBlock[2];

CARDDir *__CARDGetDirBlock(CARDControl *card);
s32 __CARDUpdateDir(s32 chan, CARDCallback callback);
s32 __CARDPutControlBlock(CARDControl *card, s32 result);
s64 OSGetTime(void);
void *memcpy(void *dst, const void *src, u32 n);
#pragma push
#pragma use_lmw_stmw on
void CreateCallbackFat(s32 chan, s32 result)
{
    CARDControl *card;
    CARDDir *dir;
    CARDDir *ent;
    CARDCallback callback;

    card = &__CARDBlock[chan];
    callback = card->apiCallback;
    card->apiCallback = 0;
    if (result < 0) {
        goto error;
    }

    dir = __CARDGetDirBlock(card);
    ent = &dir[card->freeNo];
    memcpy(ent->gameName, card->diskID, 4);
    memcpy(ent->company, card->diskID + 4, 2);
    ent->permission = 4;
    ent->copyTimes = 0;
    ent->startBlock = card->startBlock;
    ent->bannerFormat = 0;
    ent->iconAddr = 0xFFFFFFFF;
    ent->iconFormat = 0;
    ent->iconSpeed = 0;
    ent->commentAddr = 0xFFFFFFFF;
    ent->iconSpeed = (u16)((ent->iconSpeed & ~3) | 1);
    card->fileInfo->offset = 0;
    card->fileInfo->iBlock = ent->startBlock;
    ent->time = (u32)(OSGetTime() / (*(u32 *)0x800000F8 / 4));
    result = __CARDUpdateDir(chan, callback);
    if (result < 0) {
        goto error;
    }
    return;

error:
    __CARDPutControlBlock(card, result);
    if (callback) {
        callback(chan, result);
    }
}

s32 CARDCreateAsync(s32 chan, const char *fileName, u32 size, CARDFileInfo *fileInfo, CARDCallback callback)
{
    u32 strlen(const char *s);
    int memcmp(const void *a, const void *b, u32 n);
    char *strncpy(char *dst, const char *src, u32 n);
    s32 __CARDGetControlBlock(s32 chan, CARDControl **pcard);
    u16 *__CARDGetFatBlock(CARDControl *card);
    s32 __CARDAllocBlock(s32 chan, u32 cBlock, CARDCallback callback);
    BOOL __CARDCompareFileName(CARDDir *ent, const char *fileName);
    void __CARDDefaultApiCallback(s32 chan, s32 result);
    CARDControl *card;
    CARDDir *dir;
    CARDDir *ent;
    s32 result;
    u16 fileNo;
    u16 freeNo;
    u16 *fat;

    if (strlen(fileName) > 32) {
        return -12;
    }
    result = __CARDGetControlBlock(chan, &card);
    if (result < 0) {
        return result;
    }
    if (size <= 0 || (size % card->sectorSize) != 0) {
        return -128;
    }

    freeNo = (u16)-1;
    dir = __CARDGetDirBlock(card);
    for (fileNo = 0; fileNo < 127; fileNo++) {
        ent = &dir[fileNo];
        if (ent->gameName[0] == 0xFF) {
            if (freeNo == (u16)-1) {
                freeNo = fileNo;
            }
        } else if (memcmp(ent->gameName, card->diskID, sizeof(ent->gameName)) == 0 &&
                   memcmp(ent->company, card->diskID + 4, sizeof(ent->company)) == 0 &&
                   __CARDCompareFileName(ent, fileName)) {
            return __CARDPutControlBlock(card, -7);
        }
    }
    if (freeNo == (u16)-1) {
        return __CARDPutControlBlock(card, -8);
    }

    fat = __CARDGetFatBlock(card);
    if (card->sectorSize * fat[3] < size) {
        return __CARDPutControlBlock(card, -9);
    }

    card->apiCallback = callback ? callback : __CARDDefaultApiCallback;
    card->freeNo = freeNo;
    ent = &dir[freeNo];
    ent->length = (u16)(size / card->sectorSize);
    strncpy((char *)ent->fileName, fileName, 32);

    card->fileInfo = fileInfo;
    fileInfo->chan = chan;
    fileInfo->fileNo = freeNo;

    result = __CARDAllocBlock(chan, size / card->sectorSize, CreateCallbackFat);
    if (result < 0) {
        return __CARDPutControlBlock(card, result);
    }
    return result;
}

#pragma pop

s32 CARDCreate(s32 chan, const char *fileName, u32 size, void *fileInfo)
{
    s32 CARDCreateAsync(s32 chan, const char *fileName, u32 size, void *fileInfo, CARDCallback callback);
    s32 __CARDSync(s32 chan);
    void __CARDSyncCallback(s32 chan, s32 result);
    s32 result;

    result = CARDCreateAsync(chan, fileName, size, fileInfo, __CARDSyncCallback);
    if (result < 0) {
        return result;
    }
    return __CARDSync(chan);
}
