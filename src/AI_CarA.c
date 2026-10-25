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
