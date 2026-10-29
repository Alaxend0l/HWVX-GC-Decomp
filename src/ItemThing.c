#include "common.h"


s32 Item_Init(u8 *item, u8 *data) {
    void QuaternionToSubMatrix(void *quat, void *mtx);
    f32 RandomAdd(f32 base, f32 range);
    void GenerateQuaternionXYZ(Vector4 *euler, Vector4 *quat);
    void GenerateSpinAngleFromQuaternion(Vector4 *quat, void *axis, f32 *angle);
    s32 NavHolder_FindIndexOfNearestNavPoint(void *pos, s32 hint);
    Vector4 euler __attribute__((aligned(16)));
    Vector4 quat __attribute__((aligned(16)));
    u8 *info;

    Thing_Init(item, (s32)data);
    M2C_FIELD(item, s32 *, 0xD4) = -1;
    M2C_FIELD(item, s32 *, 0xD0) = 0;
    M2C_FIELD(item, u16 *, 0x80) = 2;
    M2C_FIELD(item, f32 *, 0x70) = M2C_FIELD(M2C_FIELD(data, u8 **, 0x30), f32 *, 0xC);
    M2C_FIELD(item, f32 *, 0xE8) = M2C_FIELD(item, f32 *, 0x70) * M2C_FIELD(item, f32 *, 0x70);
    M2C_FIELD(item, Vector4 *, 0x40) = M2C_FIELD(data, Vector4 *, 0x10);
    M2C_FIELD(item, f32 *, 0x4C) = 1.0f;
    M2C_FIELD(item, Vector4 *, 0x0) = M2C_FIELD(data, Vector4 *, 0x20);
    M2C_FIELD(item, Vector4 *, 0x50) = M2C_FIELD(data, Vector4 *, 0x0);
    M2C_FIELD(item, s32 *, 0xEC) = 0;
    QuaternionToSubMatrix(item + 0x50, item + 0x10);
    M2C_FIELD(item, f32 *, 0xC8) = M2C_FIELD(data, f32 *, 0x34);
    M2C_FIELD(item, u8 *, 0xE0) = M2C_FIELD(data, u8 *, 0x38);
    if (M2C_FIELD(item, f32 *, 0xC8) == 0.0f) {
        M2C_FIELD(item, f32 *, 0xC8) = 500.0f;
    }
    M2C_FIELD(item, s32 *, 0xD8) = 0;
    M2C_FIELD(item, u32 *, 0x74) |= 0x1000000;
    M2C_FIELD(item, u8 **, 0xC0) = M2C_FIELD(data, u8 **, 0x30);
    info = M2C_FIELD(M2C_FIELD(item, u8 **, 0xC0), u8 **, 0x30);
    if (info != NULL) {
        M2C_FIELD(item, s16 *, 0xCE) = M2C_FIELD(info, s32 *, 0x20);
    }
    euler.x = 0.0f;
    euler.y = 0.017453292f * RandomAdd(M2C_FIELD(M2C_FIELD(data, u8 **, 0x30), f32 *, 0x18), M2C_FIELD(M2C_FIELD(data, u8 **, 0x30), f32 *, 0x1C));
    euler.z = 0.0f;
    GenerateQuaternionXYZ(&euler, &quat);
    GenerateSpinAngleFromQuaternion(&quat, item + 0xB0, (f32 *)(item + 0xC4));
    M2C_FIELD(item, s16 *, 0xCC) = 1000;
    Thing_InitAllSlavesNow(item, M2C_FIELD(item, u8 **, 0xC0) + 0x38, 0xFF);
    M2C_FIELD(item, u32 *, 0x74) |= 8;
    M2C_FIELD(item, s32 *, 0xD4) = NavHolder_FindIndexOfNearestNavPoint(item + 0x40, -1);
    Thing_AddToGrid(item, 3);
    return 1;
}

void Item_Release(void *item) {
    extern void *g_itemVTable[];
    void Thing_Release(s32 thing);
    void *slot;

    if (M2C_FIELD(item, void **, 0x8C) == g_itemVTable) {
        slot = M2C_FIELD(item, void **, 0x90);
        if (slot != NULL && M2C_FIELD(slot, void **, 0x24) != NULL) {
            M2C_FIELD(item, void **, 0x90) = NULL;
            M2C_FIELD(slot, void **, 0x24) = NULL;
        }
    }
    Thing_Release((s32)item);
}

s32 Item_PreMove(void) {
    return 1;
}

s8 Item_Move(u8 *item) {
    typedef struct ItemHit {
        Vector4 pos;
        Vector4 normal;
        s32 unk20;
        s32 unk24;
        u8 pad[0x28];
    } ItemHit;
    typedef struct ItemQuat {
        f32 x, y, z, w;
    } ItemQuat;
    extern f32 g_fDeltaTimeSec;
    extern s32 g_iDeltaTimeMS;
    extern Vector4 g_zeroVector;
    extern u8 *g_pNoRayCollideThing;
    s8 VectorHitsTerrain(void *pos);
    s8 RayHitsTerrainAndObjects(void *pos, void *vel, f32 *t, ItemHit *hit);
    f32 PSVECDotProduct(void *a, void *b);
    void AccumulateVector(void *vel, void *pos, void *out, f32 t);
    void GenerateQuaternion(void *axis, ItemQuat *q, f32 angle);
    void QuaternionMult(void *a, ItemQuat *b, ItemQuat *out);
    ItemHit hit __attribute__((aligned(16)));
    ItemQuat q;
    ItemQuat rot;
    f32 dt;
    u8 hitSomething;
    s8 hitTerrain;
    f32 d;
    Vector4 *vel = (Vector4 *)item;

    if (!(*(u32 *)(item + 0x74) & 8)) {
        hitSomething = 0;
        dt = g_fDeltaTimeSec;
        vel->y += -130.0f * dt;
        hitTerrain = VectorHitsTerrain(item + 0x40);
        if (hitTerrain) {
            *vel = g_zeroVector;
            *(u32 *)(item + 0x74) |= 1;
            *(u32 *)(item + 0x74) |= 8;
            return hitTerrain;
        }
        hit.unk20 = hit.unk24 = 0;
        if (*(u8 **)(item + 0x94) != NULL) {
            g_pNoRayCollideThing = *(u8 **)(*(u8 **)(item + 0x94) + 0x2C);
        }
        if (RayHitsTerrainAndObjects(item + 0x40, item, &dt, &hit)) {
            f32 push = 0.001f;
            hitSomething = 1;
            d = 2.0f * PSVECDotProduct(item, &hit.normal);
            vel->x -= d * hit.normal.x;
            vel->y -= d * hit.normal.y;
            vel->z -= d * hit.normal.z;
            ((Vector4 *)(item + 0x40))->x += hit.normal.x * push;
            ((Vector4 *)(item + 0x40))->y += hit.normal.y * push;
            ((Vector4 *)(item + 0x40))->z += hit.normal.z * push;
        }
        g_pNoRayCollideThing = NULL;
        if (hitSomething) {
            vel->x *= 0.5f;
            vel->y *= 0.5f;
            vel->z *= 0.5f;
            if (hit.normal.y > 0.7f && vel->x * vel->x + vel->y * vel->y + vel->z * vel->z < 1.0f) {
                vel->x = vel->y = vel->z = 0.0f;
                *(u32 *)(item + 0x74) |= 1;
                *(u32 *)(item + 0x74) |= 8;
            }
        }
        AccumulateVector(item, item + 0x40, item + 0x40, dt);
    }
    if (*(s16 *)(item + 0xCC) > 0) {
        *(s16 *)(item + 0xCC) -= (u16)g_iDeltaTimeMS;
        if (*(s16 *)(item + 0xCC) < 0) {
            *(u8 **)(item + 0x94) = NULL;
        }
    }
    if (*(u32 *)(item + 0xD8) != 0) {
        *(s32 *)(item + 0xDC) -= g_iDeltaTimeMS;
        if (*(s32 *)(item + 0xDC) < 0) {
            *(u32 *)(item + 0xD8) = 0;
        }
    }
    if (*(f32 *)(item + 0xC4)) {
        GenerateQuaternion(item + 0xB0, &q, 10.0f * (*(f32 *)(item + 0xC4) * g_fDeltaTimeSec));
        QuaternionMult(item + 0x50, &q, &rot);
        *(ItemQuat *)(item + 0x50) = rot;
    }
    if (**(u8 **)(item + 0xC0) == 7 && *(u8 **)(item + 0x94) != NULL && *(u8 **)(*(u8 **)(item + 0x94) + 0x2C) != NULL) {
        u8 *obj = *(u8 **)(*(u8 **)(item + 0x94) + 0x2C);
        *(s32 *)(item + 0xD4) = (*(s32 (**)(void *))(*(u8 **)(obj + 0x8C) + 0x2C))(obj);
    }
    return 1;
}

void Item_Draw(u8 *item) {
    f32 Gfx_GetMinCameraDistSquared(void *pos);
    void QuaternionToSubMatrix(void *quat, void *mtx);
    void Thing_DrawSharedSlaves(void *thing, void *slaves);

    if (*(f32 *)(item + 0xC8)) {
        if (Gfx_GetMinCameraDistSquared(item + 0x40) > *(f32 *)(item + 0xC8) * *(f32 *)(item + 0xC8)) {
            return;
        }
    }
    if (*(u32 *)(item + 0x74) & 8) {
        QuaternionToSubMatrix(item + 0x50, item + 0x10);
    }
    Thing_DrawSharedSlaves(item, *(u8 **)(item + 0xC0) + 0x38);
}

s32 Item_GetNearestNavPoint(void *arg0) {
    return M2C_FIELD(arg0, s32 *, 0xD4);
}

s32 Item_Invalidate(void *arg0, u32 arg1, u32 arg2) {
    u32 temp_r0;

    temp_r0 = M2C_FIELD(arg0, u32 *, 0xC0);
    if ((temp_r0 >= arg1) && (temp_r0 < arg2)) {
        return 1;
    }
    return 0;
}
void Item_Draw(u8 *item);
void Thing_GetMass();
void Thing_DynamicCollideImpact();
void Thing_GetCameraOrientationInfo();
void Thing_GetInterfaceInfo();
void Thing_SlaveTransform();
void Thing_GetSurfaceScript();
void Thing_GetInverseLocalInertiaTensor();
void Thing_DynamicRotation();
void Thing_CollideWithRay();
void Thing_GetTargetPos();
void Thing_GetOrientedCollision();

void *g_itemVTable[] = {
    Item_Init,
    Item_Release,
    Item_PreMove,
    Item_Move,
    Item_Draw,
    Thing_GetMass,
    Thing_DynamicCollideImpact,
    Thing_GetCameraOrientationInfo,
    Thing_GetInterfaceInfo,
    Thing_SlaveTransform,
    Thing_GetSurfaceScript,
    Item_GetNearestNavPoint,
    Thing_GetInverseLocalInertiaTensor,
    Thing_DynamicRotation,
    Thing_CollideWithRay,
    Thing_GetTargetPos,
    Thing_GetOrientedCollision,
};
