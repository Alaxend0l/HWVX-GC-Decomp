#include "common.h"


f32 RayHitsPoly(Vector4 *start, Vector4 *dir, Vector4 *poly, f32 t) {
    typedef struct Poly {
        Vector4 plane;
        Vector4 *verts;
        u32 numVerts;
    } Poly;
    f32 PSVECDotProduct(Vector4 *a, Vector4 *b);
    void PSVECSubtract(Vector4 *a, Vector4 *b, Vector4 *out);
    void PSVECCrossProduct(Vector4 *a, Vector4 *b, Vector4 *out);
    f32 Distance_Plane(void *plane, void *point);
    Vector4 edge;
    Vector4 normal;
    u32 i;
    u32 prev;
    Poly *p = (Poly *)poly;
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
