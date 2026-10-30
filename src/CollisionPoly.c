#include "common.h"

#pragma use_lmw_stmw on

typedef struct Poly {
    Vector4 plane;   /* 0x00 */
    Vector4 *verts;  /* 0x10: w holds the index of the opposite vertex */
    u32 numVerts;    /* 0x14 */
} Poly;

typedef struct OBoxZT {
    Vector4 center;      /* 0x00 */
    Vector4 corners[8];  /* 0x10 */
    f32 extents[4];      /* 0x90 */
    Vector4 *axes[3];    /* 0xA0 */
} OBoxZT;

typedef struct HitZT {
    Vector4 pos;     /* 0x00 */
    Vector4 normal;  /* 0x10 */
} HitZT;

f32 RayHitsPoly(Vector4 *start, Vector4 *dir, Poly *p, f32 t) {
    f32 PSVECDotProduct(Vector4 *a, Vector4 *b);
    void PSVECSubtract(Vector4 *a, Vector4 *b, Vector4 *out);
    void PSVECCrossProduct(Vector4 *a, Vector4 *b, Vector4 *out);
    f32 Distance_Plane(void *plane, void *point);
    Vector4 edge __attribute__((aligned(16)));
    Vector4 normal __attribute__((aligned(16)));
    u32 i;
    u32 prev;
    f32 denom;
    f32 d0;
    f32 d1;
    f32 base;
    f32 a;
    f32 b;

    denom = PSVECDotProduct(&p->plane, dir);
    if (denom >= 0.0f) {
        return t;
    }
    d0 = Distance_Plane(p, start);
    if (d0 < 0.0f) {
        return t;
    }
    d1 = denom * t + d0;
    if (d1 > 0.0f) {
        return t;
    }
    prev = p->numVerts - 1;
    for (i = 0; i < p->numVerts; i++) {
        PSVECSubtract(&p->verts[i], &p->verts[prev], &edge);
        PSVECCrossProduct(dir, &edge, &normal);
        base = PSVECDotProduct(&normal, start);
        a = PSVECDotProduct(&normal, &p->verts[i]);
        b = PSVECDotProduct(&normal, &p->verts[(u32)p->verts[i].w]);
        if ((a - base) * (b - base) > 0.0f) {
            return t;
        }
        prev = i;
    }
    if (d0 && t && d0 != d1) {
        return t * (d0 / (d0 - d1));
    }
    return 0.0f;
}
