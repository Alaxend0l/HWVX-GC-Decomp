#include "common.h"

typedef struct OBoxHolder {
    Vector4 v[3];                 /* 0x00 */
    u8 pad30[0x9D - 0x30];        /* 0x30 */
    s8 inUse;                     /* 0x9D */
    u8 pad9E[0xF0 - 0x9E];        /* 0x9E */
} OBoxHolder;

extern OBoxHolder g_aOBox[0x200];
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
    for (i = 0; i < 0x200; i++)
    {
        g_aOBox[i].inUse = 0;
    }
}

OBoxHolder *OBoxHolder_Get(void) {
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

f32 RaySphereTest(Vector4 *start, Vector4 *dir, Vector4 *center, f32 radius, f32 t, u8 *hit) {
    void PSVECSubtract(Vector4 *a, Vector4 *b, Vector4 *out);
    f32 PSVECSquareMag(Vector4 *v);
    void PSVECNormalize(Vector4 *src, Vector4 *dst);
    f32 PSVECDotProduct(Vector4 *a, Vector4 *b);
    void PSVECCrossProduct(Vector4 *a, Vector4 *b, Vector4 *axb);
    f32 PSVECSquareDistance(Vector4 *a, Vector4 *b);
    f32 MagnitudeVector(Vector4 *v);
    f32 Sqrt(f32 x);
    Vector4 end;
    Vector4 seg;
    Vector4 toCenter;
    Vector4 endToCenter;
    Vector4 cross;
    Vector4 unit;
    Vector4 *pEnd = &end;
    f32 radiusSq = radius * radius;
    f32 crossSq;
    f32 segSq;
    f32 h;
    f32 len;
    f32 inv;
    f32 dist;

    PSVECSubtract(center, start, &toCenter);
    if (PSVECSquareMag(&toCenter) < radiusSq) {
        if (hit != NULL) {
            *(Vector4 *)hit = *start;
            PSVECSubtract((Vector4 *)hit, center, (Vector4 *)(hit + 0x10));
            if (PSVECSquareMag((Vector4 *)(hit + 0x10))) {
                PSVECNormalize((Vector4 *)(hit + 0x10), (Vector4 *)(hit + 0x10));
            }
        }
        return 0.0f;
    }
    pEnd->x = dir->x * t + start->x;
    pEnd->y = dir->y * t + start->y;
    pEnd->z = dir->z * t + start->z;
    PSVECSubtract(pEnd, start, &seg);
    if (PSVECDotProduct(&seg, &toCenter) < 0.0f) {
        return t;
    }
    PSVECSubtract(center, pEnd, &endToCenter);
    if (PSVECDotProduct(&seg, &endToCenter) > 0.0f && PSVECSquareMag(&endToCenter) > radiusSq) {
        return t;
    }
    PSVECCrossProduct(&seg, &toCenter, &cross);
    crossSq = cross.z * cross.z + (cross.x * cross.x + cross.y * cross.y);
    segSq = seg.z * seg.z + (seg.x * seg.x + seg.y * seg.y);
    if (crossSq > radiusSq * segSq) {
        return t;
    }
    if (PSVECDotProduct(&seg, &endToCenter) > 0.0f && PSVECSquareDistance(pEnd, center) > radiusSq) {
        return t;
    }
    h = Sqrt(radiusSq - crossSq / segSq);
    len = MagnitudeVector(&seg);
    inv = 1.0f / len;
    unit.x = seg.x * inv;
    unit.y = seg.y * inv;
    unit.z = seg.z * inv;
    dist = inv * (t * (PSVECDotProduct(&unit, &toCenter) - h));
    if (len < 0.0001f) {
        return t;
    }
    if (hit != NULL) {
        ((Vector4 *)hit)->x = dist * dir->x + start->x;
        ((Vector4 *)hit)->y = dist * dir->y + start->y;
        ((Vector4 *)hit)->z = dist * dir->z + start->z;
        PSVECSubtract((Vector4 *)hit, center, (Vector4 *)(hit + 0x10));
        if (PSVECSquareMag((Vector4 *)(hit + 0x10))) {
            PSVECNormalize((Vector4 *)(hit + 0x10), (Vector4 *)(hit + 0x10));
        }
    }
    return dist;
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
