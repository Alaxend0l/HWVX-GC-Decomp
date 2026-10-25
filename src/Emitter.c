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
