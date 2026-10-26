#include "common.h"


void AI_Creature_Heartbeat(void) {
}

void AI_Creature_AsyncHeartbeat(u8 *creature) {
    int rand(void);
    f32 ArcTan(f32 y, f32 x);
    f32 PSVECSquareDistance(void *a, void *b);
    void Creature_Release(s32 arg0);
    void Control_ClearControl(void *control);
    void AI_AsyncHeartbeat(void *arg0);
    u8 *list;
    u8 *thing;
    u8 *node;
    u8 *next;
    u8 *spawner;
    f32 angle;
    f32 diff;

    if (*(u8 *)(creature + 0xCD) == 0 || *(s8 *)(creature + 0xCC) || *(u8 **)(creature + 0x2C) == NULL) {
        *(s32 *)(creature + 0xC8) = 0;
        return;
    }
    AI_AsyncHeartbeat(creature);
    Control_ClearControl(creature);
    node = *(u8 **)(creature + 0xE0);
    if (node != NULL && (*(u32 *)(node + 0x14) == 0 || PSVECSquareDistance(node, *(u8 **)(creature + 0x2C) + 0x40) < 25.0f)) {
        node = *(u8 **)(creature + 0xE0);
        if (*(u32 *)(node + 0x18) != 0) {
            next = (*(u8 ***)(node + 0x1C))[rand() % *(u32 *)(node + 0x18)];
            if (*(u32 *)(next + 0x14) != 0) {
                *(u8 **)(creature + 0xE0) = next;
            }
        }
    }
    if (*(u32 *)(creature + 0xF8) != 0) {
        if (*(s32 *)(creature + 0xFC) != 0 || 1.0f != *(f32 *)(*(u8 **)(creature + 0x2C) + 0x1C0)) {
            *(s32 *)(creature + 0xC8) = 1000;
        } else {
            spawner = *(u8 **)(creature + 0xD8);
            if (*(u8 **)(creature + 0xE0) == NULL) {
                if (*(u32 *)(spawner + 0xC) == 0) {
                    goto end;
                }
                *(u8 **)(creature + 0xE0) = (*(u8 ***)(spawner + 0x10))[rand() % *(u32 *)(spawner + 0xC)];
            }
            if (PSVECSquareDistance(*(u8 **)(creature + 0xE0), *(u8 **)(creature + 0x2C) + 0x40) < 25.0f) {
                Creature_Release(*(s32 *)(creature + 0x2C));
                *(u8 *)(creature + 0xCD) = 0;
            } else {
                *(s32 *)(creature + 0xC8) = 0xFA;
                node = *(u8 **)(creature + 0xE0);
                angle = ArcTan(*(f32 *)(node + 0x0) - *(f32 *)(*(u8 **)(creature + 0x2C) + 0x40),
                               *(f32 *)(node + 0x8) - *(f32 *)(*(u8 **)(creature + 0x2C) + 0x48));
                *(f32 *)(creature + 0x4) = angle;
                thing = *(u8 **)(creature + 0x2C);
                diff = ArcTan(*(f32 *)(thing + 0x30), *(f32 *)(thing + 0x38)) - angle;
                while (diff > 3.1415927f) {
                    diff -= 6.2831855f;
                }
                while (diff < -3.1415927f) {
                    diff += 6.2831855f;
                }
                if (*(f32 *)(creature + 0xF0) >= 0.0f) {
                    *(s8 *)(creature + 0x1) = 127;
                } else {
                    *(s8 *)(creature + 0x1) = -127;
                }
                *(u8 *)(creature + 0x14) = 0xFF;
            }
        }
    } else {
        thing = *(u8 **)(creature + 0x2C);
        if (*(u32 *)(thing + 0x1B8) & 1) {
            if (*(u8 *)(creature + 0xEC) == 0) {
                *(f32 *)(creature + 0xF0) = (f32)(rand() % 201 + 800) / 1000.0f;
                *(f32 *)(creature + 0xF4) = 0.017453292f * (f32)(rand() % 360 - 180);
                *(u8 *)(creature + 0xED) = 1;
                *(u8 *)(creature + 0xEC) = rand() % 64 + 64;
            }
            *(f32 *)(creature + 0x4) = *(f32 *)(creature + 0xF4);
            if (*(f32 *)(creature + 0xF0) >= 0.0f) {
                *(s8 *)(creature + 0x1) = 127;
            } else {
                *(s8 *)(creature + 0x1) = -127;
            }
            if (*(s8 *)(creature + 0xED)) {
                *(u8 *)(creature + 0x14) = 0xFF;
            }
        } else {
            list = *(u8 **)(creature + 0xE4);
            if (list == NULL || *(u32 *)list == 0) {
                if (*(u32 *)(creature + 0xDC) == -1) {
                    if (PSVECSquareDistance(thing + 0x40, *(u8 **)(thing + 0x90)) > 25.0f) {
                        thing = *(u8 **)(creature + 0x2C);
                        node = *(u8 **)(thing + 0x90);
                        angle = ArcTan(*(f32 *)(node + 0x0) - *(f32 *)(thing + 0x40), *(f32 *)(node + 0x8) - *(f32 *)(thing + 0x48));
                        *(f32 *)(creature + 0x4) = angle;
                        thing = *(u8 **)(creature + 0x2C);
                        diff = ArcTan(*(f32 *)(thing + 0x30), *(f32 *)(thing + 0x38)) - angle;
                        while (diff > 3.1415927f) {
                            diff -= 6.2831855f;
                        }
                        while (diff < -3.1415927f) {
                            diff += 6.2831855f;
                        }
                        if (*(f32 *)(creature + 0xF0) >= 0.0f) {
                            *(s8 *)(creature + 0x1) = 127;
                        } else {
                            *(s8 *)(creature + 0x1) = -127;
                        }
                        *(u8 *)(creature + 0x14) = 0xFF;
                        *(s32 *)(creature + 0xC8) = 0xFA;
                    }
                } else {
                    AI_Creature_VolumeState(creature);
                }
            }
        }
    }
end:
    if (*(f32 *)(creature + 0xF0) >= 0.0f) {
        *(f32 *)(creature + 0x8) = *(f32 *)(creature + 0xF0);
    } else {
        *(f32 *)(creature + 0x8) = -*(f32 *)(creature + 0xF0);
    }
}

