#include "common.h"

void OSClearContext(void *);
void OSSetCurrentContext(void *);

static u32 RecalibrateBits;
static u32 WaitingBits;
static u32 CheckingBits;
static u32 PendingBits;
void (*SamplingCallback)(void);
s32 __PADSpec;

u32 ResettingChan = 0x20;
u32 XPatchBits = 0xF0000000;
u32 AnalogMode = 0x300;
s32 Spec = 5;
void *MakeStatus = NULL;
f32 CmdReadOrigin = 8.0f;
f32 CmdCalibrate = 32.0f;

u8 Origin[4][12];
u32 CmdProbeDevice[4];

void SPEC0_MakeStatus(s32, void *, u32 *);
void SPEC1_MakeStatus(s32, void *, u32 *);
void SPEC2_MakeStatus_80076F6C(s32, void *, u32 *);

static void ClampS8(s8 *var, s8 org)
{
    s8 v = *var;

    if (0 < org) {
        s8 min = (s8)(org - 128);
        if (v < min) {
            v = min;
        }
    } else if (org < 0) {
        s8 max = (s8)(org + 127);
        if (max < v) {
            v = max;
        }
    }
    *var = v - org;
}

static void ClampU8(u8 *var, u8 org)
{
    if (*var < org) {
        *var = org;
    }
    *var -= org;
}

void UpdateOrigin(s32 chan)
{
    typedef struct {
        u16 button;
        s8 stickX;
        s8 stickY;
        s8 substickX;
        s8 substickY;
        u8 triggerLeft;
        u8 triggerRight;
        u8 analogA;
        u8 analogB;
        s8 err;
    } PADStatus_;

    PADStatus_ *origin;
    u32 chanBit = 0x80000000 >> chan;

    origin = (PADStatus_ *)Origin[chan];
    switch (AnalogMode & 0x700) {
    case 0x000:
    case 0x500:
    case 0x600:
    case 0x700:
        origin->triggerLeft &= ~15;
        origin->triggerRight &= ~15;
        origin->analogA &= ~15;
        origin->analogB &= ~15;
        break;
    case 0x100:
        origin->substickX &= ~15;
        origin->substickY &= ~15;
        origin->analogA &= ~15;
        origin->analogB &= ~15;
        break;
    case 0x200:
        origin->substickX &= ~15;
        origin->substickY &= ~15;
        origin->triggerLeft &= ~15;
        origin->triggerRight &= ~15;
        break;
    case 0x300:
    case 0x400:
        break;
    }

    origin->stickX -= 128;
    origin->stickY -= 128;
    origin->substickX -= 128;
    origin->substickY -= 128;

    if (XPatchBits & chanBit) {
        if (64 < origin->stickX && (SIGetType(chan) & 0xFFFF0000) == 0x09000000) {
            origin->stickX = 0;
        }
    }
}

void PADSetSpec(s32 spec)
{
    __PADSpec = 0;
    switch (spec) {
    case 0:
        MakeStatus = SPEC0_MakeStatus;
        break;
    case 1:
        MakeStatus = SPEC1_MakeStatus;
        break;
    case 2:
    case 3:
    case 4:
    case 5:
        MakeStatus = SPEC2_MakeStatus_80076F6C;
        break;
    }
    Spec = spec;
}

void SPEC2_MakeStatus(s32 chan, void *arg1, u32 *data)
{
    typedef struct {
        u16 button;
        s8 stickX;
        s8 stickY;
        s8 substickX;
        s8 substickY;
        u8 triggerLeft;
        u8 triggerRight;
        u8 analogA;
        u8 analogB;
        s8 err;
    } PADStatus_;

    PADStatus_ *status = arg1;
    PADStatus_ *origin;

    status->button = (u16)((data[0] >> 16) & 0x3FFF);
    status->stickX = (s8)(data[0] >> 8);
    status->stickY = (s8)(data[0]);
    switch (AnalogMode & 0x700) {
    case 0x000:
    case 0x500:
    case 0x600:
    case 0x700:
        status->substickX = (s8)(data[1] >> 24);
        status->substickY = (s8)(data[1] >> 16);
        status->triggerLeft = (u8)(((data[1] >> 12) & 0x0F) << 4);
        status->triggerRight = (u8)(((data[1] >> 8) & 0x0F) << 4);
        status->analogA = (u8)(((data[1] >> 4) & 0x0F) << 4);
        status->analogB = (u8)(((data[1] >> 0) & 0x0F) << 4);
        break;
    case 0x100:
        status->substickX = (s8)(((data[1] >> 28) & 0x0F) << 4);
        status->substickY = (s8)(((data[1] >> 24) & 0x0F) << 4);
        status->triggerLeft = (u8)(data[1] >> 16);
        status->triggerRight = (u8)(data[1] >> 8);
        status->analogA = (u8)(((data[1] >> 4) & 0x0F) << 4);
        status->analogB = (u8)(((data[1] >> 0) & 0x0F) << 4);
        break;
    case 0x200:
        status->substickX = (s8)(((data[1] >> 28) & 0x0F) << 4);
        status->substickY = (s8)(((data[1] >> 24) & 0x0F) << 4);
        status->triggerLeft = (u8)(((data[1] >> 20) & 0x0F) << 4);
        status->triggerRight = (u8)(((data[1] >> 16) & 0x0F) << 4);
        status->analogA = (u8)(data[1] >> 8);
        status->analogB = (u8)(data[1] >> 0);
        break;
    case 0x300:
        status->substickX = (s8)(data[1] >> 24);
        status->substickY = (s8)(data[1] >> 16);
        status->triggerLeft = (u8)(data[1] >> 8);
        status->triggerRight = (u8)(data[1] >> 0);
        status->analogA = 0;
        status->analogB = 0;
        break;
    case 0x400:
        status->substickX = (s8)(data[1] >> 24);
        status->substickY = (s8)(data[1] >> 16);
        status->triggerLeft = 0;
        status->triggerRight = 0;
        status->analogA = (u8)(data[1] >> 8);
        status->analogB = (u8)(data[1] >> 0);
        break;
    }

    status->stickX -= 128;
    status->stickY -= 128;
    status->substickX -= 128;
    status->substickY -= 128;

    origin = (PADStatus_ *)Origin[chan];
    ClampS8(&status->stickX, origin->stickX);
    ClampS8(&status->stickY, origin->stickY);
    ClampS8(&status->substickX, origin->substickX);
    ClampS8(&status->substickY, origin->substickY);
    ClampU8(&status->triggerLeft, origin->triggerLeft);
    ClampU8(&status->triggerRight, origin->triggerRight);
}

static void SamplingHandler(s16 interrupt, void *context)
{
    u32 exceptionContext[0x2C8 / 4];

    if (SamplingCallback) {
        OSClearContext(exceptionContext);
        OSSetCurrentContext(exceptionContext);
        SamplingCallback();
    }
    OSClearContext(exceptionContext);
    OSSetCurrentContext(context);
}

s32 SIRegisterPollingHandler(void *handler);
s32 SIUnregisterPollingHandler(void *handler);
void *PADSetSamplingCallback(void (*callback)(void))
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


