#include "common.h"

extern NavHolder gs_NavHolder_803C6CD8;
extern s8 gs_useMainRouteOnly_803C7000;
extern PathNode *gs_pOpenListHead_803C7008;
extern PathNode *gs_pClosedListHead_803C7004;

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

