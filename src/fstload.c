#include "common.h"

typedef struct DVDCommandBlock DVDCommandBlock;
typedef void (*DVDCBCallback)(s32 result, DVDCommandBlock *block);

typedef struct DVDBB2 {
    u32 bootFilePosition;
    u32 FSTPosition;
    u32 FSTLength;
    u32 FSTMaxLength;
    void *FSTAddress;
    u32 userPosition;
    u32 userLength;
    u32 padding0;
} DVDBB2;

typedef struct DVDDiskID {
    char gameName[4];
    char company[2];
    u8 diskNumber;
    u8 gameVersion;
    u8 streaming;
    u8 streamingBufSize;
    u8 padding[22];
} DVDDiskID;

struct DVDCommandBlock {
    u8 unk0[0x30];
};

typedef struct OSBootInfo {
    DVDDiskID DVDDiskID;
    u32 magic;
    u32 version;
    u32 memorySize;
    u32 consoleType;
    void *arenaLo;
    void *arenaHi;
    void *FSTLocation;
    u32 FSTMaxLength;
} OSBootInfo;

void *OSGetArenaHi(void);
void OSSetArenaHi(void *newHi);
s32 DVDGetDriveStatus(void);
void OSReport(const char *msg, ...);
void *memcpy(void *dst, const void *src, u32 n);

BOOL DVDReadAbsAsyncForBS(DVDCommandBlock *block, void *addr, s32 length, s32 offset, DVDCBCallback callback);
BOOL DVDReadDiskID(DVDCommandBlock *block, DVDDiskID *diskID, DVDCBCallback callback);
void DVDReset(void);

static s32 status;
static DVDBB2 *bb2;
static DVDDiskID *idTmp;
static u8 bb2Buf[sizeof(DVDBB2) + 31];

static void cb(s32 result, DVDCommandBlock *block) {
    if (result > 0) {
        switch (status) {
        case 0:
            status = 1;
            DVDReadAbsAsyncForBS(block, bb2, 0x20, 0x420, cb);
            break;
        case 1:
            status = 2;
            DVDReadAbsAsyncForBS(block, bb2->FSTAddress, (bb2->FSTLength + 0x1F) & ~0x1F, bb2->FSTPosition, cb);
            break;
        }
    } else if (result == -1) {
    } else if (result == -4) {
        status = 0;
        DVDReset();
        DVDReadDiskID(block, idTmp, cb);
    }
}

void __fstLoad(void) {
    OSBootInfo *bootInfo;
    DVDDiskID *id;
    u8 idTmpBuf[sizeof(DVDDiskID) + 31];
    static DVDCommandBlock block;
    void *arenaHi;

    arenaHi = OSGetArenaHi();
    bootInfo = (OSBootInfo *)0x80000000;

    idTmp = (DVDDiskID *)(((u32)idTmpBuf + 31) & ~31);
    bb2 = (DVDBB2 *)(((u32)bb2Buf + 31) & ~31);

    DVDReset();
    DVDReadDiskID(&block, idTmp, cb);
    while (DVDGetDriveStatus() != 0) {
    }

    bootInfo->FSTLocation = bb2->FSTAddress;
    bootInfo->FSTMaxLength = bb2->FSTMaxLength;

    id = &bootInfo->DVDDiskID;
    memcpy(id, idTmp, sizeof(DVDDiskID));
    OSReport("\n");
    OSReport("  Game Name ... %c%c%c%c\n", id->gameName[0], id->gameName[1], id->gameName[2], id->gameName[3]);
    OSReport("  Company ..... %c%c\n", id->company[0], id->company[1]);
    OSReport("  Disk # ...... %d\n", id->diskNumber);
    OSReport("  Game ver .... %d\n", id->gameVersion);
    OSReport("  Streaming ... %s\n", (id->streaming == 0) ? "OFF" : "ON");
    OSReport("\n");
    OSSetArenaHi(bb2->FSTAddress);
}

