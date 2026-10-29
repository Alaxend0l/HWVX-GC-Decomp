
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
void OSSleepThread(OSThreadQueue *queue);
void OSInitThreadQueue(OSThreadQueue *queue);
void OSCreateAlarm(void *alarm);
void OSInitAlarm(void);
void DSPInit(void);

typedef struct OSResetFunctionInfo {
    s32 (*func)(s32 final);
    u32 priority;
    struct OSResetFunctionInfo *next;
    struct OSResetFunctionInfo *prev;
} OSResetFunctionInfo;

void OSRegisterResetFunction(OSResetFunctionInfo *info);
s32 OnReset(s32 final);
void __CARDSetDiskID(u8 *id);

static OSResetFunctionInfo ResetFunctionInfo = { OnReset, 127, NULL, NULL };

static inline s32 GetResultCode(s32 chan) {
    CARDControl *card;

    if (chan < 0 || chan >= 2) {
        return -128;
    }
    card = &__CARDBlock[chan];
    return card->result;
}

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

void CARDInit(void) {
    int chan;

    if (__CARDBlock[0].diskID && __CARDBlock[1].diskID) {
        return;
    }
    DSPInit();
    OSInitAlarm();
    for (chan = 0; chan < 2; ++chan) {
        CARDControl *card = &__CARDBlock[chan];

        card->result = -3;
        OSInitThreadQueue(&card->threadQueue);
        OSCreateAlarm(&card->alarm);
    }
    __CARDSetDiskID((u8 *)0x80000000);
    OSRegisterResetFunction(&ResetFunctionInfo);
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

s32 CARDGetEncoding(s32 chan, u16 *encode) {
    CARDControl *card;
    CARDID *id;
    s32 result;

    result = __CARDGetControlBlock(chan, &card);
    if (result < 0) {
        return result;
    }
    id = card->workArea;
    *encode = id->encode;
    return __CARDPutControlBlock(card, 0);
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

void OSSleepThread(OSThreadQueue *queue);
s32 __CARDSync(s32 chan) {
    CARDControl *block;
    s32 result;
    s32 enabled;

    block = &__CARDBlock[chan];
    enabled = OSDisableInterrupts();
    while ((result = GetResultCode(chan)) == -1) {
        OSSleepThread(&block->threadQueue);
    }
    OSRestoreInterrupts(enabled);
    return result;
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

