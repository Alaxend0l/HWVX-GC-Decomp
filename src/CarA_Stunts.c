#include "common.h"


void Drone_GetCameraOrientationInfo(void *drone, void *camera, void *info) {
    M2C_FIELD(info, s32 *, 0x88) = 1;
    M2C_FIELD(info, void **, 0x84) = (u8 *)drone + 0x154;
    M2C_FIELD(info, f32 *, 0x8C) = 1.0f;
    M2C_FIELD(info, f32 *, 0x30) = 0.0f;
    M2C_FIELD(info, f32 *, 0x34) = 1.0f;
    M2C_FIELD(info, f32 *, 0x38) = 0.0f;
}

s32 Drone_Init(u8 *drone, u8 *data) {
    extern Vector4 g_zeroVector;
    void *LoadDataChunkFile(char *name);
    void QuaternionToSubMatrix(void *quat, void *mtx);
    typedef struct {
        Vector4 v[3];
    } Box;
    Box box;
    u8 *props;

    Thing_Init(drone, (s32)data);
    *(void **)(drone + 0xD4) = LoadDataChunkFile("NPP.ANI");
    *(f32 *)(drone + 0x1E4) = *(f32 *)(*(u8 **)(drone + 0xD4) + 0x8);
    *(s32 *)(drone + 0x94) = 0;
    *(Vector4 *)(drone + 0x40) = *(Vector4 *)data;
    QuaternionToSubMatrix(data + 0x30, drone + 0x10);
    *(Vector4 *)(drone + 0x50) = *(Vector4 *)(data + 0x30);
    *(s32 *)(drone + 0x15C) = 0;
    drone[0x154] = 0;
    *(void **)(drone + 0x158) = drone + 0x160;
    *(f32 *)(drone + 0x164) = 20.0f;
    *(f32 *)(drone + 0x170) = 20.0f;
    *(f32 *)(drone + 0x160) = 75.0f;
    *(f32 *)(drone + 0x16C) = 40.0f;
    *(f32 *)(drone + 0x168) = 5.0f;
    *(void **)(drone + 0xC0) = *(void **)(data + 0x40);
    props = *(u8 **)(*(u8 **)(drone + 0xC0) + 0x10);
    switch (props[0x10]) {
    case 0:
        *(f32 *)(drone + 0x70) = *(f32 *)(*(u8 **)(props + 0x44) + 0x20);
        *(u16 *)(drone + 0x80) = 1;
        break;
    case 1:
        break;
    }
    *(u32 *)(drone + 0x78) = *(u32 *)(*(u8 **)(*(u8 **)(drone + 0xC0) + 0x10) + 0x14);
    *(f32 *)(drone + 0x180) = 0.0f;
    *(Vector4 *)(drone + 0x100) = g_zeroVector;
    *(Vector4 *)(drone + 0xF0) = *(Vector4 *)(drone + 0x100);
    *(s32 *)(drone + 0x184) = 0;
    *(s32 *)(drone + 0x17C) = 0;
    props = *(u8 **)(*(u8 **)(drone + 0xC0) + 0x10);
    switch (props[0x10]) {
    case 0:
        box = **(Box **)(props + 0x44);
        break;
    case 1:
        break;
    }
    *(f32 *)(drone + 0x120) = 1.0f / (*(f32 *)(*(u8 **)(*(u8 **)(drone + 0xC0) + 0x10) + 0x0) * *(f32 *)(*(u8 **)(*(u8 **)(drone + 0xC0) + 0x10) + 0x20) * (box.v[1].y * box.v[1].y + box.v[1].z * box.v[1].z));
    *(f32 *)(drone + 0x124) = 1.0f / (*(f32 *)(*(u8 **)(*(u8 **)(drone + 0xC0) + 0x10) + 0x4) * *(f32 *)(*(u8 **)(*(u8 **)(drone + 0xC0) + 0x10) + 0x20) * (box.v[1].x * box.v[1].x + box.v[1].z * box.v[1].z));
    *(f32 *)(drone + 0x128) = 1.0f / (*(f32 *)(*(u8 **)(*(u8 **)(drone + 0xC0) + 0x10) + 0x8) * *(f32 *)(*(u8 **)(*(u8 **)(drone + 0xC0) + 0x10) + 0x20) * (box.v[1].x * box.v[1].x + box.v[1].y * box.v[1].y));
    Thing_AddToGrid(drone, 0);
    *(Vector4 *)(drone + 0x110) = g_zeroVector;
    *(Vector4 *)(drone + 0x130) = g_zeroVector;
    *(u32 *)(drone + 0x74) |= 4;
    *(u32 *)(drone + 0x74) |= 0x80;
    *(f32 *)(drone + 0x190) = -3.3944604f;
    *(f32 *)(drone + 0x194) = -0.48040998f;
    *(f32 *)(drone + 0x198) = -4.5236802f;
    *(f32 *)(drone + 0x1A0) = 3.3944604f;
    *(f32 *)(drone + 0x1A4) = -0.48040998f;
    *(f32 *)(drone + 0x1A8) = -4.5236802f;
    *(f32 *)(drone + 0x1B0) = -3.3944604f;
    *(f32 *)(drone + 0x1B4) = -0.48040998f;
    *(f32 *)(drone + 0x1B8) = 5.6166801f;
    *(f32 *)(drone + 0x1C0) = 3.3944604f;
    *(f32 *)(drone + 0x1C4) = -0.48040998f;
    *(f32 *)(drone + 0x1C8) = 5.6166801f;
    drone[0x1DC] = 0;
    return 1;
}

void Drone_DynamicCollideImpact(u8 *drone, u8 *collision, f32 force)
{
    void PSVECSubtract(Vector4 *a, Vector4 *b, Vector4 *out);
    void PSVECCrossProduct(Vector4 *a, Vector4 *b, Vector4 *out);
    void XformVecOrientation(void *mtx, Vector4 *in, Vector4 *out);
    void InverseXformVecOrientation(void *mtx, Vector4 *in, Vector4 *out);
    void Thing_ProcessEndEvent(u8 *thing, u8 *record, u8 *collision, Vector4 *angVel);
    void Thing_Release(u8 *thing);
    Vector4 normal;
    Vector4 pos;
    Vector4 impulse;
    Vector4 torque;
    Vector4 r;
    Vector4 scaled;
    Vector4 local __attribute__((aligned(16)));
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
        InverseXformVecOrientation(drone + 0x10, (Vector4 *)(drone + 0x110), &local);
        scaled.x = ((Vector4 *)(drone + 0x120))->x * local.x;
        scaled.y = ((Vector4 *)(drone + 0x120))->y * local.y;
        scaled.z = ((Vector4 *)(drone + 0x120))->z * local.z;
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

