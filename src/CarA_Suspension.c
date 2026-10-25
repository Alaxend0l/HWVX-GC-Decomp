#include "common.h"


f32 CarA_GetImpactMultiplier(void *car, void *other, f32 scale) {
    f32 PSVECDotProduct(void *a, void *b);
    f32 impact;

    impact = scale * PSVECDotProduct((u8 *)car + 0x20, (u8 *)other + 0x10);
    if (impact < 0.0f) {
        return 1.0f;
    }
    return 1.0f - impact;
}
