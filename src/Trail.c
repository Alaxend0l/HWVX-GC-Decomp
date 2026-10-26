#include "common.h"


void Trail_Create(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

void Trail_Init(void) {
    extern u8 g_TrialData[1000][0x60];
    extern s32 g_nextTrail;
    extern f32 g_trailTimer;
    s32 i;

    for (i = 0; i < 1000; i++) {
        *(s32 *)(g_TrialData[i] + 0x50) = 0;
    }
    g_nextTrail = 0;
    g_trailTimer = 0.0f;
}

void Trail_Heartbeat(void) {
    extern u8 g_TrialData[1000][0x60];
    extern s32 g_iDeltaTimeMS;
    extern f32 g_trailTimer;
    extern f32 g_fDeltaTimeSec;
    u8 *trail;
    u32 alpha;
    s32 i;
    s32 delta;

    delta = g_iDeltaTimeMS;
    for (i = 0; i < 1000; i++) {
        trail = g_TrialData[i];
        if (*(s32 *)(trail + 0x50) > 0) {
            *(s32 *)(trail + 0x50) -= delta;
            if (*(s32 *)(trail + 0x50) <= 0) {
                *(s32 *)(trail + 0x50) = 0;
            } else if (*(s32 *)(trail + 0x50) < 2000) {
                alpha = (*(s32 *)(trail + 0x50) << 7) / 2000;
                if (alpha < *(u32 *)(trail + 0xC)) {
                    *(u32 *)(trail + 0xC) = alpha;
                }
            }
        }
    }
    if (g_trailTimer > 0.06666667f) {
        g_trailTimer -= 0.06666667f;
    }
    g_trailTimer += g_fDeltaTimeSec;
}

void Trail_Render(void) {
    extern u8 g_TrialData[1000][0x60];
    void *Gfx_Begin_GroupQuadFlat(u16 texture, s32 arg1, s32 arg2);
    void Gfx_End_GroupTri(u32 count);
    void MemCpy32(void *dst, void *src, u32 size);
    u8 *trail;
    u32 i;
    u16 texture;
    u32 count;
    u8 *dst;

    count = 0;
    texture = 0xFFFF;
    dst = NULL;
    for (i = 0; i < 1000; i++) {
        trail = g_TrialData[i];
        if (*(s32 *)(trail + 0x50) > 0 && *(u16 *)(trail + 0x5E) != 0) {
            if (texture != *(u16 *)(trail + 0x5C)) {
                if (count != 0) {
                    Gfx_End_GroupTri(count);
                    count = 0;
                }
                dst = Gfx_Begin_GroupQuadFlat(*(u16 *)(trail + 0x5C), 7, 0);
                texture = *(u16 *)(trail + 0x5C);
            }
            MemCpy32(dst, trail, 0x50);
            dst += 0x50;
            count++;
        }
    }
    if (count != 0) {
        Gfx_End_GroupTri(count);
    }
}

