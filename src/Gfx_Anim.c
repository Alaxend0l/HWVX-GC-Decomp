

#include "common.h"

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

typedef struct DLSkinGroup {
    Geometry *lod;
    s32 lightState;
    f32 *matrices;
    u32 count;
    u16 matrix;
    u8 group;
    u8 window;
} DLSkinGroup;

void AnimExtractKeyFrame(void);
void AnimTraverseHeirarchyToMatrices(void);
void *GetRenderWindow(void);
void MatrixToMtxTranspose(void *dst, void *src);

void Gfx_AnimExtractKeyFrame(void)
{
    AnimExtractKeyFrame();
}

void Gfx_AnimTraverseHeirarchyToMatrices(void)
{
    AnimTraverseHeirarchyToMatrices();
}

