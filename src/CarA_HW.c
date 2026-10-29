#include "CarA.h"
#include "common.h"

typedef struct CarAShiftSoundParams {
    Vector4 pos;
    Vector4 animRefPos;
    Vector4 vel;
    Vector4 angVel;
    u32 id;
    void *player;
    f32 unk48;
    f32 unk4C;
} CarAShiftSoundParams;

extern f32 g_fDeltaTimeSec;
extern u32 g_iDeltaTimeMS;
extern void *g_pSourcePlayer;
extern Vector4 g_zeroVector;
extern char stunt_msg_buffer[];
extern char *g_stuntLossString;

s8 Player_IsHuman(void *player);
s32 RandomAdd_s32(s32 base, s32 range);
void *ThingManager_DARYL_CreateThing(s32 type, void *params);
f32 CarA_Transmission_GetTorqueAtWheels(CarA *car);


s8 CarA_DoFreeStyleStunts(CarA *car) {
    s32 threshold;
    Player *pad;

    threshold = car->vehicleIface->stunt->unk28;
    if (car->player == NULL || !Player_IsHuman(car->player)) {
        return 0;
    }
    pad = car->player;
    if (pad->button10 == 0 && pad->button1A == 0 &&
        pad->stickX > -0x40 && pad->stickX < 0x40 && pad->stickY > -0x40 && pad->stickY < 0x40) {
        return 0;
    }
    if (car->unk460 > threshold && car->lowSpeedFlag > threshold &&
        !(car->transmissionFlags & 0x40002)) {
        return 1;
    }
    return 0;
}

void CarA_StuntImpulse(CarA *car, Vector4 *linImpulse, Vector4 *angImpulse) {
    void XformVecOrientation(void *mtx, Vector4 *in, Vector4 *out);
    void MotionState_InitAllSlavesNow(void *motionState, void *slaves, s32 arg2);
    Vector4 lin;
    Vector4 ang;
    Vector4 scaled __attribute__((aligned(16)));
    CarAInfo *info;

    scaled.x = angImpulse->x / car->inverseBodyInertia.v[0].x;
    scaled.y = angImpulse->y / car->inverseBodyInertia.v[1].y;
    scaled.z = angImpulse->z / car->inverseBodyInertia.v[2].z;
    XformVecOrientation(&car->worldMatrix, &scaled, &ang);
    XformVecOrientation(&car->worldMatrix, linImpulse, &lin);
    car->velocity.x += lin.x;
    car->velocity.y += lin.y;
    car->velocity.z += lin.z;
    car->angularMomentum.x += ang.x;
    car->angularMomentum.y += ang.y;
    car->angularMomentum.z += ang.z;
    car->transmissionFlags |= 2;
    info = car->vehicleIface;
    if (info->numSlaves > 4) {
        MotionState_InitAllSlavesNow(&car->motionState, info->slaves + 0x34, 4);
    }
}

int sprintf(char *buf, const char *fmt, ...);
void CarA_StuntFail(CarA *car) {
    CarAStuntInfo *stunt;
    u32 count;
    u32 secs;
    u32 idx;
    f32 minTime;
    f32 maxTime;
    CarAShiftSoundParams params __attribute__((aligned(16)));

    stunt = car->vehicleIface->stunt;
    minTime = stunt->minFailTime;
    if (car->stuntTime > minTime) {
        count = stunt->numFailThings;
        maxTime = stunt->maxFailTime;
        secs = car->stuntTime;
        if (stunt->failMessage != NULL) {
            car->stuntMessage = stunt->failMessage;
        } else {
            sprintf(stunt_msg_buffer, "Stunt Failed");
            car->stuntMessage = &g_stuntLossString;
        }
        if (car->vehicleIface->numSlaves > 10) {
            MotionState_InitAllSlavesNow(&car->motionState, car->vehicleIface->slaves + 0x7C, 10);
        }
        if (count != 0 && (f32)secs > minTime) {
            idx = 0;
            if (count > 1) {
                idx = (f32)count * (((f32)secs - minTime) / (maxTime - minTime)) + 0.5f;
                if (idx >= count) {
                    idx = count - 1;
                }
            }
            params.id = stunt->failThings[idx];
            if (params.id != 0) {
                params.pos = car->worldMatrix.v[3];
                params.animRefPos = car->animRefPos;
                params.vel.x = car->velocity.x;
                params.vel.y = 0.0f;
                params.vel.z = car->velocity.z;
                params.player = NULL;
                params.unk48 = 0.0f;
                params.unk4C = 0.0f;
                params.angVel = g_zeroVector;
                ThingManager_DARYL_CreateThing(1, &params);
            }
        }
    }
    car->stuntTime = 0.0f;
    car->stuntSpin = g_zeroVector;
    car->transmissionFlags |= 0x40000;
}

void CarA_SlaveWeaponStop(CarA *car, s32 weapon) {
    void MotionState_ReleaseSlavesByType(MotionState *state, u8 type);
    void ThingManagerList_RemoveReleasedThings(void *list);

    car->slaveWeaponRpm[weapon] = 10001;
    MotionState_ReleaseSlavesByType(&car->motionState, weapon + 0xF0);
    ThingManagerList_RemoveReleasedThings(&car->motionState.slaves);
}

#pragma use_lmw_stmw on
s32 CarA_PreMove_Control_HW(void *thing) {
    void MotionState_InitAllSlavesNow(void *motionState, void *slaves, s32 arg2);
    void MotionState_ReleaseSlavesByType(MotionState *state, u8 type);
    CarA *car = thing;
    u8 mode = 0;
    u8 throttling = 0;
    u8 reversing = 0;
    f32 decel = -car->vehicleIface->accel->unk18;
    f32 coast;
    f32 steerRate;
    f32 steer;
    f32 torque;
    f32 accel;
    u32 id;
    CarAInfo *info;

    if (car->player->button15) {
        car->transmissionFlags |= 0x8000;
    } else {
        car->transmissionFlags &= ~0x8000;
    }

    if (!CarA_DoFreeStyleStunts(car)) {
        if (car->player->throttle) {
            if (car->player->brake == 0 && car->gradeOrReverse > -2.0f) {
                if (((car->transmissionFlags & 0x1000) &&
                     car->rpm >= car->vehicleIface->unk1C * car->vehicleIface->unk30) ||
                    (!(car->transmissionFlags & 0x1000) && car->rpm >= car->vehicleIface->unk1C)) {
                    car->gradeOrReverse += -0.1f * car->gradeOrReverse * g_fDeltaTimeSec;
                    car->rpm = car->gradeOrReverse / car->vehicleIface->gears[car->curveTableIdx].ratio;
                    if (car->rpm < car->vehicleIface->unk24 && car->curveTableIdx > 1) {
                        car->curveTableIdx--;
                        car->rpm = car->gradeOrReverse / car->vehicleIface->gears[car->curveTableIdx].ratio;
                    }
                    mode = 2;
                    decel = 255.0f * (0.2f * -car->vehicleIface->accel->unk1C);
                } else {
                    throttling = 1;
                }
            } else {
                car->reverseTimer = 0;
                mode = 1;
                decel = -(f32)car->player->throttle * car->vehicleIface->accel->unk20;
            }
        } else if (car->player->brake) {
            if (car->player->throttle == 0 && car->gradeOrReverse < 1.0f) {
                car->reverseTimer += g_iDeltaTimeMS;
                if (car->reverseTimer > car->vehicleIface->engine->unk18) {
                    reversing = 1;
                    car->transmissionFlags |= 4;
                } else {
                    mode = 1;
                }
            } else {
                car->reverseTimer = 0;
                mode = 1;
                decel = -(f32)car->player->brake * car->vehicleIface->accel->unk18;
            }
        } else {
            coast = 0.05f * car->gradeOrReverse;
            car->gradeOrReverse += -0.1f * car->gradeOrReverse * g_fDeltaTimeSec;
            car->rpm = car->gradeOrReverse / car->vehicleIface->gears[car->curveTableIdx].ratio;
            if (car->rpm < car->vehicleIface->unk24 && car->curveTableIdx > 1) {
                car->curveTableIdx--;
                car->rpm = car->gradeOrReverse / car->vehicleIface->gears[car->curveTableIdx].ratio;
                if (car->vehicleIface->numShiftSounds != 0) {
                    id = car->vehicleIface->shiftSounds[RandomAdd_s32(0, car->vehicleIface->numShiftSounds)];
                    if (id != 0) {
                        CarAShiftSoundParams p __attribute__((aligned(16)));
                        p.id = id;
                        p.pos = car->worldMatrix.v[3];
                        p.animRefPos = car->animRefPos;
                        p.vel = car->velocity;
                        p.player = NULL;
                        p.unk48 = 0.0f;
                        p.unk4C = 0.0f;
                        p.angVel = g_zeroVector;
                        ThingManager_DARYL_CreateThing(1, &p);
                    }
                }
            }
            mode = 2;
            decel = 255.0f * (0.5f * -car->vehicleIface->accel->unk1C);
            if (car->gradeOrReverse < 20.0f) {
                if (coast < 0.1f) {
                    coast = 0.1f;
                }
                decel *= coast;
            }
        }
    }

    g_pSourcePlayer = car->player;
    if (reversing) {
        torque = 0.5f * CarA_Transmission_GetTorqueAtWheels(car);
        accel = -torque * (f32)car->player->brake;
        car->rpm += accel * g_fDeltaTimeSec;
        if (car->rpm < -car->vehicleIface->unk20) {
            car->rpm = -car->vehicleIface->unk20;
        }
        if (car->rpm < car->vehicleIface->unk24 && car->curveTableIdx > 1) {
            car->curveTableIdx--;
            car->rpm = car->gradeOrReverse / car->vehicleIface->gears[car->curveTableIdx].ratio;
            if (car->vehicleIface->numShiftSounds != 0) {
                id = car->vehicleIface->shiftSounds[RandomAdd_s32(0, car->vehicleIface->numShiftSounds)];
                if (id != 0) {
                    CarAShiftSoundParams p __attribute__((aligned(16)));
                    p.id = id;
                    p.pos = car->worldMatrix.v[3];
                    p.animRefPos = car->animRefPos;
                    p.vel = car->velocity;
                    p.player = NULL;
                    p.unk48 = 0.0f;
                    p.unk4C = 0.0f;
                    p.angVel = g_zeroVector;
                    ThingManager_DARYL_CreateThing(1, &p);
                }
            }
        }
        car->gradeOrReverse = car->rpm * car->vehicleIface->gears[car->curveTableIdx].ratio;
    } else if (throttling) {
        torque = 0.5f * CarA_Transmission_GetTorqueAtWheels(car);
        if (!(car->transmissionFlags & 0x1000)) {
            if (car->rpm >= car->vehicleIface->unk1C) {
                accel = 0.0f;
            } else {
                accel = torque * (f32)car->player->throttle;
            }
        } else {
            accel = torque * (f32)car->player->throttle;
        }
        if (car->player->button15 && car->rpm < 1000.0f) {
            if (car->rpm < -1000.0f) {
                car->unk4FC = 1.0f;
            } else {
                car->unk4FC = 0.0f;
            }
            car->rpm = 6000.0f;
        } else {
            car->rpm += accel * g_fDeltaTimeSec;
            if (car->rpm < 0.0f) {
                car->rpm *= -1.0f;
            }
        }
        if (car->player->button15 && car->unk4FC < 0.5f) {
            car->unk4FC = 0.2f;
        }
        if (car->rpm > car->vehicleIface->unk20 &&
            car->curveTableIdx < car->vehicleIface->numGears - 1) {
            car->curveTableIdx++;
            car->rpm = car->gradeOrReverse / car->vehicleIface->gears[car->curveTableIdx].ratio;
        }
        car->gradeOrReverse = car->rpm * car->vehicleIface->gears[car->curveTableIdx].ratio;
        if (car->transmissionFlags & 0x1000) {
            if (car->gradeOrReverse > car->vehicleIface->engine->unk44 * car->vehicleIface->unk30) {
                car->gradeOrReverse = car->vehicleIface->engine->unk44 * car->vehicleIface->unk30;
            }
        } else {
            if (car->gradeOrReverse > car->vehicleIface->engine->unk44) {
                car->gradeOrReverse = car->vehicleIface->engine->unk44;
            }
        }
    }

    if (mode) {
        if (car->gradeOrReverse > 0.0f) {
            car->gradeOrReverse += decel * g_fDeltaTimeSec;
            if (car->gradeOrReverse < 0.0f) {
                car->gradeOrReverse = 0.0f;
            }
        } else {
            car->gradeOrReverse -= decel * g_fDeltaTimeSec;
            if (car->gradeOrReverse > 0.0f) {
                car->gradeOrReverse = 0.0f;
            }
        }
        car->rpm = car->gradeOrReverse / car->vehicleIface->gears[car->curveTableIdx].ratio;
        if (car->rpm < car->vehicleIface->unk24 && car->curveTableIdx > 1) {
            car->curveTableIdx--;
            car->rpm = car->gradeOrReverse / car->vehicleIface->gears[car->curveTableIdx].ratio;
            if (car->vehicleIface->numShiftSounds != 0) {
                id = car->vehicleIface->shiftSounds[RandomAdd_s32(0, car->vehicleIface->numShiftSounds)];
                if (id != 0) {
                    CarAShiftSoundParams p __attribute__((aligned(16)));
                    p.id = id;
                    p.pos = car->worldMatrix.v[3];
                    p.animRefPos = car->animRefPos;
                    p.vel = car->velocity;
                    p.player = NULL;
                    p.unk48 = 0.0f;
                    p.unk4C = 0.0f;
                    p.angVel = g_zeroVector;
                    ThingManager_DARYL_CreateThing(1, &p);
                }
            }
        }
        if (car->rpm < -car->vehicleIface->unk1C) {
            car->rpm = -car->vehicleIface->unk1C;
        }
        if (!(car->transmissionFlags & 0x20) && mode == 1) {
            car->transmissionFlags |= 0x20;
            if (car->vehicleIface->numSlaves >= 3) {
                MotionState_InitAllSlavesNow(&car->motionState, car->vehicleIface->slaves + 0x28, 3);
            }
        } else if (mode == 2 && (car->transmissionFlags & 0x20)) {
            car->transmissionFlags &= ~0x20;
            MotionState_ReleaseSlavesByType(&car->motionState, 3);
        }
    } else if (car->transmissionFlags & 0x20) {
        car->transmissionFlags &= ~0x20;
        MotionState_ReleaseSlavesByType(&car->motionState, 3);
    }

    info = car->vehicleIface;
    steerRate = info->engine->unk14;
    steer = 0.017453292f * (45.0f * -((f32)car->player->stickX / 128.0f));
    if (CarA_DoFreeStyleStunts(car)) {
        steer = 0.0f;
    }
    if (car->engineRpmState < 10000) {
        steer *= 0.5f;
    }
    if (car->unk378 < steer) {
        car->unk378 += steerRate * g_fDeltaTimeSec;
        if (car->unk378 > steer) {
            car->unk378 = steer;
        }
    } else {
        car->unk378 -= steerRate * g_fDeltaTimeSec;
        if (car->unk378 < steer) {
            car->unk378 = steer;
        }
    }
    g_pSourcePlayer = NULL;
    return 1;
}
