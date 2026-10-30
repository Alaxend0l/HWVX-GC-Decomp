#include "common.h"

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
    BOOL attached; // 0x000
    u8 unk4[0x10 - 0x4];
    u16 cBlock; // 0x010
    u8 unk12[0x10C - 0x12];
    u8 *diskID; // 0x10C
} CARDControl;

CARDDir *__CARDGetDirBlock(CARDControl *card);
s32 __CARDGetControlBlock(s32 chan, void **pcard);
s32 __CARDPutControlBlock(void *card, s32 result);

extern u8 __CARDDiskNone[32];
int memcmp(const void *a, const void *b, u32 n);


BOOL __CARDCompareFileName(CARDDir *ent, const char *fileName) {
    char *entName;
    char c1;
    char c2;
    int n;

    entName = (char *)ent->fileName;
    n = 32;
    while (0 <= --n) {
        if ((c1 = *entName++) != (c2 = *fileName++)) {
            return FALSE;
        } else if (c2 == '\0') {
            return TRUE;
        }
    }
    if (*fileName == '\0') {
        return TRUE;
    }
    return FALSE;
}

s32 __CARDAccess(CARDControl *card, CARDDir *ent) {
    if (ent->gameName[0] == 0xFF) {
        return -4;
    }
    if (card->diskID == __CARDDiskNone
        || (memcmp(ent->gameName, card->diskID, 4) == 0 && memcmp(ent->company, card->diskID + 4, 2) == 0)) {
        return 0;
    }
    return -10;
}

s32 __CARDIsPublic(CARDDir *ent) {
    if (ent->gameName[0] == 0xFF) {
        return -4;
    }
    if (ent->permission & 4) {
        return 0;
    }
    return -10;
}

s32 __CARDGetFileNo(CARDControl *card, const char *fileName, s32 *pfileNo) {
    CARDDir *dir;
    CARDDir *ent;
    s32 fileNo;
    s32 result;

    if (!card->attached) {
        return -3;
    }
    dir = __CARDGetDirBlock(card);
    for (fileNo = 0; fileNo < 127; fileNo++) {
        ent = &dir[fileNo];
        result = __CARDAccess(card, ent);
        if (result < 0) {
            continue;
        }
        if (__CARDCompareFileName(ent, fileName)) {
            *pfileNo = fileNo;
            return 0;
        }
    }
    return -4;
}

s32 CARDOpen(s32 chan, const char *fileName, CARDFileInfo *fileInfo) {
    CARDControl *card;
    CARDDir *dir;
    CARDDir *ent;
    s32 fileNo;
    s32 result;

    fileInfo->chan = -1;
    result = __CARDGetControlBlock(chan, (void **)&card);
    if (result < 0) {
        return result;
    }
    result = __CARDGetFileNo(card, fileName, &fileNo);
    if (0 <= result) {
        dir = __CARDGetDirBlock(card);
        ent = &dir[fileNo];
        if (ent->startBlock < 5 || ent->startBlock >= card->cBlock) {
            result = -6;
        } else {
            fileInfo->chan = chan;
            fileInfo->fileNo = fileNo;
            fileInfo->offset = 0;
            fileInfo->iBlock = ent->startBlock;
        }
    }
    return __CARDPutControlBlock(card, result);
}

s32 CARDClose(CARDFileInfo *fileInfo) {
    void *card;
    s32 result;

    result = __CARDGetControlBlock(fileInfo->chan, &card);
    if (result < 0) {
        return result;
    }
    fileInfo->chan = -1;
    return __CARDPutControlBlock(card, 0);
}

s32 __CARDIsOpened(void) {
    return 0;
}
