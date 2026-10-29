#include "common.h"


extern volatile u16 *__peReg;
void GXPokeAlphaMode(s32 func, u8 threshold) {
    u32 reg;

    reg = (func << 8) | threshold;
    __peReg[3] = reg;
}

void GXPokeAlphaRead(s32 mode) {
    u32 reg;

    reg = mode;
    reg = (reg & ~4) | 4;
    __peReg[4] = reg;
}
