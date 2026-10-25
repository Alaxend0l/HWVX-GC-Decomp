#include "common.h"


void Geometry_Destroy(void) {
    M2C_UNK Geometry_Init(M2C_UNK);
    extern s32 g_pGeometryHandler;
    Geometry_Init(1);
    Deallocate(g_pGeometryHandler);
    g_pGeometryHandler = 0;
}

void Geometry_Init(s8 clear) {
    void Geometry_FreeGeometry(s32 geometry);
    extern u8 *g_pGeometryHandler;
    u32 i;

    for (i = 0; i < M2C_FIELD(g_pGeometryHandler, u32 *, 0x24004); i++) {
        if (clear == 1) {
            *(s16 *)(g_pGeometryHandler + i * 0x48 + 0x44) = 0;
        }
        Geometry_FreeGeometry(*(s32 *)(g_pGeometryHandler + i * 0x48));
    }
}
