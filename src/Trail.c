#include "common.h"

typedef struct TrailSegment {
    u32 color[4];
    f32 v[4][4];
    s32 life;
    u8 pad54[8];
    u16 texture;
    u16 active;
} TrailSegment;

TrailSegment g_TrialData[1000];
u32 g_nextTrail;
f32 g_trailTimer;

static inline s32 Trail_FindFreeSegment(void) {
    s32 i;

    i = 1000;
    while (i-- > 0) {
        if (g_nextTrail == 1000) {
            g_nextTrail = 0;
        }
        if (g_TrialData[g_nextTrail++].life == 0) {
            i = g_nextTrail - 1;
            break;
        }
    }
    return i;
}

void Trail_Create(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

void Trail_Init(void) {
    s32 i;

    for (i = 0; i < 1000; i++) {
        g_TrialData[i].life = 0;
    }
    g_nextTrail = 0;
    g_trailTimer = 0.0f;
}

void Trail_Heartbeat(void) {
    extern s32 g_iDeltaTimeMS;
    extern f32 g_fDeltaTimeSec;
    TrailSegment *trail;
    u32 alpha;
    s32 i;
    s32 delta;

    delta = g_iDeltaTimeMS;
    for (i = 0; i < 1000; i++) {
        trail = &g_TrialData[i];
        if (trail->life > 0) {
            trail->life -= delta;
            if (trail->life <= 0) {
                trail->life = 0;
            } else if (trail->life < 2000) {
                alpha = (trail->life << 7) / 2000;
                if (alpha < trail->color[3]) {
                    trail->color[3] = alpha;
                }
            }
        }
    }
    if (g_trailTimer > 0.06666667f) {
        g_trailTimer -= 0.06666667f;
    }
    g_trailTimer += g_fDeltaTimeSec;
}

#pragma push
#pragma use_lmw_stmw on
void Trail_Render(void) {
    void *Gfx_Begin_GroupQuadFlat(u16 texture, s32 arg1, s32 arg2);
    void Gfx_End_GroupTri(u32 count);
    void MemCpy32(void *dst, void *src, u32 size);
    TrailSegment *trail;
    u32 i;
    u16 texture;
    u32 count;
    u8 *dst;

    count = 0;
    texture = 0xFFFF;
    dst = NULL;
    for (i = 0; i < 1000; i++) {
        trail = &g_TrialData[i];
        if (trail->life > 0 && trail->active != 0) {
            if (texture != trail->texture) {
                if (count != 0) {
                    Gfx_End_GroupTri(count);
                    count = 0;
                }
                dst = Gfx_Begin_GroupQuadFlat(trail->texture, 7, 0);
                texture = trail->texture;
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
#pragma pop
