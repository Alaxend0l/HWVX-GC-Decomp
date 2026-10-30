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

extern CARDControl __CARDBlock[2];
extern u16 __CARDVendorID;

static u32 SectorSizeTable[8] = {
    8 * 1024, 16 * 1024, 32 * 1024, 64 * 1024, 128 * 1024, 256 * 1024, 0, 0,
};

static u32 LatencyTable[8] = {
    4, 8, 16, 32, 64, 128, 256, 512,
};

int OSDisableInterrupts(void);
int OSRestoreInterrupts(int level);
void OSCancelAlarm(void *alarm);
void *EXISetExiCallback(s32 chan, void *callback);
int EXIDetach(s32 chan);
static void DoUnmount(s32 chan, s32 result);
s32 __CARDPutControlBlock(CARDControl *card, s32 result);



typedef void (*CARDCallback)(s32 chan, s32 result);
static BOOL IsCard(u32 id) {
    u32 size;
    s32 sectorSize;

    if ((id & 0xFFFF0000) && (id != 0x80000004 || __CARDVendorID == 0xFFFF)) {
        return FALSE;
    }
    if ((id & 3) != 0) {
        return FALSE;
    }
    size = id & 0xFC;
    switch (size) {
    case 4:
    case 8:
    case 16:
    case 32:
    case 64:
    case 128:
        break;
    default:
        return FALSE;
    }
    sectorSize = SectorSizeTable[(id & 0x00003800) >> 11];
    if (sectorSize == 0) {
        return FALSE;
    }
    if ((size * 1024 * 1024 / 8) / sectorSize < 8) {
        return FALSE;
    }
    return TRUE;
}

s32 EXIProbeEx(s32 chan);
u32 EXIGetState(s32 chan);
s32 EXIGetID(s32 chan, u32 dev, u32 *id);

#pragma push
#pragma use_lmw_stmw on
s32 CARDProbeEx(s32 chan, s32 *memSize, s32 *sectorSize) {
    u32 id;
    CARDControl *card;
    BOOL enabled;
    s32 result;
    s32 probe;

    if (chan < 0 || 2 <= chan) {
        return -128;
    }
    if (*(u8 *)0x800030E3 & 0x80) {
        return -3;
    }
    card = &__CARDBlock[chan];
    enabled = OSDisableInterrupts();
    probe = EXIProbeEx(chan);
    if (probe == -1) {
        result = -3;
    } else if (probe == 0) {
        result = -1;
    } else if (card->attached) {
        if (card->mountStep < 1) {
            result = -1;
        } else {
            if (memSize) {
                *memSize = card->size;
            }
            if (sectorSize) {
                *sectorSize = card->sectorSize;
            }
            result = 0;
        }
    } else if (EXIGetState(chan) & 8) {
        result = -2;
    } else if (!EXIGetID(chan, 0, &id)) {
        result = -1;
    } else if (IsCard(id)) {
        if (memSize) {
            *memSize = (s32)(id & 0xFC);
        }
        if (sectorSize) {
            *sectorSize = SectorSizeTable[(id & 0x00003800) >> 11];
        }
        result = 0;
    } else {
        result = -2;
    }
    OSRestoreInterrupts(enabled);
    return result;
}

static s32 DoMount(s32 chan) {
    typedef struct OSSramEx {
        u8 flashID[2][12];
        u32 wirelessKeyboardID;
        u16 wirelessPadID[4];
        u8 dvdErrorCode;
        u8 _padding0;
        u8 flashIDCheckSum[2];
    } OSSramEx;
    void __CARDExiHandler(s32 chan, void *context);
    void __CARDMountCallback(s32 chan, s32 result);
    s32 EXIProbe(s32 chan);
    s32 EXIUnlock(s32 chan);
    s32 __CARDClearStatus(s32 chan);
    s32 __CARDReadStatus(s32 chan, u8 *status);
    s32 __CARDUnlock(s32 chan, u8 *flashID);
    s32 __CARDEnableInterrupt(s32 chan, BOOL enable);
    s32 __CARDRead(s32 chan, u32 addr, s32 length, void *dst, CARDCallback callback);
    OSSramEx *__OSLockSramEx(void);
    BOOL __OSUnlockSramEx(BOOL commit);
    void DCInvalidateRange(void *addr, u32 nBytes);
    CARDControl *card;
    u32 id;
    u8 status;
    s32 result;
    OSSramEx *sram;
    int i;
    u8 checkSum;
    int step;

    card = &__CARDBlock[chan];

    if (card->mountStep == 0) {
        if (EXIGetID(chan, 0, &id) == 0) {
            result = -3;
        } else if (IsCard(id)) {
            result = 0;
        } else {
            result = -2;
        }
        if (result < 0) {
            goto error;
        }

        *(u32 *)card->unk108 = id;
        card->size = (u16)(id & 0xFC);
        card->sectorSize = SectorSizeTable[(id & 0x00003800) >> 11];
        card->cBlock = (u16)((card->size * 1024 * 1024 / 8) / card->sectorSize);
        card->latency = LatencyTable[(id & 0x00000700) >> 8];

        result = __CARDClearStatus(chan);
        if (result < 0) {
            goto error;
        }
        result = __CARDReadStatus(chan, &status);
        if (result < 0) {
            goto error;
        }

        if (!EXIProbe(chan)) {
            result = -3;
            goto error;
        }

        if (!(status & 0x40)) {
            result = __CARDUnlock(chan, card->unk18);
            if (result < 0) {
                goto error;
            }

            checkSum = 0;
            sram = __OSLockSramEx();
            for (i = 0; i < 12; i++) {
                sram->flashID[chan][i] = card->unk18[i];
                checkSum += card->unk18[i];
            }
            sram->flashIDCheckSum[chan] = (u8)~checkSum;
            __OSUnlockSramEx(TRUE);
            return result;
        } else {
            card->mountStep = 1;

            checkSum = 0;
            sram = __OSLockSramEx();
            for (i = 0; i < 12; i++) {
                checkSum += sram->flashID[chan][i];
            }
            __OSUnlockSramEx(FALSE);
            if (sram->flashIDCheckSum[chan] != (u8)~checkSum) {
                result = -5;
                goto error;
            }
        }
    }

    if (card->mountStep == 1) {
        if (*(u32 *)card->unk108 == 0x80000004) {
            u16 vendorID;

            sram = __OSLockSramEx();
            vendorID = *(u16 *)sram->flashID[chan];
            __OSUnlockSramEx(FALSE);
            if (__CARDVendorID == 0xFFFF || vendorID != __CARDVendorID) {
                result = -2;
                goto error;
            }
        }

        card->mountStep = 2;

        result = __CARDEnableInterrupt(chan, TRUE);
        if (result < 0) {
            goto error;
        }

        EXISetExiCallback(chan, __CARDExiHandler);
        EXIUnlock(chan);
        DCInvalidateRange(card->workArea, 0xA000);
    }

    step = card->mountStep - 2;
    result = __CARDRead(chan, (u32)card->sectorSize * step, 0x2000, (u8 *)card->workArea + (0x2000 * step),
                        __CARDMountCallback);
    if (result < 0) {
        __CARDPutControlBlock(card, result);
    }
    return result;

error:
    EXIUnlock(chan);
    DoUnmount(chan, result);
    return result;
}

#pragma pop

static s32 DoMount(s32 chan);
static void DoUnmount(s32 chan, s32 result);
s32 __CARDVerify(CARDControl *card);
int EXILock(s32 chan, u32 dev, void *unlockedCallback);
s32 __CARDUnlockedHandler(s32 chan, void *context);
s32 __CARDPutControlBlock(CARDControl *card, s32 result);

void __CARDMountCallback(s32 chan, s32 result) {
    CARDControl *card;
    CARDCallback callback;

    card = &__CARDBlock[chan];
    switch (result) {
    case 0:
        if (++card->mountStep < 7) {
            result = DoMount(chan);
            if (0 <= result) {
                return;
            }
        } else {
            result = __CARDVerify(card);
        }
        break;
    case 1:
        card->unlockCallback = __CARDMountCallback;
        if (!EXILock(chan, 0, __CARDUnlockedHandler)) {
            return;
        }
        card->unlockCallback = 0;
        result = DoMount(chan);
        if (0 <= result) {
            return;
        }
        break;
    case -5:
    case -3:
        DoUnmount(chan, result);
        break;
    }
    callback = card->apiCallback;
    card->apiCallback = 0;
    __CARDPutControlBlock(card, result);
    callback(chan, result);
}

void __CARDDefaultApiCallback(s32 chan, s32 result);
void __CARDExtHandler(s32 chan, void *context);
int EXIAttach(s32 chan, void *extCallback);
#pragma push
#pragma use_lmw_stmw on
s32 CARDMountAsync(s32 chan, void *workArea, CARDCallback detachCallback, CARDCallback attachCallback) {
    CARDControl *card;
    BOOL enabled;

    if (chan < 0 || 2 <= chan) {
        return -128;
    }
    if (*(u8 *)0x800030E3 & 0x80) {
        return -3;
    }
    card = &__CARDBlock[chan];

    enabled = OSDisableInterrupts();
    if (card->result == -1) {
        OSRestoreInterrupts(enabled);
        return -1;
    }
    if (!card->attached && (EXIGetState(chan) & 8)) {
        OSRestoreInterrupts(enabled);
        return -2;
    }

    card->result = -1;
    card->workArea = workArea;
    card->extCallback = detachCallback;
    card->apiCallback = attachCallback ? attachCallback : __CARDDefaultApiCallback;
    card->exiCallback = 0;

    if (!card->attached && !EXIAttach(chan, __CARDExtHandler)) {
        card->result = -3;
        OSRestoreInterrupts(enabled);
        return -3;
    }

    card->mountStep = 0;
    card->attached = 1;
    EXISetExiCallback(chan, 0);
    OSCancelAlarm(&card->alarm);

    card->currentDir = 0;
    card->currentFat = 0;

    OSRestoreInterrupts(enabled);

    card->unlockCallback = __CARDMountCallback;
    if (!EXILock(chan, 0, __CARDUnlockedHandler)) {
        return 0;
    }
    card->unlockCallback = 0;

    return DoMount(chan);
}
#pragma pop

s32 CARDMount(s32 chan, s32 fileNo, u8 *stat)
{
    s32 CARDMountAsync(s32 chan, s32 fileNo, u8 *stat, CARDCallback callback);
    s32 __CARDSync(s32 chan);
    void __CARDSyncCallback(s32 chan, s32 result);
    s32 result;

    result = CARDMountAsync(chan, fileNo, stat, __CARDSyncCallback);
    if (result < 0) {
        return result;
    }
    return __CARDSync(chan);
}

static void DoUnmount(s32 chan, s32 result) {
    CARDControl *card;
    int enabled;

    card = &__CARDBlock[chan];
    enabled = OSDisableInterrupts();
    if (card->attached) {
        EXISetExiCallback(chan, 0);
        EXIDetach(chan);
        OSCancelAlarm(&card->alarm);
        card->attached = 0;
        card->result = result;
        card->mountStep = 0;
    }
    OSRestoreInterrupts(enabled);
}

s32 __CARDGetControlBlock(s32 chan, CARDControl **pcard);
s32 CARDUnmount(s32 chan) {
    CARDControl *card;
    s32 result;

    result = __CARDGetControlBlock(chan, &card);
    if (result < 0) {
        return result;
    }
    DoUnmount(chan, -3);
    return 0;
}

