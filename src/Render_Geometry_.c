#include "common.h"

void GXClearVtxDesc(void);
void GXSetVtxDesc(s32 attr, s32 type);
void GXSetVtxAttrFmt(s32 vtxfmt, s32 attr, s32 cnt, s32 type, u8 frac);
void GXSetArray(s32 attr, void *data, u8 stride);
void GXSetChanCtrl(s32 chan, u8 enable, s32 amb_src, s32 mat_src, u32 light_mask, s32 diff_fn, s32 attn_fn);
void DCFlushRange(void *addr, u32 nBytes);
void GXSetChanMatColor(s32 chan, GXColor mat_color);
void GXSetChanAmbColor(s32 chan, GXColor amb_color);

extern GXColor white;
extern GXColor black;

typedef struct GeometryGroup {
    u16 unk0;
    u16 flags;
    u8 unk4[0x10 - 0x4];
    void *data;
    u16 unk14;
    u8 unk16[0x18 - 0x16];
    u16 unk18;
    u8 unk1A[0x20 - 0x1A];
} GeometryGroup;

typedef struct Geometry {
    u16 unk0;
    u16 numGroups;
    u16 unk4;
    u16 unk6;
    GeometryGroup *groups;
    f32 *lodDistances;
    f32 bboxMin[4];
    f32 bboxMax[4];
    f32 center[4];
} Geometry;

typedef struct GDLSphere {
    f32 x;
    f32 y;
    f32 z;
    u8 unkC[0x14 - 0xC];
    f32 radius;
    u8 unk18[0x20 - 0x18];
} GDLSphere;

typedef struct GDLDrawEntry {
    void *geometry;
    GXColor color;
    u32 matrix;
    s32 lightState;
    u8 unk10;
    u8 unk11;
    u8 envmapState;
    u8 unk13;
} GDLDrawEntry;

typedef struct GDLLODEntry {
    void *geometry;
    u32 matrix;
} GDLLODEntry;

typedef struct GDL {
    u8 unk0[0x2C];
    u32 numSpheres;
    u8 unk30[0x54 - 0x30];
    u32 numDrawEntries;
    u32 numLODEntries;
    u8 unk5C[0xBC800 - 0x5C];
    GDLSphere spheres[200];
    u8 unkBE100[0xDFCC0 - 0xBE100];
    GDLDrawEntry drawEntries[0x800];
    GDLLODEntry lodEntries[0x400];
} GDL;

typedef struct GDLMatrix {
    f32 m[16];
} __attribute__((aligned(16))) GDLMatrix;

extern GDL *g_pGDL;
extern s32 m_cullMode;
void Render_SetTextureHandle(s32 handle);
void Render_StateCullMode(s32 mode);
void GXDrawSphere(u8 numMajor, u8 numMinor);

extern f32 g_identity[4][4];
extern u32 m_fillMode;
void Gfx_SetWorldMatrix(f32 (*matrix)[4]);
void RenderGroup(Geometry *geometry, u32 group, f32 (*matrix)[4]);

extern u32 m_VtxDescFlag;
extern u32 *m_vertexBuffer;
extern u32 m_pointLightMask;

void Gfx_DrawGeometryLOD_Immediate(Geometry *geometry, f32 (*matrix)[4]) {
    u16 i;

    if (geometry != NULL) {
        if (matrix == g_identity) {
            Gfx_SetWorldMatrix(NULL);
        } else {
            Gfx_SetWorldMatrix(matrix);
        }
        for (i = 0; i < geometry->numGroups; i++) {
            geometry->groups[i].flags |= m_fillMode;
            RenderGroup(geometry, i, matrix);
            geometry->groups[i].flags &= ~m_fillMode;
        }
    }
}

void Render_UseLighting(u8 enable) {
    if (enable) {
        GXSetChanCtrl(4, 1, 0, 0, m_pointLightMask | 1, 2, 2);
    } else {
        GXSetChanCtrl(4, 0, 1, 1, 0, 0, 2);
    }
}

void Render_DrawSpheres(void) {
    u32 i;
    s32 cullMode;
    GDLMatrix mtx;

    cullMode = m_cullMode;
    Render_SetTextureHandle(0xFFFF);
    Render_StateCullMode(2);
    mtx = *(GDLMatrix *)g_identity;
    for (i = 0; i < g_pGDL->numSpheres; i++) {
        mtx.m[12] = g_pGDL->spheres[i].x;
        mtx.m[13] = g_pGDL->spheres[i].y;
        mtx.m[14] = g_pGDL->spheres[i].z;
        mtx.m[0] = mtx.m[5] = mtx.m[10] = g_pGDL->spheres[i].radius;
        Gfx_SetWorldMatrix((f32 (*)[4])mtx.m);
        GXDrawSphere(8, 16);
    }
    Render_StateCullMode(cullMode);
}

void SetVertexBuffer(u32 *vb) {
    u8 *data;

    if (vb != m_vertexBuffer) {
        data = (u8 *)(vb + 4);
        m_vertexBuffer = vb;
        GXSetArray(9, data, 0xC);
        GXSetArray(0xD, data + vb[0], 8);
        if (vb[1] != 0) {
            GXSetArray(0xB, data + vb[1], 4);
        }
        if (vb[2] != 0) {
            GXSetArray(0xA, data + vb[2], 0xC);
        }
        DCFlushRange(vb, vb[3]);
    }
}

void SetTriVtxDescIdx(void) {
    if (m_VtxDescFlag != 0x100) {
        m_VtxDescFlag = 0x100;
        GXClearVtxDesc();
        GXSetVtxDesc(9, 3);
        GXSetVtxDesc(0xB, 3);
        GXSetVtxDesc(0xD, 3);
        GXSetVtxAttrFmt(0, 9, 1, 4, 0);
        GXSetVtxAttrFmt(0, 0xB, 0, 1, 0);
        GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);
    }
}

void SetTriVtxDescIdxAlpha(void) {
    if (m_VtxDescFlag != 0x400) {
        m_VtxDescFlag = 0x400;
        GXClearVtxDesc();
        GXSetVtxDesc(9, 3);
        GXSetVtxDesc(0xB, 3);
        GXSetVtxDesc(0xD, 3);
        GXSetVtxAttrFmt(0, 9, 1, 4, 0);
        GXSetVtxAttrFmt(0, 0xB, 1, 5, 0);
        GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);
    }
}

void SetTriNormVtxDescIdx(void) {
    if (m_VtxDescFlag != 0x200) {
        m_VtxDescFlag = 0x200;
        GXClearVtxDesc();
        GXSetVtxDesc(9, 3);
        GXSetVtxDesc(0xA, 3);
        GXSetVtxDesc(0xD, 3);
        GXSetVtxAttrFmt(0, 9, 1, 4, 0);
        GXSetVtxAttrFmt(0, 0xA, 0, 4, 0);
        GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);
    }
}

void SetTriEnvVtxDescIdx(void) {
    if (m_VtxDescFlag != 0x800) {
        m_VtxDescFlag = 0x800;
        GXClearVtxDesc();
        GXSetVtxDesc(9, 3);
        GXSetVtxDesc(0xA, 3);
        GXSetVtxDesc(0xB, 3);
        GXSetVtxAttrFmt(0, 9, 1, 4, 0);
        GXSetVtxAttrFmt(0, 0xA, 0, 4, 0);
        GXSetVtxAttrFmt(0, 0xB, 1, 5, 0);
    }
}

void SetTriVtxDesc(void) {
    if (m_VtxDescFlag != 8) {
        m_VtxDescFlag = 8;
        GXClearVtxDesc();
        GXSetVtxDesc(9, 1);
        GXSetVtxDesc(0xB, 1);
        GXSetVtxDesc(0xD, 1);
        GXSetVtxAttrFmt(0, 9, 1, 4, 0);
        GXSetVtxAttrFmt(0, 0xB, 1, 5, 0);
        GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);
    }
}

void Render_SetConstColor(GXColor *color) {
    if (color != NULL) {
        GXSetChanMatColor(4, *color);
        GXSetChanAmbColor(4, white);
        GXSetChanCtrl(4, 1, 0, 0, 1, 2, 2);
    } else {
        GXSetChanCtrl(4, 0, 0, 1, 0, 0, 2);
        GXSetChanAmbColor(4, black);
        GXSetChanMatColor(4, white);
    }
}

