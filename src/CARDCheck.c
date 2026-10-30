#include "common.h"

typedef struct CARDControl {
    s32 attached;              // 0x00
    s32 result;                // 0x04
    u16 size;                  // 0x08
    u8  unkA[0xC - 0xA];
    s32 sectorSize;            // 0x0C
    u16 cBlock;                // 0x10
    u8  unk12[0x80 - 0x12];
    void *workArea;            // 0x80
    u8 *currentDir;            // 0x84
    u16 *currentFat;           // 0x88
    u8  unk8C[0x110 - 0x8C];
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

typedef struct OSSramEx {
    u8 flashID[2][12];
} OSSramEx;

u16 OSGetFontEncode(void);
OSSramEx *__OSLockSramEx(void);
BOOL __OSUnlockSramEx(BOOL commit);

typedef struct CARDDirCheck {
    u8 padding0[0x3A];
    s16 checkCode;
    u16 checkSum;
    u16 checkSumInv;
} CARDDirCheck;

/* A macro in the SDK: an inline function adds a stack slot (GC/1.2.5n) */
#define __CARDGetDirCheck(dir) ((CARDDirCheck *)((u8 *)(dir) + 0x1FC0))

typedef void (*CARDCallback)(s32 chan, s32 result);

typedef struct CARDDir {
    u8 gameName[4];
    u8 company[2];
    u8 unk6[0x36 - 0x6];
    u16 startBlock;
    u16 length;
    u8 unk3A[0x40 - 0x3A];
} CARDDir;

void *memcpy(void *dst, const void *src, u32 n);
void *memset(void *dst, int c, u32 n);
s32 CARDCheckExAsync(s32 chan, s32 *xferBytes, CARDCallback callback);
s32 __CARDGetControlBlock(s32 chan, CARDControl **pcard);
s32 __CARDPutControlBlock(CARDControl *card, s32 result);
s32 __CARDUpdateDir(s32 chan, CARDCallback callback);
s32 __CARDUpdateFatBlock(s32 chan, u16 *fat, CARDCallback callback);
void __CARDSyncCallback(s32 chan, s32 result);
s32 __CARDSync(s32 chan);
s32 VerifyID(CARDControl *card);
s32 VerifyDir(CARDControl *card, s32 *outCurrent);
s32 VerifyFAT(CARDControl *card, s32 *outCurrent);
void __CARDCheckSum(void *ptr, int length, u16 *checksum, u16 *checksumInv) {
    u16 *p;
    int i;

    length /= sizeof(u16);
    *checksum = *checksumInv = 0;
    for (i = 0, p = ptr; i < length; i++, p++) {
        *checksum += *p;
        *checksumInv += ~*p;
    }
    if (*checksum == 0xFFFF) {
        *checksum = 0;
    }
    if (*checksumInv == 0xFFFF) {
        *checksumInv = 0;
    }
}

#pragma push
#pragma use_lmw_stmw on
s32 VerifyID(CARDControl *card) {
    CARDID *id;
    u16 checksum;
    u16 checksumInv;
    OSSramEx *sramEx;
    s64 rand;
    int i;

    id = card->workArea;

    if (id->deviceID != 0 || id->size != card->size) {
        return -6;
    }

    __CARDCheckSum(id, sizeof(CARDID) - sizeof(u32), &checksum, &checksumInv);
    if (id->checkSum != checksum || id->checkSumInv != checksumInv) {
        return -6;
    }

    if (id->encode != OSGetFontEncode()) {
        return -13;
    }

    rand = *(s64 *)&id->serial[12];
    sramEx = __OSLockSramEx();
    for (i = 0; i < 12; i++) {
        rand = (rand * 1103515245 + 12345) >> 16;
        if (id->serial[i] != (u8)(sramEx->flashID[card - __CARDBlock][i] + rand)) {
            __OSUnlockSramEx(FALSE);
            return -6;
        }
        rand = ((rand * 1103515245 + 12345) >> 16) & 0x7FFF;
    }
    __OSUnlockSramEx(FALSE);

    return 0;
}
#pragma pop

s32 VerifyDir(CARDControl *card, s32 *outCurrent) {
    u8 *dir[2];
    CARDDirCheck *check[2];
    u16 checkSum;
    u16 checkSumInv;
    s32 i;
    s32 errors;
    s32 current;

    errors = 0;
    current = 0;
    for (i = 0; i < 2; i++) {
        dir[i] = (u8 *)card->workArea + (1 + i) * 0x2000;
        check[i] = (CARDDirCheck *)(dir[i] + 0x1FC0);
        __CARDCheckSum(dir[i], 0x2000 - sizeof(u32), &checkSum, &checkSumInv);
        if (check[i]->checkSum != checkSum || check[i]->checkSumInv != checkSumInv) {
            card->currentDir = NULL;
            current = i;
            errors++;
        }
    }
    if (errors == 0) {
        if (card->currentDir == NULL) {
            if ((check[0]->checkCode - check[1]->checkCode) < 0) {
                current = 0;
            } else {
                current = 1;
            }
            card->currentDir = dir[current];
            memcpy(dir[current], dir[current ^ 1], 0x2000);
        } else {
            current = (card->currentDir == dir[0]) ? 0 : 1;
        }
    }
    if (outCurrent) {
        *outCurrent = current;
    }
    return errors;
}

s32 VerifyFAT(CARDControl *card, s32 *outCurrent) {
    u16 *fat[2];
    u16 *fatp;
    u16 nBlock;
    u16 cFree;
    s32 i;
    u16 checkSum;
    u16 checkSumInv;
    s32 errors;
    s32 current;

    errors = 0;
    current = 0;
    for (i = 0; i < 2; i++) {
        fatp = fat[i] = (u16 *)((u8 *)card->workArea + (3 + i) * 0x2000);

        __CARDCheckSum(&fatp[2], 0x2000 - sizeof(u32), &checkSum, &checkSumInv);
        if (fatp[0] != checkSum || fatp[1] != checkSumInv) {
            card->currentFat = NULL;
            current = i;
            errors++;
            continue;
        }

        cFree = 0;
        for (nBlock = 5; nBlock < card->cBlock; nBlock++) {
            if (fatp[nBlock] == 0) {
                cFree++;
            }
        }
        if (cFree != fatp[3]) {
            card->currentFat = NULL;
            current = i;
            errors++;
            continue;
        }
    }

    if (errors == 0) {
        if (card->currentFat == NULL) {
            if (((s16)fat[0][2] - (s16)fat[1][2]) < 0) {
                current = 0;
            } else {
                current = 1;
            }
            card->currentFat = fat[current];
            memcpy(fat[current], fat[current ^ 1], 0x2000);
        } else {
            current = (card->currentFat == fat[0]) ? 0 : 1;
        }
    }
    if (outCurrent) {
        *outCurrent = current;
    }
    return errors;
}

s32 __CARDVerify(void *card) {
    s32 result;
    s32 errors;

    result = VerifyID(card);
    if (result < 0) {
        return result;
    }
    errors = VerifyDir(card, NULL);
    errors += VerifyFAT(card, NULL);
    switch (errors) {
    case 0:
        return 0;
    case 1:
        return -6;
    default:
        return -6;
    }
}

#pragma push
#pragma use_lmw_stmw on
s32 CARDCheckExAsync(s32 chan, s32 *xferBytes, CARDCallback callback) {
    CARDControl *card;
    CARDDir *dir[2];
    u16 *fat[2];
    u16 *map;
    s32 result;
    s32 errors;
    s32 currentFat;
    s32 currentDir;
    s32 fileNo;
    u16 iBlock;
    u16 cBlock;
    u16 cFree;
    BOOL updateFat = FALSE;
    BOOL updateDir = FALSE;
    BOOL updateOrphan = FALSE;

    if (xferBytes) {
        *xferBytes = 0;
    }
    result = __CARDGetControlBlock(chan, &card);
    if (result < 0) {
        return result;
    }
    result = VerifyID(card);
    if (result < 0) {
        return __CARDPutControlBlock(card, result);
    }
    errors = VerifyDir(card, &currentDir);
    errors += VerifyFAT(card, &currentFat);
    if (1 < errors) {
        return __CARDPutControlBlock(card, -6);
    }
    dir[0] = (CARDDir *)((u8 *)card->workArea + (1 + 0) * 0x2000);
    dir[1] = (CARDDir *)((u8 *)card->workArea + (1 + 1) * 0x2000);
    fat[0] = (u16 *)((u8 *)card->workArea + (3 + 0) * 0x2000);
    fat[1] = (u16 *)((u8 *)card->workArea + (3 + 1) * 0x2000);
    switch (errors) {
    case 0:
        break;
    case 1:
        if (!card->currentDir) {
            card->currentDir = (u8 *)dir[currentDir];
            memcpy(dir[currentDir], dir[currentDir ^ 1], 0x2000);
            updateDir = TRUE;
        } else {
            card->currentFat = fat[currentFat];
            memcpy(fat[currentFat], fat[currentFat ^ 1], 0x2000);
            updateFat = TRUE;
        }
        break;
    }
    map = fat[currentFat ^ 1];
    memset(map, 0, 0x2000);
    for (fileNo = 0; fileNo < 127; fileNo++) {
        CARDDir *ent = &((CARDDir *)card->currentDir)[fileNo];
        if (ent->gameName[0] == 0xFF) {
            continue;
        }
        for (iBlock = ent->startBlock, cBlock = 0; iBlock != 0xFFFF && cBlock < ent->length;
             iBlock = card->currentFat[iBlock], ++cBlock) {
            if (!(iBlock >= 5 && iBlock < card->cBlock) || 1 < ++map[iBlock]) {
                return __CARDPutControlBlock(card, -6);
            }
        }
        if (cBlock != ent->length || iBlock != 0xFFFF) {
            return __CARDPutControlBlock(card, -6);
        }
    }
    cFree = 0;
    for (iBlock = 5; iBlock < card->cBlock; iBlock++) {
        u16 nextBlock = card->currentFat[iBlock];
        if (map[iBlock] == 0) {
            if (nextBlock != 0) {
                card->currentFat[iBlock] = 0;
                updateOrphan = TRUE;
            }
            cFree++;
        } else if (!(nextBlock >= 5 && nextBlock < card->cBlock) && nextBlock != 0xFFFF) {
            return __CARDPutControlBlock(card, -6);
        }
    }
    if (cFree != card->currentFat[3]) {
        card->currentFat[3] = cFree;
        updateOrphan = TRUE;
    }
    if (updateOrphan) {
        __CARDCheckSum(&card->currentFat[2], 0x2000 - sizeof(u32), &card->currentFat[0], &card->currentFat[1]);
    }
    memcpy(fat[currentFat ^ 1], fat[currentFat], 0x2000);
    if (updateDir) {
        if (xferBytes) {
            *xferBytes = 0x2000;
        }
        return __CARDUpdateDir(chan, callback);
    }
    if (updateFat | updateOrphan) {
        if (xferBytes) {
            *xferBytes = 0x2000;
        }
        return __CARDUpdateFatBlock(chan, card->currentFat, callback);
    }
    __CARDPutControlBlock(card, 0);
    if (callback) {
        BOOL enabled = OSDisableInterrupts();
        callback(chan, 0);
        OSRestoreInterrupts(enabled);
    }
    return 0;
}
#pragma pop

s32 CARDCheck(s32 chan) {
    s32 xferBytes;
    s32 result;

    result = CARDCheckExAsync(chan, &xferBytes, __CARDSyncCallback);
    if (result < 0 || &xferBytes == NULL) {
        return result;
    }
    return __CARDSync(chan);
}
