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

typedef struct GfxWindow {
    u8 unk0[0x140];
    Matrix4 viewMatrix;
    Vector4 cameraPos;
    Vector4 cameraRight;
    Vector4 cameraUp;
    Vector4 cameraAt;
    u8 unk1C0[0x2D0 - 0x1C0];
} GfxWindow;

GfxWindow *GetRenderWindow(void);

typedef struct DLSkinGroup {
    void *lod;
    s32 lightState;
    f32 *matrices;
    u32 count;
    u16 matrix;
    u8 group;
    u8 window;
} DLSkinGroup;

typedef struct DLGroup {
    void *geometry;
    s32 lightState;
    u16 matrix;
    u16 unkA;
    u8 group;
    u8 unkD;
    u8 unkE;
    u8 window;
} DLGroup;

typedef struct DisplayList {
    u16 numOpaqueGroups;
    u16 numLitGroups;
    u16 numShadowGroups;
    u16 numAlphaGroups;
    u8 unk8[0x10 - 0x8];
    u16 numSkinGroups;
    u8 unk12[0x14 - 0x12];
    u16 numDecalGroups;
    u16 numOverlayGroups;
    u8 unk18[0x20 - 0x18];
    DLGroup opaqueGroups[0x9C4];
    DLGroup litGroups[0x258];
    DLGroup shadowGroups[0x100];
    DLGroup alphaGroups[0x100];
    DLGroup decalGroups[0x200];
    DLGroup overlayGroups[0x80];
    DLSkinGroup skinGroups[0x100];
    u16 unk11DE0[0x1FF];
    u16 unk121DE[0x1FF];
} DisplayList;

typedef struct GDLRect {
    u8 unk0[0x83];
    u8 window;
} GDLRect;

extern s32 g_currentWindow;

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
    u8 unk38C90[0x7EA00 - 0x38C90];
    GDLRect rects2D[0x400];
    GDLRect rects[0x20];
    u8 unkA0A80[0xC0040 - 0xA0A80];
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
extern DisplayList *g_pDL;

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

GDLRect *Gfx_Add2DRectangleRenderWindow(void) {
    if (g_pGDL->numRects >= 0x20) {
        g_pGDL->numRects = 0;
    }
    g_pGDL->rects[g_pGDL->numRects].window = g_currentWindow;
    return &g_pGDL->rects[g_pGDL->numRects++];
}

GDLRect *Gfx_Add2DRectangle(void) {
    if (g_pGDL->numRects2D >= 0x400) {
        g_pGDL->numRects2D = 0;
    }
    return &g_pGDL->rects2D[g_pGDL->numRects2D++];
}

void Gfx_Add2DRectangleBack(void)
{
    Render_DrawRectangle();
}

void DrawAlphaTable(void) {
    u16 i;
    u16 idx;

    for (i = 0; i < 0x100; i++) {
        for (idx = g_pGDL->alphaTable[i]; idx != 0; idx = g_pGDL->alphaEntries[idx].next) {
            g_pGDL->alphaEntries[idx].func(g_pGDL->alphaEntries[idx].arg);
        }
    }
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

void *Gfx_GetWorldMatrixState(u32 index) {
    if (index == 0xFFFE) {
        return &g_identity;
    }
    if (index == 0xFFFF) {
        return &GetRenderWindow()->viewMatrix;
    }
    return g_pGDL->matrices[index];
}

void Gfx_GetCameraPosition(Vector4 *pos) {
    *pos = GetRenderWindow()->cameraPos;
}

void Gfx_GetCameraVector(Vector4 *right, Vector4 *up, Vector4 *at) {
    *right = GetRenderWindow()->cameraRight;
    *up = GetRenderWindow()->cameraUp;
    *at = GetRenderWindow()->cameraAt;
}

DLSkinGroup *DisplayList_AddGroup_Skin(void) {
    DisplayList *dl = g_pDL;

    if (dl->numSkinGroups == 0x100) {
        return NULL;
    }
    return &dl->skinGroups[dl->numSkinGroups++];
}

