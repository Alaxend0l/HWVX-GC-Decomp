#include "common.h"

typedef struct Vector4 { f32 x; f32 y; f32 z; f32 w; } Vector4;


void CarA_Release(u8 *car) {
    typedef struct CarNavPath {
        u8 pad[0x568];
        u8 *path[7];
    } CarNavPath;
    u32 GameSettings_GetCodeVariable(s32, s32);
    u8 *NavHolder_GetNavPoint(s32 index);
    void Trigger_VehicleKilled(void *car);
    void OConvexHullHolder_Release(void *hull);
    void SoundSource_Stop(s32 index);
    void MotionState_Release(void *state);
    void Thing_Release(void *thing);
    u16 i;

    if (GameSettings_GetCodeVariable(0x12D, 0) >= 3 && *(u8 **)(car + 0x94) != NULL) {
        *(s16 *)(*(u8 **)(car + 0x94) + 0xAC) = *(s32 *)(car + 0x3B0);
        for (i = 0; i < 6; i++) {
            if (((CarNavPath *)car)->path[i] == NULL || ((CarNavPath *)car)->path[i + 1] == NULL) {
                break;
            }
            if (NavHolder_GetNavPoint(*(s32 *)(car + 0x3B0)) == ((CarNavPath *)car)->path[i]) {
                *(s16 *)(*(u8 **)(car + 0x94) + 0xAE) = *(s32 *)(((u8 **)(car + 0x568))[i + 1] + 0x28);
                break;
            }
        }
    }
    Trigger_VehicleKilled(car);
    OConvexHullHolder_Release(*(void **)(car + 0x370));
    *(void **)(car + 0x370) = NULL;
    if (*(u32 *)(car + 0x4F0) != 0xFFFFFFFF) {
        SoundSource_Stop(*(s32 *)(car + 0x4F0));
    }
    if (*(u32 *)(car + 0x4F4) != 0xFFFFFFFF) {
        SoundSource_Stop(*(s32 *)(car + 0x4F4));
    }
    if (*(u32 *)(car + 0x4F8) != 0xFFFFFFFF) {
        SoundSource_Stop(*(s32 *)(car + 0x4F8));
    }
    *(s32 *)(car + 0x4F0) = -1;
    *(s32 *)(car + 0x4F4) = -1;
    *(s32 *)(car + 0x4F8) = -1;
    MotionState_Release(car + 0x330);
    Thing_Release(car);
}

void CarA_GetInverseLocalInertiaTensor(void *arg0, void *arg1) {
    M2C_FIELD(arg1, f32 *, 0) = (f32) M2C_FIELD(arg0, f32 *, 0x3C0);
    M2C_FIELD(arg1, f32 *, 4) = (f32) M2C_FIELD(arg0, f32 *, 0x3D4);
    M2C_FIELD(arg1, f32 *, 8) = (f32) M2C_FIELD(arg0, f32 *, 0x3E8);
}

f32 CarA_Transmission_GetTorqueAtWheels(u8 *car)
{
    u8 *trans;
    f32 scale;
    f32 rpm;
    f32 d;
    f32 c;
    f32 b;
    f32 a;

    a = *(f32 *)(car + 0x4C0);
    b = *(f32 *)(car + 0x4C4);
    c = *(f32 *)(car + 0x4C8);
    d = *(f32 *)(car + 0x4CC);
    rpm = *(f32 *)(car + 0x4E4);
    scale = 1.0f;
    if (*(s32 *)(car + 0x470) < 10000)
    {
        scale = 0.5f;
        rpm *= scale;
    }
    else if (*(u32 *)(car + 0x48C) & 0x1000)
    {
        trans = *(u8 **)(car + 0x3BC);
        rpm /= *(f32 *)(trans + 0x30);
        scale *= *(f32 *)(trans + 0x34);
    }
    if (rpm < 0.0f)
    {
        rpm *= -1.0f;
    }
    trans = *(u8 **)(car + 0x3BC);
    return scale * (rpm * (rpm * (a * rpm + b) + c) + d) /
           ((f32 *)(*(u8 **)(trans + 0xDC)))[*(u8 *)(car + 0x4E8) * 2];
}

void CarA_SetSlavePlayers(u8 *car)
{
    u8 *slave;

    for (slave = *(u8 **)(car + 0x344); slave != NULL; slave = *(u8 **)(slave + 0xA4))
    {
        *(s32 *)(slave + 0x94) = *(s32 *)(car + 0x94);
    }
}

f32 CarA_GetMass(u8 *car)
{
    u8 *coll;

    coll = *(u8 **)(car + 0x3BC);
    coll = *(u8 **)(coll + 0xA0);
    return *(f32 *)(coll + 0x20);
}

void CarA_GetTargetPos(u8 *car, Vector4 *pos)
{
    u8 *coll;

    coll = *(u8 **)(car + 0x3BC);
    coll = *(u8 **)(coll + 0xA0);
    switch (*(u8 *)(coll + 0x10))
    {
    case 0:
        *pos = *(Vector4 *)(car + 0xC0);
        break;
    case 1:
        *pos = *(Vector4 *)(*(u8 **)(car + 0x370) + 0x3D0);
        break;
    }
}

void *CarA_GetOrientedCollision(u8 *car)
{
    void BuildOOBOffset(void *, void *, void *, void *);
    void BuildOConvexHullOffset(void *, void *, void *, void *);
    u8 *coll;

    coll = *(u8 **)(car + 0x3BC);
    coll = *(u8 **)(coll + 0xA0);
    switch (*(u8 *)(coll + 0x10))
    {
    case 0:
        if (*(u32 *)(car + 0x74) & 0x180)
        {
            BuildOOBOffset(*(u8 **)(coll + 0x44), car + 0x10, car + 0x520, car + 0xC0);
            *(u32 *)(car + 0x74) &= ~0x180;
        }
        return car + 0xC0;
    case 1:
        BuildOConvexHullOffset(*(u8 **)(car + 0x370), *(u8 **)(coll + 0x48), car + 0x10, car + 0x520);
        return *(u8 **)(car + 0x370);
    }
    return NULL;
}

s32 CarA_GetSurfaceScript(u8 *car)
{
    u8 *coll;

    coll = *(u8 **)(car + 0x3BC);
    coll = *(u8 **)(coll + 0xA0);
    return *(s32 *)(coll + 0x30);
}

s32 CarA_Invalidate(void *arg0, u32 arg1, u32 arg2) {
    u32 temp_r0;

    temp_r0 = M2C_FIELD(arg0, u32 *, 0x3BC);
    if ((temp_r0 >= arg1) && (temp_r0 < arg2)) {
        return 1;
    }
    return 0;
}

