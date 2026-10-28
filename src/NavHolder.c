#include "common.h"

extern NavHolder gs_NavHolder_803C6CD8;
extern s8 gs_useMainRouteOnly_803C7000;
extern PathNode *gs_pOpenListHead_803C7008;
extern PathNode *gs_pClosedListHead_803C7004;

void NavHolder_Init(NavData *data) {
    u32 i;

    gs_NavHolder_803C6CD8.data = data;
    gs_NavHolder_803C6CD8.objects = NULL;
    if (data->numNodes != 0) {
        gs_NavHolder_803C6CD8.objects = (void **)Allocate(data->numNodes * 4, 0, "");
    }
    for (i = 0; i < gs_NavHolder_803C6CD8.data->numNodes; i++) {
        gs_NavHolder_803C6CD8.objects[i] = (void *)Grid_AddObject(5, i, gs_NavHolder_803C6CD8.data->nodes + i * 0x60, gs_NavHolder_803C6CD8.data->nodes + i * 0x60);
    }
}

void NavHolder_Release(void) {
    Deallocate(M2C_FIELD(&gs_NavHolder_803C6CD8, s32 *, 4));
    M2C_FIELD(&gs_NavHolder_803C6CD8, s32 *, 0) = 0;
    M2C_FIELD(&gs_NavHolder_803C6CD8, s32 *, 4) = 0;
}

s32 NavHolder_IsLinked(s32 from, s32 to) {
    u8 i;

    if (from < 0 || to < 0) {
        return 0;
    }
    if (from == to) {
        return 1;
    }
    for (i = 0; i < M2C_FIELD(gs_NavHolder_803C6CD8.data->nodes + from * 0x60, u32 *, 0x1C); i++) {
        if (to == M2C_FIELD(gs_NavHolder_803C6CD8.data->nodes + from * 0x60, s32 **, 0x20)[i]) {
            return 1;
        }
    }
    return 0;
}

void NavHolder_FillPointerArrayPath(u8 *point, u8 **array, u32 max) {
    u8 **out = array;
    u32 i = 0;
    while (point != NULL && i < max) {
        *out++ = point;
        i++;
        point = *(u8 **)(point + 0x38);
    }
    if (i < max) {
        array[i] = NULL;
    }
}

void NavHolder_UseMainRouteOnly(void) {
    gs_useMainRouteOnly_803C7000 = 1;
}

u8 *NavHolder_GetNavPoint(s32 index) {
    u8 *point = NULL;
    if (gs_NavHolder_803C6CD8.data != NULL && index < (s32)gs_NavHolder_803C6CD8.data->numNodes) {
        point = gs_NavHolder_803C6CD8.data->nodes + index * 0x60;
    }
    return point;
}

void *NavHolder_GetFirstWayPoint(void) {
    return gs_NavHolder_803C6CD8.data->firstWayPoint;
}

