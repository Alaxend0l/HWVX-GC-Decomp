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

s32 g_lgInitialized;
s32 g_bGammaInitialized;

s16 g_iQuarterSineTable[0x40];
s16 g_iRampUpTable[0x40];
s32 g_bWaveTablesInitialized;

void SimThread_Init(VDevice *dev, VDevice *owner);
void VDevice_RecalcGammaTable(void);
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
    s32 force;
    s32 coefficient;
    s32 saturation;

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
    force = velocity * coefficient * 20 / 0xFF000;
    if (-saturation > force)
    {
        force = -saturation;
    }
    if (force < saturation)
    {
        force = saturation;
    }
    return force;
}

#pragma pop

#pragma pop

