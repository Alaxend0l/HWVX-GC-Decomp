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

s32 DVDConvertPathToEntrynum(char *pathPtr);
BOOL DVDGetCurrentDir(char *path, u32 maxlen);
void OSReport(const char *msg, ...);
#pragma push
#pragma scheduling on
BOOL DVDOpen(char *fileName, DVDFileInfo *fileInfo) {
    s32 entry;
    char currentDir[128];

    entry = DVDConvertPathToEntrynum(fileName);
    if (0 > entry) {
        DVDGetCurrentDir(currentDir, 128);
        OSReport("Warning: DVDOpen(): file '%s' was not found under %s.\n", fileName, currentDir);
        return FALSE;
    }
    if (((FstStart[entry * 3] & 0xff000000) == 0) ? FALSE : TRUE) {
        return FALSE;
    }
    fileInfo->startAddr = FstStart[entry * 3 + 1];
    fileInfo->length = FstStart[entry * 3 + 2];
    fileInfo->callback = (DVDCallback)NULL;
    fileInfo->cb.state = 0;
    return TRUE;
}
#pragma pop

#pragma pop

BOOL DVDClose(void *fileInfo) {
    s32 DVDCancel(void *block);

    DVDCancel(fileInfo);
    return 1;
}

static u32 myStrncpy(char *dest, char *src, u32 maxlen) {
    u32 i = maxlen;

    while ((i > 0) && (*src != 0)) {
        *dest++ = *src++;
        i--;
    }
    return maxlen - i;
}

static u32 entryToPath(u32 entry, char *path, u32 maxlen) {
    extern char *FstStringStart;
    char *name;
    u32 loc;

    if (entry == 0) {
        return 0;
    }
    name = FstStringStart + (FstStart[entry * 3] & 0x00FFFFFF);
    loc = entryToPath(FstStart[entry * 3 + 1], path, maxlen);
    if (loc == maxlen) {
        return loc;
    }
    *(path + loc++) = '/';
    loc += myStrncpy(path + loc, name, maxlen - loc);
    return loc;
}

#pragma push
#pragma scheduling on
#pragma peephole on
BOOL DVDGetCurrentDir(char *path, u32 maxlen) {
    BOOL result;
    u32 loc;
    u32 entry;

    entry = currentDirectory;
    loc = entryToPath(entry, path, maxlen);
    result = FALSE;
    if (loc == maxlen) {
        path[maxlen - 1] = result;
    } else {
        if (((FstStart[entry * 3] & 0xff000000) == 0) ? FALSE : TRUE) {
            if (loc == maxlen - 1) {
                path[loc] = '\0';
                return result;
            }
            path[loc++] = '/';
        }
        path[loc] = '\0';
        result = TRUE;
    }
    return result;
}
#pragma pop

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

void cbForReadSync(void);
s32 OSDisableInterrupts(void);
s32 OSRestoreInterrupts(s32 level);
void OSSleepThread(void *queue);
#pragma push
#pragma use_lmw_stmw on
#pragma scheduling on
#pragma peephole on
s32 DVDReadPrio(DVDFileInfo *fileInfo, void *addr, s32 length, s32 offset, s32 prio) {
    extern u32 __DVDThreadQueue[2];
    BOOL result;
    DVDCommandBlock *block;
    s32 state;
    BOOL enabled;
    s32 retVal;

    if (!((0 <= offset) && (offset < fileInfo->length))) {
        OSPanic(__FILE__, 0x329, "DVDRead(): specified area is out of the file  ");
    }
    if (!((0 <= offset + length) && (offset + length < fileInfo->length + 32))) {
        OSPanic(__FILE__, 0x32F, "DVDRead(): specified area is out of the file  ");
    }
    block = &fileInfo->cb;
    result = DVDReadAbsAsyncPrio(block, addr, length, (s32)(fileInfo->startAddr + offset), (DVDCBCallback)cbForReadSync, prio);
    if (result == FALSE) {
        return -1;
    }
    enabled = OSDisableInterrupts();
    while (1) {
        state = ((volatile DVDCommandBlock *)block)->state;
        if (state == 0) {
            retVal = (s32)block->transferredSize;
            break;
        }
        if (state == -1) {
            retVal = -1;
            break;
        }
        if (state == 10) {
            retVal = -3;
            break;
        }
        OSSleepThread(__DVDThreadQueue);
    }
    OSRestoreInterrupts(enabled);
    return retVal;
}
#pragma pop

#pragma pop

#pragma pop
void cbForReadSync(void) {
    void OSWakeupThread(void *queue);
    extern u32 __DVDThreadQueue[2];

    OSWakeupThread(__DVDThreadQueue);
}
