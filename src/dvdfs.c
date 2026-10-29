#include "common.h"

typedef struct DVDCommandBlock DVDCommandBlock;
typedef void (*DVDCBCallback)(s32 result, DVDCommandBlock *block);

struct DVDCommandBlock {
    DVDCommandBlock *next;
    DVDCommandBlock *prev;
    u32 command;
    s32 state;
    u32 offset;
    u32 length;
    void *addr;
    u32 currTransferSize;
    u32 transferredSize;
    void *id;
    DVDCBCallback callback;
    void *userData;
};

typedef struct DVDFileInfo DVDFileInfo;
typedef void (*DVDCallback)(s32 result, DVDFileInfo *fileInfo);

struct DVDFileInfo {
    DVDCommandBlock cb;
    u32 startAddr;
    u32 length;
    DVDCallback callback;
};

extern u32 *FstStart;
extern u32 currentDirectory;

#pragma push
#pragma peephole on
#pragma scheduling off
#pragma push
#pragma peephole on
#pragma scheduling off
void __DVDFSInit(void) {
    extern u8 *BootInfo;
    extern u32 *FstStart;
    extern char *FstStringStart;
    extern u32 MaxEntryNum;

    BootInfo = (u8 *)0x80000000;
    FstStart = *(u32 **)(BootInfo + 0x38);
    if (FstStart) {
        MaxEntryNum = FstStart[2];
        FstStringStart = (char *)FstStart + MaxEntryNum * 12;
    }
}
#pragma pop

BOOL DVDClose(void *fileInfo) {
    s32 DVDCancel(void *block);

    DVDCancel(fileInfo);
    return 1;
}

void OSPanic(const char *file, int line, const char *msg, ...);
BOOL DVDReadAbsAsyncPrio(DVDCommandBlock *block, void *addr, s32 length, s32 offset, DVDCBCallback callback, s32 prio);
static void cbForReadAsync(s32 result, DVDCommandBlock *block);
#pragma push
#pragma scheduling on
#pragma peephole on
BOOL DVDReadAsyncPrio(DVDFileInfo *fileInfo, void *addr, s32 length, s32 offset, DVDCallback callback, s32 prio) {
    if (!((0 <= offset) && (offset < fileInfo->length))) {
        OSPanic(__FILE__, 0x2E3, "DVDReadAsync(): specified area is out of the file  ");
    }
    if (!((0 <= offset + length) && (offset + length < fileInfo->length + 32))) {
        OSPanic(__FILE__, 0x2E9, "DVDReadAsync(): specified area is out of the file  ");
    }
    fileInfo->callback = callback;
    DVDReadAbsAsyncPrio(&fileInfo->cb, addr, length, (s32)(fileInfo->startAddr + offset), cbForReadAsync, prio);
    return TRUE;
}
#pragma pop

#pragma push
#pragma peephole on
#pragma scheduling off
static void cbForReadAsync(s32 result, DVDCommandBlock *block) {
    DVDFileInfo *fileInfo = (DVDFileInfo *)block;

    if (fileInfo->callback) {
        (fileInfo->callback)(result, fileInfo);
    }
}
#pragma pop

#pragma pop
void cbForReadSync(void) {
    void OSWakeupThread(void *queue);
    extern u32 __DVDThreadQueue[2];

    OSWakeupThread(__DVDThreadQueue);
}
