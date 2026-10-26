#include "common.h"


void LineManager_Render(void) {
    typedef struct LinePoints {
        u32 data[8];
    } LinePoints;
    typedef struct Line {
        LinePoints points;
        u8 unk20[0x50 - 0x20];
        u32 active;
        u8 unk54[0x78 - 0x54];
        u32 unk78;
        u8 unk7C[0xA8 - 0x7C];
        u32 colour;
        u8 unkAC[0xE0 - 0xAC];
        u8 unkE0;
        u8 unkE1[0xF0 - 0xE1];
    } Line;
    typedef struct LineFX {
        LinePoints points;
        u32 colour;
        u32 unk24;
        u8 unk28;
    } LineFX;
    extern Line g_lineManager[128];
    LineFX *Gfx_Add3DLineFX(void);
    u32 i;
    LineFX *fx;

    for (i = 0; i < 128; i++) {
        if (g_lineManager[i].active != 0) {
            fx = Gfx_Add3DLineFX();
            fx->points = g_lineManager[i].points;
            fx->colour = g_lineManager[i].colour;
            fx->unk24 = g_lineManager[i].unk78;
            fx->unk28 = g_lineManager[i].unkE0;
        }
    }
}
