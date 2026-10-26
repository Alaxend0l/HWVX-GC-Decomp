#include "common.h"

typedef struct Debris {
    Vector4 vel;
    u8 mtx[0x30];
    Vector4 pos;
    u8 pad50[0x24];
    u32 flags;
    u8 pad78[0x38];
    Vector4 spinAxis;
    void *visual;
    f32 gravity;
    u8 padC8[4];
    f32 mass;
    Vector4 lastPos;
    Vector4 macroVel;
    s32 macroTime;
} Debris;

void Gfx_DrawGeometry(void *geom, void *mtx);
void Gfx_DrawGeometry_MatrixNotCached(void *geom, void *mtx);

s32 Debris_Init(u8 *debris, u8 *data)
{
    Thing_Init(debris, 0);
    *(u8 **)(debris + 0xC0) = *(u8 **)(data + 0x44);
    *(Vector4 *)(debris + 0xB0) = *(Vector4 *)(data + 0x20);
    *(Vector4 *)(debris + 0x50) = *(Vector4 *)(data + 0x10);
    *(Vector4 *)(debris + 0x0) = *(Vector4 *)(data + 0x30);
    ((Vector4 *)data)->w = 1.0f;
    *(Vector4 *)(debris + 0x40) = *(Vector4 *)(data + 0x0);
    *(s32 *)(debris + 0xC8) = *(s32 *)(data + 0x48);
    *(s32 *)(debris + 0xF0) = -1;
    *(Vector4 *)(debris + 0xD0) = *(Vector4 *)(debris + 0x40);
    *(u16 *)(debris + 0x80) = 3;
    *(f32 *)(debris + 0xC4) = *(f32 *)(data + 0x40);
    *(f32 *)(debris + 0xCC) = *(f32 *)(data + 0x4C);
    if (*(u32 *)(data + 0x50) == 0) {
        *(u32 *)(debris + 0x74) |= 2;
    }
    *(u32 *)(debris + 0x74) |= 0x01000000;
    if (*(u8 **)(debris + 0xC0) != NULL) {
        *(f32 *)(debris + 0x70) = *(f32 *)(*(u8 **)(debris + 0xC0) + 0x1C);
    }
    return 1;
}

void Debris_MacroStep(void *debris, f32 dt) {
    typedef struct DebrisHit {
        Vector4 pos;
        Vector4 normal;
        u8 pad[0x30];
    } DebrisHit;
    typedef struct Debris {
        Vector4 vel;
        u8 pad10[0x30];
        Vector4 pos;
        u8 pad50[0x24];
        u32 flags;
        u8 pad78[0x38];
        Vector4 spinAxis;
        u8 padC0[4];
        f32 gravity;
        u8 padC8[8];
        Vector4 lastPos;
        Vector4 macroVel;
        s32 macroTime;
    } Debris;
    extern f32 g_fDeltaTimeSec;
    void TimeHandler_SetTimeElapsedSec_f32(f32 sec);
    s8 RayHitsTerrain_Aux3(void *, void *, void *, void *);
    void AccumulateVector(void *vel, void *pos, void *out, f32 t);
    f32 PSVECDotProduct(void *a, void *b);
    void RandomNumberGenerator_GetRandomVector(void *v);
    Debris *deb = debris;
    f32 savedDt = g_fDeltaTimeSec;
    f32 t;
    Vector4 newPos;
    DebrisHit hit;
    u8 bounced;
    f32 d;
    f32 spin;

    if (deb->flags & 1) {
        return;
    }
    TimeHandler_SetTimeElapsedSec_f32(dt);
    deb->vel.y = deb->gravity * g_fDeltaTimeSec + deb->vel.y;
    newPos.w = 1.0f;
    if (!(deb->flags & 2)) {
        t = g_fDeltaTimeSec;
        bounced = 0;
        if (RayHitsTerrain_Aux3(&deb->pos, deb, &t, &hit)) {
            AccumulateVector(&deb->vel, &deb->pos, &newPos, t * g_fDeltaTimeSec);
            bounced = 1;
            d = 2.0f * PSVECDotProduct(&deb->vel, &hit.normal);
            deb->vel.x = deb->vel.x - d * hit.normal.x;
            deb->vel.y = deb->vel.y - d * hit.normal.y;
            deb->vel.z = deb->vel.z - d * hit.normal.z;
            deb->vel.x *= 0.5f;
            deb->vel.y *= 0.5f;
            deb->vel.z *= 0.5f;
            newPos.x = g_fDeltaTimeSec * (deb->vel.x * (1.0f - t)) + newPos.x;
            newPos.y = g_fDeltaTimeSec * (deb->vel.y * (1.0f - t)) + newPos.y;
            newPos.z = g_fDeltaTimeSec * (deb->vel.z * (1.0f - t)) + newPos.z;
        } else {
            newPos.x = deb->vel.x * g_fDeltaTimeSec + deb->pos.x;
            newPos.y = deb->vel.y * g_fDeltaTimeSec + deb->pos.y;
            newPos.z = deb->vel.z * g_fDeltaTimeSec + deb->pos.z;
        }
        if (bounced) {
            if (deb->vel.z * deb->vel.z + (deb->vel.x * deb->vel.x + deb->vel.y * deb->vel.y) < 1.0f) {
                deb->vel.x = deb->vel.y = deb->vel.z = deb->spinAxis.w = 0.0f;
                deb->flags |= 1;
            } else if (deb->vel.x * deb->vel.x + deb->vel.z * deb->vel.z > 2e-05f) {
                spin = deb->spinAxis.w;
                deb->spinAxis.w *= 0.8f;
                RandomNumberGenerator_GetRandomVector(&deb->spinAxis);
                deb->spinAxis.w = spin;
            } else {
                deb->spinAxis.w = 0.0f;
            }
        }
        deb->lastPos = newPos;
        deb->macroVel.x = (1000.0f * (newPos.x - deb->pos.x)) / deb->macroTime;
        deb->macroVel.y = (1000.0f * (newPos.y - deb->pos.y)) / deb->macroTime;
        deb->macroVel.z = (1000.0f * (newPos.z - deb->pos.z)) / deb->macroTime;
    }
    TimeHandler_SetTimeElapsedSec_f32(savedDt);
}

void Debris_Draw(Debris *debris) {
    if (debris->visual != NULL) {
        if (debris->flags & 0x20000000) {
            Gfx_DrawGeometry(debris->visual, debris->mtx);
        }
        else {
            Gfx_DrawGeometry_MatrixNotCached(debris->visual, debris->mtx);
        }
    }
}

f32 Debris_GetMass(Debris *debris) {
    return debris->mass;
}
