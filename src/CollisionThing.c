#include "common.h"

extern OBox g_aOBox[0x200];
extern OConvexHull g_aOConvexHull[8];
extern CollisionPair g_aCollisionPairCache[4];
void Dprintf(const char *, ...);
extern s32 g_iDeltaTimeMS;

void OConvexHullHolder_Init(void)
{
    u32 i;

    for (i = 0; i < 8; i++)
    {
        g_aOConvexHull[i].hull = NULL;
    }
}

OConvexHull *OConvexHullHolder_Get(void *owner) {
    u32 i;

    for (i = 0; i < 8; i++) {
        if (g_aOConvexHull[i].hull == NULL) {
            g_aOConvexHull[i].hull = owner;
            return &g_aOConvexHull[i];
        }
    }
    Dprintf("OUT OF CONVEX HULLS!!!!\n");
    return NULL;
}

void OConvexHullHolder_Release(OConvexHull *hull) {
    if (hull != NULL) {
        hull->hull = NULL;
    }
}

void CollisionPairCache_Clear(void) {
    u32 i;

    for (i = 0; i < 4; i++) {
        g_aCollisionPairCache[i].objectB = 0;
        g_aCollisionPairCache[i].objectA = 0;
        g_aCollisionPairCache[i].time = 99999;
    }
}

void CollisionPairCache_Heartbeat(void)
{
    u32 i;

    for (i = 0; i < 4; i++)
    {
        if (g_aCollisionPairCache[i].objectA == 0)
        {
            continue;
        }
        g_aCollisionPairCache[i].time += g_iDeltaTimeMS;
        if ((u32) g_aCollisionPairCache[i].time > 1000)
        {
            g_aCollisionPairCache[i].objectB = 0;
            g_aCollisionPairCache[i].objectA = 0;
            g_aCollisionPairCache[i].time = 99999;
        }
    }
}
