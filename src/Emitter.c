#include "common.h"

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

/* emitter description (EMIX data) an emitter spawns effects from */
typedef struct EmitterData {
    u8 unk0;
    u8 pickMode;       /* 0x01 */
    u8 limited;        /* 0x02 */
    u8 pad3;
    s32 count;         /* 0x04 */
    s32 delayBase;     /* 0x08 */
    s32 delayRange;    /* 0x0C */
    f32 range;         /* 0x10 */
    f32 speedScale;    /* 0x14 */
    u32 fxCount;       /* 0x18 */
    u32 *fxIds;        /* 0x1C */
    u32 seq;           /* 0x20 */
} EmitterData;

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
    EmitterData *fx;   /* 0xB0 */
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
s32 Emitter_Init(u8 *emitter, u8 *data) {
    void QuaternionToSubMatrix(void *quat, void *mtx);
    void *ThingManager_DARYL_CreateThing(s32 type, void *params);
    s32 RandomAdd_s32(s32 base, s32 range);
    struct { Vector4 pos; u32 seq; u8 flag; } params __attribute__((aligned(16)));
    u8 *fx;

    Thing_Init(emitter, (s32)data);
    *(u32 *)(emitter + 0x74) = 0;
    *(u16 *)(emitter + 0x80) = 0;
    *(Vector4 *)(emitter + 0x50) = *(Vector4 *)(data + 0x10);
    *(Vector4 *)(emitter + 0x40) = *(Vector4 *)(data + 0x0);
    *(f32 *)(emitter + 0x4C) = 1.0f;
    QuaternionToSubMatrix(emitter + 0x50, emitter + 0x10);
    *(u8 **)(emitter + 0xB0) = *(u8 **)(data + 0x30);
    *(u32 *)(emitter + 0xBC) = 0;
    *(u32 *)(emitter + 0x94) = *(u32 *)(data + 0x34);
    *(f32 *)(emitter + 0xC8) = *(f32 *)(data + 0x38);
    if (*(f32 *)(emitter + 0xC8) == 0.0f) {
        fx = *(u8 **)(data + 0x30);
        *(f32 *)(emitter + 0xC8) = fx[0] ? *(f32 *)(fx + 0x10) : 0.0f;
    }
    fx = *(u8 **)(emitter + 0xB0);
    if (fx[2]) {
        *(u32 *)(emitter + 0xC4) = *(u32 *)(fx + 4);
    } else {
        *(u32 *)(emitter + 0xC4) = 0;
    }
    fx = *(u8 **)(emitter + 0xB0);
    if (!fx[1]) {
        *(s32 *)(emitter + 0xC0) = RandomAdd_s32(0, *(s32 *)(fx + 0x18));
    }
    if (*(u32 *)(*(u8 **)(emitter + 0xB0) + 0x20) != 0) {
        params.flag = 0;
        params.pos = *(Vector4 *)(emitter + 0x40);
        params.seq = *(u32 *)(*(u8 **)(emitter + 0xB0) + 0x20);
        ThingManager_DARYL_CreateThing(0x10, &params);
    }
    *(u32 *)(emitter + 0x74) |= 0x80202;
    return 1;
}

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

s32 Emitter_InCameraRange(Emitter *emitter, f32 range) {
    Vector4 positions[5] __attribute__((aligned(16)));
    Vector4 *pos;
    f32 dist;
    u32 count;
    u32 i;

    if (range == 0.0f) {
        return 1;
    }
    if (emitter->flags & 0x10) {
        count = Gfx_GetCameraPositions(positions);
        emitter->distSq = PSVECSquareDistance((f32 *)&emitter->pos, (f32 *)&positions[0]);
        if (count > 1) {
            for (i = 0, pos = positions; i < 5; i++, pos++) {
                if (g_CurrentViews & (1 << i)) {
                    dist = PSVECSquareDistance((f32 *)&emitter->pos, (f32 *)pos);
                    if (dist < emitter->distSq) {
                        emitter->distSq = dist;
                    }
                }
            }
        }
        emitter->flags &= ~0x10;
    }
    return range * range > emitter->distSq;
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
