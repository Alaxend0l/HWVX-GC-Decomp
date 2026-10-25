#include "common.h"

typedef struct Vector4 {
    f32 x;
    f32 y;
    f32 z;
    f32 w;
} Vector4;

/* Action/EMIX data referenced by emitters and action-FX points. */
typedef struct ActionFX {
    u8 pad0[0x74];
    u32 flags; /* 0x74 */
} ActionFX;

typedef struct ActionFXPoint {
    u8 pad0[0x30];
    ActionFX *fx; /* 0x30 */
    s32 mode;     /* 0x34 */
} ActionFXPoint;

typedef struct ActionFXList {
    s32 mode;          /* 0x00 */
    u32 pointCount;    /* 0x04 */
    ActionFXPoint *points[1]; /* 0x08 */
} ActionFXList;

void *g_emitterVTable[17];

typedef struct Emitter {
    u8 pad0[0x40];     /* 0x00 world transform */
    Vector4 pos;       /* 0x40 */
    Vector4 quat;      /* 0x50 */
    u8 pad60[0x14];
    u32 flags;         /* 0x74 */
    u8 pad78[0x14];
    void *vtable;      /* 0x8C */
    void *thingSlot;   /* 0x90 */
    u32 thingParam1;   /* 0x94 */
    u8 pad98[0x18];
    ActionFX *fx;      /* 0xB0 */
    u32 fxId;          /* 0xB4 */
    u32 padB8;         /* 0xB8 */
    u32 timeToNextFx;  /* 0xBC */
    u32 fxIndex;       /* 0xC0 */
    s32 fxCount;       /* 0xC4 */
    f32 range;         /* 0xC8 */
    f32 distSq;        /* 0xCC */
} Emitter;
