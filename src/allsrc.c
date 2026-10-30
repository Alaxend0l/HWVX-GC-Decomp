#include "common.h"

// Force feedback library (LG devices / virtual devices / effects)

typedef struct EffectCondition {
    u8 unk0[2];
    u8 negSaturation;
    u8 posSaturation;
    s16 negCoefficient;
    s16 posCoefficient;
} EffectCondition;

typedef struct EffectState {
    u8 unk0[0xC];
    s32 position;
    u8 unk10[0x54];
    s32 velocity;
} EffectState;

typedef struct EffectEnvelope {
    s32 attackTime;
    s32 fadeTime;
    u8 attackLevel;
    u8 fadeLevel;
} EffectEnvelope;

typedef struct EffectConstant {
    s16 magnitude;
    u16 angle;
    EffectEnvelope envelope;
} EffectConstant;

typedef struct EffectRamp {
    s16 start;
    s16 end;
    u16 angle;
} EffectRamp;

typedef struct EffectPeriodic {
    u8 magnitude;
    u8 unk1;
    u16 angle;
    u16 period;
    u16 phase;
    s16 offset;
    u8 unkA[2];
    EffectEnvelope envelope;
} EffectPeriodic;

typedef struct Effect {
    u8 type;
    u8 unk1[3];
    u32 duration;
    u32 startDelay;
    union {
        EffectConstant constant;
        EffectRamp ramp;
        EffectPeriodic periodic;
        EffectCondition condition;
        u8 raw[0x18];
    } params;
    EffectState *state;
    u32 flags;
    u32 elapsed;
    u32 delay;
    s32 force;
    union {
        s32 deadbandLow;
        u32 phase;
    };
    s32 deadbandHigh;
} Effect;

typedef struct SimThread {
    struct VDevice *owner;
    u32 lastTick;
    s32 pos;
    s32 posScaled;
    s32 weights[10];
    s32 history[10];
    s32 historyIdx;
    s32 accel;
    s32 deltaPos[16];
    s32 deltaTime[16];
    s32 deltaIdx;
    s32 lastForce;
} SimThread;

typedef struct VDevice {
    SimThread sim;
    s32 *unkF0;
    Effect *activeEffects[64];
    Effect effects[64];
    s32 unk11F4;
} VDevice;

typedef struct SISteeringStatus {
    u16 button;
    u8 misc;
    s8 steering;
    u8 gas;
    u8 brake;
    u8 left;
    u8 right;
    u8 err;
    s8 unk9;
} SISteeringStatus;

typedef struct LGDevice {
    u8 unk0[4];
    u8 opened : 1;
    u8 started : 1;
    u8 unk4_pad : 6;
    s32 unk8;
    SISteeringStatus status;
    u8 unk16[2];
    SISteeringStatus calibMin;
    SISteeringStatus calibMax;
    SISteeringStatus calibDeadzone;
    SISteeringStatus prev;
    u8 unk40[4];
    u16 curButton;
    u8 curMisc;
    s8 curSteering;
    u8 unk48[5];
    s8 unk4D;
    u8 unk4E[2];
    VDevice vdev;
} LGDevice;

LGDevice g_lgDevices[4];
s8 g_iGammaLookup[129];

s32 g_lgInitialized;
s32 g_bGammaInitialized;

s16 g_iQuarterSineTable[0x40];
s16 g_iRampUpTable[0x40];
s32 g_bWaveTablesInitialized;

static inline s32 Effect_MulDiv(s32 a, s32 b, s32 c) {
    s32 half = c / 2;

    if (a < 0) {
        half = -half;
    }
    a *= b;
    a += half;
    return a / c;
}

static inline s32 Effect_WaveLookup(s16 *table, u32 a) {
    if (a < 0x40) {
        return table[a];
    } else if (a < 0x80) {
        return table[0x7F - a];
    } else if (a < 0xC0) {
        return -table[a - 0x80];
    } else {
        return -table[0xFF - a];
    }
}

void SimThread_Init(VDevice *dev, VDevice *owner);

static inline void SimThread_SetWeights(SimThread *t, const s32 *weights) {
    s32 i;

    for (i = 0; i < 10; i++) {
        t->weights[i] = weights[i];
        t->history[i] = 0;
    }
    t->historyIdx = 0;
}

static inline s32 SimThread_AddDelta(SimThread *t, s32 dPos, s32 dt) {
    s32 i;
    s32 n;
    s32 sumP;
    u32 sumT;

    t->deltaIdx--;
    if (t->deltaIdx < 0) {
        t->deltaIdx = 15;
    }
    t->deltaPos[t->deltaIdx] = dPos;
    t->deltaTime[t->deltaIdx] = dt;
    i = t->deltaIdx;
    for (n = 0, sumT = 0, sumP = 0; n < 16 && sumT < 16; n++) {
        sumP += t->deltaPos[i];
        sumT += t->deltaTime[i++];
        if (i == 16) {
            i = 0;
        }
    }
    if (sumT > 16) {
        s32 over;
        s32 half;
        s32 lastT;
        s32 lastP;
        if (--i < 0) {
            i = 15;
        }
        over = sumT - 16;
        lastT = t->deltaTime[i];
        lastP = t->deltaPos[i];
        half = lastT / 2;
        if (over < 0) {
            half = -half;
        }
        sumP -= (over * lastP + half) / lastT;
    }
    return sumP;
}

static inline u32 SimThread_Sample(SimThread *t) {
    u32 OSGetTick(void);
    u32 now;
    u32 last;
    s32 oldPos;
    s32 n;
    s32 accel;

    now = OSGetTick() / ((*(u32 *)0x800000F8 / 4) / 1000);
    last = t->lastTick;
    t->lastTick = now;
    oldPos = t->pos;
    t->pos = ((LGDevice *)t->owner->unkF0)->curSteering;
    t->posScaled = t->pos << 12;
    t->history[t->historyIdx++] = SimThread_AddDelta(t, t->pos - oldPos, now - last);
    if (t->historyIdx > 9) {
        t->historyIdx = 0;
    }
    accel = 0;
    for (n = 9; n >= 0; n--) {
        accel += t->history[t->historyIdx++] * t->weights[n];
        if (t->historyIdx > 9) {
            t->historyIdx = 0;
        }
    }
    t->accel = accel;
    return now - last;
}

void VDevice_RecalcGammaTable(void);
double pow(double x, double y);

s32 VDevice_Shutdown(VDevice *dev);
s32 VDevice_StartEffect(VDevice *dev, u32 handle);
u32 VDevice_GetFreeEffect(VDevice *dev);
void Effect_Init(Effect *effect);
s32 Effect_UpdateEffect(Effect *effect, u8 *desc);

#pragma push
#pragma optimize_for_size on
#pragma push
#pragma optimize_for_size on
void SteeringResetCallback(s32 index, s32 result)
{
    if (result != 0)
        g_lgDevices[index].unk4D = -1;
}

#pragma dont_inline on

s8 AutoCalibrateAxisSigned(s8 *min, s8 *max, s8 value, s8 deadzone, s8 *out)
{
    s32 lo = *min;
    s32 hi = *max;
    s32 range;
    s32 result;

    if (value < lo)
    {
        *min = value;
        lo = value;
    }
    if (value > hi)
    {
        hi = value;
        *max = value;
    }
    range = hi - lo - deadzone * 2;
    if (range == 0)
        range = 1;
    result = (value - (lo + deadzone)) * 255 / range - 127;
    if (result < -128)
        result = -128;
    if (result > 127)
        result = 127;
    *out = result;
    return result;
}

#pragma dont_inline off

u8 AutoCalibrateAxisUnsigned(u8 *min, u8 *max, u8 value, u8 deadzone, u8 *out)
{
    s32 lo = *min;
    s32 hi = *max;
    s32 range;
    s32 result;

    if (value < lo)
    {
        *min = value;
        lo = value;
    }
    if (value > hi)
    {
        hi = value;
        *max = value;
    }
    range = hi - lo - deadzone * 2;
    if (range == 0)
        range = 1;
    result = (value - (lo + deadzone)) * 255 / range;
    if (result < 0)
        result = 0;
    if (result > 255)
        result = 255;
    *out = result;
    return result;
}

#pragma push
#pragma optimize_for_size on
#pragma use_lmw_stmw on
#pragma dont_inline on
void HandlePedals(u8 *cur, SISteeringStatus *status, SISteeringStatus *calib)
{
    s32 combined;

    if ((status->misc ^ calib[3].misc) & 8)
    {
        *(s32 *)&calib[4] = 0x40;
        calib[0].gas = 0x50;
        calib[0].brake = 0x50;
        combined = (calib[0].gas - calib[0].brake) / 2;
        if (combined < -128)
        {
            combined = -128;
        }
        else if (combined > 127)
        {
            combined = 127;
        }
        calib[0].left = combined;
        calib[1].gas = 0xB0;
        calib[1].brake = 0xB0;
        combined = (calib[1].gas - calib[1].brake) / 2;
        if (combined < -128)
        {
            combined = -128;
        }
        else if (combined > 127)
        {
            combined = 127;
        }
        calib[1].left = combined;
        calib[2].gas = 10;
        calib[2].brake = 10;
    }
    if (*(s32 *)&calib[4] > 0)
    {
        (*(s32 *)&calib[4])--;
        cur[5] = 0;
        cur[4] = 0;
        cur[6] = 0;
    }
    else if (status->misc & 8)
    {
        AutoCalibrateAxisUnsigned(&calib[0].gas, &calib[1].gas, status->gas, calib[2].gas, &cur[4]);
        AutoCalibrateAxisUnsigned(&calib[0].brake, &calib[1].brake, status->brake, calib[2].brake, &cur[5]);
        combined = (cur[4] - cur[5]) / 2;
        if (combined < -128)
        {
            combined = -128;
        }
        else if (combined > 127)
        {
            combined = 127;
        }
        cur[6] = combined;
    }
    else
    {
        cur[5] = 0;
        cur[4] = 0;
        cur[6] = 0;
    }
}
#pragma pop

#pragma push
#pragma optimize_for_size on
#pragma use_lmw_stmw on
#pragma dont_inline on
void HandleTriggers(u8 *cur, SISteeringStatus *status, SISteeringStatus *calib)
{
    s32 diff;

    if ((status->button ^ calib[3].button) & 0x40)
    {
        diff = status->left - calib[2].right;
        calib[1].right = 0 > diff ? 0 : diff;
        cur[7] = 0xFF;
    }
    else if (status->button & 0x40)
    {
        cur[7] = 0xFF;
    }
    else
    {
        AutoCalibrateAxisUnsigned(&calib[0].right, &calib[1].right, status->left, calib[2].right, &cur[7]);
    }
    if ((status->button ^ calib[3].button) & 0x20)
    {
        diff = status->right - calib[2].err;
        calib[1].err = 0 > diff ? 0 : diff;
        cur[8] = 0xFF;
    }
    else if (status->button & 0x20)
    {
        cur[8] = 0xFF;
    }
    else
    {
        AutoCalibrateAxisUnsigned(&calib[0].err, &calib[1].err, status->right, calib[2].err, &cur[8]);
    }
}
#pragma pop
#pragma pop

#pragma push
#pragma optimize_for_size on
#pragma use_lmw_stmw on
void SteeringSamplingCallback(void)
{
    void SIReadSteering(s32 chan, SISteeringStatus *status);
    void HandlePedals(void *cur, SISteeringStatus *status, SISteeringStatus *calib);
    void HandleTriggers(void *cur, SISteeringStatus *status, SISteeringStatus *calib);
    s32 SimThread_Step(VDevice *dev);
    s32 i;
    LGDevice *device;
    SISteeringStatus *status;

    for (i = 0; i < 4; i++)
    {
        device = &g_lgDevices[i];
        if (device->opened)
        {
            status = &device->status;
            SIReadSteering(i, status);
            switch ((s8)status->err)
            {
            case -1:
                device->unk4D = -1;
                device->started = 1;
                break;
            case 0:
                AutoCalibrateAxisSigned(&device->calibMin.steering, &device->calibMax.steering,
                                        status->steering, device->calibDeadzone.steering,
                                        &device->curSteering);
                HandlePedals(&device->curButton, status, &device->calibMin);
                HandleTriggers(&device->curButton, status, &device->calibMin);
                device->curButton = status->button;
                device->curMisc = status->misc;
                device->prev = *status;
                device->unk4D = 0;
                SimThread_Step(&device->vdev);
                break;
            }
        }
    }
}

void InitDevice(LGDevice *device, s32 index)
{
    s32 combined;

    *(s32 *)device->unk0 = index;
    device->opened = 0;
    device->started = 0;
    device->unk8 = -1;
    device->status.button = 0;
    device->status.misc = 0;
    device->status.steering = 0;
    device->status.gas = 0x80;
    device->status.brake = 0x80;
    device->status.left = 0x80;
    device->status.right = 0x80;
    *(s8 *)&device->status.err = -1;
    device->calibMin.steering = -0x40;
    device->calibMin.right = 0x50;
    *(u8 *)&device->calibMin.err = 0x50;
    device->calibMax.steering = 0x40;
    device->calibMax.right = 0xB0;
    *(u8 *)&device->calibMax.err = 0xB0;
    device->calibDeadzone.steering = 6;
    device->calibDeadzone.right = 0xC;
    *(u8 *)&device->calibDeadzone.err = 0xC;
    device->calibMin.gas = 0x50;
    device->calibMin.brake = 0x50;
    combined = (device->calibMin.gas - device->calibMin.brake) / 2;
    if (combined < -128)
    {
        combined = -128;
    }
    else if (combined > 127)
    {
        combined = 127;
    }
    device->calibMin.left = combined;
    device->calibMax.gas = 0xB0;
    device->calibMax.brake = 0xB0;
    combined = (device->calibMax.gas - device->calibMax.brake) / 2;
    if (combined < -128)
    {
        combined = -128;
    }
    else if (combined > 127)
    {
        combined = 127;
    }
    device->calibMax.left = combined;
    device->calibDeadzone.gas = 10;
    device->calibDeadzone.brake = 10;
    AutoCalibrateAxisSigned(&device->calibMin.steering, &device->calibMax.steering, device->status.steering,
                            device->calibDeadzone.steering, &device->curSteering);
    HandlePedals((u8 *)&device->curButton, &device->status, &device->calibMin);
    HandleTriggers((u8 *)&device->curButton, &device->status, &device->calibMin);
    device->curButton = device->status.button;
    device->curMisc = device->status.misc;
    device->prev = device->status;
    device->unk4D = 0;
}

#pragma push
#pragma optimize_for_size on
#pragma use_lmw_stmw on
void LGInit(void)
{
    void VDevice_Init(VDevice *dev);
    void InitDevice(LGDevice *device, s32 index);
    void SIResetSteeringAsync(s32 chan, void *callback);
    void SIReadSteering(s32 chan, void *status);
    void SISetSteeringSamplingCallback(void *callback);
    void SteeringSamplingCallback(void);
    LGDevice *device;
    s32 i;

    if (g_lgInitialized == 0)
    {
        for (i = 0; i < 4; i++)
        {
            device = &g_lgDevices[i];
            VDevice_Init(&device->vdev);
            InitDevice(device, i);
            SIResetSteeringAsync(i, SteeringResetCallback);
            SIReadSteering(i, &device->status);
        }
        SISetSteeringSamplingCallback(SteeringSamplingCallback);
        g_lgInitialized = 1;
    }
}

s32 LGOpen(s32 chan, LGDevice **out)
{
    void InitDevice(LGDevice *device, s32 index);
    s32 SIResetSteeringAsync(s32 chan, void *callback);
    void SIReadSteering(s32 chan, SISteeringStatus *status);
    void HandlePedals(u8 *cur, SISteeringStatus *status, SISteeringStatus *calib);
    void HandleTriggers(u8 *cur, SISteeringStatus *status, SISteeringStatus *calib);
    s32 VDevice_Initialize(VDevice *dev, s32 owner);
    s32 LGClose(LGDevice *device);
    void AutoCalibrateAxisSigned(s8 *min, s8 *max, s8 value, s8 deadzone, s8 *out);
    LGDevice *device;
    SISteeringStatus *calib;
    SISteeringStatus *status;
    u8 *cur;

    if (chan >= 4 || chan < 0)
    {
        return -2;
    }
    if (out == NULL)
    {
        return -2;
    }
    device = &g_lgDevices[chan];
    if (device->started)
    {
        LGClose(device);
    }
    if (device->opened)
    {
        *out = (LGDevice *)device->unk8;
        return -3;
    }
    if (SIResetSteeringAsync(chan, SteeringResetCallback) != -1)
    {
        InitDevice(device, chan);
        status = &device->status;
        SIReadSteering(chan, status);
        switch ((s8)g_lgDevices[chan].status.err)
        {
        case -1:
            g_lgDevices[chan].unk4D = -1;
            return -4;
        case 0:
            calib = &device->calibMin;
            cur = (u8 *)&device->curButton;
            AutoCalibrateAxisSigned(&calib->steering, &calib[1].steering,
                                    g_lgDevices[chan].status.steering,
                                    g_lgDevices[chan].calibDeadzone.steering, (s8 *)cur + 3);
            HandlePedals(cur, status, calib);
            HandleTriggers(cur, status, calib);
            g_lgDevices[chan].curButton = g_lgDevices[chan].status.button;
            g_lgDevices[chan].curMisc = g_lgDevices[chan].status.misc;
            g_lgDevices[chan].prev = g_lgDevices[chan].status;
            g_lgDevices[chan].unk4D = 0;
        }
        g_lgDevices[chan].unk4D = 0;
        *out = device;
        *(s32 *)&device->unk0[0] = chan;
        device->unk8 = (s32)*out;
        VDevice_Initialize(&device->vdev, (s32)device);
        device->opened = 1;
        return 0;
    }
    return -4;
}

#pragma pop

s32 LGClose(LGDevice *device)
{
    if (device == NULL || (u32)device == 0xFFFFFFFF)
        return -2;
    if (!device->opened)
        return -2;
    device->opened = 0;
    device->started = 0;
    VDevice_Shutdown(&device->vdev);
    device->unk8 = -1;
    return 0;
}

s32 LGGetDeviceProperty(LGDevice *device, u32 property, s32 *value) {
    s32 result = -2;

    if (device == NULL || (u32)device == 0xFFFFFFFF) {
        return result;
    }
    if (value == NULL) {
        return result;
    }
    if (property != 3) {
        return result;
    }
    if (!device->opened) {
        return result;
    }
    if (device->started) {
        LGClose(device);
        return result;
    }
    *value = device->vdev.unk11F4;
    return 0;
}

s32 LGSetDeviceProperty(LGDevice *device, u32 property, s32 value) {
    if (device == NULL || (u32)device == 0xFFFFFFFF) {
        return -2;
    }
    if (value < 0 || value > 0xFF) {
        return -2;
    }
    if (property != 3) {
        return -2;
    }
    if (!device->opened) {
        return -2;
    }
    if (device->started) {
        if (device != NULL && (u32)device != 0xFFFFFFFF && device->opened) {
            device->opened = 0;
            device->started = 0;
            VDevice_Shutdown(&device->vdev);
            device->unk8 = -1;
        }
        return -2;
    }
    device->vdev.unk11F4 = value;
    return 0;
}

#pragma use_lmw_stmw on
s32 LGRead(SISteeringStatus *status)
{
    s32 i;
    LGDevice *device;

    if (status == NULL)
        return -2;

    for (i = 0; i < 4; i++)
    {
        device = &g_lgDevices[i];
        if (device->started)
        {
            if (device != NULL && (u32)device != 0xFFFFFFFF && device->opened)
            {
                device->opened = 0;
                device->started = 0;
                VDevice_Shutdown(&device->vdev);
                device->unk8 = -1;
            }
        }
        if (device->opened)
        {
            status[i] = *(SISteeringStatus *)&device->curButton;
        }
        else
        {
            status[i].unk9 = -1;
        }
    }
    return 0;
}

s32 VDevice_DownloadEffect(VDevice *dev, u32 *handle, u8 *desc);
s32 LGDownloadForceEffect(LGDevice *device, u32 *handle, u8 *desc)
{
    if (device == NULL || device == (LGDevice *)0xFFFFFFFF)
        return -2;

    if (!device->opened)
        return -2;

    if (handle == NULL)
        return -2;

    if (desc == NULL)
        return -2;

    if (device->started) {
        LGClose(device);
        return -2;
    }

    return VDevice_DownloadEffect(&device->vdev, handle, desc);
}

s32 LGUpdateForceEffect(u32 handle, u8 *desc)
{
    s32 VDevice_UpdateEffect(VDevice *dev, u32 handle, u8 *desc);
    u32 index = handle >> 16;
    LGDevice *device;

    if (index >= 4)
        return -2;

    device = &g_lgDevices[index];
    if (!device->opened)
        return -2;

    if (desc == NULL)
        return -2;

    if (device->started) {
        if (device != NULL && (u32)device != 0xFFFFFFFF && device->opened) {
            device->opened = 0;
            device->started = 0;
            VDevice_Shutdown(&device->vdev);
            device->unk8 = -1;
        }
        return -2;
    }

    return VDevice_UpdateEffect(&device->vdev, handle, desc);
}

#pragma pop

s32 LGStartForceEffect(u32 handle)
{
    u32 index = handle >> 16;
    LGDevice *device;

    if (index >= 4)
        return -2;

    device = &g_lgDevices[index];
    if (!device->opened)
        return -2;

    if (device->started) {
        if (device != NULL && (u32)device != 0xFFFFFFFF && device->opened) {
            device->opened = 0;
            device->started = 0;
            VDevice_Shutdown(&device->vdev);
            device->unk8 = -1;
        }
        return -2;
    }

    return VDevice_StartEffect(&device->vdev, handle);
}

s32 VDevice_DestroyEffect(VDevice *dev, u32 handle);
s32 LGDestroyForceEffect(u32 handle)
{
    u32 index = handle >> 16;
    LGDevice *device;

    if (index >= 4)
        return -2;

    device = &g_lgDevices[index];
    if (!device->opened)
        return -2;

    if (device->started) {
        if (device != NULL && (u32)device != 0xFFFFFFFF && device->opened) {
            device->opened = 0;
            device->started = 0;
            VDevice_Shutdown(&device->vdev);
            device->unk8 = -1;
        }
        return -2;
    }

    return VDevice_DestroyEffect(&device->vdev, handle);
}

void zero_mem_aligned(void *dst, u32 n) {
    u32 i;
    union {
        u8 *cpd;
        u32 *lpd;
    } dstu;

    dstu.cpd = ((u8 *)dst) - 1;

    if (n >= 32) {
        dstu.lpd = ((u32 *)dst) - 1;

        i = n >> 5;
        if (i) {
            do {
                *++dstu.lpd = 0;
                *++dstu.lpd = 0;
                *++dstu.lpd = 0;
                *++dstu.lpd = 0;
                *++dstu.lpd = 0;
                *++dstu.lpd = 0;
                *++dstu.lpd = 0;
                *++dstu.lpd = 0;
            } while (--i);
        }

        i = (n & 31) >> 2;
        if (i) {
            do
                *++dstu.lpd = 0;
            while (--i);
        }

        dstu.cpd = ((u8 *)(dstu.lpd + 1)) - 1;
        n &= 3;
    }

    if (n) {
        do
            *++dstu.cpd = 0;
        while (--n);
    }
}

void VDevice_Init(VDevice *dev)
{
    s32 i;

    SimThread_Init(dev, dev);
    dev->unk11F4 = 0xFF;
    for (i = 0; i < 64; i++) {
        dev->activeEffects[i] = NULL;
    }
    if (g_bGammaInitialized == 0) {
        VDevice_RecalcGammaTable();
        g_bGammaInitialized = 1;
    }
}

#pragma push
#pragma optimize_for_size on
#pragma use_lmw_stmw on
void VDevice_RecalcGammaTable(void) {
    s32 i;

    for (i = 0; i < 129; i++) {
        g_iGammaLookup[i] = VDevice_GammaCurve(i);
    }
}
#pragma pop

#pragma push
#pragma use_lmw_stmw on
s32 VDevice_DownloadEffect(VDevice *dev, u32 *handle, u8 *desc)
{
    s32 ret = 0;
    u32 index;
    Effect *effect;

    *handle = VDevice_GetFreeEffect(dev);
    if (*handle == 0xFFFFFFFF) {
        ret = -5;
    } else {
        index = *handle & 0x3F;
        effect = &dev->effects[index];
        switch (desc[0]) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
            Effect_Init(effect);
            effect->state = (EffectState *)dev;
            Effect_UpdateEffect(effect, desc);
            break;
        default:
            ret = -2;
            *handle = -1;
            break;
        }
        if (effect != NULL) {
            dev->activeEffects[index] = effect;
        }
    }
    return ret;
}

s32 VDevice_UpdateEffect(VDevice *dev, u32 handle, u8 *desc)
{
    s32 ret = -2;
    Effect *effect = dev->activeEffects[handle & 0x3F];

    if (effect != NULL) {
        ret = Effect_UpdateEffect(effect, desc);
    }
    return ret;
}

#pragma push
#pragma optimize_for_size on
s32 Effect_StartEffect(Effect *effect);
s32 VDevice_DestroyEffect(VDevice *dev, u32 handle)
{
    s32 ret = -2;
    u32 index = handle & 0x3F;

    if (dev->activeEffects[index] != NULL)
    {
        dev->activeEffects[index] = NULL;
        ret = 0;
    }
    return ret;
}

#pragma push
#pragma optimize_for_size on
s32 VDevice_StartEffect(VDevice *dev, u32 handle)
{
    s32 ret = -2;
    Effect *effect = dev->activeEffects[handle & 0x3F];

    if (effect != NULL)
    {
        ret = Effect_StartEffect(effect);
    }
    return ret;
}

#pragma push
#pragma optimize_for_size on
s32 VDevice_Initialize(VDevice *dev, s32 *index)
{
    s32 i;

    dev->unk11F4 = 0xFF;
    for (i = 0; i < 64; i++)
    {
        dev->activeEffects[i] = NULL;
    }
    dev->unkF0 = index;
    return 0;
}

#pragma push
#pragma optimize_for_size on
s32 VDevice_Shutdown(VDevice *dev)
{
    s32 i;

    for (i = 0; i < 64; i++)
    {
        if (dev->activeEffects[i] != NULL)
        {
            dev->activeEffects[i] = NULL;
        }
    }
    return 0;
}

#pragma push
#pragma optimize_for_size on
u32 VDevice_GetFreeEffect(VDevice *dev)
{
    static u32 jumbleeffectid;
    static u32 createcount;
    u32 handle = 0xFFFFFFFF;
    s32 i;
    u32 index;

    for (i = 0; i < 64; i++)
    {
        index = (i + jumbleeffectid) & 0x3F;
        if (dev->activeEffects[index] == NULL)
        {
            jumbleeffectid = index + 1;
            jumbleeffectid &= 0x3F;
            handle = (*dev->unkF0 << 16) | ((createcount & 0xFF) << 8) | (index & 0x3F);
            break;
        }
    }
    return handle;
}

#pragma push
#pragma optimize_for_size on
void Effect_Init(Effect *effect)
{
    void zero_mem_aligned(void *dst, s32 count);
    double sin(double);
    s32 i;

    zero_mem_aligned(effect, 0x40);
    if (g_bWaveTablesInitialized == 0)
    {
        for (i = 0; i < 0x40; i++)
        {
            g_iQuarterSineTable[i] = 1024.0f * (f32)sin((3.1415927410125732f * i) / 128.0f);
            g_iRampUpTable[i] = (i * 0x400) / 63;
        }
        g_bWaveTablesInitialized = 1;
    }
}

s32 Effect_UpdateEffect(Effect *effect, u8 *desc)
{
Effect *src = (Effect *)desc;
s32 value;
s32 ret = 0;
u16 period;
u8 sat;
effect->type = src->type;
effect->duration = src->duration;
effect->startDelay = src->startDelay;
switch (src->type)
{
case 0:
effect->flags |= 0x14;
value = src->params.constant.magnitude;
value = (value < -0xff) ? -0xff : value;
value = (value > 0xff) ? 0xff : value;
effect->params.constant.magnitude = value;
effect->params.constant.angle = (src->params.constant.angle % 360) * 0x100 / 360;
effect->params.constant.envelope.attackTime = src->params.constant.envelope.attackTime;
effect->params.constant.envelope.fadeTime = src->params.constant.envelope.fadeTime;
value = src->params.constant.envelope.attackLevel;
value = (value > 0xff) ? 0xff : value;
effect->params.constant.envelope.attackLevel = value;
value = src->params.constant.envelope.fadeLevel;
value = (value > 0xff) ? 0xff : value;
effect->params.constant.envelope.fadeLevel = value;
break;
case 1:
effect->flags |= 0x24;
value = src->params.ramp.start;
value = (value < -0xff) ? -0xff : value;
value = (value > 0xff) ? 0xff : value;
effect->params.ramp.start = value;
value = src->params.ramp.end;
value = (value < -0xff) ? -0xff : value;
value = (value > 0xff) ? 0xff : value;
effect->params.ramp.end = value;
effect->params.ramp.angle = (src->params.ramp.angle % 360) * 0x100 / 360;
break;
case 2: case 3: case 4: case 5: case 6:
effect->flags |= 0x44;
value = src->params.periodic.magnitude;
value = (value > 0xff) ? 0xff : value;
effect->params.periodic.magnitude = value;
effect->params.periodic.angle = (src->params.periodic.angle % 360) * 0x100 / 360;
period = src->params.periodic.period;
period = (period < 2) ? 2 : period;
effect->params.periodic.period = period;
effect->params.periodic.phase = (src->params.periodic.phase % 360) * 0x100 / 360;
value = src->params.periodic.offset;
value = (value < -0xff) ? -0xff : value;
value = (value > 0xff) ? 0xff : value;
effect->params.periodic.offset = value;
effect->params.periodic.envelope.attackTime = src->params.periodic.envelope.attackTime;
effect->params.periodic.envelope.fadeTime = src->params.periodic.envelope.fadeTime;
value = src->params.periodic.envelope.attackLevel;
value = (value > 0xff) ? 0xff : value;
effect->params.periodic.envelope.attackLevel = value;
value = src->params.periodic.envelope.fadeLevel;
value = (value > 0xff) ? 0xff : value;
effect->params.periodic.envelope.fadeLevel = value;
if (effect->params.periodic.period < 4) { effect->params.periodic.phase = 0x40; }
break;
case 7: case 8:
effect->flags |= 0x80;
effect->params.condition.unk0[0] = src->params.condition.unk0[0];
effect->params.condition.unk0[1] = src->params.condition.unk0[1];
value = src->params.condition.posCoefficient;
value = (value < -0xff) ? -0xff : value;
value = (value > 0xff) ? 0xff : value;
effect->params.condition.posCoefficient = value;
value = src->params.condition.negCoefficient;
value = (value < -0xff) ? -0xff : value;
value = (value > 0xff) ? 0xff : value;
effect->params.condition.negCoefficient = value;
sat = src->params.condition.negSaturation;
effect->params.condition.negSaturation = (sat >> 1) & 0xff;
sat = src->params.condition.posSaturation;
effect->params.condition.posSaturation = (sat >> 1) & 0xff;
effect->deadbandLow = ((s8)effect->params.condition.unk0[0] - ((u32)effect->params.condition.unk0[1] >> 1)) << 12;
effect->deadbandHigh = ((s8)effect->params.condition.unk0[0] + ((u32)effect->params.condition.unk0[1] >> 1)) << 12;
break;
default:
ret = -2;
break;
}
return ret;
}

#pragma pop
s32 Effect_StartEffect(Effect *effect)
{
    effect->flags |= 1;
    effect->flags &= 0xFFFFFFFD;
    effect->elapsed = 0;
    effect->delay = effect->startDelay;
    if (effect->flags & 0x40)
    {
        effect->phase = effect->params.periodic.phase << 16;
    }
    return 0;
}

s32 Effect_PerformEnvelope(Effect *effect, EffectEnvelope *env, s32 magnitude);
void Effect_PolarToRect(s32 magnitude, s32 angle, s32 *out);
s32 Effect_UpdateSpring(Effect *effect, EffectCondition *cond);
s32 Effect_UpdateDamper(Effect *effect, EffectCondition *cond);
s32 Effect_Update(Effect *effect, u32 dt) {
    s32 active;
    s16 magnitude;
    s32 wave;
    s32 round;
    s32 level;
    u32 a;
    EffectCondition *cond;

    if (effect->flags & 1) {
        if (effect->delay != 0) {
            effect->delay -= dt;
            if ((s32)effect->delay < 0) {
                effect->delay = 0;
            }
        } else {
            effect->elapsed += dt;
            if (effect->duration != 0xFFFFFFFF && effect->duration < effect->elapsed && (effect->flags & 2)) {
                effect->flags &= ~1;
            }
        }
    }
    effect->flags |= 2;

    active = 0;
    if ((effect->flags & 1) && effect->delay == 0) {
        active = 1;
    }
    if (active) {
        if (effect->flags & 0x10) {
            Effect_PolarToRect(Effect_PerformEnvelope(effect, &effect->params.constant.envelope, effect->params.constant.magnitude), effect->params.constant.angle, &effect->force);
        } else if (effect->flags & 0x20) {
            Effect_PolarToRect(effect->params.ramp.start + Effect_MulDiv(effect->params.ramp.end - effect->params.ramp.start, effect->elapsed, effect->duration), effect->params.ramp.angle, &effect->force);
        } else if (effect->flags & 0x40) {
            dt = (dt << 24) / effect->params.periodic.period;
            effect->phase += dt;
            if (effect->phase >= 0x1000000) {
                effect->phase -= effect->phase & 0xFF000000;
            }
            magnitude = Effect_PerformEnvelope(effect, &effect->params.periodic.envelope, effect->params.periodic.magnitude);
            switch (effect->type) {
            case 2:
                wave = Effect_WaveLookup(g_iQuarterSineTable, (effect->phase >> 16) & 0xFF);
                break;
            case 3:
                wave = -0x400;
                if ((effect->phase >> 16) < 0x80) {
                    wave = 0x400;
                }
                break;
            case 4:
                wave = Effect_WaveLookup(g_iRampUpTable, (effect->phase >> 16) & 0xFF);
                break;
            case 5:
                a = (effect->phase >> 16) & 0xFF;
                wave = g_iRampUpTable[a >> 2];
                break;
            case 6:
                a = (effect->phase >> 16) & 0xFF;
                wave = g_iRampUpTable[(0xFF - a) >> 2];
                break;
            default:
                wave = 0;
                break;
            }
            round = 0x200;
            if (wave < 0) {
                round = -0x200;
            }
            level = wave * magnitude;
            level += round;
            Effect_PolarToRect(level / 0x400 + effect->params.periodic.offset, effect->params.periodic.angle, &effect->force);
        } else if (effect->flags & 0x80) {
            cond = &effect->params.condition;
            switch (effect->type) {
            case 7:
                effect->force = Effect_UpdateSpring(effect, cond);
                break;
            case 8:
                effect->force = Effect_UpdateDamper(effect, cond);
                if (effect->force == 0) {
                    effect->force++;
                }
                break;
            default:
                effect->force = 0;
                break;
            }
        } else {
            return 0;
        }
    } else {
        effect->force = 0;
    }
    return 1;
}

s32 Effect_PerformEnvelope(Effect *effect, EffectEnvelope *env, s32 magnitude) {
    s32 start;
    s32 level;
    s32 negative;
    u32 elapsed;
    u32 fadeStart;

    elapsed = effect->elapsed;
    negative = magnitude < 0;
    start = magnitude < 0 ? -magnitude : magnitude;
    level = start;
    if (elapsed < env->attackTime) {
        level = env->attackLevel + Effect_MulDiv(elapsed, start - env->attackLevel, env->attackTime);
    } else if (effect->duration != 0xFFFFFFFF) {
        fadeStart = effect->duration - env->fadeTime;
        if (elapsed > fadeStart) {
            if (elapsed < effect->duration) {
                level = start + Effect_MulDiv(elapsed - fadeStart, env->fadeLevel - start, env->fadeTime);
            } else {
                level = env->fadeLevel;
            }
        }
    }
    if (negative) {
        level = -level;
    }
    return level;
}

#pragma push
#pragma optimize_for_size on
s32 Effect_UpdateSpring(Effect *effect, EffectCondition *cond)
{
    s32 force;
    s32 saturation;
    s32 delta;

    if (effect->state->position < effect->deadbandLow)
    {
        saturation = cond->negSaturation;
        delta = effect->state->position - effect->deadbandLow;
        force = delta * cond->negCoefficient / 0x20000;
        if (-saturation > force)
        {
            force = -saturation;
        }
        if (saturation < force)
        {
            force = saturation;
        }
        return -force;
    }

    if (effect->state->position > effect->deadbandHigh)
    {
        saturation = cond->posSaturation;
        delta = effect->state->position - effect->deadbandHigh;
        force = delta * cond->posCoefficient / 0x20000;
        if (-saturation > force)
        {
            force = -saturation;
        }
        if (saturation < force)
        {
            force = saturation;
        }
        return -force;
    }
    return 0;
}
#pragma pop

#pragma push
#pragma optimize_for_size on
s32 Effect_UpdateDamper(Effect *effect, EffectCondition *cond)
{
    s32 velocity = effect->state->velocity;
    s32 coefficient;
    s32 saturation;
    s32 force;

    if (velocity < 0)
    {
        coefficient = -cond->negCoefficient;
        saturation = cond->negSaturation;
    }
    else
    {
        coefficient = -cond->posCoefficient;
        saturation = cond->posSaturation;
    }
    velocity *= coefficient;
    force = (velocity * 20) / 0xFF000;
    if (-saturation > force)
    {
        force = -saturation;
    }
    if (saturation < force)
    {
        force = saturation;
    }
    return force;
}

void SimThread_Init(VDevice *dev, VDevice *owner) {
    static f32 a[10] = {
        0.0995392948f, 0.0998378024f, 0.1000645086f, 0.1002266929f, 0.1003241092f,
        0.1003241092f, 0.1002266929f, 0.1000645086f, 0.0998378024f, 0.0995392948f,
    };
    static s32 ia[10];
    SimThread *t = &dev->sim;
    s32 w[1] = {0x1000};
    s32 i;
    s32 j;

    t->lastTick = 0;
    t->historyIdx = 0;
    t->deltaIdx = 0;
    for (i = 0; i < 16; i++) {
        t->deltaPos[i] = 0;
        t->deltaTime[i] = 0;
    }
    t->owner = owner;
    t->lastForce = 0;
    t->posScaled = 0;
    t->pos = 0;
    t->accel = 0;
    t->lastTick = OSGetTick() / ((*(u32 *)0x800000F8 / 4) / 1000);
    SimThread_SetWeights(t, w);
    for (j = 0; j < 10; j++) {
        ia[j] = 4096.0f * a[j];
    }
    SimThread_SetWeights(t, ia);
}

s32 SimThread_Step(VDevice *dev) {
    s32 SIControlSteering(s32 chan, s32 cmd, s32 force);
    SimThread *t = &dev->sim;
    s32 sumA = 0;
    s32 sumB = 0;
    u32 dt;
    Effect *e;
    u32 k;
    s32 f;
    s32 flag;
    s32 gain;
    s32 force;

    dt = SimThread_Sample(t);
    for (k = 0; k < 64; k++) {
        e = t->owner->activeEffects[k];
        if (e != NULL && (e->flags & 1)) {
            Effect_Update(e, dt);
            if ((e->flags & 1) && e->delay == 0) {
                f = e->force;
                f = f < -127 ? -127 : f;
                f = f > 127 ? 127 : f;
                flag = e->flags & 4;
            } else {
                f = 0;
                flag = 0;
            }
            if (flag) {
                sumA += f;
            } else {
                sumB += f;
            }
        }
    }
    gain = t->owner->unk11F4;
    f = sumA > 128 ? 128 : sumA;
    f = f < -128 ? -128 : f;
    if (f < 0) {
        f = -(u8)g_iGammaLookup[-f];
    } else {
        f = (u8)g_iGammaLookup[f];
    }
    if (gain < 255) {
        f = (f * gain + (f < 0 ? -127 : 127)) / 255;
        sumB = (sumB * gain + (sumB < 0 ? -127 : 127)) / 255;
    }
    force = f + sumB;
    force = force > 128 ? 128 : force;
    force = force < -128 ? -128 : force;
    if (t->lastForce != force) {
        t->lastForce = force;
        if (force == 0) {
            SIControlSteering(*t->owner->unkF0, 0x400, 0);
        } else {
            SIControlSteering(*t->owner->unkF0, 0x600, force);
        }
    }
    return force;
}

#pragma pop

#pragma pop

