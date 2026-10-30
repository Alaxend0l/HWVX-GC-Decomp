#include "CarA.h"
#include "common.h"

#pragma use_lmw_stmw on

typedef struct AnimKeyFrame {
    Quaternion rot;
    f32 pos[4];
} AnimKeyFrame;

void QuaternionMult(Vector4 *a, Vector4 *b, Vector4 *out);

/* Stacks an additive animation's key frames onto dst. */
static inline void Anim_AddKeyFrames(AnimKeyFrame *dst, AnimKeyFrame *src, u32 count) {
    u32 i;
    Quaternion q;

    for (i = 0; i < count; i++) {
        if (src[i].rot.w != 1.0f || src[i].rot.x != 0.0f || src[i].rot.y != 0.0f || src[i].rot.z != 0.0f) {
            QuaternionMult((Vector4 *)&dst[i].rot, (Vector4 *)&src[i].rot, (Vector4 *)&q);
            dst[i].rot = q;
        }
        dst[i].pos[0] += src[i].pos[0];
        dst[i].pos[1] += src[i].pos[1];
        dst[i].pos[2] += src[i].pos[2];
    }
}

/* Moves the body offset (centre of mass) to (x, y, z), shifting the car's
   position so the body stays where it is in the world. */
static inline void CarA_SetBodyOffset(CarA *car, f32 x, f32 y, f32 z) {
    void XformVecOrientation(void *mtx, Vector4 *in, Vector4 *out);
    Vector4 world __attribute__((aligned(16)));
    Vector4 delta __attribute__((aligned(16)));

    delta.x = x - car->bodyOffset.x;
    delta.y = y - car->bodyOffset.y;
    delta.z = z - car->bodyOffset.z;
    XformVecOrientation(&car->worldMatrix, &delta, &world);
    car->worldMatrix.v[3].x -= world.x;
    car->worldMatrix.v[3].y -= world.y;
    car->worldMatrix.v[3].z -= world.z;
    car->bodyOffset.x = x;
    car->bodyOffset.y = y;
    car->bodyOffset.z = z;
    car->collDirtyFlags |= 0x100;
}

/* Angular velocity (CarA.localAxis) from angular momentum and the inverse
   body inertia tensor. */
static inline void CarA_UpdateAngularVelocity(CarA *car) {
    void XformVecOrientation(void *mtx, Vector4 *in, Vector4 *out);
    void InverseXformVecOrientation(void *mtx, Vector4 *in, Vector4 *out);
    Vector4 local __attribute__((aligned(16)));
    Vector4 scaled __attribute__((aligned(16)));

    InverseXformVecOrientation(&car->worldMatrix, &car->angularMomentum, &local);
    scaled.x = car->inverseBodyInertia.v[0].x * local.x;
    scaled.y = car->inverseBodyInertia.v[1].y * local.y;
    scaled.z = car->inverseBodyInertia.v[2].z * local.z;
    XformVecOrientation(&car->worldMatrix, &scaled, &car->localAxis);
}

static inline Vector4 Vector4_OrZero(Vector4 *v) {
    extern Vector4 g_zeroVector;

    if (v != NULL) {
        return *v;
    }
    return g_zeroVector;
}

/* Releases the damage-state slaves and re-triggers the ones whose health
   threshold the car has dropped below. */
static inline void CarA_UpdateDamageSlaves(CarA *car) {
    typedef struct DamageSlave {
        u8 pad00[0x14];
        f32 threshold;
        u8 pad18[8];
    } DamageSlave;
    typedef struct DamageSlaves {
        u8 pad00[0x1C];
        u32 count;
        DamageSlave *slaves;
    } DamageSlaves;
    extern Vector4 g_zeroVector;
    void MotionState_ReleaseSlavesByType(MotionState *state, u8 type);
    void ThingManagerList_TriggerSlaves(void *list, void *key, void *slave, Vector4 *color, Vector4 *vel, s32 type);
    MotionState *ms;
    u32 i;
    DamageSlaves *slaves;
    f32 frac;

    if (car->vehicleIface->numSlaves >= 2) {
        ms = &car->motionState;
        slaves = (DamageSlaves *)car->vehicleIface->slaves;
        frac = 100.0f * (f32)car->speed / (f32)car->vehicleIface->collider->maxHealth;
        MotionState_ReleaseSlavesByType(ms, 2);
        for (i = 0; i < slaves->count; i++) {
            Vector4 color = {0.0f, 0.0f, 0.0f, 0.0f};

            if (&slaves->slaves[i] != NULL && frac < slaves->slaves[i].threshold) {
                ThingManagerList_TriggerSlaves(&ms->slaves, &slaves->slaves[i], &slaves->slaves[i], &color, &g_zeroVector, 2);
            }
        }
    }
}

void CarA_Extract_Keyframes(CarA *car, AnimKeyFrame *frames) {
    void MotionState_ExtractBlendedKeyFrame(MotionState *state, AnimKeyFrame *keyFrame);
    void Gfx_AnimExtractKeyFrame(void *anim, f32 time, AnimKeyFrame *keyFrame, u32 count);
    void SinCosRad(f32 *s, f32 *c, f32 rad);
    u32 *numBones;
    u32 i;
    CarTrailBone *wheel;
    AnimKeyFrame keys[64];

    numBones = car->motionState.model->numBones;
    MotionState_ExtractBlendedKeyFrame(&car->motionState, frames);
    for (i = 0; i < car->vehicleIface->numWheels; i++) {
        wheel = &car->vehicleIface->wheels[i];
        SinCosRad(&frames[wheel->bone].rot.x, &frames[wheel->bone].rot.w, 0.5f * car->wheelStates[i].rotation);
        frames[wheel->bone].rot.y = 0.0f;
        frames[wheel->bone].rot.z = 0.0f;
    }
    if (car->vehicleIface->steerAnim != NULL) {
        Gfx_AnimExtractKeyFrame(car->vehicleIface->steerAnim,
                                10.0f * (car->unk378 / 0.7853982f) * car->vehicleIface->engine->animSpeed,
                                keys, *numBones);
        Anim_AddKeyFrames(frames, keys, *numBones);
    }
    if (car->vehicleIface->modelRef->anims[0] != NULL) {
        Gfx_AnimExtractKeyFrame(car->vehicleIface->modelRef->anims[0], -car->animTimes[0], keys, *numBones);
        Anim_AddKeyFrames(frames, keys, *numBones);
    }
    if (car->vehicleIface->modelRef->anims[1] != NULL) {
        Gfx_AnimExtractKeyFrame(car->vehicleIface->modelRef->anims[1], -car->animTimes[1], keys, *numBones);
        Anim_AddKeyFrames(frames, keys, *numBones);
    }
    if (car->vehicleIface->modelRef->anims[2] != NULL) {
        Gfx_AnimExtractKeyFrame(car->vehicleIface->modelRef->anims[2], -car->animTimes[2], keys, *numBones);
        Anim_AddKeyFrames(frames, keys, *numBones);
    }
}

void CarA_InitInverseBodyInertiaTensor(CarA *car) {
    extern Matrix4 g_identity;
    void PSVECAdd(void *a, void *b, Vector4 *out);
    Vector4 ext[4] __attribute__((aligned(16)));
    Vector4 bounds[2] __attribute__((aligned(16)));
    CarACollider *coll;
    Vector4 *box;
    CarAHull *hull;
    u32 i;

    car->inverseBodyInertia = g_identity;
    coll = car->vehicleIface->collider;
    switch (coll->type) {
    case 0:
        box = coll->box;
        PSVECAdd(&box[1], &car->bodyOffset, &ext[2]);
        PSVECAdd(&box[0], &car->bodyOffset, &ext[1]);
        break;
    case 1:
        hull = coll->hull;
        bounds[0].x = 9999.9f;
        bounds[0].y = 9999.9f;
        bounds[0].z = 9999.9f;
        bounds[1].x = -9999.9f;
        bounds[1].y = -9999.9f;
        bounds[1].z = -9999.9f;
        for (i = 0; i < hull->numVerts; i++) {
            if (bounds[0].x > hull->verts[i].x) {
                bounds[0].x = hull->verts[i].x;
            }
            if (bounds[0].y > hull->verts[i].y) {
                bounds[0].y = hull->verts[i].y;
            }
            if (bounds[0].z > hull->verts[i].z) {
                bounds[0].z = hull->verts[i].z;
            }
            if (bounds[1].x < hull->verts[i].x) {
                bounds[1].x = hull->verts[i].x;
            }
            if (bounds[1].y < hull->verts[i].y) {
                bounds[1].y = hull->verts[i].y;
            }
            if (bounds[1].z < hull->verts[i].z) {
                bounds[1].z = hull->verts[i].z;
            }
        }
        PSVECAdd(&bounds[1], &car->bodyOffset, &ext[2]);
        PSVECAdd(&bounds[0], &car->bodyOffset, &ext[1]);
        break;
    }
    car->inverseBodyInertia.v[0].x = 1.0f / (car->vehicleIface->collider->inertiaScale[0] * car->vehicleIface->collider->mass * (ext[2].y * ext[2].y + ext[2].z * ext[2].z));
    car->inverseBodyInertia.v[1].y = 1.0f / (car->vehicleIface->collider->inertiaScale[1] * car->vehicleIface->collider->mass * (ext[2].x * ext[2].x + ext[2].z * ext[2].z));
    car->inverseBodyInertia.v[2].z = 1.0f / (car->vehicleIface->collider->inertiaScale[2] * car->vehicleIface->collider->mass * (ext[2].x * ext[2].x + ext[2].y * ext[2].y));
}

void CarA_GetAngularVelocity(void *car, Vector4 *vel) {
    *vel = M2C_FIELD(car, Vector4 *, 0x410);
}

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

void CarA_ProcessForce(u8 *car, Vector4 *force, Vector4 *dir)
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
    Vector4 carVel;
    Vector4 scaled __attribute__((aligned(16)));

    carVel = *(Vector4 *)car;
    PSVECScale(dir, &scaled, *(u32 *)(car + 8));
    PSVECSubtract(force, (Vector4 *)(car + 0x40), &r);
    GetComponentOfVector(dir, &carVel);
    PSVECCrossProduct((Vector4 *)(car + 0x410), &r, &cross);
    PSVECAdd((Vector4 *)car, &cross, &pointVel);
    PSVECCrossProduct(&r, &scaled, &torque);

    *(f32 *)(car + 0x390) += scaled.x;
    *(f32 *)(car + 0x394) += scaled.y;
    *(f32 *)(car + 0x398) += scaled.z;
    *(f32 *)(car + 0x3A0) += torque.x;
    *(f32 *)(car + 0x3A4) += torque.y;
    *(f32 *)(car + 0x3A8) += torque.z;
}

void CarA_ProcessImpulse(void *thing, Vector4 *normal, Vector4 *pos, Vector4 *pointVel, f32 restitution) {
    void PSVECSubtract(Vector4 *a, Vector4 *b, Vector4 *out);
    void PSVECCrossProduct(Vector4 *a, Vector4 *b, Vector4 *out);
    f32 PSVECDotProduct(Vector4 *a, Vector4 *b);
    f32 PSVECSquareMag(Vector4 *v);
    f32 Sqrt(f32 x);
    void XformVecOrientation(void *mtx, Vector4 *in, Vector4 *out);
    void InverseXformVecOrientation(void *mtx, Vector4 *in, Vector4 *out);
    CarA *car = thing;
    Vector4 impulse __attribute__((aligned(16)));
    Vector4 r __attribute__((aligned(16)));
    Vector4 vel __attribute__((aligned(16)));
    Vector4 rxn __attribute__((aligned(16)));
    Vector4 ang __attribute__((aligned(16)));
    Vector4 angxr __attribute__((aligned(16)));
    Vector4 localRxn __attribute__((aligned(16)));
    Vector4 scaledRxn __attribute__((aligned(16)));
    Vector4 torque __attribute__((aligned(16)));
    Vector4 localAng __attribute__((aligned(16)));
    Vector4 scaledAng __attribute__((aligned(16)));
    f32 num;
    f32 j;
    f32 maxAng;
    f32 mag;

    vel = *pointVel;
    PSVECSubtract(pos, &car->worldMatrix.v[3], &r);
    num = -(restitution * car->vehicleIface->collider->material->restitution + 1.0f) * PSVECDotProduct(&vel, normal);
    PSVECCrossProduct(&r, normal, &rxn);
    InverseXformVecOrientation(&car->worldMatrix, &rxn, &localRxn);
    scaledRxn.x = car->inverseBodyInertia.v[0].x * localRxn.x;
    scaledRxn.y = car->inverseBodyInertia.v[1].y * localRxn.y;
    scaledRxn.z = car->inverseBodyInertia.v[2].z * localRxn.z;
    XformVecOrientation(&car->worldMatrix, &scaledRxn, &ang);
    PSVECCrossProduct(&ang, &r, &angxr);
    j = num / (1.0f / car->vehicleIface->collider->mass + PSVECDotProduct(&angxr, normal));
    impulse.x = j * normal->x;
    impulse.y = j * normal->y;
    impulse.z = j * normal->z;
    car->velocity.x += impulse.x / car->vehicleIface->collider->mass;
    car->velocity.y += impulse.y / car->vehicleIface->collider->mass;
    car->velocity.z += impulse.z / car->vehicleIface->collider->mass;
    PSVECCrossProduct(&r, &impulse, &torque);
    car->angularMomentum.x += torque.x;
    car->angularMomentum.y += torque.y;
    car->angularMomentum.z += torque.z;
    if (car->numWheelsGrounded <= 1) {
        maxAng = car->vehicleIface->engine->maxAngularVel;
        InverseXformVecOrientation(&car->worldMatrix, &car->angularMomentum, &localAng);
        scaledAng.x = car->inverseBodyInertia.v[0].x * localAng.x;
        scaledAng.y = car->inverseBodyInertia.v[1].y * localAng.y;
        scaledAng.z = car->inverseBodyInertia.v[2].z * localAng.z;
        mag = PSVECSquareMag(&scaledAng);
        if (mag > maxAng * maxAng) {
            mag = maxAng / Sqrt(mag);
            scaledAng.x *= mag;
            scaledAng.y *= mag;
            scaledAng.z *= mag;
            localAng.x = scaledAng.x / car->inverseBodyInertia.v[0].x;
            localAng.y = scaledAng.y / car->inverseBodyInertia.v[1].y;
            localAng.z = scaledAng.z / car->inverseBodyInertia.v[2].z;
            XformVecOrientation(&car->worldMatrix, &localAng, &car->angularMomentum);
        }
    }
    CarA_UpdateAngularVelocity(car);
}

s32 CarA_PreMove(void *thing) {
    typedef struct EffectVtbl {
        u8 pad0[0x18];
        void (*spawn)(void *thing, void *params, s32 arg, f32 scale);
    } EffectVtbl;
    typedef struct EffectThing {
        u8 pad0[0x8C];
        EffectVtbl *vtbl;
    } EffectThing;
    typedef struct TurboFx {
        Vector4 pos;
        Vector4 dir;
        void *owner;
        s32 unk24;
        s32 unk28;
        u8 pad2C[4];
        Vector4 vel;
        u8 unk40;
    } TurboFx;
    extern f32 g_fDeltaTimeSec;
    extern s32 g_iDeltaTimeMS;
    extern void *g_pSourcePlayer;
    extern Vector4 g_zeroVector;
    s8 Timer_Interrupt300(void);
    s32 CarA_PreMove_Control_HW(void *thing);
    f32 PSVECSquareMag(Vector4 *v);
    void PSVECNormalize(Vector4 *in, Vector4 *out);
    f32 PSVECDotProduct(Vector4 *a, Vector4 *b);
    void MotionState_InitAllSlavesNow(void *motionState, void *slaves, s32 arg2);
    void MotionState_ReleaseSlavesByType(MotionState *state, u8 type);
    s8 GameSettings_IsCheatEnabled(u32 cheat);
    s8 Player_IsHuman(void *player);
    void CarA_ComputeForces(CarA *car);
    void CarA_SlaveWeaponHeartbeat(CarA *car);
    void Thing_CollideWithForceBoxes(void *thing);
    s32 NavHolder_FindIndexOfNearestNavPoint(Vector4 *pos, s32 index);
    s32 RandomAdd_s32(s32 base, s32 range);
    f32 _fabs(f32 x);
    void SoundSource_Set(s32 id, s32 param, void *value);
    void CarA_ExecuteCombo(CarA *car);
    void Thing_PassiveTargeting(void *thing);
    CarA *car = thing;
    TurboFx fx __attribute__((aligned(16)));
    f32 engineVolume;
    f32 enginePitch;
    f32 windVolume;
    f32 windPitch;
    f32 maxRpm;
    f32 invMass;
    f32 speed;
    f32 spin;
    CarTrailBone *wheel;
    u32 i;

    car->prevVelocity = car->velocity;
    car->unk4FC += g_fDeltaTimeSec;
    if (car->unk4FC >= 1.0f) {
        car->unk4FC = 1.0f;
    }
    car->transmissionFlags &= ~0x20000;
    if (Timer_Interrupt300()) {
        car->transmissionFlags &= ~0x10000;
    }

    if (car->player != NULL) {
        switch (car->vehicleIface->driveMode) {
        case 0:
            break;
        case 1:
            CarA_PreMove_Control_HW(car);
            break;
        }

        if (car->turboCooldown >= 0) {
            if (!(car->transmissionFlags & 0x1000) && (car->player->buttonsHeld & 4) && car->turboTime > 0) {
                car->transmissionFlags |= 0x1000;
                if (car->unk460 == 0) {
                    fx.owner = car->player->vehicle;
                    fx.unk24 = 0;
                    fx.pos.x = car->worldMatrix.v[3].x - 8.0f * car->worldMatrix.v[2].x - 3.0f * car->worldMatrix.v[1].x;
                    fx.pos.y = car->worldMatrix.v[3].y - 8.0f * car->worldMatrix.v[2].y - 3.0f * car->worldMatrix.v[1].y;
                    fx.pos.z = car->worldMatrix.v[3].z - 8.0f * car->worldMatrix.v[2].z - 3.0f * car->worldMatrix.v[1].z;
                    fx.dir.x = car->worldMatrix.v[2].x - 0.2f * car->worldMatrix.v[1].x;
                    fx.dir.y = car->worldMatrix.v[2].y - 0.2f * car->worldMatrix.v[1].y;
                    fx.dir.z = car->worldMatrix.v[2].z - 0.2f * car->worldMatrix.v[1].z;
                    if (PSVECSquareMag(&fx.dir)) {
                        PSVECNormalize(&fx.dir, &fx.dir);
                    }
                    fx.unk28 = 0;
                    fx.vel = g_zeroVector;
                    fx.unk40 = 0;
                    ((EffectThing *)car->player->vehicle)->vtbl->spawn(car->player->vehicle, &fx, 0, 280.0f);
                }
                if (car->vehicleIface->numSlaves > 1) {
                    MotionState_InitAllSlavesNow(&car->motionState, car->vehicleIface->slaves + 0x10, 1);
                }
            }
            car->turboCooldown -= g_iDeltaTimeMS;
        } else if (car->player->buttonsHeld & 4) {
            car->turboCooldown = 700;
        }

        if (car->transmissionFlags & 0x1000) {
            if (car->player->throttle && !car->player->brake) {
                car->turboTime -= g_iDeltaTimeMS;
                if (GameSettings_IsCheatEnabled(5) && car->player != NULL && Player_IsHuman(car->player)) {
                    car->turboTime = 30000;
                }
                if (car->turboTime <= 0) {
                    car->transmissionFlags &= ~0x1000;
                    MotionState_ReleaseSlavesByType(&car->motionState, 1);
                }
            } else {
                car->transmissionFlags &= ~0x1000;
                MotionState_ReleaseSlavesByType(&car->motionState, 1);
            }
        }

        if (car->tiltState != 0) {
            if (car->tiltState != 3 && car->player->stickX < 0x40 && car->player->stickX > -0x40) {
                CarA_SetBodyOffset(car, car->vehicleIface->engine->bodyOffset[0],
                                   car->vehicleIface->engine->bodyOffset[1],
                                   car->vehicleIface->engine->bodyOffset[2]);
                car->tiltState = 0;
            }
            if (car->worldMatrix.v[1].y > 0.35f) {
                CarA_SetBodyOffset(car, car->vehicleIface->engine->bodyOffset[0],
                                   car->vehicleIface->engine->bodyOffset[1],
                                   car->vehicleIface->engine->bodyOffset[2]);
                car->tiltState = 0;
                car->stuckCounter = 0;
                car->unk45C = 0;
            }
        }
        if (car->stuckCounter > 600) {
            if (car->player->stickX > 0x40) {
                if (car->tiltState != 1) {
                    CarA_SetBodyOffset(car, car->tiltX - 1.0f, 1.0f - car->tiltY, 0.0f);
                    car->tiltState = 1;
                }
            } else if (car->player->stickX < -0x40) {
                if (car->tiltState != 2) {
                    CarA_SetBodyOffset(car, 1.0f - car->tiltX, 1.0f - car->tiltY, 0.0f);
                    car->tiltState = 2;
                }
            } else if (car->tiltState != 0) {
                CarA_SetBodyOffset(car, car->vehicleIface->engine->bodyOffset[0],
                                   car->vehicleIface->engine->bodyOffset[1],
                                   car->vehicleIface->engine->bodyOffset[2]);
                car->tiltState = 0;
            }
        }
    } else {
        car->gradeOrReverse = 0.0f;
        car->rpm = 0.0f;
        car->curveTableIdx = 1;
    }

    if (car->unk4FC >= 1.0f) {
        if (car->transmissionFlags & 0x4000) {
            MotionState_ReleaseSlavesByType(&car->motionState, 9);
            car->transmissionFlags &= ~0x4000;
        }
    } else if (!(car->transmissionFlags & 0x4000)) {
        if (car->vehicleIface->numSlaves > 9) {
            MotionState_InitAllSlavesNow(&car->motionState, car->vehicleIface->slaves + 0x70, 9);
            car->transmissionFlags |= 0x4000;
        }
    }

    CarA_ComputeForces(car);
    car->velocity.x += g_fDeltaTimeSec * (car->forwardAccel * car->worldMatrix.v[0].x);
    car->velocity.y += g_fDeltaTimeSec * (car->forwardAccel * car->worldMatrix.v[0].y);
    car->velocity.z += g_fDeltaTimeSec * (car->forwardAccel * car->worldMatrix.v[0].z);
    g_pSourcePlayer = car->player;
    CarA_SlaveWeaponHeartbeat(car);
    g_pSourcePlayer = NULL;

    invMass = g_fDeltaTimeSec / car->vehicleIface->collider->mass;
    if (car->collDirtyFlags & 0x10000) {
        car->torqueAccum = g_zeroVector;
        car->forceAccum = car->torqueAccum;
    } else {
        car->velocity.x += invMass * car->forceAccum.x;
        car->velocity.y += invMass * car->forceAccum.y;
        car->velocity.z += invMass * car->forceAccum.z;
        car->angularMomentum.x += g_fDeltaTimeSec * car->torqueAccum.x;
        car->angularMomentum.y += g_fDeltaTimeSec * car->torqueAccum.y;
        car->angularMomentum.z += g_fDeltaTimeSec * car->torqueAccum.z;
        CarA_UpdateAngularVelocity(car);
        car->torqueAccum = g_zeroVector;
        car->forceAccum = car->torqueAccum;
    }

    Thing_CollideWithForceBoxes(car);
    car->navUpdateTimer -= g_iDeltaTimeMS;
    if (car->navUpdateTimer < 0) {
        car->navPointIndex = NavHolder_FindIndexOfNearestNavPoint(&car->worldMatrix.v[3], car->navPointIndex);
        car->navUpdateTimer = RandomAdd_s32(300, 100);
    }
    car->lowSpeedFlag += g_iDeltaTimeMS;
    if (car->unk45C > 0) {
        car->unk45C -= g_iDeltaTimeMS;
        car->stuckCounter += g_iDeltaTimeMS;
    }

    if ((u32)car->soundIds[1] != 0xFFFFFFFF) {
        if (_fabs(car->unk4E0) < (maxRpm = car->vehicleIface->engine->unk68)) {
            engineVolume = (maxRpm - _fabs(car->rpm)) / maxRpm;
            enginePitch = car->vehicleIface->engine->unk64 + _fabs(car->rpm) / car->vehicleIface->unk1C;
            if (engineVolume < 0.0f) {
                engineVolume = 0.0f;
            }
            if (engineVolume > 1.0f) {
                engineVolume = 1.0f;
            }
            if (car->vehicleIface->numSoundSets != 0) {
                enginePitch *= car->vehicleIface->soundSets[1]->pitchScale;
            }
            SoundSource_Set(car->soundIds[1], 5, &car->worldMatrix.v[3]);
            SoundSource_Set(car->soundIds[1], 6, &car->velocity);
            SoundSource_Set(car->soundIds[1], 1, &enginePitch);
            SoundSource_Set(car->soundIds[1], 0, &engineVolume);
        }
    }
    if ((u32)car->soundIds[0] != 0xFFFFFFFF) {
        windVolume = 1.0f - _fabs(car->gradeOrReverse) / 300.0f;
        windPitch = 3.0f * (car->rpm / car->vehicleIface->unk1C);
        if (windPitch < 0.0f) {
            windPitch *= -1.0f;
        }
        windPitch += 0.5f;
        if (windPitch > 5.0f) {
            windPitch = 5.0f;
        }
        if (windVolume < 0.0f) {
            windVolume = 0.0f;
        }
        if (windVolume > 1.0f) {
            windVolume = 1.0f;
        }
        SoundSource_Set(car->soundIds[0], 0, &windVolume);
        SoundSource_Set(car->soundIds[0], 5, &car->worldMatrix.v[3]);
        SoundSource_Set(car->soundIds[0], 6, &car->velocity);
        SoundSource_Set(car->soundIds[0], 1, &windPitch);
    }

    speed = g_fDeltaTimeSec * PSVECDotProduct(&car->velocity, &car->worldMatrix.v[2]);
    spin = car->gradeOrReverse * g_fDeltaTimeSec;
    if (car->player != NULL && car->player->brake == 0) {
        for (i = 0; i < car->vehicleIface->numWheels; i++) {
            wheel = &car->vehicleIface->wheels[i];
            car->wheelStates[i].rotation += (wheel->unk14 * spin + (1.0f - wheel->unk14) * speed) / wheel->offset;
        }
    } else {
        for (i = 0; i < car->vehicleIface->numWheels; i++) {
            wheel = &car->vehicleIface->wheels[i];
            car->wheelStates[i].rotation += (wheel->unk14 * spin + (1.0f - wheel->unk14) * speed) / wheel->offset;
        }
    }
    if (car->player != NULL) {
        CarA_ExecuteCombo(car);
    }
    Thing_PassiveTargeting(car);
    return 1;
}

void CarA_SetSlavePlayers(u8 *car)
{
    u8 *slave;

    for (slave = *(u8 **)(car + 0x344); slave != NULL; slave = *(u8 **)(slave + 0xA4))
    {
        *(s32 *)(slave + 0x94) = *(s32 *)(car + 0x94);
    }
}

void CarA_DrawReflection(void *thing) {
    typedef struct { void *geom; u16 mtxIndex; u8 pad6[2]; } ModelPart;
    typedef struct {
        u32 numGeoms;
        void **geoms;
        u32 numParts;
        ModelPart *parts;
        u8 pad10[4];
        s32 *skel;
    } CarModel;
    void LightManager_LightThing(void *thing);
    void Gfx_DrawGeometry(void *geom, void *mtx);
    void CarA_Extract_Keyframes(CarA *car, void *frames, void *state);
    void TEST_AnimTraverseHeirarchyToMatricesWorld(Vector4 *quat, void *pos, void *frames, s32 skel, void *matrices, f32 time);
    void Create_Plane3(Vector4 *plane, Vector4 *normal, Vector4 *point);
    f32 Distance_Plane(void *plane, void *point);
    void Gfx_AnimTraverseHeirarchyToMatrices(void *frames, s32 skel, Matrix4 *matrices);
    void MotionState_DrawSlaves(void *state, s32 arg1);
    void MotionState_DrawSlavesAttachCoronaOnly(void *state, s32 arg1);
    Matrix4 matrices[64];
    u8 frames[0x800];
    u8 frames2[0x800];
    Matrix4 partMtx;
    Matrix4 mtx;
    Matrix4 tmp;
    Matrix4 partMtx2;
    Vector4 plane;
    Vector4 pos;
    CarA *car = thing;
    f32 offset = 0.0f;
    Matrix4 *oldMatrices = car->motionState.animMatrices;
    f32 h;

    if (car->motionState.model != NULL) {
        LightManager_LightThing(car);
        car->motionState.animMatrices = matrices;
        {
            s32 *skel = ((CarModel *)car->motionState.model)->skel;
            CarA_Extract_Keyframes(car, frames, car->motionState.model);
            XformVec(&car->worldMatrix, &car->bodyOffset, &pos);
            TEST_AnimTraverseHeirarchyToMatricesWorld(&car->animRefPos, &pos, frames, *skel, matrices, 1.0f);
        }
        Create_Plane3(&plane, &car->worldMatrix.v[1], &pos);
        h = -Distance_Plane(&plane, &matrices[car->vehicleIface->wheels->bone].v[3]);
        offset += 2.0f * h + 2.0f * car->vehicleIface->wheels->offset;
        {
            u32 i;
            for (i = 0; i < ((CarModel *)car->motionState.model)->numParts; i++) {
                ModelPart *e = &((CarModel *)car->motionState.model)->parts[i];
                Gfx_DrawGeometry(e->geom, &matrices[e->mtxIndex]);
            }
        }
        {
            u32 j;
            CarPartList *list = car->vehicleIface->modelRef->data->parts;
            CarPart *part;
            u32 bit;
            void *geom;
            if (list != NULL) {
                for (j = 0; j < list->count; j++) {
                    part = &list->parts[j];
                    geom = part->geom;
                    bit = car->partVisMask & (1 << j);
                    if (bit && !(part->flags & 1)) {
                        continue;
                    }
                    if (!bit && (part->flags & 1)) {
                        continue;
                    }
                    if (part->xform) {
                        partMtx = matrices[part->mtxIndex];
                        XformVec(&partMtx, list->parts[j].offset, &partMtx.v[3]);
                        Gfx_DrawGeometry(geom, &partMtx);
                    } else {
                        Gfx_DrawGeometry(geom, &matrices[part->mtxIndex]);
                    }
                }
            }
        }
        MotionState_DrawSlaves(&car->motionState, 0);
        mtx = car->worldMatrix;
        {
            u32 i;
            s32 *skel = ((CarModel *)car->motionState.model)->skel;
            CarA_Extract_Keyframes(car, frames2, car->motionState.model);
            Gfx_AnimTraverseHeirarchyToMatrices(frames2, *skel, matrices);
            mtx.v[1].x *= -1.0f;
            mtx.v[1].y *= -1.0f;
            mtx.v[1].z *= -1.0f;
            mtx.v[0].x *= -1.0f;
            mtx.v[0].y *= -1.0f;
            mtx.v[0].z *= -1.0f;
            XformVec(&car->worldMatrix, &car->bodyOffset, &mtx.v[3]);
            mtx.v[3].x += mtx.v[1].x * offset;
            mtx.v[3].y += mtx.v[1].y * offset;
            mtx.v[3].z += mtx.v[1].z * offset;
            for (i = 0; i < *skel; i++) {
                tmp = matrices[i];
                MultiplyMatrix(&tmp, &mtx, &matrices[i]);
            }
        }
        {
            u32 i;
            for (i = 0; i < ((CarModel *)car->motionState.model)->numParts; i++) {
                ModelPart *e = &((CarModel *)car->motionState.model)->parts[i];
                Gfx_DrawGeometry(e->geom, &matrices[e->mtxIndex]);
            }
        }
        {
            u32 j;
            CarPartList *list = car->vehicleIface->modelRef->data->parts;
            CarPart *part;
            u32 bit;
            void *geom;
            if (list != NULL) {
                for (j = 0; j < list->count; j++) {
                    part = &list->parts[j];
                    geom = part->geom;
                    bit = car->partVisMask & (1 << j);
                    if (bit && !(part->flags & 1)) {
                        continue;
                    }
                    if (!bit && (part->flags & 1)) {
                        continue;
                    }
                    if (part->xform) {
                        partMtx2 = matrices[part->mtxIndex];
                        XformVec(&partMtx2, list->parts[j].offset, &partMtx2.v[3]);
                        Gfx_DrawGeometry(geom, &partMtx2);
                    } else {
                        Gfx_DrawGeometry(geom, &matrices[part->mtxIndex]);
                    }
                }
            }
        }
        MotionState_DrawSlavesAttachCoronaOnly(&car->motionState, 0);
        car->motionState.animMatrices = oldMatrices;
    }
}

void CarA_DrawScript(u8 *car) {
    typedef struct { void *geom; u16 mtxIndex; u8 pad6[2]; } ModelPart;
    typedef struct {
        u32 numGeoms;
        void **geoms;
        u32 numParts;
        ModelPart *parts;
        u8 pad10[4];
        s32 *skel;
    } Model;
    typedef struct CarPart {
        u8 flags;
        u8 xform;
        u8 pad2[2];
        u16 mtxIndex;
        u8 pad6[2];
        void *geom;
        Vector4 *offset;
        u8 pad10[0xC];
    } CarPart;
    typedef struct CarPartList {
        u32 count;
        CarPart *parts;
    } CarPartList;
    void LightManager_LightThing(void *thing);
    void Gfx_DrawAnimGeometryAux(void *geom, void *mtx, s32 skel, s32 arg3);
    void Gfx_DrawGeometry_MatrixNotCached(void *geom, void *mtx);
    void Gfx_DrawGeometry(void *geom, void *mtx);
    Matrix4 *matrices;
    u32 i;
    s32 *skel;
    u32 j;
    CarPartList *list;
    CarPart *part;
    u32 bit;
    void *geom;
    Matrix4 mtx;

    matrices = *(Matrix4 **)(car + 0x340);
    if (matrices == NULL) {
        return;
    }
    LightManager_LightThing(car);
    skel = (*(Model **)(car + 0x330))->skel;
    for (i = 0; i < (*(Model **)(car + 0x330))->numGeoms; i++) {
        Gfx_DrawAnimGeometryAux((*(Model **)(car + 0x330))->geoms[i], matrices, *skel, 0);
    }
    for (i = 0; i < (*(Model **)(car + 0x330))->numParts; i++) {
        ModelPart *e = &(*(Model **)(car + 0x330))->parts[i];
        Gfx_DrawGeometry_MatrixNotCached(e->geom, &matrices[e->mtxIndex]);
    }
    list = *(CarPartList **)(*(u8 **)(*(u8 **)(*(u8 **)(car + 0x3BC) + 0x78) + 0x10) + 0x38);
    if (list != NULL) {
        for (j = 0; j < list->count; j++) {
            part = &list->parts[j];
            geom = part->geom;
            bit = *(u32 *)(car + 0x374) & (1 << j);
            if (bit && !(part->flags & 1)) {
                continue;
            }
            if (!bit && (part->flags & 1)) {
                continue;
            }
            if (part->xform) {
                mtx = matrices[part->mtxIndex];
                XformVec(&mtx, list->parts[j].offset, &mtx.v[3]);
                Gfx_DrawGeometry(geom, &mtx);
            } else {
                Gfx_DrawGeometry_MatrixNotCached(geom, &matrices[part->mtxIndex]);
            }
        }
    }
}

void CarA_Draw(u8 *car)
{
    s8 Player_IsHuman(void *player);
    s32 StateManager_CurrState(void);
    u32 ViewManager_GetViewFormat(void);
    void Gfx_DrawGeometry(void *, Matrix4 *);
    void CarA_DrawScript(u8 *);
    void XformVec(void *, Vector4 *, Vector4 *);
    void MotionState_DrawSlaves(void *, s32);
    extern void *g_pNoRayCollideThing;
    Matrix4 mtx __attribute__((aligned(16)));
    u8 *p2;

    if (*(u8 **)(car + 0x94) != NULL && (*(u32 *)(*(u8 **)(car + 0x94) + 0x6C) & 0x10) &&
        !(*(u32 *)(*(u8 **)(car + 0x94) + 0x6C) & 0x80) && ViewManager_GetViewFormat() <= 1) {
        p2 = *(u8 **)(*(u8 **)(car + 0x3BC) + 0x80);
        if ((*((u8 **)(p2 + 0x84)))[((*(u8 **)(car + 0x94))[0x31] % *(u32 *)(p2 + 0x80)) * 0xC] == 4) {
            return;
        }
    }
    CarA_DrawScript(car);
    if (*(void **)(*(u8 **)(car + 0x3BC) + 0x54) != NULL && StateManager_CurrState() == 5) {
        mtx = *(Matrix4 *)(car + 0x10);
        XformVec(car + 0x10, (Vector4 *)(car + 0x520), &mtx.v[3]);
        if (Player_IsHuman(*(void **)(car + 0x94))) {
            Gfx_DrawGeometry(*(void **)(*(u8 **)(car + 0x3BC) + 0x54), &mtx);
        }
    }
    g_pNoRayCollideThing = car;
    MotionState_DrawSlaves(car + 0x330, 0);
    g_pNoRayCollideThing = NULL;
}

f32 CarA_GetMass(u8 *car)
{
    u8 *coll;

    coll = *(u8 **)(car + 0x3BC);
    coll = *(u8 **)(coll + 0xA0);
    return *(f32 *)(coll + 0x20);
}

void CarA_DynamicRotation(u8 *car, f32 dt) {
    typedef struct OBox {
        Vector4 center;
        Vector4 corners[8];
        u8 pad[0x60];
    } OBox;
    typedef struct RayHit {
        Vector4 pos;
        Vector4 normal;
        u8 pad20[8];
        u8 *surface;
        u8 pad2C[0x24];
    } RayHit;
    extern u8 *g_pNoRayCollideThing;
    f32 MagnitudeVector(Vector4 *v);
    void GenerateQuaternion(Vector4 *axis, Quaternion *q, f32 angle);
    void QuaternionToSubMatrix(Quaternion *q, Matrix4 *m);
    void CarA_StuntsGetNewOrientation(u8 *car, Quaternion *q, f32 t);
    void *CarA_GetOrientedCollision(void *car);
    void BuildOOBOffset(void *oob, void *pos, void *rot, void *out);
    s8 RayHitsTerrainAndObjects(void *pos, void *vel, f32 *t, RayHit *hit);
    s8 OBoxHitsTerrainAndObjectsZeroTimeEdgesOnly(void *obox, void *thing, void *hit);
    void PSVECSubtract(void *a, void *b, Vector4 *out);
    void PSVECCrossProduct(void *a, void *b, Vector4 *out);
    OBox obox;
    RayHit hit;
    Matrix4 mtx;
    Quaternion q __attribute__((aligned(16)));
    Quaternion newQ __attribute__((aligned(16)));
    Vector4 axis __attribute__((aligned(16)));
    Vector4 dir __attribute__((aligned(16)));
    Vector4 r __attribute__((aligned(16)));
    Vector4 v __attribute__((aligned(16)));
    f32 t;
    f32 mag;
    u32 i;
    OBox *old;
    void *box;
    s8 hitAny;

    t = dt;
    axis = *(Vector4 *)(car + 0x410);
    mag = MagnitudeVector(&axis);
    if (mag > 0.0001f) {
        axis.x /= mag;
        axis.y /= mag;
        axis.z /= mag;
    }
    GenerateQuaternion(&axis, &q, mag * t);
    QuaternionMult((Vector4 *)&q, (Vector4 *)(car + 0x50), (Vector4 *)&newQ);
    CarA_StuntsGetNewOrientation(car, &newQ, t);
    QuaternionToSubMatrix(&newQ, &mtx);
    mtx.v[3] = *(Vector4 *)(car + 0x40);
    switch ((*(u8 **)(*(u8 **)(car + 0x3BC) + 0xA0))[0x10]) {
    case 0:
        box = *(void **)(*(u8 **)(*(u8 **)(car + 0x3BC) + 0xA0) + 0x44);
        old = CarA_GetOrientedCollision(car);
        hitAny = 0;
        BuildOOBOffset(box, &mtx, car + 0x520, &obox);
        g_pNoRayCollideThing = car;
        for (i = 0; i < 8; i++) {
            if (t > 0.0f) {
                dir.x = (obox.corners[i].x - old->corners[i].x) / dt;
                dir.y = (obox.corners[i].y - old->corners[i].y) / dt;
                dir.z = (obox.corners[i].z - old->corners[i].z) / dt;
                if (RayHitsTerrainAndObjects(&old->corners[i], &dir, &t, &hit)) {
                    hitAny = 1;
                    CarA_ProcessImpulse(car, &hit.normal, &hit.pos, &dir, *(f32 *)(hit.surface + 0x18));
                }
            }
        }
        g_pNoRayCollideThing = NULL;
        if (!hitAny) {
            if (OBoxHitsTerrainAndObjectsZeroTimeEdgesOnly(&obox, car, &hit)) {
                PSVECSubtract(&hit.pos, car + 0x40, &r);
                PSVECCrossProduct(car + 0x410, &r, &v);
                CarA_ProcessImpulse(car, &hit.normal, &hit.pos, &v, 0.5f);
            } else {
                *(Quaternion *)(car + 0x50) = newQ;
                *(Matrix4 *)(car + 0x10) = mtx;
                *(OBox *)(car + 0xC0) = obox;
                *(u8 **)(car + 0x160) = car + 0x30;
                *(u8 **)(car + 0x164) = car + 0x10;
                *(u8 **)(car + 0x168) = car + 0x20;
                *(u32 *)(car + 0x74) &= ~0x180;
            }
        }
        break;
    case 1:
        break;
    }
}

void CarA_ProcessDamageSystem(CarA *car, Vector4 *hitPos, s32 level, f32 impulse) {
    typedef struct DebrisParams {
        Vector4 pos;
        Vector4 quat;
        Vector4 vel;
        Vector4 angVel;
        u32 id;
        void *player;
        f32 unk48;
        f32 unk4C;
    } DebrisParams;
    typedef struct PartParams {
        Vector4 pos;
        Vector4 quat;
        Vector4 axis;
        Vector4 vel;
        f32 gravity;
        void *geom;
        s32 life;
        u32 unk4C;
        s32 unk50;
        u8 pad54[0xC];
    } PartParams;
    typedef struct SlaveParams {
        Vector4 pos;
        Vector4 quat;
        Vector4 vel;
        void *info;
        u32 unk34;
        f32 unk38;
        u8 pad3C[4];
    } SlaveParams;
    extern Vector4 g_zeroVector;
    void XformVec(void *mtx, void *in, Vector4 *out);
    f32 _fabs(f32 x);
    f32 PSVECSquareDistance(void *a, void *b);
    f32 RandomNumberGenerator_GetRandomFloat(f32 min, f32 max);
    void RandomNumberGenerator_GetRandomVector(Vector4 *v);
    f32 RandomAdd(f32 a, f32 b);
    void SubMatrixToQuaternion(void *mtx, Vector4 *quat);
    void *ThingManager_DARYL_CreateThing(s32 type, void *params);
    u8 *ThingManager_DARYL_CreateSlave(s32 type, void *params, u8 id);
    void ThingManagerList_Append(void *list, void *thing);
    PartParams partParams;
    DebrisParams debrisParams;
    SlaveParams slaveParams;
    Vector4 pos __attribute__((aligned(16)));
    Vector4 vel __attribute__((aligned(16)));
    Vector4 dir __attribute__((aligned(16)));
    Vector4 quat __attribute__((aligned(16)));
    u32 bit;
    u32 i;
    CarPartList *list;
    f32 scale;
    u8 *thing;
    u8 *slave;
    f32 absImpulse;
    u32 debris;

    absImpulse = _fabs(impulse);
    if (hitPos == NULL) {
        return;
    }
    if (car->vehicleIface->damageLevel > level && !(absImpulse > 70.0f)) {
        return;
    }
    if (!(100.0f * (f32)car->speed / (f32)car->vehicleIface->collider->maxHealth <= car->vehicleIface->damagePercent)) {
        return;
    }
    list = car->vehicleIface->modelRef->data->parts;
    if (list == NULL) {
        return;
    }
    for (i = 0; i < list->count; i++) {
        if (car->partVisMask & (bit = 1 << i)) {
            continue;
        }
        if (list->parts[i].xform) {
            XformVec(&car->motionState.animMatrices[list->parts[i].mtxIndex], list->parts[i].offset, &pos);
        } else {
            pos = car->motionState.animMatrices[list->parts[i].mtxIndex].v[3];
        }
        if (PSVECSquareDistance(&pos, hitPos) < 25.0f) {
            if (list->parts[i].flags == 0) {
                scale = RandomNumberGenerator_GetRandomFloat(40.0f, 80.0f);
                RandomNumberGenerator_GetRandomVector(&dir);
                vel = car->velocity;
                vel.x = dir.x * scale + vel.x;
                vel.y = dir.y * scale + vel.y;
                vel.z = dir.z * scale + vel.z;
                SubMatrixToQuaternion(&car->motionState.animMatrices[list->parts[i].mtxIndex], &quat);
                debris = list->parts[i].debris;
                partParams.life = 3000;
                partParams.gravity = -50.0f;
                partParams.vel = vel;
                partParams.pos = pos;
                partParams.geom = list->parts[i].geom;
                partParams.quat = quat;
                partParams.unk50 = 1;
                if (debris != 0) {
                    debrisParams.id = debris;
                    debrisParams.pos = pos;
                    debrisParams.quat = quat;
                    debrisParams.vel = vel;
                    debrisParams.player = NULL;
                    debrisParams.unk48 = 0.0f;
                    debrisParams.unk4C = 0.0f;
                    debrisParams.angVel = g_zeroVector;
                    ThingManager_DARYL_CreateThing(1, &debrisParams);
                }
                RandomNumberGenerator_GetRandomVector(&partParams.axis);
                partParams.axis.w = 0.017453292f * RandomAdd(100.0f, 1260.0f);
                thing = ThingManager_DARYL_CreateThing(6, &partParams);
                if (thing != NULL && list->parts[i].slaveInfo != NULL) {
                    slaveParams.unk38 = 0.0f;
                    slaveParams.quat.w = 1.0f;
                    slaveParams.quat.x = 0.0f;
                    slaveParams.quat.y = 0.0f;
                    slaveParams.quat.z = 0.0f;
                    slaveParams.pos = Vector4_OrZero(list->parts[i].slavePos);
                    slaveParams.info = list->parts[i].slaveInfo;
                    slaveParams.unk34 = 0;
                    slaveParams.vel = g_zeroVector;
                    slave = ThingManager_DARYL_CreateSlave(2, &slaveParams, 0xFF);
                    if (slave != NULL) {
                        slave[0x83] = 0xFF;
                        ThingManagerList_Append(thing + 0x88, slave);
                    }
                }
            }
            car->partVisMask |= bit;
        }
    }
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

void CarA_GetInterfaceInfo(u8 *car, u8 *info) {
    void *ThingManagerList_GetByType(void *list, s32 type);
    s8 GameSettings_IsCheatEnabled(s32 cheat);
    u32 GameSettings_GetCodeVariable(s32, s32);
    u32 GameSettings_GetNumOfPlayers(void);
    u32 CameraManager_GetActiveCameras(void);
    f32 _fabs(f32 x);
    typedef struct {
        u32 active;
        u8 unk4[0x3C];
        s16 ammo;
        u8 unk42[0xA];
        u8 *item;
    } RackSlot;
    typedef struct {
        u8 unk0[0xB0];
        RackSlot slots[12];
        u8 current;
    } WeaponRack;
    WeaponRack *rack;
    u32 count;

    rack = ThingManagerList_GetByType(car + 0x344, 8);
    *(u32 *)(info + 0x10) = 100.0f * ((f32)*(s32 *)(car + 0x78) / (f32)*(s32 *)(*(u8 **)(*(u8 **)(car + 0x3BC) + 0xA0) + 0x14));
    if (*(u32 *)(info + 0x10) > 100.0f) {
        *(u32 *)(info + 0x10) = 100;
    }
    if (rack != NULL && rack->slots[rack->current].item != NULL) {
        if (GameSettings_IsCheatEnabled(0)) {
            rack->slots[rack->current].ammo = *(u32 *)(*(u8 **)(rack->slots[rack->current].item + 0x30) + 0x28);
        }
        if (*(u32 *)(*(u8 **)(rack->slots[rack->current].item + 0x30) + 0x24) != 0) {
            *(u32 *)(info + 0x38) = 100.0f * ((f32)rack->slots[rack->current].ammo / (f32)*(u32 *)(*(u8 **)(rack->slots[rack->current].item + 0x30) + 0x28));
            *(u32 *)(info + 0x3C) = *(u32 *)(info + 0x38) / *(u32 *)(*(u8 **)(rack->slots[rack->current].item + 0x30) + 0x24);
        } else {
            *(u32 *)(info + 0x38) = 0;
            *(u32 *)(info + 0x3C) = 0;
        }
        if (*(u8 **)(rack->slots[rack->current].item + 0x30) != NULL) {
            *(u32 *)(info + 0x68) = *(u32 *)(*(u8 **)(rack->slots[rack->current].item + 0x30) + 0x10);
        }
    }
    *(u32 *)(info + 0x14) = _fabs(0.681818f * *(f32 *)(car + 0x458));
    if (*(s8 *)(car + 0x584) && *(u32 *)(info + 0x14) < *(u32 *)(car + 0x58C)) {
        (*(u32 *)(car + 0x588))++;
        if (*(u32 *)(car + 0x588) > 5) {
            *(s8 *)(car + 0x584) = 0;
            *(u32 *)(car + 0x588) = 0;
            *(u32 *)(car + 0x58C) = *(u32 *)(info + 0x14);
        } else {
            *(u32 *)(info + 0x14) = *(u32 *)(car + 0x58C);
        }
    } else if (*(s8 *)(car + 0x584)) {
        *(u32 *)(car + 0x588) = 0;
        *(u32 *)(car + 0x58C) = *(u32 *)(info + 0x14);
    }
    if (!*(s8 *)(car + 0x584) && *(u32 *)(info + 0x14) > *(u32 *)(car + 0x58C)) {
        (*(u32 *)(car + 0x588))++;
        if (*(u32 *)(car + 0x588) > 5) {
            *(s8 *)(car + 0x584) = 1;
            *(u32 *)(car + 0x588) = 0;
            *(u32 *)(car + 0x58C) = *(u32 *)(info + 0x14);
        } else {
            *(u32 *)(info + 0x14) = *(u32 *)(car + 0x58C);
        }
    } else if (!*(s8 *)(car + 0x584)) {
        *(u32 *)(car + 0x588) = 0;
        *(u32 *)(car + 0x58C) = *(u32 *)(info + 0x14);
    }
    *(u32 *)(info + 0x18) = *(f32 *)(car + 0x4E4);
    *(f32 *)(info + 0x20) = *(s32 *)(car + 0x400) / 300;
    *(f32 *)(info + 0x2C) = 0.2f * (100.0f + *(f32 *)(car + 0x44));
    if (*(u8 **)(car + 0x94) != NULL && (*(u32 *)(*(u8 **)(car + 0x94) + 0x6C) & 0x200)) {
        if (GameSettings_GetCodeVariable(0x198, 0)) {
            count = CameraManager_GetActiveCameras();
        } else {
            count = GameSettings_GetNumOfPlayers();
        }
        if (count > 1) {
            if (count == 2) {
                *(u32 *)(info + 0x4) = *(u32 *)(*(u8 **)(*(u8 **)(car + 0x3BC) + 0x84) + 0x18);
                if (*(u32 *)(*(u8 **)(car + 0x94) + 0x64) == 1) {
                    *(s32 *)(info + 0x8) = 0;
                    *(s32 *)(info + 0xC) = 10;
                } else {
                    *(s32 *)(info + 0x8) = 0;
                    *(s32 *)(info + 0xC) = 230;
                }
            } else {
                *(u32 *)(info + 0x4) = *(u32 *)(*(u8 **)(*(u8 **)(car + 0x3BC) + 0x84) + 0x1C);
                if (*(u32 *)(*(u8 **)(car + 0x94) + 0x64) == 1) {
                    *(s32 *)(info + 0x8) = 10;
                    *(s32 *)(info + 0xC) = 10;
                } else if (*(u32 *)(*(u8 **)(car + 0x94) + 0x64) == 2) {
                    *(s32 *)(info + 0x8) = 310;
                    *(s32 *)(info + 0xC) = 10;
                } else if (*(u32 *)(*(u8 **)(car + 0x94) + 0x64) == 3) {
                    *(s32 *)(info + 0x8) = 10;
                    *(s32 *)(info + 0xC) = 230;
                } else {
                    *(s32 *)(info + 0x8) = 310;
                    *(s32 *)(info + 0xC) = 230;
                }
            }
        } else {
            *(u32 *)(info + 0x4) = *(u32 *)(*(u8 **)(*(u8 **)(car + 0x3BC) + 0x84) + 0x10);
            *(s32 *)(info + 0x8) = 0;
            *(s32 *)(info + 0xC) = 0;
        }
    }
    if (rack != NULL && rack->slots[rack->current].active != 0) {
        info[0x34] = 1;
        *(s16 *)(info + 0x6C) = *(u32 *)(*(u8 **)(rack->slots[rack->current].item + 0x30) + 0x18);
    }
    if (*(u32 *)(car + 0x494) != 0) {
        *(u32 *)(info + 0x44) = *(u32 *)(car + 0x494);
        *(u32 *)(car + 0x494) = 0;
    }
    if (*(u32 *)(car + 0x498) != 0) {
        *(u32 *)(info + 0x40) = *(u32 *)(car + 0x498);
        *(u32 *)(car + 0x498) = 0;
    }
    if (*(u32 *)(car + 0x49C) != 0) {
        *(u32 *)(info + 0x48) = *(u32 *)(car + 0x49C);
        *(u32 *)(car + 0x49C) = 0;
    }
    if (*(u32 *)(car + 0x4A0) != 0) {
        *(u32 *)(info + 0x4C) = *(u32 *)(car + 0x4A0);
        *(u32 *)(car + 0x4A0) = 0;
    }
}

#pragma push
#pragma use_lmw_stmw on
void CarA_DynamicCollideImpact(void *thing, void *collision, s32 damage, f32 impulse) {
    typedef struct CollideSurface {
        u8 pad00[4];
        f32 minImpactA;
        f32 maxImpactA;
        f32 minImpactB;
        f32 maxImpactB;
        f32 bounce;
        f32 grip;
        u8 pad1C[0x44 - 0x1C];
        u32 numSoundsA;
        u32 *soundsA;
        u32 numSoundsB;
        u32 *soundsB;
    } CollideSurface;
    typedef struct CollideInfo {
        Vector4 pos;
        Vector4 normal;
        CarA *thingA;
        CarA *thingB;
        CollideSurface *surface;
    } CollideInfo;
    typedef struct DropItem {
        Vector4 velocity;
        Matrix4 mtx;
        u8 pad50[0x24];
        u32 flags;
        u8 pad78[8];
        s16 gridCell;
        u8 bone;
        u8 pad83[9];
        ThingVTable *vtable;
        u8 pad90[0x3C];
        s16 timer;
    } DropItem;
    typedef struct DeathInfo {
        u8 pad00[0x10];
        f32 height;
        u8 pad14[0x14];
        u32 sound;
    } DeathInfo;
    typedef struct DeathTile {
        u8 pad00[0x50];
        DeathInfo *death;
    } DeathTile;
    typedef struct DeathParams {
        Vector4 quat;
        Vector4 pos;
        Vector4 vel;
        DeathInfo *death;
        f32 unk34;
        u8 unk38;
    } DeathParams;
    typedef struct SpawnParams {
        Vector4 pos;
        Vector4 quat;
        Vector4 vel;
        Vector4 angVel;
        u32 id;
        void *player;
        f32 unk48;
        f32 unk4C;
    } SpawnParams;
    extern u8 g_carAVTable[];
    extern u8 g_itemVTable[];
    extern u8 g_destructibleVTable[];
    extern u8 g_physicalObjectVTable[];
    extern Vector4 g_zeroVector;
    f32 PSVECDotProduct(void *a, void *b);
    void PSVECSubtract(void *a, void *b, Vector4 *out);
    void PSVECCrossProduct(void *a, void *b, Vector4 *out);
    f32 PSVECSquareMag(Vector4 *v);
    void PSVECNormalize(Vector4 *in, Vector4 *out);
    f32 _fabs(f32 x);
    f32 Sqrt(f32 x);
    f64 acos(f64 x);
    f32 MagnitudeVector(Vector4 *v);
    void ProjectVectorToNormal(void *normal, void *v);
    void XformVec(void *mtx, void *in, Vector4 *out);
    void XformVecOrientation(void *mtx, Vector4 *in, Vector4 *out);
    void InverseXformVecOrientation(void *mtx, Vector4 *in, Vector4 *out);
    void MultiplyMatrix(void *a, void *b, Matrix4 *out);
    void GenerateQuaternion(Vector4 *axis, Vector4 *q, f32 angle);
    void Create_Plane(Vector4 *plane, Vector4 *normal, Vector4 *point);
    f32 Distance_Plane(Vector4 *plane, Vector4 *point);
    void CarA_StuntFail(CarA *car);
    void CarA_ProcessDamageSystem(CarA *car, CollideInfo *coll, s32 damage, f32 impulse);
    s8 GameSettings_IsCheatEnabled(s32 cheat);
    s8 Player_IsHuman(void *player);
    void MotionState_ReleaseSlavesByType(MotionState *state, u8 type);
    void *ThingManagerList_GetByType(void *list, s32 type);
    void ThingManagerList_Remove(void *list, void *thing);
    void ThingManagerList_Append(void *list, void *thing);
    void ThingManagerList_RemoveReleasedThings(void *list);
    s8 RayHitsTerrain(void *start, void *end, f32 *t, void *hit);
    void Trigger_ItemDropped(CarA *car, void *item);
    void Thing_AddToGrid(void *thing, s32 type);
    void Input_SetForceFeedback(s32 chan, s32 type, f32 strength, f32 time);
    f32 Gfx_GetMinCameraDistSquared(Vector4 *pos);
    void *ThingManager_DARYL_CreateThing(s32 type, void *params);
    void Thing_ProcessEndEvent(CarA *car, void *collider, CollideInfo *coll, Vector4 *axis);
    void Wheel_StopCarAirborne(s32 chan);
    void Wheel_PlayFrontalCollisionForce(s32 chan, s32 force);
    CollideInfo *coll = collision;
    CarA *car = thing;
    SpawnParams sp;
    SpawnParams snd;
    SpawnParams ex;
    Matrix4 m;
    DeathParams dp;
    Vector4 n __attribute__((aligned(16)));
    Vector4 p __attribute__((aligned(16)));
    Vector4 vel __attribute__((aligned(16)));
    Vector4 plane __attribute__((aligned(16)));
    Vector4 itemPos __attribute__((aligned(16)));
    Vector4 rel __attribute__((aligned(16)));
    Vector4 refUp __attribute__((aligned(16)));
    Vector4 axis __attribute__((aligned(16)));
    Vector4 imp __attribute__((aligned(16)));
    Vector4 torque __attribute__((aligned(16)));
    Vector4 r __attribute__((aligned(16)));
    Vector4 c2 __attribute__((aligned(16)));
    Vector4 nn __attribute__((aligned(16)));
    Vector4 local __attribute__((aligned(16)));
    Vector4 w __attribute__((aligned(16)));
    f32 t;
    f32 d;
    f32 d2;
    f32 dist;
    f32 dv;
    f32 adv;
    f32 dur;
    f32 mag;
    f32 rem;
    f32 bounce;
    f32 bx;
    f32 by;
    f32 bz;
    f32 cosA;
    f32 a;
    f32 b;
    f32 c;
    f32 s;
    f32 tw;
    f32 sgn;
    f32 maxW;
    f32 mag2;
    f32 scale;
    f32 fb;
    DropItem *item;
    DropItem *rack;
    DeathInfo *death;
    CarA *other;
    void *spawned;
    Player *player;
    u32 id;
    u32 exId;
    s32 idx;

    n = coll->normal;
    p = coll->pos;
    vel = car->velocity;
    if (car->worldMatrix.v[1].y < 0.3f) {
        d = PSVECDotProduct(&car->worldMatrix.v[1], &n);
        d2 = PSVECDotProduct(&car->worldMatrix.v[1], &car->velocity);
        if (impulse < 0.0f) {
            d *= -1.0f;
        }
        if (d < -0.5f && d2 > 0.0f) {
            if (coll->thingB == NULL || coll->thingA->player != coll->thingB->player) {
                CarA_StuntFail(car);
            }
            car->unk45C = 100;
            car->angularMomentum.x *= 0.5f;
            car->angularMomentum.y *= 0.5f;
            car->angularMomentum.z *= 0.5f;
            CarA_UpdateAngularVelocity(car);
            car->velocity.x *= 0.5f;
            car->velocity.y *= 0.5f;
            car->velocity.z *= 0.5f;
        }
        if (_fabs(PSVECDotProduct(&car->worldMatrix.v[0], &n)) > 0.7f) {
            car->unk45C = 100;
        }
        if (_fabs(PSVECDotProduct(&car->worldMatrix.v[2], &n)) > 0.7f) {
            if (car->tiltState == 0) {
                CarA_SetBodyOffset(car, car->vehicleIface->engine->bodyOffset[0], 1.0f - car->tiltY,
                                   car->vehicleIface->engine->bodyOffset[2]);
                car->tiltState = 3;
            }
        } else if (car->tiltState == 3) {
            CarA_SetBodyOffset(car, car->vehicleIface->engine->bodyOffset[0], car->vehicleIface->engine->bodyOffset[1],
                               car->vehicleIface->engine->bodyOffset[2]);
            car->tiltState = 0;
        }
        if (coll->normal.y > 0.7f && car->groundNormal.y < 0.5f) {
            car->groundNormal.x = 0.0f;
            car->groundNormal.y = 1.0f;
            car->groundNormal.z = 0.0f;
        }
        car->transmissionFlags &= ~0x80000;
        MotionState_ReleaseSlavesByType(&car->motionState, 8);
    }
    if (coll->thingB == NULL && PSVECDotProduct(&coll->normal, &car->worldMatrix.v[2]) < -0.7f) {
        Create_Plane(&plane, &car->worldMatrix.v[1], &car->worldMatrix.v[3]);
        dist = Distance_Plane(&plane, &p);
        if (dist < 5.0f && dist > 0.0f) {
            d = PSVECDotProduct(&n, &car->worldMatrix.v[1]);
            n.x = n.x - d * car->worldMatrix.v[1].x;
            n.y = n.y - d * car->worldMatrix.v[1].y;
            n.z = n.z - d * car->worldMatrix.v[1].z;
            dist *= 0.5f;
            p.x = p.x - car->worldMatrix.v[1].x * dist;
            p.y = p.y - car->worldMatrix.v[1].y * dist;
            p.z = p.z - car->worldMatrix.v[1].z * dist;
        }
    }
    damage -= car->vehicleIface->collider->damageThreshold;
    if (damage > 0 && car->damageRpm >= 10000) {
        CarA_ProcessDamageSystem(car, coll, damage, impulse);
        car->damageAccum += damage;
        if (car->damageAccum > 30) {
            while (car->damageAccum > 30) {
                car->speed -= 30;
                car->damageAccum -= 30;
            }
            if (GameSettings_IsCheatEnabled(6) && car->player != NULL && Player_IsHuman(car->player)) {
                car->speed = car->vehicleIface->collider->maxHealth;
            }
            CarA_UpdateDamageSlaves(car);
        }
    }
    if (coll->thingB != NULL && (u8 *)coll->thingA->vtable == g_carAVTable && (u8 *)coll->thingB->vtable == g_carAVTable &&
        _fabs(impulse) > car->vehicleIface->engine->itemDropImpact) {
        item = ThingManagerList_GetByType(&car->motionState.slaves, 4);
        if (item != NULL) {
            t = 1.0f;
            rack = ThingManagerList_GetByType(&car->motionState.slaves, 8);
            rack->flags &= 0xFFBFFDFF;
            XformVec(&car->motionState.animMatrices[item->bone], &item->mtx.v[3], &itemPos);
            if (!RayHitsTerrain(&car->worldMatrix.v[3], &itemPos, &t, NULL)) {
                Trigger_ItemDropped(car, item);
                ThingManagerList_Remove(&car->motionState.slaves, item);
                MultiplyMatrix(&item->mtx, &car->worldMatrix, &m);
                item->mtx = m;
                ThingManagerList_Append(&g_thingManager_DARYL.activeList, item);
                item->timer = 1000;
                item->flags &= 0xFFFBFFF5;
                item->velocity = car->velocity;
                item->velocity.y += 2.0f;
                if (item->gridCell != 0 && (u8 *)item->vtable == g_itemVTable) {
                    Thing_AddToGrid(item, 3);
                }
            }
        }
    }
    dv = impulse / car->vehicleIface->collider->mass;
    adv = _fabs(dv);
    car->velocity.x += n.x * dv;
    car->velocity.y += n.y * dv;
    car->velocity.z += n.z * dv;
    if (car->player != NULL && Player_IsHuman(car->player) && adv > 0.0f) {
        mag = 0.01f * adv;
        player = car->player;
        dur = 0.3f;
        rem = mag;
        if (mag > 1.0f) {
            mag = 1.0f;
        }
        if (rem > 2.0f) {
            rem -= 1.0f;
            Input_SetForceFeedback(player->wheelChannel, 0, 1.0f, 0.1f);
        }
        while (rem > 1.0f) {
            rem *= 0.5f;
            dur += 0.2f;
            if (dur > 1.5f) {
                dur = 1.5f;
                break;
            }
        }
        Input_SetForceFeedback(player->wheelChannel, 1, mag, dur);
    }
    if (coll->surface != NULL || coll->thingB != NULL) {
        if (coll->surface == NULL) {
            if (coll->thingA == car) {
                coll->surface = coll->thingB->vtable->getSurface(coll->thingB);
            } else {
                coll->surface = coll->thingA->vtable->getSurface(coll->thingA);
            }
        }
        if (impulse > 0.0f) {
            bounce = 0.5f * -coll->surface->bounce;
        } else {
            bounce = 0.5f * coll->surface->bounce;
        }
        if (0.0f != impulse && PSVECDotProduct(&coll->normal, &car->worldMatrix.v[1]) < 0.5f) {
            bx = car->velocity.x * bounce;
            by = car->velocity.y * bounce;
            bz = car->velocity.z * bounce;
            car->velocity.x = car->velocity.x + bx;
            car->velocity.y = car->velocity.y + by;
            car->velocity.z = car->velocity.z + bz;
            n.x += 0.1f * bx;
            n.y += 0.1f * by;
            n.z += 0.1f * bz;
        }
        if (coll->thingB == NULL && Gfx_GetMinCameraDistSquared(&car->worldMatrix.v[3]) < 250000.0f) {
            {
                Vector4 up = {0.0f, 1.0f, 0.0f, 0.0f};
                Vector4 side = {1.0f, 0.0f, 0.0f, 0.0f};

                refUp = up;
                axis = side;
            }
            id = 0;
            cosA = PSVECDotProduct(&coll->normal, &refUp);
            PSVECSubtract(&coll->pos, &car->worldMatrix.v[3], &rel);
            if (PSVECDotProduct(&car->worldMatrix.v[1], &rel) < 0.0f) {
                idx = (s32)((f32)coll->surface->numSoundsA * ((dv - coll->surface->minImpactA) / (coll->surface->maxImpactA - coll->surface->minImpactA)) - 1.0f);
                if (idx >= (s32)coll->surface->numSoundsA) {
                    idx = coll->surface->numSoundsA - 1;
                }
                if (idx >= 0 && coll->surface->numSoundsA != 0) {
                    id = coll->surface->soundsA[idx];
                }
            } else {
                idx = (s32)((f32)coll->surface->numSoundsB * ((dv - coll->surface->minImpactB) / (coll->surface->maxImpactB - coll->surface->minImpactB)) - 1.0f);
                if (idx >= (s32)coll->surface->numSoundsB) {
                    idx = coll->surface->numSoundsB - 1;
                }
                if (idx >= 0 && coll->surface->numSoundsB != 0) {
                    id = coll->surface->soundsB[idx];
                }
            }
            if (id != 0) {
                if (cosA < -0.99f) {
                    GenerateQuaternion(&axis, &sp.quat, 3.1415927f);
                } else if (cosA > 0.99f) {
                    sp.quat.x = sp.quat.y = sp.quat.z = 0.0f;
                    sp.quat.w = 1.0f;
                } else {
                    a = acos(cosA);
                    PSVECCrossProduct(&refUp, &coll->normal, &axis);
                    if (PSVECSquareMag(&axis)) {
                        PSVECNormalize(&axis, &axis);
                    }
                    GenerateQuaternion(&axis, &sp.quat, a);
                }
                sp.id = id;
                sp.pos = coll->pos;
                sp.vel = car->velocity;
                sp.player = car->player;
                sp.unk48 = 0.0f;
                sp.unk4C = 0.0f;
                sp.angVel = g_zeroVector;
                ThingManager_DARYL_CreateThing(1, &sp);
            }
        }
    }
    PSVECSubtract(&p, &car->worldMatrix.v[3], &r);
    imp.x = impulse * n.x;
    imp.y = impulse * n.y;
    imp.z = impulse * n.z;
    PSVECCrossProduct(&r, &imp, &torque);
    a = PSVECDotProduct(&r, &car->worldMatrix.v[0]);
    PSVECCrossProduct(&imp, &r, &c2);
    b = PSVECDotProduct(&c2, &car->worldMatrix.v[1]);
    c = PSVECDotProduct(&r, &car->worldMatrix.v[2]);
    if (_fabs(PSVECDotProduct(&coll->normal, &car->worldMatrix.v[1])) < 0.3f && c > 0.0f && car->numWheelsGrounded != 0 &&
        a * b < 0.0f && PSVECDotProduct(&vel, &car->worldMatrix.v[2]) > _fabs(PSVECDotProduct(&vel, &car->worldMatrix.v[0]))) {
        s = PSVECDotProduct(&car->worldMatrix.v[2], &car->velocity);
        tw = PSVECDotProduct(&car->worldMatrix.v[1], &torque);
        if (coll->thingB == NULL ||
            (((CollideSurface *)coll->thingA->vtable->getSurface(coll->thingA))->grip > 0.0f &&
             ((CollideSurface *)coll->thingB->vtable->getSurface(coll->thingB))->grip > 0.0f)) {
            if (car->player != NULL && car->player->throttle == 0) {
                car->transmissionFlags |= 0x10;
            } else {
                car->unk4FC = 0.0f;
            }
        }
        if (coll->thingB != NULL && (!(coll->thingA->collDirtyFlags & 8) || coll->thingA->speed == 0) &&
            (!(coll->thingB->collDirtyFlags & 8) || coll->thingB->speed == 0)) {
            ProjectVectorToNormal(&car->worldMatrix.v[1], &torque);
        } else {
            sgn = 1.0f;
            torque.x = torque.x - tw * car->worldMatrix.v[1].x;
            torque.y = torque.y - tw * car->worldMatrix.v[1].y;
            torque.z = torque.z - tw * car->worldMatrix.v[1].z;
            if (tw < 0.0f) {
                sgn = -1.0f;
                tw *= sgn;
            }
            if (tw < 50.0f) {
                tw = 50.0f;
            }
            if (tw > 500.0f) {
                tw = 500.0f;
            }
            tw *= sgn;
            torque.x = torque.x - tw * car->worldMatrix.v[1].x;
            torque.y = torque.y - tw * car->worldMatrix.v[1].y;
            torque.z = torque.z - tw * car->worldMatrix.v[1].z;
        }
        if (coll->thingA == car) {
            if (s < 0.0f) {
                ProjectVectorToNormal(&car->velocity, &n);
            }
        } else if (s > 0.0f) {
            ProjectVectorToNormal(&car->velocity, &n);
        }
    } else if (_fabs(impulse) > 10.0f && coll->thingB == NULL && car->numWheelsGrounded != 0) {
        if (_fabs(PSVECDotProduct(&car->worldMatrix.v[1], &coll->normal)) < 0.3f) {
            if (_fabs(PSVECDotProduct(&car->worldMatrix.v[0], &coll->normal)) < 0.3f && c > 0.0f) {
                car->transmissionFlags |= 0x2000;
            }
        }
    }
    car->angularMomentum.x += torque.x;
    car->angularMomentum.y += torque.y;
    car->angularMomentum.z += torque.z;
    if (car->player != NULL && Player_IsHuman(car->player) && !(car->transmissionFlags & 0x10000)) {
        nn = coll->normal;
        fb = 255.0f * _fabs(impulse / 1000.0f);
        ProjectVectorToNormal(&car->worldMatrix.v[0], &nn);
        fb = fb * MagnitudeVector(&nn);
        if (fb > 20.0f) {
            player = car->player;
            Wheel_StopCarAirborne(player->wheelChannel);
            Wheel_PlayFrontalCollisionForce(player->wheelChannel, fb <= 255.0f ? fb : 255.0f);
            car->transmissionFlags |= 0x10000;
        }
    }
    if (car->numWheelsGrounded <= 1) {
        maxW = car->vehicleIface->engine->maxAngularVel;
        InverseXformVecOrientation(&car->worldMatrix, &car->angularMomentum, &local);
        w.x = car->inverseBodyInertia.v[0].x * local.x;
        w.y = car->inverseBodyInertia.v[1].y * local.y;
        w.z = car->inverseBodyInertia.v[2].z * local.z;
        mag2 = PSVECSquareMag(&w);
        if (mag2 > maxW * maxW) {
            scale = maxW / Sqrt(mag2);
            w.x *= scale;
            w.y *= scale;
            w.z *= scale;
            local.x = w.x / car->inverseBodyInertia.v[0].x;
            local.y = w.y / car->inverseBodyInertia.v[1].y;
            local.z = w.z / car->inverseBodyInertia.v[2].z;
            XformVecOrientation(&car->worldMatrix, &local, &car->angularMomentum);
        }
    }
    CarA_UpdateAngularVelocity(car);
    if (car->speed <= 0) {
        if (car->tileData != NULL && (death = ((DeathTile *)car->tileData)->death) != NULL) {
            dp.pos = car->worldMatrix.v[3];
            dp.pos.y += death->height;
            dp.vel.x = 0.0f;
            dp.vel.y = 0.0f;
            dp.vel.z = 0.0f;
            dp.death = death;
            dp.quat.w = 1.0f;
            dp.quat.x = 0.0f;
            dp.quat.y = 0.0f;
            dp.quat.z = 0.0f;
            dp.unk34 = 0.0f;
            dp.unk38 = 0;
            spawned = ThingManager_DARYL_CreateThing(4, &dp);
            if (spawned != NULL) {
                ((CarA *)spawned)->collDirtyFlags |= 1;
                if (death->sound != 0) {
                    snd.id = death->sound;
                    snd.pos = car->worldMatrix.v[3];
                    snd.quat = dp.quat;
                    snd.vel.z = 0.0f;
                    snd.vel.y = 0.0f;
                    snd.vel.x = 0.0f;
                    snd.player = NULL;
                    snd.unk48 = 0.0f;
                    snd.angVel = g_zeroVector;
                    ThingManager_DARYL_CreateThing(1, &snd);
                }
            }
        }
        if (coll->thingA == car) {
            other = coll->thingB;
        } else {
            other = coll->thingA;
        }
        if (car->player != NULL) {
            if (other != NULL && other->player != NULL && Player_IsHuman(other->player)) {
                car->player->flags |= 0x40;
            } else {
                car->player->flags &= ~0x40;
            }
        }
        if (!Player_IsHuman(car->player)) {
            exId = car->vehicleIface->explosionThing;
            if (exId != 0) {
                ex.id = exId;
                ex.pos = car->worldMatrix.v[3];
                ex.quat = car->animRefPos;
                ex.vel = g_zeroVector;
                ex.player = car->player;
                ex.unk48 = 0.0f;
                ex.angVel = g_zeroVector;
                ThingManager_DARYL_CreateThing(1, &ex);
            }
        }
        Thing_ProcessEndEvent(car, car->vehicleIface->collider, coll, &car->localAxis);
        if (other != NULL && other->player != NULL) {
            other->player->kills++;
        }
        CarA_Release((u8 *)car);
    }
    if (coll->thingB == NULL || (u8 *)coll->thingA->vtable == g_destructibleVTable ||
        (u8 *)coll->thingA->vtable == g_physicalObjectVTable || (u8 *)coll->thingB->vtable == g_destructibleVTable ||
        (u8 *)coll->thingB->vtable == g_physicalObjectVTable) {
        car->transmissionFlags &= ~2;
        MotionState_ReleaseSlavesByType(&car->motionState, 4);
        ThingManagerList_RemoveReleasedThings(&car->motionState.slaves);
    }
    car->lowSpeedFlag = 0;
}
#pragma pop
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

void CarA_GetCameraOrientationInfo(CarA *car, u32 index, void *out) {
    void Thing_GetCameraOrientationInfo(void *thing, u32 index, void *out);
    u32 ViewManager_GetViewFormat(void);
    f32 PSVECSquareMag(Vector4 *v);
    void PSVECNormalize(Vector4 *in, Vector4 *out);
    void PSVECSubtract(Vector4 *a, Vector4 *b, Vector4 *out);
    void XformVec(void *mtx, void *in, Vector4 *out);
    typedef struct CameraOrientationInfo {
        u8 pad00[0x30];
        Vector4 up;
        u8 pad40[0x60 - 0x40];
        Vector4 pos;
        Vector4 target;
        u8 pad80[0x84 - 0x80];
        u8 *camera;
        u32 numCameras;
        f32 unk8C;
    } CameraOrientationInfo;
    typedef struct CarCameras {
        u32 count;
        u8 *list;
    } CarCameras;
    CameraOrientationInfo *info = out;
    CarCameras *cams;
    Vector4 offset __attribute__((aligned(16)));

    cams = (CarCameras *)((u8 *)car->vehicleIface->engine + 0x80);
    if (ViewManager_GetViewFormat() > 1) {
        cams = (CarCameras *)((u8 *)car->vehicleIface->engine + 0x88);
    }
    info->numCameras = cams->count;
    if (cams->count == 0) {
        Thing_GetCameraOrientationInfo(car, index, info);
    } else {
        info->camera = cams->list + (index % cams->count) * 0xC;
        info->unk8C = car->groundedScale;
        info->up = car->groundNormal;
        if (info->up.x && info->up.y && info->up.z && PSVECSquareMag(&info->up)) {
            PSVECNormalize(&info->up, &info->up);
        }
    }
    if (car->slaveWeaponRpm[0] < 10000) {
        info->unk8C = 0.0f;
    }
    PSVECSubtract(&car->bodyOffset, (Vector4 *)car->vehicleIface->engine, &offset);
    XformVec(&car->worldMatrix, &offset, &info->pos);
    switch (car->vehicleIface->collider->type) {
    case 0:
        info->target = *(Vector4 *)CarA_GetOrientedCollision((u8 *)car);
        break;
    case 1:
        break;
    }
}

s32 CarA_GetNearestNavPoint(CarA *car) {
    s32 NavHolder_FindIndexOfNearestNavPoint(Vector4 *pos);
    s32 RandomAdd_s32(s32 base, s32 range);
    s32 timer;

    if (car->navPointIndex == 0xFFFFFFFF) {
        car->navPointIndex = NavHolder_FindIndexOfNearestNavPoint(&car->worldMatrix.v[3]);
        timer = RandomAdd_s32(300, 100);
        car->navUpdateTimer = timer;
        car->navUpdateTimer = timer;
    }
    return car->navPointIndex;
}

void CarA_CollideWithRay(CarA *car, void *start, void *end, struct RayHit { u8 pad0[0x30]; Vector4 normal; } *hit, f32 maxDist) {
    void XformVec(void *mtx, void *in, Vector4 *out);
    f32 PhysicalProperties_CollideWithRay(void *props, void *pos, void *start, void *end, void *hit, f32 maxDist);
    Matrix4 local;
    f32 dist;

    local = car->worldMatrix;
    XformVec(&car->worldMatrix, &car->bodyOffset, &local.v[3]);
    dist = PhysicalProperties_CollideWithRay(car->vehicleIface->collider, &local, start, end, hit, maxDist);
    if (hit != NULL && dist < maxDist) {
        hit->normal = car->velocity;
    }
}

s32 CarA_Invalidate(void *arg0, u32 arg1, u32 arg2) {
    u32 temp_r0;

    temp_r0 = M2C_FIELD(arg0, u32 *, 0x3BC);
    if ((temp_r0 >= arg1) && (temp_r0 < arg2)) {
        return 1;
    }
    return 0;
}
