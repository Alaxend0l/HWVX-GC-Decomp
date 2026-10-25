#include "common.h"


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

