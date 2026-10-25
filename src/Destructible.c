#include "common.h"

#pragma num_str_lit above 2          /* string literals in .rodata */

typedef struct Vector4 {
    f32 x;
    f32 y;
    f32 z;
    f32 w;
} Vector4;

typedef struct OBox {
    Vector4 v[3];
} OBox;

typedef struct ConvexHull {
    u8 pad0[0x3D0];
    Vector4 point;          /* 0x3D0 */
} ConvexHull;

typedef struct DestructibleInfo {
    f32 inertia[3];         /* 0x00 */
    u8 pad0C[4];            /* 0x0C */
    u8 type;                /* 0x10 */
    u8 takesDamage;         /* 0x11 */
    u8 pad12[6];            /* 0x12 */
    s32 armour;             /* 0x18 */
    u8 pad1C[4];            /* 0x1C */
    f32 mass;               /* 0x20 */
    u8 pad24[0x0C];         /* 0x24 */
    s32 surfaceScript;      /* 0x30 */
    u8 pad34[0x10];         /* 0x34 */
    OBox *box;              /* 0x44 */
    ConvexHull *hull;       /* 0x48 */
} DestructibleInfo;

typedef struct ThingRecord {
    u8 pad00[0x10];
    DestructibleInfo *info;  /* 0x10 */
} ThingRecord;

typedef struct Destructible {
    Vector4 pos;             /* 0x00 */
    f32 matrix[12];          /* 0x10 */
    Vector4 offsetPos;       /* 0x40 */
    u8 pad44[0x30];          /* 0x44 */
    u32 flags;               /* 0x74 */
    u8 pad78[0x48];          /* 0x78 */
    ThingRecord *record;     /* 0xC0 */
    u8 padC4[8];             /* 0xC4 */
    ConvexHull *hull;        /* 0xCC */
    OBox *orientedBox;       /* 0xD0 */
} Destructible;

extern s16 g_zeroVector[];
void BuildOOB(void *oob, void *pos, void *out);
void BuildOConvexHullOffset(void *out, void *hull, void *pos, void *rot);
f32 PhysicalProperties_CollideWithRay(void *props, void *pos, void *start, void *end, void *hit);
