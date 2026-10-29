#include "common.h"

typedef struct SISteeringStatus {
    u16 button;
    u8 misc;
    s8 steering;
    u8 gas;
    u8 brake;
    u8 left;
    u8 right;
    s8 err;
} SISteeringStatus;

typedef struct SISteeringControl {
    u8 unk0[0x18];
    s32 ret;
    u8 unk1C[0x28 - 0x1C];
} SISteeringControl;

extern SISteeringControl __SISteering[4];
extern u32 __SISteeringEnableBits;
static void (*SamplingCallback)(void);

BOOL OSDisableInterrupts(void);
BOOL OSRestoreInterrupts(BOOL level);
BOOL SIIsChanBusy(s32 chan);
u32 SIGetStatus(s32 chan);
BOOL SIGetResponse(s32 chan, void *data);
void SIDisablePolling(u32 poll);

s32 SIReadSteering(s32 chan, SISteeringStatus *status) {
    BOOL enabled;
    SISteeringControl *sc;
    u32 chanBit;
    s32 ret;
    u32 data[2];

    enabled = OSDisableInterrupts();
    sc = &__SISteering[chan];
    chanBit = 0x80000000 >> chan;
    if (SIIsChanBusy(chan)) {
        sc->ret = -2;
    } else if (!(__SISteeringEnableBits & chanBit)) {
        sc->ret = -1;
    } else if (SIGetStatus(chan) & 8) {
        SIGetResponse(chan, data);
        SIDisablePolling(chanBit);
        __SISteeringEnableBits &= ~chanBit;
        sc->ret = -1;
    } else if (!SIGetResponse(chan, data) || (data[0] & 0x80000000)) {
        sc->ret = -3;
    } else {
        sc->ret = 0;
        if (status) {
            status->button = data[0] >> 16;
            status->misc = data[0] >> 8;
            status->steering = (u8)data[0] - 128;
            status->gas = data[1] >> 24;
            status->brake = data[1] >> 16;
            status->left = data[1] >> 8;
            status->right = data[1];
        }
    }
    if (status) {
        status->err = (s8)sc->ret;
    }
    ret = sc->ret;
    OSRestoreInterrupts(enabled);
    return ret;
}

void OSClearContext(void *context);
void OSSetCurrentContext(void *context);
static void SamplingHandler(s16 interrupt, void *context)
{
    u32 exceptionContext[0x2C8 / 4];

    if (SamplingCallback) {
        OSClearContext(exceptionContext);
        OSSetCurrentContext(exceptionContext);
        SamplingCallback();
        OSClearContext(exceptionContext);
        OSSetCurrentContext(context);
    }
}

s32 SIRegisterPollingHandler(void *handler);
s32 SIUnregisterPollingHandler(void *handler);
void *SISetSteeringSamplingCallback(void (*callback)(void))
{
    void *prev;

    prev = SamplingCallback;
    SamplingCallback = callback;
    if (callback != NULL) {
        SIRegisterPollingHandler(SamplingHandler);
    } else {
        SIUnregisterPollingHandler(SamplingHandler);
    }
    return prev;
}

s32 SISetCommand(s32 chan, u32 command);
s32 SITransferCommands(void);
void SIControlSteering(s32 chan, u32 control, s32 level) {
    BOOL enabled;
    u32 command;

    control &= 0x600;
    if (level <= -128) {
        command = 0;
    } else if (level >= 128) {
        command = 0x100;
    } else {
        command = level + 128;
    }
    command |= control;
    command &= 0x7FF;
    enabled = OSDisableInterrupts();
    if (__SISteeringEnableBits & (0x80000000 >> chan)) {
        command |= 0x300000;
        SISetCommand(chan, command);
        SITransferCommands();
    }
    OSRestoreInterrupts(enabled);
}

