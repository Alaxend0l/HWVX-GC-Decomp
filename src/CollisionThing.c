#include "common.h"

#pragma use_lmw_stmw on

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

typedef struct BoxPlane {
    Vector4 plane;
    u8 type;
    u8 pad;
    u16 next;
    u8 pad14[0xC];
} BoxPlane;

static inline f32 RayBoxTestLocal(Vector4 *start, Vector4 *dir, Vector4 *box, f32 t, Vector4 *hit) {
    Vector4 *BSPN0_CollideWithSegment(BoxPlane *node, Vector4 *hit, Vector4 *p0, Vector4 *p1);
    f32 Distance_Plane(void *plane, void *point);
    BoxPlane planes[6] = {
        { { 1.0f, 0.0f, 0.0f, 0.0f }, 2, 0, 1 },
        { { 0.0f, 1.0f, 0.0f, 0.0f }, 2, 0, 1 },
        { { 0.0f, 0.0f, 1.0f, 0.0f }, 2, 0, 1 },
        { { -1.0f, 0.0f, 0.0f, 0.0f }, 2, 0, 1 },
        { { 0.0f, -1.0f, 0.0f, 0.0f }, 2, 0, 1 },
        { { 0.0f, 0.0f, -1.0f, 0.0f }, 0, 0, 0 },
    };
    Vector4 seg __attribute__((aligned(16)));
    Vector4 end __attribute__((aligned(16)));
    Vector4 *res;
    f32 d0;
    f32 d1;
    f32 dist = t;

    planes[0].plane.w = -box[1].x;
    planes[1].plane.w = -box[1].y;
    planes[2].plane.w = -box[1].z;
    planes[3].plane.w = box[0].x;
    planes[4].plane.w = box[0].y;
    planes[5].plane.w = box[0].z;
    seg.x = end.x = t * dir->x + start->x;
    seg.y = end.y = t * dir->y + start->y;
    seg.z = end.z = t * dir->z + start->z;
    res = BSPN0_CollideWithSegment(planes, NULL, start, &seg);
    if (res != NULL) {
        d0 = Distance_Plane(res, start);
        d1 = Distance_Plane(res, &end);
        dist *= d0 / (d0 - d1);
        hit[0].x = dir->x * dist + start->x;
        hit[0].y = dir->y * dist + start->y;
        hit[0].z = dir->z * dist + start->z;
        hit[1] = *res;
    }
    return dist;
}

f32 PhysicalProperties_CollideWithRay(u8 *props, void *mtx, Vector4 *start, Vector4 *dir, f32 t, u8 *hit) {
    typedef struct RayHit {
        Vector4 pos;
        Vector4 normal;
        u8 pad[0x30];
    } RayHit;
    void XformVec(void *mtx, Vector4 *in, Vector4 *out);
    void InverseXformVec(void *mtx, Vector4 *in, Vector4 *out);
    void XformVecOrientation(void *mtx, Vector4 *in, Vector4 *out);
    void InverseXformVecOrientation(void *mtx, Vector4 *in, Vector4 *out);
    f32 result = t;

    switch (props[0x10]) {
    case 0: {
        Vector4 *box = *(Vector4 **)(props + 0x44);
        Vector4 localDir __attribute__((aligned(16)));
        Vector4 localStart __attribute__((aligned(16)));
        RayHit localHit __attribute__((aligned(16)));

        InverseXformVec(mtx, start, &localStart);
        InverseXformVecOrientation(mtx, dir, &localDir);
        result = RayBoxTestLocal(&localStart, &localDir, box, t, &localHit.pos);
        if (hit != NULL && result < t) {
            XformVec(mtx, &localHit.pos, (Vector4 *)hit);
            XformVecOrientation(mtx, &localHit.normal, (Vector4 *)(hit + 0x10));
            *(s32 *)(hit + 0x28) = *(s32 *)(props + 0x30);
        }
        break;
    }
    case 1: {
        Vector4 localDir __attribute__((aligned(16)));
        Vector4 localStart __attribute__((aligned(16)));
        RayHit localHit __attribute__((aligned(16)));
        f32 d = t;

        InverseXformVec(mtx, start, &localStart);
        InverseXformVecOrientation(mtx, dir, &localDir);
        if (hit != NULL && d < t) {
            XformVec(mtx, &localHit.pos, (Vector4 *)hit);
            XformVecOrientation(mtx, &localHit.normal, (Vector4 *)(hit + 0x10));
            *(s32 *)(hit + 0x28) = *(s32 *)(props + 0x30);
        }
        break;
    }
    case 7: {
        u32 *list = *(u32 **)(props + 0x44);
        u32 i;
        Vector4 center __attribute__((aligned(16)));
        f32 d;

        for (i = 0; i < list[0]; i++) {
            Vector4 *sphere = (Vector4 *)(list[1] + i * 0x20);
            XformVec(mtx, sphere, &center);
            d = RaySphereTest(start, dir, &center, sphere[1].x, result, hit);
            if (hit != NULL && d < result) {
                result = d;
                *(s32 *)(hit + 0x28) = *(s32 *)(props + 0x30);
            }
        }
        break;
    }
    }
    return result;
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
        if (g_aCollisionPairCache[i].objectA != 0)
        {
            g_aCollisionPairCache[i].time += g_iDeltaTimeMS;
        }
        if ((u32) g_aCollisionPairCache[i].time > 1000)
        {
            g_aCollisionPairCache[i].objectB = 0;
            g_aCollisionPairCache[i].objectA = 0;
            g_aCollisionPairCache[i].time = 99999;
        }
    }
}

void OBoxCache_Set(u32 box0, u32 box1, Vector4 *pos, Vector4 *normal)
{
    u32 i;
    u32 maxTime;
    u32 slot;

    maxTime = 0;
    slot = 0;
    for (i = 0; i < 4; i++)
    {
        if (box0 == g_aCollisionPairCache[i].objectA && box1 == g_aCollisionPairCache[i].objectB)
        {
            g_aCollisionPairCache[i].time = 0;
            g_aCollisionPairCache[i].normal = *normal;
            g_aCollisionPairCache[i].pos = *pos;
            return;
        }
        if (box1 == g_aCollisionPairCache[i].objectA && box0 == g_aCollisionPairCache[i].objectB)
        {
            g_aCollisionPairCache[i].time = 0;
            g_aCollisionPairCache[i].normal.x = -normal->x;
            g_aCollisionPairCache[i].normal.y = -normal->y;
            g_aCollisionPairCache[i].normal.z = -normal->z;
            g_aCollisionPairCache[i].pos = *pos;
            return;
        }
    }

    for (i = 0; i < 4; i++)
    {
        if (g_aCollisionPairCache[i].objectA == 0)
        {
            slot = i;
            break;
        }
        if ((u32)g_aCollisionPairCache[i].time > maxTime)
        {
            maxTime = g_aCollisionPairCache[i].time;
            slot = i;
        }
    }

    g_aCollisionPairCache[slot].time = 0;
    g_aCollisionPairCache[slot].normal = *normal;
    g_aCollisionPairCache[slot].pos = *pos;
    g_aCollisionPairCache[slot].objectA = box0;
    g_aCollisionPairCache[slot].objectB = box1;
}

