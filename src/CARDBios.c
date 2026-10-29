
#include "common.h"

typedef void (*CARDCallback)(s32 chan, s32 result);

typedef struct OSThreadQueue { struct OSThread *head; struct OSThread *tail; } OSThreadQueue;

typedef struct CARDControl {
    s32 attached;
    s32 result;
    u16 size;
    u8 unkA[0xC - 0xA];
    s32 sectorSize;
    u16 cBlock;
    u8 unk12[0x14 - 0x12];
    s32 latency;
    u8 unk18[0x24 - 0x18];
    s32 mountStep;
    s32 formatStep;
    u8 unk2C[0x80 - 0x2C];
    void *workArea;
    u8 *currentDir;
    u16 *currentFat;
    OSThreadQueue threadQueue;
    u8 cmd[9];
    s32 cmdlen;
    s32 mode;
    s32 retry;
    u32 repeat;
    void *addr;
    s32 offset;
    u8 unkB8[0xBE - 0xB8];
    u16 startBlock;
    struct CARDFileInfo *fileInfo;
    CARDCallback extCallback;
    CARDCallback txCallback;
    CARDCallback exiCallback;
    CARDCallback apiCallback;
    CARDCallback xferCallback;
    CARDCallback eraseCallback;
    CARDCallback unlockCallback;
    u8 alarm[0x28];
    u8 unk108[0x10C - 0x108];
    u8 *diskID;
} CARDControl;

typedef struct CARDID {
    u8 serial[32];
    u16 deviceID;
    u16 size;
    u16 encode;
} CARDID;

CARDControl __CARDBlock[2];
u8 __CARDDiskNone[32];

void OSWakeupThread(OSThreadQueue *queue);
s32 __CARDGetControlBlock(s32 chan, CARDControl **pcard);

void __CARDDefaultApiCallback(void)
{
}

void __CARDSyncCallback(s32 chan, s32 result) {
    OSWakeupThread(&__CARDBlock[chan].threadQueue);
}

s32 EXIProbe(s32 chan);
void __CARDUnlockedHandler(s32 chan, void *context) {
    CARDControl *card;
    CARDCallback callback;

    card = &__CARDBlock[chan];
    callback = card->unlockCallback;
    if (callback) {
        card->unlockCallback = 0;
        callback(chan, EXIProbe(chan) ? 1 : -3);
    }
}

void __CARDSetDiskID(u8 *id) {
    __CARDBlock[0].diskID = id ? id : __CARDDiskNone;
    __CARDBlock[1].diskID = id ? id : __CARDDiskNone;
}

s32 __CARDPutControlBlock(CARDControl *card, s32 result) {
    BOOL enabled;

    enabled = OSDisableInterrupts();
    if (card->attached) {
        card->result = result;
    } else if (card->result == -1) {
        card->result = result;
    }
    OSRestoreInterrupts(enabled);
    return result;
}

s32 CARDGetSectorSize(s32 chan, u32 *size) {
    CARDControl *card;
    s32 result;

    result = __CARDGetControlBlock(chan, &card);
    if (result < 0) {
        return result;
    }
    *size = card->sectorSize;
    return __CARDPutControlBlock(card, 0);
}

extern int CARDUnmount(int);
s32 OnReset(s32 arg0)
{
    if ((arg0 == 0) && ((CARDUnmount(0) == -1) || (CARDUnmount(1) == -1)))
    {
        return 0;
    }
    return 1;
}

