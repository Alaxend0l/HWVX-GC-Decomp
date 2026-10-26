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
    s32 unkF0;
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
    s8 err;
    u8 unk9;
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
#pragma pop

#pragma pop

