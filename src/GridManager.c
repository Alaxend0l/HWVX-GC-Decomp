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

u32 Grid_GetNearestObjects(Vector4 *min, Vector4 *max, u32 typeMask, GridResult *results, u32 maxResults, s8 (*filter)(GridNode *, s32), s32 filterArg) {
    s8 first;
    u32 count;
    u32 x;
    u32 z;
    u32 type;
    GridResult *result;
    u32 i;
    s8 found;
    GridNode *node;
    u32 x0;
    u32 z0;
    u32 x1;
    u32 z1;

    first = 1;
    count = 0;
    Grid_GetCell(min, (s32 *)&x0, (s32 *)&z0);
    Grid_GetCell(max, (s32 *)&x1, (s32 *)&z1);
    while (count == 0) {
        for (x = x0; x <= x1; x++) {
            for (z = z0; z <= z1; z++) {
                if (first || x == x0 || x == x1 || z == z0 || z == z1) {
                    for (type = 0; type < 7; type++) {
                        if (typeMask & (1 << type)) {
                            node = gGridManager.cells[type][x * gGridManager.numZ + z].head;
                            while (node != NULL) {
                                result = results;
                                found = 0;
                                for (i = 0; i < count && !found; i++) {
                                    if (result->id == node->id) {
                                        found = 1;
                                    }
                                    result++;
                                }
                                if (!found) {
                                    if (count < maxResults) {
                                        if (filter == NULL || filter(node, filterArg) == 1) {
                                            results[count] = *(GridResult *)node;
                                            count++;
                                        }
                                    } else {
                                        node = NULL;
                                    }
                                }
                                if (node != NULL) {
                                    node = node->next;
                                }
                            }
                        }
                    }
                }
            }
        }
        if (count == 0) {
            first = 0;
            if (x0 == 0 && z0 == 0 && x1 >= gGridManager.numX - 1 && z1 >= gGridManager.numZ - 1) {
                break;
            }
            if (x0 != 0) {
                x0--;
            }
            if (z0 != 0) {
                z0--;
            }
            if (x1 < gGridManager.numX - 1) {
                x1++;
            } else {
                x1 = gGridManager.numX - 1;
            }
            if (z1 < gGridManager.numZ - 1) {
                z1++;
            } else {
                z1 = gGridManager.numZ - 1;
            }
        }
    }
    return count;
}

