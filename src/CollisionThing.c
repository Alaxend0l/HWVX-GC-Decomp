#include "common.h"

typedef struct Vector4 {
    f32 x;
    f32 y;
    f32 z;
    f32 w;
} Vector4;

typedef struct OBox {
    Vector4 center;            /* 0x00 */
    Vector4 corners[8];        /* 0x10 */
    u8 pad90[0x9D - 0x90];     /* 0x90 */
    s8 inUse;                  /* 0x9D */
    u8 pad9E[0x52];            /* 0x9E */
} OBox;

typedef struct OConvexHull {
    Vector4 verts[32];         /* 0x00 */
    Vector4 planes[16];        /* 0x200 */
    void *hull;                /* 0x300 */
    u8 pad304[0x3E0 - 0x304];  /* 0x304 */
} OConvexHull;

typedef struct CollisionPair {
    u8 pad00[0x20];            /* 0x00 */
    u32 objectA;               /* 0x20 */
    u32 objectB;               /* 0x24 */
    s32 time;                  /* 0x28 */
    u8 pad2C[4];               /* 0x2C */
} CollisionPair;

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

void OBoxHolder_Init(void)
{
    u32 i;

    Dprintf("OBOXHOLDER: %x\n", g_aOBox);
    for (i = 0; i < 0x200; i++) {
        g_aOBox[i].inUse = 0;
    }
}

OBox *OBoxHolder_Get(void) {
    u32 i;

    for (i = 0; i < 0x200; i++) {
        if (g_aOBox[i].inUse == 0) {
            g_aOBox[i].inUse = 1;
            return &g_aOBox[i];
        }
    }
    Dprintf("OUT OF OBOXES!!!!\n");
    return NULL;
}

void OBoxHolder_Release(OBox *obox) {
    if (obox != NULL) {
        obox->inUse = 0;
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

