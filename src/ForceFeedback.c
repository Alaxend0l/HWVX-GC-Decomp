#include "common.h"

typedef struct Vector4 {
    f32 x;
    f32 y;
    f32 z;
    f32 w;
} Vector4;

typedef struct ForceFeedback {
    void *effect;                 /* 0x00 */
    u8 pad04[0x1C - 0x4];         /* 0x04 */
    u8 active;                    /* 0x1C */
    u8 pad1D[0x30 - 0x1D];        /* 0x1D */
    u8 playing;                   /* 0x30 */
    u8 pad31[0x40 - 0x31];        /* 0x31 */
    Vector4 dir;                  /* 0x40 */
    s32 magnitude;                /* 0x50 */
    s32 unk54;                    /* 0x54 */
    s32 unk58;                    /* 0x58 */
    u8 pad5C[0x60 - 0x5C];        /* 0x5C */
} ForceFeedback;

extern ForceFeedback g_forcefeedback[24];
