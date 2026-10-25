#include "common.h"

typedef struct Vector4 {
    f32 x;
    f32 y;
    f32 z;
    f32 w;
} Vector4;

typedef struct Creature {
    u8 unk0[0x10];
    u8 mtx[0x40 - 0x10];
    Vector4 pos;
    Vector4 quat;
    f32 unk60;
    u8 unk64[0x70 - 0x64];
    f32 unk70;
    u32 flags;
    u32 unk78;
    u8 unk7C[0x80 - 0x7C];
    s16 unk80;
    u8 unk82[0xB0 - 0x82];
    f32 unkB0;
    f32 unkB4;
    u8 *anims;
    u8 unkBC[0xBF - 0xBC];
    u8 unkBF;
    f32 unkC0;
    u8 unkC4[0xCC - 0xC4];
    u8 motion[0x130 - 0xCC];
    f32 unk130;
    u8 unk134[0x190 - 0x134];
    Vector4 startPos;
    u8 unk1A0[0x1B0 - 0x1A0];
    s32 unk1B0;
    s32 unk1B4;
    s32 unk1B8;
    f32 mass;
    f32 unk1C0;
    u8 unk1C4[0x1C8 - 0x1C4];
    s32 unk1C8;
} Creature;

void MotionState_Draw(void *motionState);
void MotionState_Release(void *motionState);
void Thing_Release(void *thing);

f32 Creature_GetMass(Creature *creature) {
    return creature->mass;
}
