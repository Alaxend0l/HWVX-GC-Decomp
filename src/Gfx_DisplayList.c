#include "common.h"

void Deallocate(void *ptr);

typedef struct GDLSprite {
    u8 unk0[0x20];
    s32 next;
    u8 unk24[0x30 - 0x24];
} GDLSprite;

typedef struct GDLLine {
    u8 unk0[0x30];
} GDLLine;

typedef struct GDLGroupTri {
    u8 type;
    u8 unk1[0xC - 0x1];
    u32 count;
    u8 unk10[0x20 - 0x10];
} GDLGroupTri;

typedef struct GDLAlphaEntry {
    void (*func)(u16);
    u16 arg;
    u16 next;
} GDLAlphaEntry;

typedef struct GfxState {
    u8 unk0[0x8F];
    s8 unk8F;
    u8 unk90[0xA0 - 0x90];
} GfxState;

extern GfxState g_state;
extern Matrix4 g_identity;
void *GetRenderWindow(void);

typedef struct GDL {
    u32 numAlphaEntries;
    u32 num2DSprites;
    u32 numSprites;
    u32 numAlphaGroups;
    u8 unk10[0x14 - 0x10];
    u32 numTris;
    u32 numRects2D;
    u8 unk1C[0x20 - 0x1C];
    u32 numRects;
    u32 num3DLines;
    u8 unk28[0x2C - 0x28];
    u32 numSpheres;
    u32 numBoxes;
    u32 numLines;
    s32 unk38;
    s32 unk3C;
    s32 unk40;
    s32 numGroupTris;
    u8 unk48[0x6C - 0x48];
    u32 numEnvmapStates;
    u8 unk70[0x74 - 0x70];
    u32 numMatrices;
    u8 unk78[0x80 - 0x78];
    u16 alphaTable[0x100];
    GDLAlphaEntry alphaEntries[0x4268];
    GDLSprite sprites[0x7CF];
    u8 unk38C90[0xC0040 - 0x38C90];
    GDLLine lines[0x100];
    u8 unkC3040[0xF3C00 - 0xC3040];
    void *matrices[0x578];
    u8 unkF51E0[0x24CDE4 - 0xF51E0];
    GDLGroupTri *groupTris;
    void *unk24CDE8;
    void *unk24CDEC;
    void *unk24CDF0;
    u16 spriteHeads[0x1FF];
    u8 unk24D1F2;
} GDL;

extern GDL *g_pGDL;
extern void *g_pDL;

void Gfx_DisplayList_Destroy(void) {
    Deallocate(g_pGDL);
    Deallocate(g_pDL);
}

GDLLine *Gfx_Add3DLineFX(void) {
    if (g_pGDL->numLines >= 0x100) {
        g_pGDL->numLines = 0;
    }
    return &g_pGDL->lines[g_pGDL->numLines++];
}

GDLSprite *Gfx_AddBillBoardSprite(u16 tex) {
    GDLSprite *sprite;

    if (g_pGDL->numSprites == 0x7CF) {
        g_pGDL->numSprites = 0;
    }
    sprite = &g_pGDL->sprites[g_pGDL->numSprites];
    sprite->next = g_pGDL->spriteHeads[tex];
    g_pGDL->spriteHeads[tex] = g_pGDL->numSprites++;
    return sprite;
}

void Gfx_End_GroupTri(s32 n) {
    g_pGDL->groupTris[g_pGDL->numGroupTris].count += n;
    if (g_pGDL->groupTris[g_pGDL->numGroupTris].count != 0) {
        switch (g_pGDL->groupTris[g_pGDL->numGroupTris].type) {
        case 0:
            g_pGDL->unk38 += n;
            break;
        case 1:
            g_pGDL->unk3C += n;
            break;
        case 2:
            break;
        case 4:
            g_pGDL->unk40 += n;
            break;
        }
        g_pGDL->numGroupTris++;
    }
}

void Gfx_Add2DRectangleBack(void)
{
    Render_DrawRectangle();
}

s32 Gfx_GetEnvmapState(void) {
    if (g_state.unk8F) {
        return 0;
    }
    return g_pGDL->numEnvmapStates - 1;
}

u32 Gfx_AddWorldMatrixState_NonCached(void *matrix) {
    if (matrix == NULL) {
        return 0xFFFE;
    }
    if (g_pGDL->numMatrices >= 0x578) {
        g_pGDL->numMatrices = 0;
    }
    g_pGDL->matrices[g_pGDL->numMatrices] = matrix;
    return g_pGDL->numMatrices++;
}

