#include "common.h"
#include "CarA.h"

/* What a suspension ray hit is made of (RayHit.surface). */
typedef struct SuspSurface {
    u8 type;                  /* 0x00 1: no skids, 2: own skid texture */
    u8 pad01;
    u16 skidTexture;          /* 0x02 */
    u8 pad04[0x10];
    f32 grip;                 /* 0x14 */
    u8 pad18[4];
    f32 dustRate;             /* 0x1C */
    f32 particleInterval;     /* 0x20 */
    u8 pad24[8];
    GXColor *skidColor;       /* 0x2C */
    u32 dustThing;            /* 0x30 */
    u32 particle;             /* 0x34 */
    u8 pad38[4];
    struct { u8 pad[0x14]; u32 sample; } *rollSound; /* 0x3C */
    u32 sparkThing;           /* 0x40 */
} SuspSurface;

typedef struct SuspRayHit {
    Vector4 pos;              /* 0x00 */
    Vector4 normal;           /* 0x10 */
    u8 pad20[8];
    SuspSurface *surface;     /* 0x28 */
    u8 pad2C[4];
    Vector4 velocity;         /* 0x30 */
    u8 pad40[0x10];
} SuspRayHit;

/* ThingManager_DARYL_CreateThing(1, ...) parameters (same as CarA_HW.c's). */
typedef struct SuspThingParams {
    Vector4 pos;
    Vector4 animRefPos;
    Vector4 vel;
    Vector4 angVel;
    u32 id;
    void *player;
    f32 unk48;
    f32 unk4C;
} SuspThingParams;

extern f32 g_fDeltaTimeSec;
extern s32 g_iDeltaTimeMS;
extern u32 g_iLastDeltaTimeMS;
extern CarA *g_pNoRayCollideThing;
extern Vector4 g_zeroVector;

f32 PSVECDotProduct(void *a, void *b);
void PSVECAdd(void *a, void *b, Vector4 *out);
void PSVECSubtract(void *a, void *b, Vector4 *out);
void PSVECCrossProduct(void *a, void *b, Vector4 *out);
f32 PSVECSquareMag(Vector4 *v);
void PSVECNormalize(Vector4 *in, Vector4 *out);
f32 MagnitudeVector(void *v);
f32 _fabs(f32 x);
void XformVec(void *mtx, Vector4 *in, Vector4 *out);
void ProjectVectorToNormal(void *normal, void *v);
s8 Player_IsHuman(void *player);
s8 RayHitsTerrainAndObjects(void *start, void *dir, f32 *dist, SuspRayHit *hit);
void CarA_ProcessForce(CarA *car, Vector4 *pos, Vector4 *dir, f32 scale);
void *ThingManager_DARYL_CreateThing(s32 type, void *params);
s32 ViewManager_GetViewFormat(void);
f32 Gfx_GetMinCameraDistSquared(Vector4 *pos);
s32 ParticleManager_CanCreatureParticle(u32 id);
f32 RandomAdd(f32 base, f32 range);
void CreateParticle(Vector4 *pos, Vector4 *vel, f32 *accel, f32 *color, u32 id, f32 a, f32 b);
void SoundSource_Set(s32 id, s32 type, void *value);
void Wheel_PlaySpringForce(s32 chan, s8 offset, u8 saturation, s16 coefficient);
void Wheel_PlayDamperForce(s32 chan, s16 coefficient, u8 saturation);
void Wheel_PlayConstantForce(s32 chan, s16 magnitude, u16 direction);
void Wheel_PlayCarAirborne(s32 chan);
void Wheel_StopCarAirborne(s32 chan);
void CarA_ComputeSuspensionForces_Aux1(CarA *car);


f32 CarA_GetImpactMultiplier(void *car, void *other, f32 scale) {
    f32 PSVECDotProduct(void *a, void *b);
    f32 impact;

    impact = scale * PSVECDotProduct((u8 *)car + 0x20, (u8 *)other + 0x10);
    if (impact < 0.0f) {
        return 1.0f;
    }
    return 1.0f - impact;
}
