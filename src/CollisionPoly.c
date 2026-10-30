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

typedef struct CollisionPoly {
    Vector4 plane;   /* 0x00 */
    Vector4 *verts;  /* 0x10: w holds the index of the opposite vertex */
    u32 numVerts;    /* 0x14 */
    u32 unk18;       /* 0x18 */
    u32 material;    /* 0x1C */
} CollisionPoly;

typedef struct PolyOBox {
    Vector4 center;      /* 0x00 */
    Vector4 corners[8];  /* 0x10 */
    f32 extent[4];       /* 0x90 */
    Vector4 *axis[3];    /* 0xA0 */
    u8 unkAC[0xCC - 0xAC];
    u32 unkCC;           /* 0xCC */
} PolyOBox;

typedef struct PolyHit {
    Vector4 pos;     /* 0x00 */
    Vector4 normal;  /* 0x10 */
    u8 unk20[8];
    u32 unk28;       /* 0x28 */
    u8 unk2C[0x40 - 0x2C];
    u8 type;         /* 0x40 */
} PolyHit;

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

s8 OBoxHitsPolyZeroTime(OBoxZT *box, Poly *p, HitZT *hit) {
    f32 PSVECDotProduct(void *a, void *b);
    void PSVECSubtract(void *a, void *b, void *out);
    void PSVECCrossProduct(void *a, void *b, void *out);
    void PSVECNormalize(void *a, void *out);
    f32 PSVECSquareMag(void *v);
    f32 Sqrt(f32 x);
    void Create_Plane(void *plane, void *normal, void *point);
    f32 Distance_Plane(void *plane, void *point);
    Vector4 edgeDir __attribute__((aligned(16)));
    Vector4 n __attribute__((aligned(16)));
    Vector4 pt __attribute__((aligned(16)));
    Vector4 c2 __attribute__((aligned(16)));
    Vector4 e __attribute__((aligned(16)));
    Vector4 c1 __attribute__((aligned(16)));
    Vector4 plane __attribute__((aligned(16)));
    Vector4 tmp __attribute__((aligned(16)));
    f32 best;
    f32 planeD;
    f32 d;
    f32 t;
    s32 v;
    u8 type;
    u32 feature;
    u8 axis;
    u8 edge;
    s8 flip;
    s32 i;
    s32 k;
    s32 prev;

    best = -99999.9f;
    planeD = -p->plane.w;
    type = 0;
    axis = 0;
    edge = 0;
    for (i = 0; i < 3; i++) {
        u32 j;
        u8 minIdx = 0;
        u8 maxIdx = 0;
        f32 min;
        f32 max;
        f32 c;
        f32 lo;
        f32 hi;
        min = max = PSVECDotProduct(box->axes[i], &p->verts[0]);
        for (j = 1; j < p->numVerts; j++) {
            d = PSVECDotProduct(box->axes[i], &p->verts[j]);
            if (d < min) {
                min = d;
                minIdx = j;
            } else if (d > max) {
                max = d;
                maxIdx = j;
            }
        }
        c = PSVECDotProduct(box->axes[i], box);
        lo = c - box->extents[i];
        hi = c + box->extents[i];
        if (max < lo || hi < min) {
            return 0;
        }
        if (min < hi && min - hi > best) {
            best = min - hi;
            feature = minIdx;
            axis = i;
            type = 1;
            flip = 0;
        }
        if (lo < max && lo - max > best) {
            best = lo - max;
            feature = maxIdx;
            axis = i;
            type = 1;
            flip = 1;
        }
    }
    {
        u32 j;
        u8 minIdx = 0;
        f32 min;
        f32 max;
        min = max = PSVECDotProduct(p, &box->corners[0]);
        for (j = 1; j < 8; j++) {
            d = PSVECDotProduct(p, &box->corners[j]);
            if (d < min) {
                min = d;
                minIdx = j;
            } else if (d > max) {
                max = d;
            }
        }
        if (planeD < min || max < planeD) {
            return 0;
        }
        if (planeD < max && planeD - max > best) {
            best = planeD - max;
            feature = minIdx;
            type = 2;
        }
        if (min < planeD && min - planeD > best) {
            best = min - planeD;
            feature = minIdx;
            type = 2;
        }
    }
    prev = p->numVerts - 1;
    for (v = 0; v < (s32)p->numVerts; prev = v++) {
        PSVECSubtract(&p->verts[v], &p->verts[prev], &edgeDir);
        for (k = 0; k < 3; k++) {
            s32 j;
            u8 maxIdx = 0;
            u8 minIdx = 0;
            f32 min;
            f32 max;
            f32 lo;
            f32 hi;
            f32 a;
            f32 mag;
            PSVECCrossProduct(box->axes[k], &edgeDir, &n);
            mag = PSVECSquareMag(&n);
            if (mag < 0.0001f) {
                continue;
            }
            mag = 1.0f / Sqrt(mag);
            n.x *= mag;
            n.y *= mag;
            n.z *= mag;
            min = max = PSVECDotProduct(&n, &box->corners[0]);
            for (j = 1; j < 8; j++) {
                d = PSVECDotProduct(&n, &box->corners[j]);
                if (d < min) {
                    min = d;
                    minIdx = j;
                } else if (d > max) {
                    maxIdx = j;
                    max = d;
                }
            }
            a = PSVECDotProduct(&n, &p->verts[v]);
            hi = PSVECDotProduct(&n, &p->verts[(u32)p->verts[v].w]);
            if (a > hi) {
                lo = hi;
                hi = a;
            } else {
                lo = a;
            }
            if (hi < min || max < lo) {
                return 0;
            }
            if (lo < max && lo - max > best) {
                best = lo - max;
                axis = k;
                edge = v;
                feature = maxIdx;
                type = 3;
            }
            if (min < hi && min - hi > best) {
                best = min - hi;
                axis = k;
                edge = v;
                feature = minIdx;
                type = 3;
            }
        }
    }
    if (hit != NULL && best < 0.0f) {
        switch (type) {
        case 1:
            hit->pos = p->verts[feature];
            hit->normal = *box->axes[axis];
            if (flip) {
                hit->normal.x *= -1.0f;
                hit->normal.y *= -1.0f;
                hit->normal.z *= -1.0f;
            }
            break;
        case 2:
            hit->pos = box->corners[feature];
            hit->normal = p->plane;
            break;
        case 3:
            pt = box->corners[feature];
            PSVECSubtract(&p->verts[edge], &p->verts[(edge - 1 + p->numVerts) % p->numVerts], &e);
            PSVECCrossProduct(&e, box->axes[axis], &c1);
            PSVECCrossProduct(&e, &c1, &c2);
            Create_Plane(&plane, &c2, &p->verts[edge]);
            d = Distance_Plane(&plane, &pt);
            t = -d / PSVECDotProduct(&plane, box->axes[axis]);
            pt.x += box->axes[axis]->x * t;
            pt.y += box->axes[axis]->y * t;
            pt.z += box->axes[axis]->z * t;
            hit->pos = pt;
            hit->normal = c1;
            if (PSVECSquareMag(&hit->normal)) {
                PSVECNormalize(&hit->normal, &hit->normal);
            }
            PSVECSubtract(box, hit, &tmp);
            if (PSVECDotProduct(&tmp, &hit->normal) < 0.0f) {
                hit->normal.x *= -1.0f;
                hit->normal.y *= -1.0f;
                hit->normal.z *= -1.0f;
            }
            break;
        }
    }
    return 1;
}

