#include "common.h"
#include "CarA.h"

typedef struct ThingLightingData {
    u32 numSteps;           /* 0x00 frames to fade over, -1: never fade */
    u32 step;               /* 0x04 */
    CarA *thing;            /* 0x08 */
    GXColor parallel;       /* 0x0C */
    GXColor parallelTarget; /* 0x10 */
    GXColor parallelDelta;  /* 0x14 */
    GXColor ambient;        /* 0x18 */
    GXColor ambientTarget;  /* 0x1C */
    GXColor ambientDelta;   /* 0x20 */
    s8 dirty;               /* 0x24 */
} ThingLightingData;

ThingLightingData g_ThingLightingData[0x400];
Light *g_directionalLights[5];
u32 g_numThingsToLight;
u32 g_numDirectionalLights;
s8 g_bLightVolumesExist;
GXColor g_ambientColor;
