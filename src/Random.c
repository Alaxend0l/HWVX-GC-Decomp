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

void RandomNumberGenerator_GetRandomVector(Vector4 *v);
void GenerateQuaternion(Vector4 *axis, Vector4 *q, f32 angle);
void RandomNumberGenerator_GetRandomQuaternion(Vector4 *q) {
    Vector4 axis __attribute__((aligned(16)));
    f32 angle;

    angle = RandomAdd(0.0f, 6.2831855f);
    RandomNumberGenerator_GetRandomVector(&axis);
    GenerateQuaternion(&axis, q, angle);
}

s32 RandomNumberGenerator_GetRandomInteger(s32 min, s32 max) {
    return RandomAdd_s32(min, max - min);
}

f32 RandomNumberGenerator_GetRandomFloat(f32 min, f32 max) {
    RandomNumberGenerator *rng = &g_randomNumberGenerator;
    if (++rng->index >= 0x400) {
        rng->index = 0;
    }
    return rng->floats[rng->index] * (max - min) + min;
}

f32 RandomAdd(f32 base, f32 range) {
    RandomNumberGenerator *rng = &g_randomNumberGenerator;
    if (++rng->index >= 0x400) {
        rng->index = 0;
    }
    return rng->floats[rng->index] * range + base;
}

s32 RandomAdd_s32(s32 base, s32 range) {
    RandomNumberGenerator *rng = &g_randomNumberGenerator;
    if (++rng->index >= 0x400) {
        rng->index = 0;
    }
    return rng->floats[rng->index] * (f32)range + (f32)base;
}

f32 RandomDirVar(f32 base, f32 var) {
    RandomNumberGenerator *rng = &g_randomNumberGenerator;
    if (++rng->index >= 0x400) {
        rng->index = 0;
    }
    return (base - var) + rng->floats[rng->index] * (var * 2.0f);
}

