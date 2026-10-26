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

void Thing_Release(s32 thing);

u32 Gfx_GetCameraPositions(Vector4 *positions);
f32 PSVECSquareDistance(f32 *a, f32 *b);
extern u8 g_CurrentViews;
void Emitter_Release(Emitter *emitter) {
    void *slot;

    if (emitter->vtable == g_emitterVTable) {
        slot = emitter->thingSlot;
        if (slot != NULL) {
            emitter->thingSlot = NULL;
            *((void **)slot + 0x0C) = NULL;
        }
    }
    Thing_Release((s32)emitter);
}

s32 ActionFXPointChangeMode(ActionFXList *list) {
    s32 mode = list->mode;
    u32 count = list->pointCount;
    u32 i;

    for (i = 0; i < count; i++) {
        ActionFXPoint *point = list->points[i];
        ActionFX *fx = point->fx;

        switch (mode) {
        case 0:
            if (fx != NULL) {
                fx->flags |= 0x02000000;
            }
            break;
        case 1:
            if (fx != NULL) {
                fx->flags &= ~0x02000000;
            }
            break;
        case 2:
            if (fx->flags & 0x02000000) {
                if (fx != NULL) {
                    fx->flags &= ~0x02000000;
                }
            } else {
                if (fx != NULL) {
                    fx->flags |= 0x02000000;
                }
            }
            break;
        }
        point->mode = mode;
    }
    return 1;
}

s32 Emitter_Invalidate(Emitter *emitter, u32 start, u32 end) {
    if ((u32)emitter->fx >= start && (u32)emitter->fx < end) {
        return 1;
    }
    return 0;
}

