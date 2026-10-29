#include "common.h"


extern volatile u16 *__peReg;
void GXPokeAlphaRead(s32 mode) {
    u32 reg;

    reg = mode;
    reg = (reg & ~4) | 4;
    __peReg[4] = reg;
}
