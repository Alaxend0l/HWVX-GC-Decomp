#include "common.h"

static inline s8 AI_Car_CanTarget(u8 *ai, u8 *def) {
    s8 ok = 1;

    if (ai == NULL) {
        ok = 0;
    } else if (def[1] == 2) {
        ok = 0;
    }
    return ok;
}


s32 AI_Car_CheckIfStuck(u8 *ai) {
    f32 PSVECSquareDistance(void *a, void *b);
    f32 PSVECDotProduct(void *a, void *b);
    f32 AI_AngleToTarget(u8 *ai, Vector4 *target);
    double acos(double x);
    u8 *car = *(u8 **)(ai + 0x2C);
    s32 timer;
    u8 mode;
    f32 dot;

    if (*(s32 *)(*(u8 **)(ai + 0x2C) + 0x464) > 600) {
        ai[0] = 0x7F;
        return 1;
    }
    timer = *(s32 *)(ai + 0x634);
    if (timer == 0) {
        if (*(s32 *)(*(u8 **)(ai + 0x2C) + 0x404) < 500 && PSVECSquareDistance(ai + 0x620, *(u8 **)(ai + 0x2C) + 0x40) < 0.5f) {
            (*(u32 *)(ai + 0x630))++;
            if (*(u32 *)(ai + 0x630) >= 10) {
                *(s32 *)(ai + 0x634) = 120;
                *(s16 *)(ai + 0x640) = AI_AngleToTarget(car, (Vector4 *)(ai + 0x400)) < 0.0f ? -127 : 127;
                *(Quaternion *)(ai + 0x610) = *(Quaternion *)(car + 0x30);
            }
        } else {
            *(u32 *)(ai + 0x630) = 0;
        }
        *(Quaternion *)(ai + 0x620) = *(Quaternion *)(*(u8 **)(ai + 0x2C) + 0x40);
        if (*(s32 *)(ai + 0x634) == 0) {
            return 0;
        }
    } else if (timer < 0) {
        *(s32 *)(ai + 0x634) = timer + 1;
        return 0;
    }
    ai[0] = *(s16 *)(ai + 0x640);
    (*(s32 *)(ai + 0x634))--;
    if (*(s32 *)(ai + 0x634) == 0) {
        *(s32 *)(ai + 0x634) = -120;
    }
    mode = (*(u8 **)(*(u8 **)(ai + 0x2C) + 0x3BC))[0x11];
    if (mode == 1) {
        ai[0x13] = 0xFF;
        dot = PSVECDotProduct(car + 0x30, ai + 0x610);
        if (dot > 1.0f) {
            dot = 1.0f;
        }
        if (dot < -1.0f) {
            dot = -1.0f;
        }
        if (30.0 <= 57.29578f * acos(dot)) {
            *(s32 *)(ai + 0x634) = -120;
        }
    } else if (mode == 2) {
        ai[0x14] = 0xFF;
        dot = PSVECDotProduct(car + 0x30, ai + 0x610);
        if (dot > 1.0f) {
            dot = 1.0f;
        }
        if (dot < -1.0f) {
            dot = -1.0f;
        }
        if (45.0 <= 57.29578f * acos(dot)) {
            *(s32 *)(ai + 0x634) = -180;
        }
    }
    return 1;
}

void AI_Car_DriveToTarget(u8 *ai, Vector4 *target, f32 speed) {
    f32 PSVECSquareDistance(void *a, void *b);
    f32 PSVECSquareMag(void *v);
    f32 PSVECDotProduct(void *a, void *b);
    f32 Sqrt(f32 x);
    f32 _fabs(f32 x);
    f32 AI_AngleToTarget(u8 *ai, Vector4 *target);
    void CarA_GetAngularVelocity(void *car, Vector4 *vel);
    void DistanceXYZ(void *a, void *b, f32 *dist);
    Vector4 angVel;
    f32 dist;
    f32 angle;
    u8 *car;
    u8 *range;
    f32 distSq;
    f32 velSq;
    f32 vel;
    f32 deg;
    f32 t;
    f32 yaw;
    f32 r;
    f32 far;
    f32 near;
    f32 mid;
    f32 margin;
    f32 hi;
    f32 lo;
    s32 v;
    s16 s;
    s8 full;
    u8 brake;
    u8 reverse;
    u8 accel;

    car = *(u8 **)(ai + 0x2C);
    brake = 0;
    reverse = 0;
    accel = 0;
    distSq = PSVECSquareDistance(ai + 0x400, car + 0x40);
    velSq = PSVECSquareMag(car);
    vel = Sqrt(velSq);
    angle = AI_AngleToTarget(car, (Vector4 *)(ai + 0x400));
    deg = 57.29578f * angle;
    if (__fabs(deg) <= 5.0 || 0.0f == velSq) {
        brake = 0xFF;
        *(s16 *)(ai + 0x640) = (s8)(-6.0f * deg);
        ai[0] = *(s16 *)(ai + 0x640);
    } else if (velSq < 1600.0f && __fabs(deg) >= 100.0 && __fabs(deg) <= 125.0 && distSq < 900.0f) {
        accel = 0xFF;
        if ((*(u8 **)(*(u8 **)(ai + 0x2C) + 0x3BC))[0x11] == 1) {
            *(s32 *)(ai + 0x634) = 80;
        } else {
            *(s32 *)(ai + 0x634) = 120;
        }
        *(s16 *)(ai + 0x640) = deg < 0.0f ? -127 : 127;
        *(Quaternion *)(ai + 0x610) = *(Quaternion *)(car + 0x30);
    } else if (velSq > 1600.0f && __fabs(deg) >= 45.0 && distSq < 5625.0f) {
        accel = 0xFF;
        reverse = 0xFF;
        *(s16 *)(ai + 0x640) = deg < 0.0f ? 127 : -127;
        ai[0] = *(s16 *)(ai + 0x640);
    } else {
        t = Sqrt(distSq / velSq);
        CarA_GetAngularVelocity(car, &angVel);
        yaw = PSVECDotProduct(car + 0x20, &angVel);
        if (yaw <= 0.01f) {
            yaw = 0.01f;
        }
        r = angle / yaw;
        if (r < 0.0f) {
            r = -r;
            r += yaw * (yaw >= 0.0f ? 3.0f : -3.0f);
        }
        r /= t;
        if (*(s16 *)(ai + 0x640) == 0 || ((*(s16 *)(ai + 0x640) << 16) ^ *(s32 *)&angle) < 0) {
            *(s16 *)(ai + 0x640) = (-127.0f / 3.1415927f) * angle;
        }
        full = angle > 0.0f ? -127 : 127;
        if (r <= 1.0f) {
            brake = 0xFF;
            *(s16 *)(ai + 0x640) = r * (f32)(full - *(s16 *)(ai + 0x640)) + (f32)*(s16 *)(ai + 0x640);
        } else {
            s = r * (f32)*(s16 *)(ai + 0x640);
            if (_fabs(s) < _fabs(full)) {
                *(s16 *)(ai + 0x640) = s;
                brake = 0xFF;
            } else {
                *(s16 *)(ai + 0x640) = full;
                reverse = 0xFF;
                brake = 0xFF;
            }
        }
    }
    DistanceXYZ(car + 0x40, target, &dist);
    range = **(u8 ***)(ai + 0x3E4);
    far = *(f32 *)(range + 0x28);
    near = *(f32 *)(range + 0x24);
    mid = (far - near) / 2.0f + near;
    if (dist < mid && accel != 0xFF) {
        v = 255 - (s32)(255.0f * ((dist - near) / (mid - near)));
        if (v > 255) {
            v = 255;
        }
        if (accel < v) {
            accel = v;
        }
        brake = 0;
    } else if (dist < far && brake) {
        v = 255.0f * ((dist - mid) / (far - mid));
        if (brake > v) {
            brake = v;
        }
    }
    if (*(f32 *)(ai + 0x63C) && (!speed || *(f32 *)(ai + 0x63C) < speed)) {
        speed = *(f32 *)(ai + 0x63C);
    }
    margin = 0.1f * speed;
    hi = speed + margin;
    lo = speed - margin;
    if (speed && vel > speed && accel != 0xFF) {
        v = 255.0f * ((vel - speed) / (hi - speed));
        if (v > 255) {
            v = 255;
        }
        if (accel < v) {
            accel = v;
        }
        brake = 0;
    } else if (speed && vel > lo && brake) {
        v = 255 - (s32)(255.0f * ((vel - lo) / (speed - lo)));
        if (brake > v) {
            brake = v;
        }
    }
    if (velSq < 100.0f || *(f32 *)(car + 0x458) < 0.0f) {
        brake = 0xFF;
        accel = 0;
    }
    if (*(s16 *)(ai + 0x640) > 127) {
        *(s16 *)(ai + 0x640) = 127;
    } else if (*(s16 *)(ai + 0x640) < -127) {
        *(s16 *)(ai + 0x640) = -127;
    }
    ai[0] = *(s16 *)(ai + 0x640);
    if (brake) {
        ai[0x12] = brake;
    } else if (accel) {
        ai[0x13] = accel;
    }
    if (reverse) {
        ai[0x15] = reverse;
    }
}

void AI_Car_WeaponAsyncHeartbeat_80008210(u8 *ai) {
    typedef struct WeaponParams {
        u8 pad0[0x24];
        u32 minAmmo;
        u8 pad28[0x30 - 0x28];
        u32 reloadTime;
        u8 priority;
        u8 pad35[3];
        f32 minRange;
        f32 maxRange;
        f32 aimAngle;
        f32 farAngle;
        f32 nearAngle;
    } WeaponParams;
    typedef struct WeaponDef {
        u8 type;
        u8 pad1[0x20 - 0x1];
        char **name;
        u8 pad24[0x30 - 0x24];
        WeaponParams *params;
    } WeaponDef;
    typedef struct WeaponSlot {
        u8 pad0[0x40];
        s16 ammo;
        u8 pad42[0x4C - 0x42];
        WeaponDef *def;
    } WeaponSlot;
    typedef struct WeaponHolder {
        u8 pad0[0xB0];
        WeaponSlot slots[12];
        u8 current;
    } WeaponHolder;
    extern u32 g_attackDelay;
    extern u8 g_itemVTable[];
    extern const char g_jetBoostersName_800CC3E4[];
    u32 GameSettings_GetCodeVariable(s32, s32);
    s32 ThingManager_GetActiveCars();
    s8 Player_IsHuman(void *player);
    f32 PSVECSquareDistance(void *a, void *b);
    f32 Sqrt(f32 x);
    f32 AI_AngleToTarget(u8 *ai, Vector4 *target);
    s8 AI_CheckAngleInRange(f32 angle, f32 center, f32 range);
    void *ThingManagerList_GetByType(void *list, s32 type);
    s32 RandomAdd_s32(s32 base, s32 range);
    f32 RandomAdd(f32 base, f32 range);
    int strcmp(const char *a, const char *b);
    u8 *cars[8];
    u8 *target;
    WeaponHolder *holder;
    WeaponParams *params;
    s16 i;
    s8 lowHealth;
    s16 best;
    s16 preferred;
    WeaponSlot *slot;
    WeaponParams *reload;
    WeaponDef *def;
    Vector4 *targetPos;
    f32 nearest;
    f32 distSq;
    f32 dist;
    f32 angle;
    f32 angleRange;
    u32 numCars;
    u8 n;
    u8 type;

    target = NULL;
    lowHealth = 0;
    if (*(u8 **)(ai + 0x2C) == NULL) {
        return;
    }
    if (GameSettings_GetCodeVariable(0x12D, 0) >= 3) {
        numCars = ThingManager_GetActiveCars(cars, 8);
        nearest = 1e10f;
        for (n = 0; n < numCars; n++) {
            if (cars[n] != *(u8 **)(ai + 0x2C) && !(*(u32 *)(cars[n] + 0x74) & 0x1000) &&
                (GameSettings_GetCodeVariable(0x12D, 0) != 4 || Player_IsHuman(*(void **)(cars[n] + 0x94)))) {
                distSq = PSVECSquareDistance(*(u8 **)(ai + 0x2C) + 0x40, cars[n] + 0x40);
                if (nearest > distSq) {
                    nearest = distSq;
                    target = cars[n];
                }
            }
        }
    } else if (*(u8 **)(ai + 0x3E8) != NULL) {
        target = *(u8 **)(ai + 0x3E8);
    } else if (*(u8 **)(ai + 0x3EC) != NULL &&
               (*(u8 **)(*(u8 **)(ai + 0x3EC) + 0x8C) == g_carAVTable ||
                (*(u8 **)(*(u8 **)(ai + 0x3EC) + 0x8C) == g_itemVTable && **(u8 **)(*(u8 **)(ai + 0x3EC) + 0xC0) == 2))) {
        target = *(u8 **)(ai + 0x3EC);
    }
    if (target != NULL && *(u8 **)(ai + 0x2C) != NULL) {
        targetPos = (Vector4 *)(target + 0x40);
        best = -1;
        preferred = -1;
        distSq = PSVECSquareDistance(*(u8 **)(ai + 0x2C) + 0x40, targetPos);
        dist = Sqrt(distSq);
        angle = 57.29578f * AI_AngleToTarget(*(u8 **)(ai + 0x2C), targetPos);
        if ((*(u8 **)(*(u8 **)(ai + 0x2C) + 0x3BC))[0x11] == 1 && g_attackDelay == 0 && distSq < 400.0f) {
            if (AI_CheckAngleInRange(angle, 90.0f, 12.5f)) {
                ai[0x10] = 0xFF;
                *(u32 *)(ai + 0xC) |= 1;
            } else if (AI_CheckAngleInRange(angle, 270.0f, 12.5f)) {
                ai[0x1A] = 0xFF;
                *(u32 *)(ai + 0xC) |= 0x400;
            }
        }
        if ((f32)*(s32 *)(*(u8 **)(ai + 0x2C) + 0x78) <
            0.3f * ((f32)*(s32 *)(*(u8 **)(*(u8 **)(*(u8 **)(ai + 0x2C) + 0x3BC) + 0xA0) + 0x14) *
                    **(f32 **)(ai + 0xD0))) {
            lowHealth = 1;
        }
        holder = ThingManagerList_GetByType(*(u8 **)(ai + 0x2C) + 0x344, 8);
        for (i = 0; holder != NULL && i < 12; i++) {
            slot = &holder->slots[i];
            if (slot == NULL || slot->def == NULL) {
                continue;
            }
            params = slot->def->params;
            if (slot->ammo < params->minAmmo) {
                continue;
            }
            type = slot->def->type;
            if (type == 1) {
                if (lowHealth) {
                    best = i;
                    preferred = -1;
                    break;
                }
                continue;
            }
            if (lowHealth && type == 6 && dist <= params->maxRange && dist >= params->minRange) {
                best = i;
                preferred = i;
            }
            if (preferred != -1) {
                continue;
            }
            if (best != -1 && params->priority != (*(u8 **)(ai + 0xD0))[0x26] &&
                holder->slots[best].def->params->priority <= params->priority) {
                continue;
            }
            if ((params->maxRange || params->minRange) &&
                !(dist <= params->maxRange && dist >= params->minRange)) {
                continue;
            }
            angleRange = params->nearAngle + (params->farAngle - params->nearAngle) * (dist - params->minRange) /
                                                 (params->maxRange - params->minRange);
            if ((params->nearAngle || params->farAngle) &&
                !AI_CheckAngleInRange(angle, params->aimAngle, angleRange)) {
                continue;
            }
            best = i;
            if (params->priority == (*(u8 **)(ai + 0xD0))[0x26]) {
                preferred = i;
            }
        }
        if (preferred != -1) {
            best = preferred;
        }
        if (holder->current == best) {
            def = holder->slots[best].def;
            if (g_attackDelay == 0 || RandomAdd_s32(0, 50) == 0 || def->type != 3 ||
                strcmp(*def->name, g_jetBoostersName_800CC3E4) == 0) {
                ai[0x1B] = 0xFF;
                *(u32 *)(ai + 0xC) |= 0x800;
            }
        } else if (best != -1 && *(u32 *)(ai + 0x658) == 0) {
            ai[0x11] = 0xFF;
            *(u32 *)(ai + 0xC) |= 2;
            *(u32 *)(ai + 0x658) = 3000;
        }
        reload = NULL;
        if (best != -1) {
            reload = holder->slots[best].def->params;
        } else if (holder->slots[holder->current].def != NULL) {
            reload = holder->slots[holder->current].def->params;
        }
        if (reload != NULL) {
            *(u32 *)(ai + 0x654) = (f32)reload->reloadTime *
                                   RandomAdd(*(f32 *)(*(u8 **)(ai + 0xD0) + 0x14), *(f32 *)(*(u8 **)(ai + 0xD0) + 0x18));
        }
    }
    if (*(u32 *)(ai + 0x654) == 0) {
        *(u32 *)(ai + 0x654) =
            300.0f * RandomAdd(*(f32 *)(*(u8 **)(ai + 0xD0) + 0x14), *(f32 *)(*(u8 **)(ai + 0xD0) + 0x18));
    }
}

void AI_Car_ClearedNavPoint_800087C8(u8 *car, s8 arg1) {
    s32 NavHolder_GetTotalNavPoints(void);
    u8 *NavHolder_GetNavPoint(s32 index);
    s32 AI_Car_PrepareNavPath(u8 *car, void *holder, s32 dest);
    u8 *point;
    u8 *nav;
    u8 *thing;
    u8 *target;
    void *holder;
    s32 dest;
    f32 dist;

    if (!arg1) {
        if (*(u32 *)(car + 0x4A0) < NavHolder_GetTotalNavPoints()) {
            nav = NavHolder_GetNavPoint(*(u32 *)(car + 0x4A0));
            point = ((u8 **)(car + 0x420))[*(u32 *)(car + 0x4A4)];
            if (point == nav) {
                if (*(u8 *)(point + 0x51) == 1) {
                    *(s32 *)(car + 0x4A0) = *(s32 *)(point + 0x4C);
                } else {
                    *(s32 *)(car + 0x4A0) = -1;
                }
            }
        }
        (*(u32 *)(car + 0x4A4))++;
        if (*(u8 **)(car + 0x3E4) != NULL && *(u32 *)(car + 0x4A4) >= 30) {
            thing = *(u8 **)(car + 0x2C);
            holder = (*(void *(**)(void *))(*(u8 **)(thing + 0x8C) + 0x2C))(thing);
            target = *(u8 **)(car + 0x3E4);
            if (**(u8 **)target == 10 || **(u8 **)target == 14) {
                dest = *(s32 *)(*(u8 **)(target + 0x10) + 0x28);
            } else {
                thing = *(u8 **)(target + 0x10);
                dest = (*(s32 (**)(void *))(*(u8 **)(thing + 0x8C) + 0x2C))(thing);
            }
            AI_Car_PrepareNavPath(car, holder, dest);
        }
    }
    if (*(u32 *)(car + 0x4A4) < 32) {
        point = ((u8 **)(car + 0x420))[*(u32 *)(car + 0x4A4)];
        if (point != NULL) {
            dist = *(f32 *)(*(u8 **)(car + 0xD0) + 0xC);
            if (*(f32 *)(point + 0x2C) < dist) {
                dist = *(f32 *)(point + 0x2C);
            }
            if (dist > 150.0f || 0.0f == dist) {
                *(s32 *)(car + 0x4AC) = 3000;
            } else if (dist > 100.0f) {
                *(s32 *)(car + 0x4AC) = 4500;
            } else {
                *(s32 *)(car + 0x4AC) = 6000;
            }
        }
    }
}

s32 AI_Car_PrepareNavPath(u8 *car, void *holder, s32 dest) {
    void *memset(void *dst, s32 val, u32 n);
    u8 *NavHolder_FindPath(void *holder, s32 dest);
    void NavHolder_FillPointerArrayPath(u8 *point, u8 **array, u32 max);
    void AI_Car_ClearedNavPoint_800087C8(u8 *car, s32 arg1);
    f32 LinearPointTest(void *pos, void *a, void *b);
    s32 result = 1;
    u8 *point;

    memset(car + 0x420, 0, 0x80);
    *(u32 *)(car + 0x4A4) = 0;
    point = NavHolder_FindPath(holder, dest);
    if (point != NULL) {
        NavHolder_FillPointerArrayPath(point, (u8 **)(car + 0x420), 0x20);
        *(s32 *)(car + 0x4A8) = dest;
        AI_Car_ClearedNavPoint_800087C8(car, 1);
        while (*(u32 *)(car + 0x4A4) < 31) {
            u8 *a = ((u8 **)(car + 0x420))[*(u32 *)(car + 0x4A4)];
            u8 *b = ((u8 **)(car + 0x420))[*(u32 *)(car + 0x4A4) + 1];
            if (a == NULL || b == NULL) {
                break;
            }
            if (LinearPointTest(*(u8 **)(car + 0x2C) + 0x40, a, b) < 0.0f) {
                break;
            }
            AI_Car_ClearedNavPoint_800087C8(car, 0);
        }
    } else {
        result = 0;
        *(s32 *)(car + 0x4A8) = -1;
    }
    return result;
}

void AI_Car_AsyncScanDestructibles(u8 *car) {
    s32 RandomAdd_s32(s32 base, s32 range);
    void *RayHitsObjects_GetObject(void *pos, void *target, f32 *dist, s32 arg3, s32 arg4);
    f32 dist;
    u8 *target;
    u32 index;

    if (M2C_FIELD(car, u8 **, 0x2C) != NULL) {
        index = M2C_FIELD(car, u32 *, 0x4A4);
        target = NULL;
        if (index < 32 && ((u8 **)(car + 0x420))[index] != NULL) {
            target = ((u8 **)(car + 0x420))[index];
        } else if (M2C_FIELD(car, u8 **, 0x3EC) != NULL) {
            target = M2C_FIELD(car, u8 **, 0x3EC) + 0x40;
        }
        M2C_FIELD(car, void **, 0x3E8) = NULL;
        if (target != NULL) {
            dist = 1.0f;
            M2C_FIELD(car, void **, 0x3E8) = RayHitsObjects_GetObject(M2C_FIELD(car, u8 **, 0x2C) + 0x40, target, &dist, 1, 0);
            M2C_FIELD(car, s32 *, 0x608) = RandomAdd_s32(400, 400);
        }
    }
}

s8 AI_Car_IsTargetValid_800094EC(u8 *ai, u8 *target, f32 maxDist, f32 minDist, f32 center, f32 range) {
    extern u8 g_itemVTable[];
    s8 Player_IsHuman(void *player);
    f32 PSVECSquareDistance(void *a, void *b);
    f32 AI_AngleToTarget(u8 *ai, Vector4 *target);
    s8 AI_CheckAngleInRange(f32 angle, f32 center, f32 range);
    u32 NavHolder_GetTotalNavPoints(void);
    void *NavHolder_GetNavPoint(u32 index);
    typedef struct {
        s32 limit0;
        s32 limit4;
        s32 limit8;
        s32 limitC;
        u8 mode;
        u8 compare;
    } TargetLimits;
    typedef struct {
        u8 type;
        u8 subtype;
        u8 pad2[3];
        u8 unk5;
        u8 pad6[0x58 - 6];
        TargetLimits *limits;
    } TargetDef;
    typedef struct {
        u8 *thing;
        s16 v4;
        s16 v6;
        s16 v8;
        s16 vA;
        u8 padC[8];
    } TargetEntry;
    typedef struct {
        u8 pad0[0x4B0];
        TargetEntry entries[16];
    } AIData;
    s16 i;
    s8 valid;
    TargetLimits *limits;
    u8 *pos;
    u8 *thing;
    u8 type;
    TargetDef *def;
    f32 dist;

    def = *(TargetDef **)target;
    type = def->type;
    if (type == 6 || (u8)(type - 7) <= 2 || type == 11 || type == 13) {
        thing = *(u8 **)(target + 0x10);
        if (thing != NULL && *(u8 **)(thing + 0x8C) == g_itemVTable && !(*(u32 *)(thing + 0x74) & 0x1000)) {
            pos = NULL;
            if (**(u8 **)(thing + 0xC0) == 7 && *(u8 **)(thing + 0x94) != NULL && *(u32 *)(*(u8 **)(thing + 0x94) + 0x2C) != 0) {
                if (Player_IsHuman(*(u8 **)(thing + 0x94))) {
                    pos = *(u8 **)(*(u8 **)(*(u8 **)(target + 0x10) + 0x94) + 0x2C) + 0x40;
                }
            } else {
                pos = thing + 0x40;
            }
            if (pos != NULL) {
                dist = PSVECSquareDistance(pos, *(u8 **)(ai + 0x2C) + 0x40);
                if ((!minDist && !maxDist) || (dist <= maxDist && dist >= minDist)) {
                    if (AI_CheckAngleInRange(57.29578f * AI_AngleToTarget(*(u8 **)(ai + 0x2C), (Vector4 *)pos), center, range)) {
                        return 1;
                    }
                }
            }
        }
    } else if ((u8)(type - 1) <= 1 || type == 3) {
        valid = 1;
        if ((*(u32 *)(*(u8 **)(target + 0x10) + 0x74) & 0x1000) || !AI_Car_CanTarget(ai, (u8 *)def)) {
            return 0;
        }
        if (def->subtype == 3) {
            limits = def->limits;
            valid = limits->mode != 1;
            for (i = 0; i < 16 && ((AIData *)ai)->entries[i].thing != NULL; i++) {
                if (((AIData *)ai)->entries[i].thing == *(u8 **)(target + 0x10)) {
                    if (limits->compare == 1) {
                        if ((limits->limit8 > 0 && ((TargetEntry *)(ai + 0x4B0))[i].v8 > limits->limit8) ||
                            (limits->limitC > 0 && ((TargetEntry *)(ai + 0x4B0))[i].vA > limits->limitC) ||
                            (limits->limit0 > 0 && ((TargetEntry *)(ai + 0x4B0))[i].v4 > limits->limit0) ||
                            (limits->limit4 > 0 && ((TargetEntry *)(ai + 0x4B0))[i].v6 > limits->limit4)) {
                            valid = limits->mode == 1;
                        }
                    } else {
                        if ((limits->limit8 == 0 || ((TargetEntry *)(ai + 0x4B0))[i].v8 > limits->limit8) &&
                            (limits->limitC == 0 || ((TargetEntry *)(ai + 0x4B0))[i].vA > limits->limitC) &&
                            (limits->limit0 == 0 || ((TargetEntry *)(ai + 0x4B0))[i].v4 > limits->limit0) &&
                            (limits->limit4 == 0 || ((TargetEntry *)(ai + 0x4B0))[i].v6 > limits->limit4)) {
                            valid = limits->mode == 1;
                        }
                    }
                    break;
                }
            }
        }
        if (valid) {
            dist = PSVECSquareDistance(*(u8 **)(target + 0x10) + 0x40, *(u8 **)(ai + 0x2C) + 0x40);
            if ((!minDist && !maxDist) || (dist <= maxDist && dist >= minDist)) {
                if (AI_CheckAngleInRange(57.29578f * AI_AngleToTarget(*(u8 **)(ai + 0x2C), (Vector4 *)(*(u8 **)(target + 0x10) + 0x40)), center, range)) {
                    return 1;
                }
            }
        }
        return 0;
    } else if (type == 10) {
        u8 *nav = *(u8 **)(target + 0x10);

        if (nav != NULL && nav[0x50] != 0) {
            if (def->unk5 != 0 || PSVECSquareDistance(nav, *(u8 **)(ai + 0x2C) + 0x40) > *(f32 *)(nav + 0x24) * *(f32 *)(nav + 0x24)) {
                return 1;
            }
        }
    } else if (type == 14) {
        if (*(u8 **)(target + 0x10) != NULL) {
            if (*(u32 *)(ai + 0x4A0) < NavHolder_GetTotalNavPoints()) {
                *(void **)(target + 0x10) = NavHolder_GetNavPoint(*(u32 *)(ai + 0x4A0));
                return 1;
            }
        }
    }
    return 0;
}

s8 AI_Car_EnterState(u8 *ai, u8 *branch, s32 slot) {
    typedef struct { u8 *info; u32 timer0; u32 timer1; u32 timer2; u8 pad10[4]; u8 count; } AIBranch;
    extern Vector4 g_zeroVector;
    u32 AI_UpdateTimer(u32 *timer);
    s8 AI_Car_DetermineTarget(u8 *ai, u8 *branch, s32 slot);
    void *ThingManager_DARYL_CreateThing(s32 type, void *params);
    s32 RandomAdd_s32(s32 base, s32 range);
    struct { Vector4 a; Vector4 b; Vector4 c; Vector4 zero; u32 id; u8 *player; f32 f0; f32 f1; } p;
    u32 id;
    u8 *car;
    u8 i;
    s8 done;

    done = 0;
    if (branch == ((u8 **)(ai + 0x3D4))[(u8)slot]) {
        if (*(u32 *)(branch + 4) != 0) {
            AI_UpdateTimer((u32 *)((u32)branch + 4));
        }
        if (*(u32 *)(branch + 4) == 0 && (*(u32 *)(((AIBranch *)branch)->info + 0x48) != 0 || *(u32 *)(((AIBranch *)branch)->info + 0x4C) != 0)) {
            done = 1;
        } else if (!(s8)AI_Car_CheckLocalStats(ai, ((AIBranch *)branch)->info)) {
            done = 1;
        }
 else if (!AI_Car_DetermineTarget(ai, branch, slot)) {
            done = 1;
        }
        if (done) {
            branch = NULL;
        }
    } else {
        if (((AIBranch *)branch)->info[6] != 0 && ((AIBranch *)branch)->count == 0) {
            return 0;
        }
        if (((AIBranch *)branch)->timer1 != 0) {
            return 0;
        }
        if (!(s8)AI_Car_CheckLocalStats(ai, ((AIBranch *)branch)->info)) {
            return 0;
        }
        if (!AI_Car_DetermineTarget(ai, branch, slot)) {
            return 0;
        }
        if (((AIBranch *)branch)->count != 0) {
            ((AIBranch *)branch)->count--;
        }
        id = *(u32 *)(((AIBranch *)branch)->info + 0x5C);
        if (id != 0) {
            p.id = id;
            car = *(u8 **)(ai + 0x2C);
            p.a = *(Vector4 *)(car + 0x40);
            p.b = *(Vector4 *)(car + 0x50);
            p.c = *(Vector4 *)(car + 0x0);
            p.player = ai;
            p.f0 = 1.0f;
            p.zero = g_zeroVector;
            ThingManager_DARYL_CreateThing(1, &p);
        }
        ((AIBranch *)branch)->timer0 = RandomAdd_s32(*(s32 *)(((AIBranch *)branch)->info + 0x38), *(s32 *)(((AIBranch *)branch)->info + 0x3C));
        done = 1;
    }
    if (done) {
        for (i = slot; i < 4; i++) {
            if (*(AIBranch **)(ai + 0x3D4 + i * 4) == NULL) {
                break;
            }
            (*(AIBranch **)(ai + 0x3D4 + i * 4))->timer1 = RandomAdd_s32(*(s32 *)((*(AIBranch **)(ai + 0x3D4 + i * 4))->info + 0x48), *(s32 *)((*(AIBranch **)(ai + 0x3D4 + i * 4))->info + 0x4C));
            *(AIBranch **)(ai + 0x3D4 + i * 4) = NULL;
        }
    }
    return branch != NULL;
}

s8 AI_Car_DetermineStateBranch(u8 *car, void *arg, s8 skip) {
    typedef struct AIStateInfo {
        u8 unk0[6];
        u8 unk6;
    } AIStateInfo;
    typedef struct AIState {
        AIStateInfo *info;
        u32 numChildren;
        struct AIState *children;
    } AIState;
    typedef struct AIStateSlot {
        AIStateInfo *info;
        s32 unk4;
        s32 unk8;
        s32 unkC;
        u8 unk10[4];
        u8 unk14;
    } AIStateSlot;
    typedef struct AICar {
        u8 pad0[0xD0];
        u8 *params;
        AIStateSlot slots[32];
        AIStateSlot *active[4];
        AIStateSlot *current;
    } AICar;
    static u8 iTotal;
    static u8 iValid;
    AICar *ai = (AICar *)car;
    AIState *state = arg;
    AIStateInfo *found = NULL;
    u8 i;

    if (state == (AIState *)(ai->params + 0x28)) {
        iTotal = 0;
        iValid = 0;
    }
    if (state->info != NULL) {
        if (!skip && AI_Car_EnterState(car, (u8 *)&ai->slots[iTotal], iValid)) {
            found = state->info;
            ai->active[iValid++] = &ai->slots[iTotal];
        }
        iTotal++;
    }
    skip = (!skip && (state->info == NULL || found != NULL)) == 0;
    for (i = 0; i < state->numChildren; i++) {
        if (AI_Car_DetermineStateBranch(car, &state->children[i], skip)) {
            return 1;
        }
    }
    if (skip) {
        return 0;
    }
    if (found != NULL) {
        ai->current = ai->active[iValid - 1];
        ai->active[iValid] = NULL;
        return 1;
    }
    return 0;
}

s32 AI_Car_DetermineState(u8 *car) {
    typedef struct { u8 *info; u32 timer0; u32 timer1; u32 timer2; u8 pad10[8]; } AIBranch;
    typedef struct { u8 pad0[0xD4]; AIBranch branches[32]; } AICarBranches;
    typedef void *(*GetNavHolderFn)(void *thing);
    u32 AI_UpdateTimer(u32 *timer);
    s8 AI_Car_DetermineStateBranch(u8 *car, void *branch, s32 arg2);
    u32 i;
    u32 prev;

    if (*(u8 **)(car + 0x3E4) != NULL) {
        prev = *(u32 *)(*(u8 **)(car + 0x3E4) + 0x10);
    } else {
        prev = 0;
    }
    for (i = 0; i < 32; i++) {
        if (((AICarBranches *)car)->branches[i].info == NULL) {
            break;
        }
        AI_UpdateTimer(&((AICarBranches *)car)->branches[i].timer0);
        AI_UpdateTimer(&((AICarBranches *)car)->branches[i].timer1);
        if (((AICarBranches *)car)->branches[i].info[5] != 0) {
            AI_UpdateTimer(&((AICarBranches *)car)->branches[i].timer2);
        }
    }
    if (AI_Car_DetermineStateBranch(car, *(u8 **)(car + 0xD0) + 0x28, 0)) {
        if (*(u32 *)(*(u8 **)(car + 0x3E4) + 0x10) != prev) {
            if (*(u32 *)(car + 0x3EC) != 0) {
                *(u32 *)(car + 0x4A4) = 0x20;
                *(u32 *)(car + 0xC8) = 1;
            } else if (*(u8 **)(car + 0x3F0) != NULL) {
                AI_Car_PrepareNavPath(car,
                    (*(GetNavHolderFn *)(*(u8 **)(*(u8 **)(car + 0x2C) + 0x8C) + 0x2C))(*(u8 **)(car + 0x2C)),
                    *(s32 *)(*(u8 **)(car + 0x3F0) + 0x28));
            }
        }
        return 1;
    }
    return 0;
}

void AI_Car_Reset(u8 *car) {
    typedef struct AICarParams {
        f32 speedScale;
        f32 gripScale;
        u8 unk8[4];
        f32 unkC;
        u8 unk10[0x18];
        u8 state[1];
    } AICarParams;
    typedef struct AICarTarget {
        int value;
        u8 unk4[0x10];
    } AICarTarget;
    typedef struct AICarTargets {
        AICarTarget targets[16];
        u8 unk0[0x4B0];
    } AICarTargets;
    void AI_Car_ResetState(u8 *car, void *state);
    s32 RandomAdd_s32(s32 base, s32 range);
    void *memset(void *dst, s32 c, u32 n);
    register u8 *vehicle;
    u16 i;

    AI_Reset(car);
    if (*(AICarParams **)(car + 0xD0) != NULL) {
        *(f32 *)(car + 0x63C) = (*(AICarParams **)(car + 0xD0))->unkC;
        vehicle = *(u8 **)(car + 0x2C);
        if (vehicle != NULL) {
            *(s32 *)(vehicle + 0x78) = *(s32 *)(vehicle + 0x78) * (*(AICarParams **)(car + 0xD0))->speedScale;
            *(f32 *)(*(u8 **)(car + 0x2C) + 0x468) *= (*(AICarParams **)(car + 0xD0))->gripScale;
        }
        AI_Car_ResetState(car, (*(AICarParams **)(car + 0xD0))->state);
    }
    *(s32 *)(car + 0x3EC) = 0;
    *(s32 *)(car + 0x3F0) = 0;
    *(s32 *)(car + 0x5F0) = 0;
    *(s32 *)(car + 0x5F4) = 0;
    *(s32 *)(car + 0x4A8) = -1;
    *(s32 *)(car + 0x4A4) = 0;
    *(s32 *)(car + 0x4AC) = 0;
    *(s32 *)(car + 0x4A0) = -2;
    memset(car + 0x420, 0, 0x80);
    for (i = 0; i < 16; i++) {
        ((AICarTargets *)car)->targets[i].value = 0;
    }
    *(s32 *)(car + 0x608) = RandomAdd_s32(1000, 1000);
    *(s32 *)(car + 0x3E8) = 0;
    *(s32 *)(car + 0x3E4) = 0;
    *(s32 *)(car + 0x630) = 0;
    *(s32 *)(car + 0x634) = -120;
    *(s32 *)(car + 0x654) = 1;
    *(s32 *)(car + 0x658) = 3000;
}

void AI_Car_Init(u8 *car, u8 *params) {
    void AI_Car_AsyncHeartbeat(u8 *car);
    void AI_Car_WeaponAsyncHeartbeat_80008210(u8 *car);
    void *memset(void *dst, s32 c, u32 n);

    M2C_FIELD(car, s32 *, 0xD0) = M2C_FIELD(params, s32 *, 0x0);
    AI_Init();
    M2C_FIELD(car, void **, 0xB8) = car;
    M2C_FIELD(car, void **, 0xBC) = AI_Car_AsyncHeartbeat;
    M2C_FIELD(car, u8 *, 0xC4) = 0;
    M2C_FIELD(car, void **, 0x644) = car;
    M2C_FIELD(car, void **, 0x648) = AI_Car_WeaponAsyncHeartbeat_80008210;
    M2C_FIELD(car, u8 *, 0x650) = 0;
    M2C_FIELD(car, void **, 0x5F8) = car;
    M2C_FIELD(car, void **, 0x5FC) = AI_Car_AsyncScanDestructibles;
    M2C_FIELD(car, u8 *, 0x604) = 0;
    if (M2C_FIELD(params, s8 *, 0xC)) {
        M2C_FIELD(car, u8 *, 0xCD) = 1;
    } else {
        M2C_FIELD(car, u8 *, 0xCD) = 3;
    }
    M2C_FIELD(car, f32 *, 0x65C) = M2C_FIELD(params, f32 *, 0x4);
    M2C_FIELD(car, f32 *, 0x660) = M2C_FIELD(params, f32 *, 0x8);
    M2C_FIELD(car, s32 *, 0x4A0) = -2;
    memset(car + 0x3D4, 0, 0x60);
    M2C_FIELD(car, u8 *, 0xCC) = 1;
}

