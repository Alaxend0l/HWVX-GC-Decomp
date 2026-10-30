
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

typedef struct CARDDir {
    u8 gameName[4];
    u8 company[2];
    u8 _padding0;
    u8 bannerFormat;
    u8 fileName[32];
    u32 time;
    u32 iconAddr;
    u16 iconFormat;
    u16 iconSpeed;
    u8 permission;
    u8 copyTimes;
    u16 startBlock;
    u16 length;
    u8 _padding1[2];
    u32 commentAddr;
} CARDDir;

void *__CARDGetFatBlock(CARDControl *card);
CARDDir *__CARDGetDirBlock(CARDControl *card);

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
s32 EXIProbe(s32 chan);
s32 EXIDeselect(s32 chan);
s32 EXIUnlock(s32 chan);
s32 EXISelect(s32 chan, u32 dev, u32 freq);
s32 EXIImm(s32 chan, void *buf, s32 len, u32 type, void *callback);
s32 EXISync(s32 chan);
void EXISetExiCallback(s32 chan, void *callback);
void OSCancelAlarm(void *alarm);
s32 EXIImmEx(s32 chan, void *buf, s32 len, u32 mode);
s32 __CARDStart(s32 chan, CARDCallback txCallback, CARDCallback exiCallback);
int EXILock(s32 chan, u32 dev, void *unlockedCallback);
s32 Retry(s32 chan);
s32 EXIDma(s32 chan, void *buf, s32 len, u32 type, void *callback);
s32 __CARDReadStatus(s32 chan, u8 *status);
s32 __CARDClearStatus(s32 chan);
s32 OSDisableInterrupts(void);
s32 OSRestoreInterrupts(s32 level);

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

void __CARDExtHandler(s32 chan, void *context) {
    CARDControl *card;
    CARDCallback callback;

    card = &__CARDBlock[chan];
    if (card->attached) {
        card->attached = FALSE;
        EXISetExiCallback(chan, 0);
        OSCancelAlarm(&card->alarm);
        callback = card->exiCallback;
        if (callback) {
            card->exiCallback = 0;
            callback(chan, -3);
        }
        if (card->result != -1) {
            card->result = -3;
        }
        callback = card->extCallback;
        if (callback && 7 <= card->mountStep) {
            card->extCallback = 0;
            callback(chan, -3);
        }
    }
}

void __CARDExiHandler(s32 chan, void *context) {
    CARDControl *card;
    CARDCallback callback;
    u8 status;
    s32 result;

    card = &__CARDBlock[chan];

    OSCancelAlarm(card->alarm);

    if (!card->attached) {
        return;
    }

    if (!EXILock(chan, 0, 0)) {
        result = -128;
        goto fatal;
    }

    if ((result = __CARDReadStatus(chan, &status)) < 0 || (result = __CARDClearStatus(chan)) < 0) {
        goto error;
    }

    if ((result = (status & 0x18) ? -5 : 0) == -5 && --card->retry > 0) {
        result = Retry(chan);
        if (result >= 0) {
            return;
        }
        goto fatal;
    }

error:
    EXIUnlock(chan);

fatal:
    callback = card->exiCallback;
    if (callback) {
        card->exiCallback = 0;
        callback(chan, result);
    }
}

void __CARDTxHandler(s32 chan, void *context) {
    CARDControl *card;
    CARDCallback callback;
    BOOL err;

    card = &__CARDBlock[chan];
    err = !EXIDeselect(chan);
    EXIUnlock(chan);
    callback = card->txCallback;
    if (callback) {
        card->txCallback = 0;
        callback(chan, (!err && EXIProbe(chan)) ? 0 : -3);
    }
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

s32 __CARDEnableInterrupt(s32 chan, BOOL enable) {
    BOOL err;
    u32 cmd;

    if (!EXISelect(chan, 0, 4)) {
        return -3;
    }

    cmd = enable ? 0x81010000 : 0x81000000;
    err = FALSE;
    err |= !EXIImm(chan, &cmd, 2, 1, 0);
    err |= !EXISync(chan);
    err |= !EXIDeselect(chan);

    if (err) {
        return -3;
    }
    return 0;
}

s32 __CARDReadStatus(s32 chan, u8 *status) {
    BOOL err;
    u32 cmd;

    if (!EXISelect(chan, 0, 4)) {
        return -3;
    }

    cmd = 0x83000000;
    err = FALSE;
    err |= !EXIImm(chan, &cmd, 2, 1, 0);
    err |= !EXISync(chan);
    err |= !EXIImm(chan, status, 1, 0, 0);
    err |= !EXISync(chan);
    err |= !EXIDeselect(chan);

    if (err) {
        return -3;
    }
    return 0;
}

s32 __CARDClearStatus(s32 chan) {
    BOOL err;
    u32 cmd;

    if (!EXISelect(chan, 0, 4)) {
        return -3;
    }

    cmd = 0x89000000;
    err = FALSE;
    err |= !EXIImm(chan, &cmd, 1, 1, 0);
    err |= !EXISync(chan);
    err |= !EXIDeselect(chan);

    if (err) {
        return -3;
    }
    return 0;
}

void EXISetExiCallback(s32 chan, void *callback);
void OSSetAlarm(void *alarm, s64 tick, void *handler);
static void TimeoutHandler(void *alarm, void *context) {
    s32 chan;
    CARDControl *card;
    CARDCallback callback;

    for (chan = 0; chan < 2; ++chan) {
        card = &__CARDBlock[chan];
        if (alarm == &card->alarm) {
            break;
        }
    }
    if (!card->attached) {
        return;
    }
    EXISetExiCallback(chan, NULL);
    callback = card->exiCallback;
    if (callback) {
        card->exiCallback = NULL;
        callback(chan, -5);
    }
}

int EXILock(s32 chan, u32 dev, void *unlockedCallback);
s32 Retry(s32 chan);
void UnlockedCallback(s32 chan, s32 result) {
    CARDCallback callback;
    CARDControl *card;

    card = &__CARDBlock[chan];
    if (result >= 0) {
        card->unlockCallback = UnlockedCallback;
        if (!EXILock(chan, 0, __CARDUnlockedHandler)) {
            result = 0;
        } else {
            card->unlockCallback = 0;
            result = Retry(chan);
        }
    }

    if (result < 0) {
        switch (card->cmd[0]) {
        case 0x52:
            callback = card->txCallback;
            if (callback) {
                card->txCallback = 0;
                callback(chan, result);
            }
            break;
        case 0xF2:
        case 0xF4:
        case 0xF1:
            callback = card->exiCallback;
            if (callback) {
                card->exiCallback = 0;
                callback(chan, result);
            }
            break;
        }
    }
}

void OSSetAlarm(void *alarm, s64 tick, void *handler);
void UnlockedCallback(s32 chan, s32 result);
#pragma push
#pragma use_lmw_stmw on
s32 __CARDStart(s32 chan, CARDCallback txCallback, CARDCallback exiCallback) {
    BOOL enabled;
    CARDControl *card;
    s32 result;

    enabled = OSDisableInterrupts();

    card = &__CARDBlock[chan];
    if (!card->attached) {
        result = -3;
    } else {
        if (txCallback) {
            card->txCallback = txCallback;
        }
        if (exiCallback) {
            card->exiCallback = exiCallback;
        }
        card->unlockCallback = UnlockedCallback;
        if (!EXILock(chan, 0, __CARDUnlockedHandler)) {
            result = -1;
        } else {
            card->unlockCallback = 0;
            if (!EXISelect(chan, 0, 4)) {
                EXIUnlock(chan);
                result = -3;
            } else {
                OSCancelAlarm(&card->alarm);
                switch (card->cmd[0]) {
                case 0xF2:
                    OSSetAlarm(&card->alarm, ((*(u32 *)0x800000F8 / 4) / 1000) * 100, TimeoutHandler);
                    break;
                case 0xF3:
                    break;
                case 0xF4:
                case 0xF1:
                    OSSetAlarm(&card->alarm, (s64)2 * (*(u32 *)0x800000F8 / 4) * (card->sectorSize / 0x2000), TimeoutHandler);
                    break;
                }
                result = 0;
            }
        }
    }
    OSRestoreInterrupts(enabled);
    return result;
}
#pragma pop

s32 __CARDReadSegment(s32 chan, CARDCallback callback) {
    CARDControl *card;
    s32 result;

    card = &__CARDBlock[chan];
    card->cmd[0] = 0x52;
    card->cmd[1] = (u8)(((u32)card->addr >> 17) & 0x7F);
    card->cmd[2] = (u8)(((u32)card->addr >> 9) & 0xFF);
    card->cmd[3] = (u8)(((u32)card->addr >> 7) & 3);
    card->cmd[4] = (u8)((u32)card->addr & 0x7F);
    card->cmdlen = 5;
    card->mode = 0;
    card->retry = 0;
    result = __CARDStart(chan, callback, 0);
    if (result == -1) {
        result = 0;
    } else if (result >= 0) {
        if (!EXIImmEx(chan, card->cmd, card->cmdlen, 1) ||
            !EXIImmEx(chan, (u8 *)card->workArea + 0x200, card->latency, 1) ||
            !EXIDma(chan, (void *)card->offset, 0x200, card->mode, __CARDTxHandler)) {
            card->txCallback = 0;
            EXIDeselect(chan);
            EXIUnlock(chan);
            result = -3;
        } else {
            result = 0;
        }
    }
    return result;
}

s32 EXIDma(s32 chan, void *buf, s32 len, u32 type, void *callback);
s32 __CARDWritePage(s32 chan, CARDCallback callback) {
    CARDControl *card;
    s32 result;

    card = &__CARDBlock[chan];
    card->cmd[0] = 0xF2;
    card->cmd[1] = (u8)(((u32)card->addr >> 17) & 0x7F);
    card->cmd[2] = (u8)(((u32)card->addr >> 9) & 0xFF);
    card->cmd[3] = (u8)(((u32)card->addr >> 7) & 3);
    card->cmd[4] = (u8)((u32)card->addr & 0x7F);
    card->cmdlen = 5;
    card->mode = 1;
    card->retry = 3;
    result = __CARDStart(chan, 0, callback);
    if (result == -1) {
        result = 0;
    } else if (result >= 0) {
        if (!EXIImmEx(chan, card->cmd, card->cmdlen, 1) ||
            !EXIDma(chan, (void *)card->offset, 128, card->mode, __CARDTxHandler)) {
            card->exiCallback = 0;
            EXIDeselect(chan);
            EXIUnlock(chan);
            result = -3;
        } else {
            result = 0;
        }
    }
    return result;
}

s32 __CARDEraseSector(s32 chan, u32 addr, CARDCallback callback) {
    CARDControl *card;
    s32 result;

    card = &__CARDBlock[chan];
    card->cmd[0] = 0xF1;
    card->cmd[1] = (u8)((addr >> 17) & 0x7F);
    card->cmd[2] = (u8)((addr >> 9) & 0xFF);
    card->cmdlen = 3;
    card->mode = -1;
    card->retry = 3;

    result = __CARDStart(chan, 0, callback);
    if (result == -1) {
        result = 0;
    } else if (result >= 0) {
        if (!EXIImmEx(chan, card->cmd, card->cmdlen, 1)) {
            card->exiCallback = 0;
            result = -3;
        } else {
            result = 0;
        }
        EXIDeselect(chan);
        EXIUnlock(chan);
    }
    return result;
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

s32 OSDisableInterrupts(void);
s32 OSRestoreInterrupts(s32 level);
s32 __CARDGetControlBlock(s32 chan, CARDControl **pcard) {
    BOOL enabled;
    s32 result;
    CARDControl *card;

    card = &__CARDBlock[chan];
    if (chan < 0 || chan >= 2 || card->diskID == NULL) {
        return -128;
    }

    enabled = OSDisableInterrupts();
    if (!card->attached) {
        result = -3;
    } else if (card->result == -1) {
        result = -1;
    } else {
        card->result = -1;
        result = 0;
        card->apiCallback = NULL;
        *pcard = card;
    }
    OSRestoreInterrupts(enabled);
    return result;
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

s32 CARDFreeBlocks(s32 chan, s32 *byteNotUsed, s32 *filesNotUsed) {
    CARDControl *card;
    s32 result;
    u16 *fat;
    CARDDir *dir;
    CARDDir *ent;
    u16 fileNo;

    result = __CARDGetControlBlock(chan, &card);
    if (result < 0) {
        return result;
    }

    fat = __CARDGetFatBlock(card);
    dir = __CARDGetDirBlock(card);
    if (fat == 0 || dir == 0) {
        return __CARDPutControlBlock(card, -6);
    }

    if (byteNotUsed) {
        *byteNotUsed = (s32)(card->sectorSize * fat[3]);
    }

    if (filesNotUsed) {
        *filesNotUsed = 0;
        for (fileNo = 0; fileNo < 127; fileNo++) {
            ent = &dir[fileNo];
            if (ent->fileName[0] == 0xFF) {
                ++*filesNotUsed;
            }
        }
    }

    return __CARDPutControlBlock(card, 0);
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

