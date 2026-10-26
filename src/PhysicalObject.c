#include "common.h"


void PhysicalObject_ProcessForce(u8 *obj, Vector4 *force, Vector4 *dir)
{
    void PSVECScale(Vector4 *src, Vector4 *dst, u32 scale);
    void PSVECSubtract(Vector4 *a, Vector4 *b, Vector4 *dst);
    void PSVECAdd(Vector4 *a, Vector4 *b, Vector4 *dst);
    void PSVECCrossProduct(Vector4 *a, Vector4 *b, Vector4 *dst);
    f32 GetComponentOfVector(Vector4 *v, Vector4 *dir);

    Vector4 pointVel;
    Vector4 cross;
    Vector4 r;
    Vector4 torque;
    Vector4 objVel;
    Vector4 scaled __attribute__((aligned(16)));

    objVel = *(Vector4 *)obj;
    PSVECScale(dir, &scaled, *(u32 *)(obj + 8));
    PSVECSubtract(force, (Vector4 *)(obj + 0x40), &r);
    GetComponentOfVector(dir, &objVel);
    PSVECCrossProduct((Vector4 *)(obj + 0x130), &r, &cross);
    PSVECAdd((Vector4 *)obj, &cross, &pointVel);
    PSVECCrossProduct(&r, &scaled, &torque);

    *(f32 *)(obj + 0xF0) += scaled.x;
    *(f32 *)(obj + 0xF4) += scaled.y;
    *(f32 *)(obj + 0xF8) += scaled.z;
    *(f32 *)(obj + 0x100) += torque.x;
    *(f32 *)(obj + 0x104) += torque.y;
    *(f32 *)(obj + 0x108) += torque.z;
}

void Mover_Init(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

