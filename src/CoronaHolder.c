#include "common.h"

s32 g_coronaRayTest;
u32 g_totalCoronas;          /* compared unsigned: cmplwi r0, 0x40 */

typedef struct Vector4 { f32 x; f32 y; f32 z; f32 w; } Vector4;

/* An emitter's corona: its colour track and the interpolation state. */
typedef struct Corona {
    u8 *track;          /* 0x00 */
    u8 pad04[0x0C];     /* 0x04 */
    Vector4 pos;        /* 0x10 */
    f32 color[4];       /* 0x20 */
    u8 pad30[0x1C];     /* 0x30 */
    u8 active;          /* 0x4C */
    u8 pad4D[0x03];     /* 0x4D */
    s32 flare;          /* 0x50 */
    u32 time;           /* 0x54 */
    u8 playing;         /* 0x58 */
    u8 pad59[7];        /* 0x59 */
} Corona;

Corona g_aCorona[64];
extern u32 g_iDeltaTimeMS;
void ExtractColorKey(void *track, f32 *color, f64 time);

void CoronaHolder_Init(void) {
    g_coronaRayTest = 0;
    g_totalCoronas = 0;
}

s32 CoronaHolder_Add(void *corona, Vector4 *pos)
{
    Corona *c;
    void *lens;
    u32 n;

    n = g_totalCoronas;
    if (n >= 64) {
        return -1;
    }
    if (corona == NULL) {
        return -1;
    }
    g_aCorona[n].track = (u8 *)corona;
    c = &g_aCorona[n];
    c->pos = *pos;
    c->flare = -1;
    c->playing = 0;
    c->time = 0;
    c->active = 1;
    lens = *(void **)((u8 *)corona + 0x20);
    if (lens != NULL) {
        c->flare = LensFlare_Add(lens, 0, pos);
    }
    return g_totalCoronas++;
}

#pragma peepblock off
void CoronaHolder_Heartbeat(void) {
    Corona *corona;
    u32 i;
    u32 delta;
    f64 t;

    delta = g_iDeltaTimeMS;
    for (i = 0; i < g_totalCoronas; i++) {
        corona = &g_aCorona[i];
        if (corona->playing) {
            if (corona->time < *(u16 *)(corona->track + 0x02)) {
                corona->active = 1;
                corona->time += delta;
                t = corona->time;
                ExtractColorKey(corona->track + 0x18, corona->color, t);
            }
        } else if (corona->time != 0) {
            corona->active = 1;
            if (delta < corona->time) {
                corona->time -= delta;
                t = corona->time;
                ExtractColorKey(corona->track + 0x18, corona->color, t);
            } else {
                corona->time = 0;
            }
        }
    }
}
#pragma peepblock on
