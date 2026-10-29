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

    for (i = 1000; i-- > 0;) {
        if (g_nextTrail == 1000) {
            g_nextTrail = 0;
        }
        if (g_TrialData[g_nextTrail++].life == 0) {
            return g_nextTrail - 1;
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

#pragma push
#pragma use_lmw_stmw on
void Trail_AddSegment(u32 *handle, f32 *pos, f32 *dir, u16 texture, u8 *color, s32 life, f32 width0, f32 width1) {
    void MemCpy32(void *dst, void *src, u32 size);
    void DistanceXYZ(void *a, void *b, f32 *dist);
    s32 i;
    s32 last;
    u32 r;
    u32 g;
    u32 b;
    u32 a;
    f32 dist;

    if (*handle < 0xFFFFFFFF) {
        if (g_trailTimer > 0.06666667f) {
            last = *handle;
            i = last;
            if (g_TrialData[i].active == 1) {
                i = Trail_FindFreeSegment();
                if (i == 0) {
                    *handle = -1;
                    return;
                }
                r = color[0];
                g = color[1];
                b = color[2];
                a = color[3];
                g_TrialData[i].life = life << 4;
                g_TrialData[i].texture = texture;
                g_TrialData[i].color[0] = r;
                g_TrialData[i].color[1] = g;
                g_TrialData[i].color[2] = b;
                g_TrialData[i].color[3] = a;
                g_TrialData[i].v[0][3] = g_TrialData[last].v[1][3];
                MemCpy32(g_TrialData[i].v[0], g_TrialData[last].v[2], 0x10);
                MemCpy32(g_TrialData[i].v[1], g_TrialData[last].v[3], 0x10);
            }
            g_TrialData[i].v[2][0] = pos[0] + width1 * dir[2];
            g_TrialData[i].v[2][1] = pos[1];
            g_TrialData[i].v[2][2] = pos[2] - width1 * dir[0];
            g_TrialData[i].v[3][0] = pos[0] - width0 * dir[2];
            g_TrialData[i].v[3][1] = pos[1];
            g_TrialData[i].v[3][2] = pos[2] + width0 * dir[0];
            DistanceXYZ(g_TrialData[i].v[0], g_TrialData[i].v[2], &dist);
            g_TrialData[i].v[1][3] = 0.5f * dist + g_TrialData[i].v[0][3];
            if (g_TrialData[i].v[0][3] >= 16.0f) {
                g_TrialData[i].v[0][3] -= 16.0f;
                g_TrialData[i].v[1][3] -= 16.0f;
            }
            g_TrialData[i].active = 1;
            *handle = i;
        }
    } else {
        i = Trail_FindFreeSegment();
        if (i == 0) {
            *handle = -1;
            return;
        }
        r = color[0];
        g = color[1];
        b = color[2];
        a = color[3];
        g_TrialData[i].active = 0;
        g_TrialData[i].life = life << 4;
        g_TrialData[i].texture = texture;
        g_TrialData[i].color[0] = r;
        g_TrialData[i].color[1] = g;
        g_TrialData[i].color[2] = b;
        g_TrialData[i].color[3] = a;
        g_TrialData[i].v[0][3] = 0.0f;
        g_TrialData[i].v[0][0] = pos[0] + width1 * dir[2];
        g_TrialData[i].v[0][1] = pos[1];
        g_TrialData[i].v[0][2] = pos[2] - width1 * dir[0];
        g_TrialData[i].v[1][0] = pos[0] - width0 * dir[2];
        g_TrialData[i].v[1][1] = pos[1];
        g_TrialData[i].v[1][2] = pos[2] + width0 * dir[0];
        *handle = i;
    }
}
#pragma pop

#pragma pop
