#include "common.h"

s32 g_coronaRayTest;
s32 g_totalCoronas;

/* An emitter's corona: its colour track and the interpolation state. */
typedef struct Corona {
    u8 *track;          /* 0x00 */
    u8 pad04[0x1C];     /* 0x04 */
    f32 color[4];       /* 0x20 */
    u8 active;          /* 0x4C */
    u8 pad4D[7];        /* 0x4D */
    u32 time;           /* 0x54 */
    u8 playing;         /* 0x58 */
    u8 pad59[7];        /* 0x59 */
} Corona;

extern Corona g_aCorona[64];
extern u32 g_iDeltaTimeMS;
void ExtractColorKey(void *track, f32 *color, f64 time);

void CoronaHolder_Init(void) {
    g_coronaRayTest = 0;
    g_totalCoronas = 0;
}
