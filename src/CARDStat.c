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

typedef struct CARDStat {
    char fileName[32];    // 0x00
    u32 length;           // 0x20
    u32 time;             // 0x24
    u8 gameName[4];       // 0x28
    u8 company[2];        // 0x2C
    u8 bannerFormat;      // 0x2E
    u8 _padding0;         // 0x2F
    u32 iconAddr;         // 0x30
    u16 iconFormat;       // 0x34
    u16 iconSpeed;        // 0x36
    u32 commentAddr;      // 0x38
    u32 offsetBanner;     // 0x3C
    u32 offsetBannerTlut; // 0x40
    u32 offsetIcon[8];    // 0x44
    u32 offsetIconTlut;   // 0x64
    u32 offsetData;       // 0x68
} CARDStat;

typedef struct CARDControl {
    BOOL attached;    // 0x000
    s32 result;       // 0x004
    u16 size;         // 0x008
    u16 pageSize;     // 0x00A
    s32 sectorSize;   // 0x00C
} CARDControl;

void *memcpy(void *dst, const void *src, u32 n);
CARDDir *__CARDGetDirBlock(CARDControl *card);
s32 __CARDGetControlBlock(s32 chan, CARDControl **pcard);
s32 __CARDPutControlBlock(CARDControl *card, s32 result);
s32 __CARDAccess(CARDControl *card, CARDDir *ent);
s32 __CARDIsPublic(CARDDir *ent);
void UpdateIconOffsets(CARDDir *ent, CARDStat *stat);


typedef void (*CARDCallback)(s32 chan, s32 result);
void UpdateIconOffsets(CARDDir *ent, CARDStat *stat)
{
    u32 offset;
    BOOL iconTlut;
    int i;

    offset = ent->iconAddr;
    if (offset == 0xFFFFFFFF) {
        stat->bannerFormat = 0;
        stat->iconFormat = 0;
        stat->iconSpeed = 0;
        offset = 0;
    }

    iconTlut = FALSE;
    switch (ent->bannerFormat & 3) {
    case 1:
        stat->offsetBanner = offset;
        stat->offsetBannerTlut = offset + 96 * 32;
        offset += 96 * 32 + 2 * 256;
        break;
    case 2:
        stat->offsetBanner = offset;
        stat->offsetBannerTlut = 0xFFFFFFFF;
        offset += 2 * 96 * 32;
        break;
    default:
        stat->offsetBanner = 0xFFFFFFFF;
        stat->offsetBannerTlut = 0xFFFFFFFF;
        break;
    }
    for (i = 0; i < 8; ++i) {
        switch ((ent->iconFormat >> (2 * i)) & 3) {
        case 1:
            stat->offsetIcon[i] = offset;
            offset += 32 * 32;
            iconTlut = TRUE;
            break;
        case 2:
            stat->offsetIcon[i] = offset;
            offset += 2 * 32 * 32;
            break;
        default:
            stat->offsetIcon[i] = 0xFFFFFFFF;
            break;
        }
    }
    if (iconTlut) {
        stat->offsetIconTlut = offset;
        offset += 2 * 256;
    } else {
        stat->offsetIconTlut = 0xFFFFFFFF;
    }
    stat->offsetData = offset;
}

s32 CARDGetStatus(s32 chan, s32 fileNo, CARDStat *stat)
{
    CARDControl *card;
    CARDDir *dir;
    CARDDir *ent;
    s32 result;

    if (fileNo < 0 || 127 <= fileNo) {
        return -128;
    }
    result = __CARDGetControlBlock(chan, &card);
    if (result < 0) {
        return result;
    }

    dir = __CARDGetDirBlock(card);
    ent = &dir[fileNo];
    result = __CARDAccess(card, ent);
    if (result == -10) {
        result = __CARDIsPublic(ent);
    }

    if (0 <= result) {
        memcpy(stat->gameName, ent->gameName, sizeof(stat->gameName));
        memcpy(stat->company, ent->company, sizeof(stat->company));
        stat->length = (u32)ent->length * card->sectorSize;
        memcpy(stat->fileName, ent->fileName, 32);
        stat->time = ent->time;

        stat->bannerFormat = ent->bannerFormat;
        stat->iconAddr = ent->iconAddr;
        stat->iconFormat = ent->iconFormat;
        stat->iconSpeed = ent->iconSpeed;
        stat->commentAddr = ent->commentAddr;

        UpdateIconOffsets(ent, stat);
    }
    return __CARDPutControlBlock(card, result);
}

s32 CARDSetStatus(s32 chan, s32 fileNo, u8 *stat)
{
    s32 CARDSetStatusAsync(s32 chan, s32 fileNo, u8 *stat, CARDCallback callback);
    s32 __CARDSync(s32 chan);
    void __CARDSyncCallback(s32 chan, s32 result);
    s32 result;

    result = CARDSetStatusAsync(chan, fileNo, stat, __CARDSyncCallback);
    if (result < 0) {
        return result;
    }
    return __CARDSync(chan);
}
