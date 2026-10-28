#include "common.h"

extern f32 g_fDeltaTimeSec;
extern s32 g_iDeltaTimeMS;
extern Vector4 g_zeroVector;

u8 FloatsEqual(f32 a, f32 b, f32 eps);
f32 _fabs(f32 x);
double acos(double x);
f32 Sqrt(f32 x);
f32 MagnitudeVector(Vector4 *v);
f32 PSVECDotProduct(Vector4 *a, Vector4 *b);
void PSVECCrossProduct(Vector4 *a, Vector4 *b, Vector4 *out);
void PSVECSubtract(Vector4 *a, Vector4 *b, Vector4 *out);
f32 PSVECSquareMag(Vector4 *v);
void PSVECNormalize(Vector4 *src, Vector4 *dst);
f32 PSVECSquareDistance(Vector4 *a, Vector4 *b);
void GenerateQuaternion(Vector4 *axis, Quaternion *q, f32 angle);
void QuaternionMult(Quaternion *a, Quaternion *b, Quaternion *out);
void QuaternionToSubMatrix(Quaternion *quat, u8 *mtx);
void InverseXformVec(u8 *mtx, Vector4 *in, Vector4 *out);
s32 ThingManager_GetActiveCars(u8 **cars, u32 max);
u8 *ThingManager_DARYL_CreateSlave(s32 type, void *params, u32 id);
void ThingManagerList_Append(void *list, u8 *thing);
void Projectile_Release(u8 *proj);

typedef struct SmartProjParams {
    u8 unk0;
    u8 invertZ;
    u8 pad2[0x4 - 0x2];
    f32 turnMax;
    f32 turnOut;
    f32 turnIn;
    f32 gain;
    f32 turnGain;
    u8 pad18[0x1C - 0x18];
    f32 fov;
    f32 minRange;
} SmartProjParams;

typedef struct SmartProjInfo {
    u8 pad0[0xC];
    s32 damage;
    u8 pad20[0x34 - 0x20];
    f32 f34;
    u8 pad38[0x40 - 0x38];
    f32 speedGain;
    f32 maxSpeed;
    u8 pad48[0x74 - 0x48];
    u32 slaveId;
    u8 pad78[0xA8 - 0x78];
    SmartProjParams *params;
} SmartProjInfo;

typedef struct SmartTarget {
    Vector4 vec00;
    u8 pad10[0x40 - 0x10];
    Vector4 pos;
    u8 pad50[0x8C - 0x50];
    void **vtable;
} SmartTarget;

typedef struct SmartProjVTable {
    u8 pad0[0x18];
    void (*applyImpulse)(u8 *obj, void *hit, f32 impulse, s32 damage);
    u8 pad1C[0x3C - 0x1C];
    void (*getPos)(u8 *obj, Vector4 *out);
} SmartProjVTable;

typedef struct SmartProjectile {
    Vector4 vel;
    u8 mtx[0x30];
    Vector4 pos;
    Quaternion quat;
    u8 pad60[0x74 - 0x60];
    u32 flags;
    u8 pad78[0x94 - 0x78];
    s32 player;
    u8 pad98[0x120 - 0x98];
    SmartProjInfo *info;
    u8 pad124[0x14C - 0x124];
    SmartTarget *target;
    f32 turnX;
    f32 turnY;
    f32 turnZ;
    u16 hits15C;
    u16 timer;
} SmartProjectile;

s32 SmartProjectileA_PreMove(SmartProjectile *proj)
{
    return 0;
}
