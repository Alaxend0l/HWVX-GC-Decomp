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

static inline s8 LightManager_AddThing(CarA *thing, u32 numSteps) {
    static u32 offsetCount;
    ThingLightingData *data;

    if (g_numThingsToLight >= 0x400) {
        thing->lightIndex = -1;
        return 0;
    }
    if (thing->lightIndex < g_numThingsToLight && g_ThingLightingData[thing->lightIndex].thing == thing) {
        g_ThingLightingData[thing->lightIndex].dirty = 1;
        return 1;
    }
    data = &g_ThingLightingData[g_numThingsToLight];
    data->thing = thing;
    data->numSteps = numSteps;
    data->step = offsetCount % numSteps;
    data->dirty = 1;
    thing->lightIndex = g_numThingsToLight;
    g_numThingsToLight++;
    offsetCount += 5;
    if (offsetCount == 0xFFFFFFFF) {
        offsetCount = 0;
    }
    return 1;
}
