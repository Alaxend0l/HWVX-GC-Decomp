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
    GXColor color;
    u8 unk8[0xC - 0x8];
    struct UVScroll *uvScroll;
    u32 dlSize;
    u16 unk14;
    u8 unk16[0x18 - 0x16];
    u16 unk18;
    u16 unk1A;
    void *displayList;
} GeometryGroup;

typedef struct UVScroll {
    f32 u;
    f32 v;
    GXColor color;
    u32 flags;
} UVScroll;

typedef struct GfxState {
    u8 unk0[0x74];
    GXColor constColor;
    u8 unk78[0x7C - 0x78];
    u32 flags;
    u8 unk80[0xA0 - 0x80];
} GfxState;

extern GfxState g_state;
extern s32 g_debugTriCount;

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

typedef struct GDLEnvmapState {
    f32 unk0;
    f32 unk4;
    u16 unk8;
    u16 unkA;
} GDLEnvmapState;

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
    u8 unkEBCC0[0xF3A80 - 0xEBCC0];
    GDLEnvmapState envmapStates[0x20];
} GDL;

typedef struct GDLMatrix {
    f32 m[16];
} __attribute__((aligned(16))) GDLMatrix;

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

extern DisplayList *g_pDL;
extern u8 g_CurrentViews;

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

extern s32 m_curLightState;
extern s32 m_curEnvmapState;
void Render_Geometry(Geometry *geom, f32 *m);
void Gfx_EnableState(s32 state);
void Gfx_DisableState(s32 state);
void Gfx_SetBaseColor(GXColor *color);
void *Gfx_GetWorldMatrixState(u32 index);
void Gfx_DrawGeometryLOD_Immediate(Geometry *geometry, f32 (*matrix)[4]);
void Gfx_SetRenderWindow(u32 view);
void Render_StateSetScissor(void);
#pragma use_lmw_stmw on
void Render_GeometryListSplitScreen(void) {
    u8 view;
    u32 i;

    for (view = 1; view < 5; view++) {
        if (g_CurrentViews & (1 << view)) {
            Gfx_SetRenderWindow(view);
            Render_StateSetScissor();
            for (i = 0; i < g_pDL->numOpaqueGroups; i++) {
                Geometry *geometry = g_pDL->opaqueGroups[i].geometry;
                void *matrix = Gfx_GetWorldMatrixState(g_pDL->opaqueGroups[i].matrix);
                if (g_pDL->opaqueGroups[i].window == view) {
                    if (matrix == g_identity) {
                        Gfx_SetWorldMatrix(NULL);
                    } else {
                        Gfx_SetWorldMatrix(matrix);
                    }
                    RenderGroup(geometry, g_pDL->opaqueGroups[i].group, matrix);
                }
            }
            for (i = 0; i < g_pDL->numLitGroups; i++) {
                Geometry *geometry = g_pDL->litGroups[i].geometry;
                void *matrix = Gfx_GetWorldMatrixState(g_pDL->litGroups[i].matrix);
                m_curLightState = g_pDL->litGroups[i].lightState;
                if (g_pDL->litGroups[i].window == view) {
                    if (matrix == g_identity) {
                        Gfx_SetWorldMatrix(NULL);
                    } else {
                        Gfx_SetWorldMatrix(matrix);
                    }
                    RenderGroup(geometry, g_pDL->litGroups[i].group, matrix);
                }
            }
            for (i = 0; i < g_pDL->numAlphaGroups; i++) {
                Geometry *geometry = g_pDL->alphaGroups[i].geometry;
                void *matrix = Gfx_GetWorldMatrixState(g_pDL->alphaGroups[i].matrix);
                if (g_pDL->alphaGroups[i].window == view) {
                    if (matrix == g_identity) {
                        Gfx_SetWorldMatrix(NULL);
                    } else {
                        Gfx_SetWorldMatrix(matrix);
                    }
                    RenderGroup(geometry, g_pDL->alphaGroups[i].group, matrix);
                }
            }
        }
    }
}
#pragma use_lmw_stmw reset
void Render_GeometryList(void) {
    u32 i;
    void *geometry;
    void *matrix;
    GXColor color;

    for (i = 0; i < g_pGDL->numDrawEntries; i++) {
        geometry = g_pGDL->drawEntries[i].geometry;
        matrix = Gfx_GetWorldMatrixState(g_pGDL->drawEntries[i].matrix);
        if (g_pGDL->drawEntries[i].unk10) {
            Gfx_EnableState(0x10);
            color = g_pGDL->drawEntries[i].color;
            Gfx_SetBaseColor(&color);
        }
        m_curLightState = g_pGDL->drawEntries[i].lightState;
        m_curEnvmapState = g_pGDL->drawEntries[i].envmapState;
        Render_Geometry(geometry, matrix);
        if (g_pGDL->drawEntries[i].unk10) {
            Gfx_DisableState(0x10);
        }
    }
    for (i = 0; i < g_pGDL->numLODEntries; i++) {
        Geometry *lod = g_pGDL->lodEntries[i].geometry;
        void *lodMatrix = Gfx_GetWorldMatrixState(g_pGDL->lodEntries[i].matrix);
        Gfx_DrawGeometryLOD_Immediate(lod, lodMatrix);
    }
}

void PSVECAdd(Vector4 *a, Vector4 *b, Vector4 *ab);
void PSVECScale(Vector4 *src, Vector4 *dst, f32 scale);
void PSVECSubtract(Vector4 *a, Vector4 *b, Vector4 *a_b);
f32 PSVECDotProduct(Vector4 *a, Vector4 *b);
f32 _fabs(f32 x);
s32 Gfx_FrustrumCullSphereTest(Vector4 *center, f32 radius);
void XformVecOrientation(void *mtx, Vector4 *in, Vector4 *out);
void *GetRenderWindow(void);
void Render_Geometry(Geometry *geom, f32 *m) {
    Vector4 *bmin = (Vector4 *)geom->bboxMin;
    Vector4 *bmax = (Vector4 *)geom->bboxMax;
    Vector4 min __attribute__((aligned(16)));
    Vector4 max __attribute__((aligned(16)));
    Vector4 center __attribute__((aligned(16)));
    Vector4 scale __attribute__((aligned(16)));
    Vector4 out __attribute__((aligned(16)));
    f32 radius;
    u16 i;

    if (geom->unk4 == 0) {
        PSVECAdd((Vector4 *)&m[12], bmin, &min);
        PSVECAdd((Vector4 *)&m[12], bmax, &max);
        PSVECAdd(&min, &max, &center);
        PSVECScale(&center, &center, 0.5f);
        scale.x = geom->bboxMin[3];
        scale.y = geom->bboxMin[3];
        scale.z = geom->bboxMin[3];
        scale.w = 0.0f;
        XformVecOrientation(m, &scale, &out);
        radius = _fabs(out.x);
        out.y = _fabs(out.y);
        out.z = _fabs(out.z);
        if (out.y > radius) {
            radius = out.y;
        }
        if (out.z > radius) {
            radius = out.z;
        }
        switch (Gfx_FrustrumCullSphereTest(&center, radius)) {
        case 1:
            Gfx_EnableState(0x20000);
            break;
        case 2:
            Gfx_DisableState(0x20000);
            break;
        default:
            return;
        }
        PSVECSubtract(&center, (Vector4 *)((u8 *)GetRenderWindow() + 0x180), &min);
        radius = PSVECDotProduct(&min, (Vector4 *)((u8 *)GetRenderWindow() + 0x190));
        radius *= ((f32 *)GetRenderWindow())[0x9D];
        for (i = 0; i < geom->numGroups; i++) {
            if (radius < geom->lodDistances[i]) {
                break;
            }
        }
        if (geom->numGroups == 1 && 0.0f == geom->lodDistances[0]) {
            i = 0;
        }
        if (i < geom->numGroups) {
            Gfx_DrawGeometryLOD_Immediate((Geometry *)((u8 *)geom->groups + i * 0x18), (f32 (*)[4])m);
        }
        Gfx_EnableState(0x20000);
    }
}

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

extern u32 g_IsAnim;
void Render_StateAlphaBlendEnable(s32 enable);
void Render_StateBlendMode(s32 mode);
void Render_SetConstColor(GXColor *color);
void SetTriEnvVtxDescIdx(void);
void SetVertexBuffer(u32 *vb, u16 flags, u16 unk);
void MatrixToMtxTranspose(f32 (*dst)[4], void *src);
void GXLoadTexMtxImm(f32 (*mtx)[4], u32 id, s32 type);
void GXSetTexCoordGen2(s32 dst, s32 func, s32 src, u32 mtx, u8 normalize, u32 pt);
void GXCallDisplayList(void *list, u32 nbytes);
#pragma use_lmw_stmw on
void EnvMap(Geometry *geometry, u32 group, void *matrix, s32 mode) {
    u32 opaque;
    GeometryGroup *grp;
    void *dl;
    GXColor color;
    f32 mtx[3][4];
    s32 blend;

    opaque = mode & 1;
    grp = &geometry->groups[group];
    dl = grp->displayList;
    color.r = grp->color.r;
    color.g = grp->color.g;
    color.b = grp->color.b;
    color.a = grp->color.a;
    if (!opaque) {
        Render_SetTextureHandle(g_pGDL->envmapStates[m_curEnvmapState].unk8);
        blend = 5;
        if (mode & 2) {
            blend = 2;
        }
        Render_StateAlphaBlendEnable(1);
        Render_StateBlendMode(blend);
        MatrixToMtxTranspose(mtx, (u8 *)GetRenderWindow() + 0x80);
        color.r = grp->color.r >> 2;
        color.g = grp->color.g >> 2;
        color.b = grp->color.b >> 2;
    } else {
        Render_SetTextureHandle(g_pGDL->envmapStates[m_curEnvmapState].unkA);
        MatrixToMtxTranspose(mtx, (u8 *)GetRenderWindow() + 0x80);
    }
    mtx[0][0] *= 0.5f;
    mtx[0][1] *= 0.5f;
    mtx[0][2] *= 0.5f;
    mtx[0][3] = 0.5f;
    mtx[1][0] *= 0.5f;
    mtx[1][1] *= 0.5f;
    mtx[1][2] *= 0.5f;
    mtx[1][3] = 0.5f;
    GXLoadTexMtxImm(mtx, 30, 1);
    GXSetTexCoordGen2(0, 1, 1, 30, 0, 125);
    Render_SetConstColor(&color);
    SetTriEnvVtxDescIdx();
    if (g_IsAnim) {
        GXSetVtxDesc(0, 1);
        m_VtxDescFlag = 0;
    }
    SetVertexBuffer((u32 *)geometry->lodDistances, grp->flags, geometry->unk4);
    DCFlushRange(dl, grp->dlSize);
    GXCallDisplayList(dl, grp->dlSize);
    Render_SetConstColor(NULL);
    GXSetTexCoordGen2(0, 1, 4, 60, 0, 125);
    if (!opaque) {
        Render_StateAlphaBlendEnable(0);
    }
}
#pragma use_lmw_stmw reset
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

void SetVertexBuffer(u32 *vb, u16 flags, u16 unk) {
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

