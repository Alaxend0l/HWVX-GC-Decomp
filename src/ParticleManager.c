#include "common.h"


void ParticleSprite_Render(void *sprite) {
    void *Gfx_AddBillBoardSprite(u16);
    void *bb;

    bb = Gfx_AddBillBoardSprite(M2C_FIELD(M2C_FIELD(M2C_FIELD(sprite, void **, 0x90), void **, 4), u32 **, 0x24)[M2C_FIELD(sprite, u8 *, 0x97)]);
    M2C_FIELD(bb, u8 *, 0x2A) = M2C_FIELD(sprite, u8 *, 0x95);
    M2C_FIELD(bb, s32 *, 0x24) = M2C_FIELD(sprite, s32 *, 0x54);
    M2C_FIELD(bb, f32 *, 0x1C) = 1.0f - M2C_FIELD(sprite, f32 *, 0x80);
    M2C_FIELD(bb, f32 *, 0xC) = M2C_FIELD(sprite, f32 *, 0x60) * M2C_FIELD(sprite, f32 *, 0x8C);
    M2C_FIELD(bb, f32 *, 0) = M2C_FIELD(sprite, f32 *, 0);
    M2C_FIELD(bb, f32 *, 4) = M2C_FIELD(sprite, f32 *, 4);
    M2C_FIELD(bb, f32 *, 8) = M2C_FIELD(sprite, f32 *, 8);
}
