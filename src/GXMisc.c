#include "common.h"

typedef union PPCWGPipe {
    u8 u8;
    u16 u16;
    u32 u32;
    s32 s32;
    f32 f32;
} PPCWGPipe;

volatile PPCWGPipe GXWGFifo : 0xCC008000;

typedef struct GXData {
    u16 vNumNot;
    u16 bpSentNot;
    u8 pad4[0x1DC - 0x4];
    u32 peCtrl;
} GXData;

extern GXData *gx;


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

void GXPokeAlphaUpdate(u8 update_enable) {
    u32 reg;

    reg = __peReg[1];
    reg = (reg & ~0x10) | (update_enable << 4);
    __peReg[1] = reg;
}

void GXPokeColorUpdate(u8 update_enable) {
    u32 reg;

    reg = __peReg[1];
    reg = (reg & ~0x8) | (update_enable << 3);
    __peReg[1] = reg;
}

void GXPokeDstAlpha(u8 enable, u8 alpha) {
    u32 reg;

    reg = alpha | (enable << 8);
    __peReg[2] = reg;
}

void GXPokeDither(u8 dither) {
    u32 reg;

    reg = __peReg[1];
    reg = (reg & ~0x4) | (dither << 2);
    __peReg[1] = reg;
}

void GXPokeZMode(u8 compare_enable, s32 func, u8 update_enable) {
    u32 reg;

    reg = compare_enable;
    reg = (reg & ~0xE) | (func << 1);
    reg = (reg & ~0x10) | (update_enable << 4);
    __peReg[0] = reg;
}

