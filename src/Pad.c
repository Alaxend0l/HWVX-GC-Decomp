#include "common.h"

void OSClearContext(void *);
void OSSetCurrentContext(void *);

static BOOL Initialized;
u32 EnabledBits;
u32 ResettingBits;
static u32 RecalibrateBits;
static u32 WaitingBits;
static u32 CheckingBits;
static u32 PendingBits;
void (*SamplingCallback)(void);
u32 __PADSpec;

s32 OSDisableInterrupts(void);
void OSRestoreInterrupts(s32);
void SIDisablePolling(u32);
s32 SIGetTypeAsync(s32, void (*)(s32, u32));
void PADTypeAndStatusCallback(s32 chan, u32 type);
void PADReceiveCheckCallback(s32 chan, u32 type);
BOOL SIBusy(void);
void *memset(void *, int, u32);
void *PADSetSamplingCallback(void (*callback)(void));

s32 ResettingChan = 0x20;
u32 XPatchBits = 0xF0000000;
u32 AnalogMode = 0x300;
u32 Spec = 5;
void *MakeStatus = NULL;
f32 CmdReadOrigin = 8.0f;
f32 CmdCalibrate = 32.0f;

static u32 Type[4];
static u8 Origin[4][12];
static u32 CmdProbeDevice[4];

extern u32 __PADFixBits;

typedef struct OSResetFunctionInfo {
    BOOL (*func)(BOOL final);
    u32 priority;
    struct OSResetFunctionInfo *next;
    struct OSResetFunctionInfo *prev;
} OSResetFunctionInfo;

void OSRegisterResetFunction(OSResetFunctionInfo *info);
BOOL OnReset(BOOL final);

static OSResetFunctionInfo ResetFunctionInfo = { OnReset, 127 };

void SPEC0_MakeStatus(s32, void *, u32 *);
void SPEC1_MakeStatus(s32, void *, u32 *);
void SPEC2_MakeStatus_80076F6C(s32, void *, u32 *);

static void ClampS8(s8 *var, s8 org)
{
    s8 v = *var;

    if (0 < org) {
        s8 min = (s8)(-128 + org);
        if (v < min) {
            v = min;
        }
    } else if (org < 0) {
        s8 max = (s8)(127 + org);
        if (max < v) {
            v = max;
        }
    }
    v -= org;
    *var = v;
}

static void ClampU8(u8 *var, u8 org)
{
    u8 v = *var;

    if (v < org) {
        v = org;
    }
    v -= org;
    *var = v;
}

void OSSetWirelessID(s32 chan, u16 id);

typedef struct PADStatus {
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
} PADStatus;

static inline void PADDisable(s32 chan)
{
    BOOL enabled;
    u32 chanBit;

    enabled = OSDisableInterrupts();
    chanBit = 0x80000000 >> chan;
    SIDisablePolling(chanBit);
    EnabledBits &= ~chanBit;
    WaitingBits &= ~chanBit;
    CheckingBits &= ~chanBit;
    PendingBits &= ~chanBit;
    OSSetWirelessID(chan, 0);
    OSRestoreInterrupts(enabled);
}

BOOL SIGetResponse(s32 chan, void *data);
void SISetCommand(s32 chan, u32 command);
u32 SIEnablePolling(u32 poll);

static inline void PADEnable(s32 chan)
{
    u32 cmd;
    u32 chanBit;
    u32 data[2];

    chanBit = 0x80000000 >> chan;
    EnabledBits |= chanBit;
    SIGetResponse(chan, data);
    cmd = (0x40 << 16) | AnalogMode;
    SISetCommand(chan, cmd);
    SIEnablePolling(EnabledBits);
}

static inline void DoReset(void)
{
    u32 chanBit;

    ResettingChan = __cntlzw(ResettingBits);
    if (ResettingChan != 32) {
        chanBit = 0x80000000 >> ResettingChan;
        ResettingBits &= ~chanBit;
        memset(Origin[ResettingChan], 0, 12);
        SIGetTypeAsync(ResettingChan, PADTypeAndStatusCallback);
    }
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

void SISetCommand(s32 chan, u32 command);
u32 SIEnablePolling(u32 poll);
void PADOriginCallback(s32 chan, u32 error, void *context)
{
    if (!(error & 0xF)) {
        UpdateOrigin(ResettingChan);
        PADEnable(ResettingChan);
    }
    DoReset();
}

void UpdateOrigin(s32 chan);
void PADOriginUpdateCallback(s32 chan, u32 error, void *context)
{
    if (!(EnabledBits & (0x80000000 >> chan))) {
        return;
    }
    if (!(error & 0xF)) {
        UpdateOrigin(chan);
    }
    if (error & 8) {
        PADDisable(chan);
    }
}

void PADProbeCallback(s32 chan, u32 error, void *context)
{
    if (!(error & 0xF)) {
        PADEnable(ResettingChan);
        WaitingBits |= 0x80000000 >> ResettingChan;
    }
    DoReset();
}

void PADReceiveCheckCallback(s32 chan, u32 type)
{
    BOOL SITransfer(s32 chan, void *output, u32 outputBytes, void *input, u32 inputBytes, void *callback, s64 delay);
    void PADOriginUpdateCallback(s32 chan, u32 error, void *context);
    u32 error;
    u32 chanBit;

    chanBit = 0x80000000 >> chan;
    if (!(EnabledBits & chanBit)) {
        return;
    }
    error = type & 0xFF;
    type &= ~0xFF;
    WaitingBits &= ~chanBit;
    CheckingBits &= ~chanBit;
    if (!(error & 0xF) && (type & 0x80000000) && (type & 0x00100000) && (type & 0x40000000)
        && !(type & 0x04000000) && !(type & 0x00080000) && !(type & 0x00040000)) {
        SITransfer(chan, &CmdReadOrigin, 1, Origin[chan], 10, PADOriginUpdateCallback, 0);
    } else {
        PADDisable(chan);
    }
}

BOOL PADReset(u32 mask)
{
    BOOL enabled;
    u32 disableBits;

    enabled = OSDisableInterrupts();
    mask |= PendingBits;
    PendingBits = 0;
    mask &= ~(WaitingBits | CheckingBits);
    ResettingBits |= mask;
    disableBits = ResettingBits & EnabledBits;
    EnabledBits &= ~mask;
    if (Spec == 4) {
        RecalibrateBits |= mask;
    }
    SIDisablePolling(disableBits);
    if (ResettingChan == 32) {
        DoReset();
    }
    OSRestoreInterrupts(enabled);
    return TRUE;
}

u8 __gUnknown800030E3 : 0x800030E3;

static inline BOOL PADRecalibrate(u32 mask)
{
    BOOL enabled;
    u32 disableBits;

    enabled = OSDisableInterrupts();
    mask |= PendingBits;
    PendingBits = 0;
    mask &= ~(WaitingBits | CheckingBits);
    ResettingBits |= mask;
    disableBits = ResettingBits & EnabledBits;
    EnabledBits &= ~mask;
    if (!(__gUnknown800030E3 & 0x40)) {
        RecalibrateBits |= mask;
    }
    SIDisablePolling(disableBits);
    if (ResettingChan == 32) {
        DoReset();
    }
    OSRestoreInterrupts(enabled);
    return TRUE;
}

static inline BOOL PADSync(void)
{
    return ResettingBits == 0 && ResettingChan == 32 && !SIBusy();
}

void PADSetSpec(s32 spec);
u16 __OSWirelessPadFixMode : 0x800030E0;
BOOL PADInit(void)
{
    s64 OSGetTime(void);
    void SIRefreshSamplingRate(void);
    s32 chan;

    if (Initialized) {
        return TRUE;
    }
    if (__PADSpec) {
        PADSetSpec(__PADSpec);
    }
    Initialized = TRUE;
    if (__PADFixBits != 0) {
        s64 time = OSGetTime();
        __OSWirelessPadFixMode = (u16)((((time) & 0xffff) + ((time >> 16) & 0xffff) + ((time >> 32) & 0xffff) + ((time >> 48) & 0xffff)) & 0x3fff);
        RecalibrateBits = 0xF0000000;
    }
    for (chan = 0; chan < 4; ++chan) {
        CmdProbeDevice[chan] = (0x4D << 24) | (chan << 22) | ((__OSWirelessPadFixMode & 0x3fff) << 8);
    }
    SIRefreshSamplingRate();
    OSRegisterResetFunction(&ResetFunctionInfo);
    return PADReset(0xF0000000);
}

u32 PADRead(PADStatus *status)
{
    BOOL SIIsChanBusy(s32 chan);
    u32 SIGetStatus(s32 chan);
    u32 SIGetType(s32 chan);
    BOOL SIGetResponse(s32 chan, void *data);
    BOOL SITransfer(s32 chan, void *output, u32 outputBytes, void *input, u32 inputBytes, void *callback, s64 delay);
    void PADOriginUpdateCallback(s32 chan, u32 error, void *context);
    BOOL enabled;
    s32 chan;
    u32 data[2];
    u32 chanBit;
    u32 sr;
    int chanShift;
    u32 motor;

    enabled = OSDisableInterrupts();
    motor = 0;
    for (chan = 0; chan < 4; chan++, status++) {
        chanBit = 0x80000000 >> chan;
        chanShift = 8 * (4 - 1 - chan);
        if (PendingBits & chanBit) {
            PADReset(0);
            status->err = -2;
            memset(status, 0, 10);
            continue;
        }
        if ((ResettingBits & chanBit) || ResettingChan == chan) {
            status->err = -2;
            memset(status, 0, 10);
            continue;
        }
        if (!(EnabledBits & chanBit)) {
            status->err = -1;
            memset(status, 0, 10);
            continue;
        }
        if (SIIsChanBusy(chan)) {
            status->err = -3;
            memset(status, 0, 10);
            continue;
        }
        sr = SIGetStatus(chan);
        if (sr & 8) {
            SIGetResponse(chan, data);
            if (WaitingBits & chanBit) {
                status->err = 0;
                memset(status, 0, 10);
                if (!(CheckingBits & chanBit)) {
                    CheckingBits |= chanBit;
                    SIGetTypeAsync(chan, PADReceiveCheckCallback);
                }
                continue;
            }
            PADDisable(chan);
            status->err = -1;
            memset(status, 0, 10);
            continue;
        }
        if (!(SIGetType(chan) & 0x20000000)) {
            motor |= chanBit;
        }
        if (!SIGetResponse(chan, data)) {
            status->err = -3;
            memset(status, 0, 10);
            continue;
        }
        if (data[0] & 0x80000000) {
            status->err = -3;
            memset(status, 0, 10);
            continue;
        }
        ((void (*)(s32, void *, u32 *))MakeStatus)(chan, status, data);
        if (status->button & 0x2000) {
            status->err = -3;
            memset(status, 0, 10);
            SITransfer(chan, &CmdReadOrigin, 1, Origin[chan], 10, PADOriginUpdateCallback, 0);
            continue;
        }
        status->err = 0;
        status->button &= ~0x0080;
    }
    OSRestoreInterrupts(enabled);
    return motor;
}

void PADControlMotor(s32 arg0, u32 arg1)
{
  M2C_UNK SISetCommand(s32, s32);
  M2C_UNK SITransferCommands();
  int new_var;
  s32 temp_r31;
  u32 var_r30;
  var_r30 = arg1;
  temp_r31 = OSDisableInterrupts();
  if ((EnabledBits & (0x80000000U >> arg0)) && (!(SIGetType(arg0) & 0x20000000)))
  {
    if ((((u32) Spec) < 2U) && (var_r30 == 2))
    {
      new_var = 0;
      var_r30 = new_var;
    }
    SISetCommand(arg0, (AnalogMode | 0x400000) | (var_r30 & 3));
    SITransferCommands();
  }
  OSRestoreInterrupts(temp_r31);
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

#pragma push
BOOL OnReset(BOOL final)
{
    static BOOL recalibrated = FALSE;
    BOOL sync;

    if (SamplingCallback) {
        PADSetSamplingCallback(NULL);
    }
    if (!final) {
        sync = PADSync();
        if (!recalibrated && sync) {
            recalibrated = PADRecalibrate(0xF0000000);
            return FALSE;
        }
        return sync;
    }
    recalibrated = FALSE;
    return TRUE;
}
#pragma pop

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

BOOL __PADDisableRecalibration(BOOL disable) {
    BOOL enabled;
    BOOL prev;

    enabled = OSDisableInterrupts();
    if (*(u8 *)0x800030E3 & 0x40) {
        prev = TRUE;
    } else {
        prev = FALSE;
    }
    *(u8 *)0x800030E3 &= 0xBF;
    if (disable) {
        *(u8 *)0x800030E3 |= 0x40;
    }
    OSRestoreInterrupts(enabled);
    return prev;
}

