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
    u8 unk0[0xBC];
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
