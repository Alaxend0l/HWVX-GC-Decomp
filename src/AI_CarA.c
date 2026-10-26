#include "common.h"


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

