#include "common.h"

typedef struct FogSettings {
    u8 type;
    u8 pad1[3];
    f32 nearDistance;
    f32 farDistance;
    GXColor color;
} FogSettings;

typedef struct WorldFog {
    u8 type;
    u8 pad1[0x14 - 0x1];
    f32 nearDistance;
    f32 farDistance;
    u8 pad1C[0xC0 - 0x1C];
    GXColor *color;
} WorldFog;

typedef struct VolumeShape {
    u8 pad0[0x8];
    s32 type;
    u8 padC[0x10 - 0xC];
    Vector4 center;
    f32 radius;
    f32 radiusSq;
    void *bsp;
    FogSettings *fog;
} VolumeShape;

typedef struct VolumeList {
    u32 count;
    VolumeShape **volumes;
} VolumeList;

typedef struct VolumeCell {
    VolumeList types[7];
} VolumeCell;

typedef struct VolumeVec3 {
    f32 x;
    f32 y;
    f32 z;
} VolumeVec3;

typedef struct VolumeHolder {
    VolumeVec3 origin;      /* 0x00 */
    u8 padC[4];
    VolumeCell *cells;      /* 0x10 */
    s32 *cellIndex;         /* 0x14 */
    u8 pad18[8];
    VolumeVec3 cellSize;    /* 0x20 */
    s32 numX;               /* 0x2C */
    s32 numY;               /* 0x30 */
    s32 numZ;               /* 0x34 */
} VolumeHolder;

s8 BSPN0_CollideWithVector(void *bsp, Vector4 *point);
void PSVECSubtract(Vector4 *a, Vector4 *b, Vector4 *out);
f32 PSVECSquareMag(Vector4 *v);

static inline s8 VolumeManager_IsPointInVolume(Vector4 *point, VolumeShape *shape) {
    Vector4 diff;

    switch (shape->type) {
    case 0:
        return BSPN0_CollideWithVector(shape->bsp, point);
    case 1:
        PSVECSubtract(point, &shape->center, &diff);
        if (PSVECSquareMag(&diff) < shape->radiusSq) {
            return 1;
        }
        return 0;
    }
    return 0;
}


void VolumeManager_Create(void) {
    extern s32 g_VolumeHolder;
    g_VolumeHolder = 0;
}

void VolumeManager_Release(void) {
    extern s32 g_VolumeHolder;
    void SetReverb(s32 a, s32 b, s32 c, s32 d);
    g_VolumeHolder = 0;
    SetReverb(0, 0, 0, 0);
    SetReverb(1, 0, 0, 0);
}

void VolumeManager_Initialize(void) {
    typedef struct World {
        u8 pad0[0x4];
        s32 volumeHolder;
    } World;
    extern s32 g_VolumeHolder;
    extern u8 g_bReset;
    VolumeManager_Release();
    g_VolumeHolder = ((World *)g_worldData.world)->volumeHolder;
    g_bReset = 1;
}

static inline s32 VolumeManager_GetCell(VolumeHolder *holder, Vector4 *point) {
    s32 x;
    s32 y;
    s32 z;

    if (holder == NULL) {
        return 0;
    }
    x = (point->x - holder->origin.x) / holder->cellSize.x;
    y = (point->y - holder->origin.y) / holder->cellSize.y;
    z = (point->z - holder->origin.z) / holder->cellSize.z;
    if (x < 0 || x >= holder->numX) {
        return 0;
    }
    if (y < 0 || y >= holder->numY) {
        return 0;
    }
    if (z < 0 || z >= holder->numZ) {
        return 0;
    }
    return holder->cellIndex[z * holder->numX + holder->numZ * (y * holder->numX) + x];
}

static inline VolumeShape *VolumeManager_FindVolume(VolumeHolder *holder, Vector4 *point, s32 type) {
    VolumeShape *volume;
    u32 count;
    s32 i;
    s32 cell;

    cell = VolumeManager_GetCell(holder, point);
    count = holder != NULL ? holder->cells[cell].types[type].count : 0;
    for (i = 0; i < count; i++) {
        volume = holder->cells[cell].types[type].volumes[i];
        if (VolumeManager_IsPointInVolume(point, volume)) {
            return volume;
        }
    }
    return NULL;
}

s32 VolumeManager_GetVolumeCellIndex(Vector4 *point) {
    extern u8 *g_VolumeHolder;
    u8 *holder = g_VolumeHolder;
    s32 x;
    s32 y;
    s32 z;

    if (holder == NULL) {
        return 0;
    }
    x = (point->x - M2C_FIELD(holder, f32 *, 0x0)) / M2C_FIELD(holder, f32 *, 0x20);
    y = (point->y - M2C_FIELD(holder, f32 *, 0x4)) / M2C_FIELD(holder, f32 *, 0x24);
    z = (point->z - M2C_FIELD(holder, f32 *, 0x8)) / M2C_FIELD(holder, f32 *, 0x28);
    if (x < 0 || x >= M2C_FIELD(holder, s32 *, 0x2C)) {
        return 0;
    }
    if (y < 0 || y >= M2C_FIELD(holder, s32 *, 0x30)) {
        return 0;
    }
    if (z < 0 || z >= M2C_FIELD(holder, s32 *, 0x34)) {
        return 0;
    }
    return M2C_FIELD(holder, s32 **, 0x14)[z * M2C_FIELD(holder, s32 *, 0x2C) + M2C_FIELD(holder, s32 *, 0x34) * (y * M2C_FIELD(holder, s32 *, 0x2C)) + x];
}

s32 VolumeManager_GetNumVolumesOfType(u32 type) {
    typedef struct Volume {
        u8 pad0[0xC];
        u32 type;
        u8 pad10[0x20];
    } Volume;
    typedef struct World {
        u8 pad0[0x68];
        u32 numVolumes;
        Volume *volumes;
    } World;
    World *world = g_worldData.world;
    s32 count = 0;
    u32 i;

    for (i = 0; i < world->numVolumes; i++) {
        if (type == world->volumes[i].type) {
            count++;
        }
    }
    return count;
}

void *VolumeManager_GetVolumeTypeByIndex(u32 type, u32 index) {
    typedef struct Volume {
        u8 pad0[0xC];
        u32 type;
        u8 pad10[0x20];
    } Volume;
    typedef struct World {
        u8 pad0[0x68];
        u32 numVolumes;
        Volume *volumes;
    } World;
    World *world = g_worldData.world;
    u32 i;

    for (i = 0; i < world->numVolumes; i++) {
        if (world->volumes[i].type == type) {
            if (index == 0) {
                return world->volumes + i;
            }
            index--;
        }
    }
    return NULL;
}

s32 VolumeManager_GetNumVolumesOfTypeByCell(s32 type, s32 cell) {
    extern s32 g_VolumeHolder;
    if ((void *)g_VolumeHolder != NULL) {
        return M2C_FIELD(M2C_FIELD(g_VolumeHolder, u8 **, 0x10) + cell * 0x38 + type * 8, s32 *, 0x0);
    }
    return 0;
}

s32 VolumeManager_GetVolumeTypeByIndexByCell(s32 type, s32 index, s32 cell) {
    typedef struct VolumeList {
        s32 count;
        s32 *volumes;
    } VolumeList;
    typedef struct VolumeCell {
        VolumeList types[7];
    } VolumeCell;
    extern s32 g_VolumeHolder;
    VolumeCell *cells = M2C_FIELD(g_VolumeHolder, VolumeCell **, 0x10);
    return cells[cell].types[type].volumes[index];
}

s8 IsPointWithinVolume(Vector4 *point, void *volume, f32 *distSq) {
    typedef struct Shape {
        u8 pad0[0x8];
        s32 type;
        u8 padC[0x10 - 0xC];
        Vector4 center;
        f32 radius;
        f32 radiusSq;
        void *bsp;
    } Shape;
    s8 BSPN0_CollideWithVector(void *bsp, Vector4 *point);
    void PSVECSubtract(Vector4 *a, Vector4 *b, Vector4 *out);
    f32 PSVECSquareMag(Vector4 *v);
    Shape *shape = volume;
    Vector4 diff __attribute__((aligned(16)));

    switch (shape->type) {
    case 0:
        return BSPN0_CollideWithVector(shape->bsp, point);
    case 1:
        PSVECSubtract(point, &shape->center, &diff);
        *distSq = PSVECSquareMag(&diff);
        return *distSq < shape->radiusSq;
    }
    return 0;
}

#pragma push
#pragma use_lmw_stmw on
VolumeShape *IsPointWithinForceBox(Vector4 *point) {
    extern VolumeHolder *g_VolumeHolder;
    VolumeShape *volume;
    u32 count;
    s32 i;
    s32 cell;

    cell = VolumeManager_GetCell(g_VolumeHolder, point);
    count = g_VolumeHolder != NULL ? g_VolumeHolder->cells[cell].types[1].count : 0;
    i = 0;
    while (i < count) {
        volume = g_VolumeHolder->cells[cell].types[1].volumes[i];
        if (VolumeManager_IsPointInVolume(point, volume)) {
            return volume;
        }
        i++;
    }
    return NULL;
}

#pragma push
#pragma use_lmw_stmw on
void VolumeManager_SetFogLevel(Vector4 *point) {
    typedef struct World {
        u8 pad0[0x9C];
        WorldFog *fog;
    } World;
    extern VolumeHolder *g_VolumeHolder;
    void Gfx_SetFog(GXColor *color, s32 type, f32 start, f32 end);
    static u32 fogColor;
    static u8 fogType = 0xFF;
    static f32 fogNearDistance = -1.0f;
    static f32 fogFarDistance = -1.0f;
    WorldFog *settings;
    s32 cell;
    u32 count;
    VolumeShape *volume;
    GXColor color;
    GXColor worldColor;
    s32 i;

    settings = ((World *)g_worldData.world)->fog;
    cell = VolumeManager_GetCell(g_VolumeHolder, point);
    count = g_VolumeHolder != NULL ? g_VolumeHolder->cells[cell].types[4].count : 0;
    for (i = 0; i < count; i++) {
        volume = g_VolumeHolder->cells[cell].types[4].volumes[i];
        if (VolumeManager_IsPointInVolume(point, volume)) {
            FogSettings *fog = volume->fog;

            if (*(u32 *)&fog->color != fogColor || fogType != fog->type || fogNearDistance != fog->nearDistance || fogFarDistance != fog->farDistance) {
                color = fog->color;
                Gfx_SetFog(&color, fog->type, fog->nearDistance, fog->farDistance);
                fogColor = *(u32 *)&fog->color;
                fogType = fog->type;
                fogNearDistance = fog->nearDistance;
                fogFarDistance = fog->farDistance;
            }
            return;
        }
    }
    if (*(u32 *)settings->color != fogColor || fogType != settings->type || fogNearDistance != settings->nearDistance || fogFarDistance != settings->farDistance) {
        worldColor = *settings->color;
        Gfx_SetFog(&worldColor, settings->type, settings->nearDistance, settings->farDistance);
        fogColor = *(u32 *)settings->color;
        fogType = settings->type;
        fogNearDistance = settings->nearDistance;
        fogFarDistance = settings->farDistance;
    }
}
#pragma pop

#pragma pop

