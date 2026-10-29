#include "common.h"

typedef struct RandomNumberGenerator {
    Vector4 vectors[0x300];
    u32 unk3000;
    u32 unk3004;
    u32 index;
    f32 floats[0x400];
    u32 unk400C;
} RandomNumberGenerator;

RandomNumberGenerator g_randomNumberGenerator;

s32 RandomAdd_s32(s32 base, s32 range);
f32 RandomAdd(f32 base, f32 range);

s32 RandomNumberGenerator_GetRandomInteger(s32 min, s32 max) {
    return RandomAdd_s32(min, max - min);
}
