#include "common.h"


void Drone_GetCameraOrientationInfo(void *drone, void *camera, void *info) {
    M2C_FIELD(info, s32 *, 0x88) = 1;
    M2C_FIELD(info, void **, 0x84) = (u8 *)drone + 0x154;
    M2C_FIELD(info, f32 *, 0x8C) = 1.0f;
    M2C_FIELD(info, f32 *, 0x30) = 0.0f;
    M2C_FIELD(info, f32 *, 0x34) = 1.0f;
    M2C_FIELD(info, f32 *, 0x38) = 0.0f;
}

void Drone_DynamicCollideImpact(u8 *drone, u8 *collision, f32 force)
{
    void PSVECSubtract(Vector4 *a, Vector4 *b, Vector4 *out);
    /* xx */
    void PSVECCrossProduct(Vector4 *a, Vector4 *b, Vector4 *out);
    void XformVecOrientation(void *mtx, Vector4 *in, Vector4 *out);
    void InverseXformVecOrientation(void *mtx, Vector4 *in, Vector4 *out);
    void Thing_ProcessEndEvent(u8 *thing, u8 *record, u8 *collision, Vector4 *angVel);
    void Thing_Release(u8 *thing);
    Vector4 impulse;
    Vector4 torque;
    Vector4 r;
    Vector4 scaled;
    Vector4 pos;
    Vector4 normal;
    f32 scale;

    normal = *(Vector4 *)(collision + 0x10);
    pos = *(Vector4 *)collision;
    if (drone[0x1DC] != 2)
    {
        scale = force / *(f32 *)(*(u8 **)(*(u8 **)(drone + 0xC0) + 0x10) + 0x20);
        ((Vector4 *)drone)->x += normal.x * scale;
        ((Vector4 *)drone)->y += normal.y * scale;
        ((Vector4 *)drone)->z += normal.z * scale;
        PSVECSubtract(&pos, (Vector4 *)(drone + 0x40), &r);
        impulse.x = force * normal.x;
        impulse.y = force * normal.y;
        impulse.z = force * normal.z;
        PSVECCrossProduct(&r, &impulse, &torque);
        ((Vector4 *)(drone + 0x110))->x += torque.x;
        ((Vector4 *)(drone + 0x110))->y += torque.y;
        ((Vector4 *)(drone + 0x110))->z += torque.z;
        InverseXformVecOrientation(drone + 0x10, (Vector4 *)(drone + 0x110), &pos);
        scaled.x = ((Vector4 *)(drone + 0x120))->x * pos.x;
        scaled.y = ((Vector4 *)(drone + 0x120))->y * pos.y;
        scaled.z = ((Vector4 *)(drone + 0x120))->z * pos.z;
        XformVecOrientation(drone + 0x10, &scaled, (Vector4 *)(drone + 0x130));
        if (*(s32 *)(drone + 0x78) <= 0)
        {
            Thing_ProcessEndEvent(drone, *(u8 **)(*(u8 **)(drone + 0xC0) + 0x10), collision, (Vector4 *)(drone + 0x130));
            Thing_Release(drone);
        }
        *(f32 *)(drone + 0x180) = 0.0f;
        *(s32 *)(drone + 0x17C) = 0;
    }
}

