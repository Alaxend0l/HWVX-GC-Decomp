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

typedef struct CARDID {
    u8 serial[32];
    u16 deviceID;
    u16 size;
    u16 encode;
    u8 padding[0x1FC - 0x26];
    u16 checkSum;
    u16 checkSumInv;
} CARDID;

typedef struct CARDDirCheck {
    u8 padding0[0x3A];
    s16 checkCode;
    u16 checkSum;
    u16 checkSumInv;
} CARDDirCheck;

static inline CARDDirCheck *__CARDGetDirCheck(u8 *dir) {
    return (CARDDirCheck *)(dir + 0x1FC0);
}

typedef struct OSSram {
    u16 checkSum;
    u16 checkSumInv;
    u32 ead0;
    u32 ead1;
    u32 counterBias;
    s8 displayOffsetH;
    u8 ntd;
    u8 language;
    u8 flags;
} OSSram;

typedef struct OSSramEx {
    u8 flashID[2][12];
    u32 wirelessKeyboardID;
    u16 wirelessPadID[4];
    u8 dvdErrorCode;
    u8 _padding0;
    u8 flashIDCheckSum[2];
    u16 gbs;
    u16 _padding1;
} OSSramEx;

s32 __CARDGetControlBlock(s32 chan, CARDControl **pcard);
void *memset(void *dst, int val, unsigned long n);
OSSram *__OSLockSram(void);
s32 __OSUnlockSram(s32 commit);
OSSramEx *__OSLockSramEx(void);
s32 __OSUnlockSramEx(s32 commit);
s64 OSGetTime(void);
void __CARDCheckSum(void *ptr, s32 length, u16 *checksum, u16 *checksumInv);
void DCStoreRange(void *addr, u32 nBytes);
void __CARDDefaultApiCallback(s32 chan, s32 result);

s32 __CARDEraseSector(s32 chan, u32 addr, CARDCallback callback);
s32 __CARDWrite(s32 chan, u32 addr, s32 length, void *dst, CARDCallback callback);
s32 __CARDPutControlBlock(CARDControl *card, s32 result);
void *memcpy(void *dst, const void *src, unsigned long n);

static void FormatCallback(s32 chan, s32 result)
{
    CARDControl *card;
    CARDCallback callback;

    card = &__CARDBlock[chan];
    if (result < 0) {
        goto error;
    }

    ++card->formatStep;
    if (card->formatStep < 5) {
        result = __CARDEraseSector(chan, (u32)card->sectorSize * card->formatStep, FormatCallback);
        if (0 <= result) {
            return;
        }
    } else if (card->formatStep < 10) {
        int step = card->formatStep - 5;
        result = __CARDWrite(chan, (u32)card->sectorSize * step, 0x2000, (u8 *)card->workArea + step * 0x2000, FormatCallback);
        if (0 <= result) {
            return;
        }
    } else {
        card->currentDir = (u8 *)card->workArea + 0x2000;
        memcpy(card->currentDir, (u8 *)card->workArea + 0x4000, 0x2000);
        card->buffer = (u8 *)card->workArea + 0x6000;
        memcpy(card->buffer, (u8 *)card->workArea + 0x8000, 0x2000);
    }

error:
    callback = card->apiCallback;
    card->apiCallback = NULL;
    __CARDPutControlBlock(card, result);
    callback(chan, result);
}

s32 __CARDFormatRegionAsync(s32 chan, u16 encode, CARDCallback callback)
{
    CARDControl *card;
    CARDID *id;
    u8 *dir;
    CARDDirCheck *check;
    u16 *fat;
    s16 i;
    s32 result;
    OSSram *sram;
    OSSramEx *sramEx;
    u16 viDTVStatus;
    s64 time;
    s64 rand;

    result = __CARDGetControlBlock(chan, &card);
    if (result < 0) {
        return result;
    }

    id = card->workArea;
    memset(id, 0xFF, 0x2000);
    viDTVStatus = *(volatile u16 *)0xCC00206E;

    id->encode = encode;

    sram = __OSLockSram();
    *(u32 *)&id->serial[20] = sram->counterBias;
    *(u32 *)&id->serial[24] = sram->language;
    __OSUnlockSram(0);

    rand = time = OSGetTime();

    sramEx = __OSLockSramEx();
    for (i = 0; i < 12; i++) {
        rand = (rand * 1103515245 + 12345) >> 16;
        id->serial[i] = (u8)(sramEx->flashID[chan][i] + rand);
        rand = ((rand * 1103515245 + 12345) >> 16) & 0x7FFF;
    }
    __OSUnlockSramEx(0);

    *(u32 *)&id->serial[28] = viDTVStatus;
    *(s64 *)&id->serial[12] = time;

    id->deviceID = 0;
    id->size = card->size;
    __CARDCheckSum(id, sizeof(CARDID) - sizeof(u32), &id->checkSum, &id->checkSumInv);

    for (i = 0; i < 2; i++) {
        dir = (u8 *)card->workArea + (1 + i) * 0x2000;
        memset(dir, 0xFF, 0x2000);
        check = __CARDGetDirCheck(dir);
        check->checkCode = i;
        __CARDCheckSum(dir, 0x2000 - sizeof(u32), &check->checkSum, &check->checkSumInv);
    }
    for (i = 0; i < 2; i++) {
        fat = (u16 *)((u8 *)card->workArea + (3 + i) * 0x2000);
        memset(fat, 0x00, 0x2000);
        fat[2] = i;
        fat[3] = card->cBlock - 5;
        fat[4] = 4;
        __CARDCheckSum(&fat[2], 0x2000 - sizeof(u32), &fat[0], &fat[1]);
    }

    card->apiCallback = callback ? callback : __CARDDefaultApiCallback;
    DCStoreRange(card->workArea, 0xA000);

    card->formatStep = 0;
    result = __CARDEraseSector(chan, card->sectorSize * card->formatStep, FormatCallback);
    if (result < 0) {
        __CARDPutControlBlock(card, result);
    }
    return result;
}

s32 CARDFormat(s32 chan)
{
    s32 __CARDFormatRegionAsync(s32 chan, u16 encode, CARDCallback callback);
    u16 OSGetFontEncode(void);
    s32 __CARDSync(s32 chan);
    void __CARDSyncCallback(s32 chan, s32 result);
    s32 result;

    result = __CARDFormatRegionAsync(chan, OSGetFontEncode(), __CARDSyncCallback);
    if (result < 0) {
        return result;
    }
    return __CARDSync(chan);
}
