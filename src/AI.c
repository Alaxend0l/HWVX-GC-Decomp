#include "common.h"


void AI_Reset(void *arg0) {
    M2C_FIELD(arg0, s32 *, 0xC8) = 1;
    M2C_FIELD(arg0, s8 *, 0xCC) = 0;
}

void AI_Init(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

void AI_AsyncProcess_80006C80(void) {
    M2C_UNK AI_Car_AsyncProcess();
    M2C_UNK AI_Creature_AsyncProcess();
    AI_Car_AsyncProcess();
    AI_Creature_AsyncProcess();
}

u32 AI_UpdateTimer(u32 *arg0) {
    extern u32 g_iDeltaTimeMS;

    if (*arg0 > g_iDeltaTimeMS) {
        *arg0 -= g_iDeltaTimeMS;
    } else {
        *arg0 = 0;
    }
    return *arg0;
}

s8 AI_CheckAngleInRange(f32 angle, f32 center, f32 range) {
    s8 inRange = 0;
    f32 diff;

    if (center == 0.0f && range == 0.0f) {
        return 1;
    }
    if (range == 180.0f) {
        return 1;
    }
    if (center > 180.0f) {
        center -= 360.0f;
    }
    diff = angle - center;
    if (diff > 180.0f) {
        diff -= 360.0f;
    } else if (diff < -180.0f) {
        diff += 360.0f;
    }
    if (diff <= range && diff >= -range) {
        inRange = 1;
    }
    return inRange;
}

s32 AI_ProcessEvent(void) {
    extern s16 g_AI_FramesSinceAsyncEventProcessed;
    extern u8 g_AI_Stats[];
    extern void *gs_pAsyncEventHead_803C6F08;
    extern void *gs_pAsyncEventTail_803C6F04;
    u8 *event;

    if (g_AI_FramesSinceAsyncEventProcessed <= 5 && M2C_FIELD(g_AI_Stats, u32 *, 0x1C) >= 250) {
        return 0;
    }
    event = gs_pAsyncEventHead_803C6F08;
    g_AI_FramesSinceAsyncEventProcessed = 0;
    if (event != NULL) {
        gs_pAsyncEventHead_803C6F08 = M2C_FIELD(event, void **, 8);
        if (gs_pAsyncEventHead_803C6F08 == NULL) {
            gs_pAsyncEventTail_803C6F04 = NULL;
        }
        M2C_FIELD(event, u8 *, 0xC) = 0;
        if (M2C_FIELD(event, void **, 0) != NULL) {
            M2C_FIELD(event, void (**)(void *), 4)(M2C_FIELD(event, void **, 0));
        } else {
            M2C_FIELD(event, void (**)(void), 4)();
        }
        return 1;
    }
    return 0;
}

