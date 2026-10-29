#include "common.h"

#pragma use_lmw_stmw on

typedef struct GridNode {
    u32 data;
    u32 id;
    struct GridNode *next;
    struct GridNode *prev;
    struct GridNode *objNext;
    struct GridCell *cell;
} GridNode;

typedef struct GridObject {
    u32 data;
    u32 id;
    GridNode *nodes;
    s32 x0;
    s32 x1;
    s32 z0;
    s32 z1;
    struct GridObject *next;
} GridObject;

typedef struct GridCell {
    GridNode *head;
    s32 count;
} GridCell;

typedef struct GridManager {
    GridCell cells[7][256];
    u32 numX;
    u32 numZ;
    u32 numCells;
    u8 pad380C[0x3810 - 0x380C];
    Vector4 min;
    Vector4 max;
    f32 cellSizeX;
    f32 cellSizeZ;
    GridNode nodes[4048];
    GridNode *freeNodes;
    GridObject objects[1024];
    GridObject *freeObjects;
    s32 numFreeObjects;
    u8 pad233C4[0x233D0 - 0x233C4];
} GridManager;

GridManager gGridManager;

static inline void Grid_GetCell(Vector4 *pos, s32 *cellX, s32 *cellZ) {
    f32 fx;
    f32 fz;

    fx = (pos->x - gGridManager.min.x) / gGridManager.cellSizeX;
    if (fx < 0.0f) {
        fx = 0.0f;
    } else if (fx >= (f32)gGridManager.numX) {
        fx = (f32)gGridManager.numX - 1.0f;
    }
    fz = (pos->z - gGridManager.min.z) / gGridManager.cellSizeZ;
    if (fz < 0.0f) {
        fz = 0.0f;
    } else if (fz >= (f32)gGridManager.numZ) {
        fz = (f32)gGridManager.numZ - 1.0f;
    }
    *cellX = fx;
    *cellZ = fz;
}

void Grid_Release(void) {

}
