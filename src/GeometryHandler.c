#include "common.h"

#pragma use_lmw_stmw on

typedef struct GeometryEntry {
    void *geometry;
    char name[0x40];
    s16 refCount;
    u8 unk46[2];
} GeometryEntry;

typedef struct GeometryHandler {
    GeometryEntry entries[0x800];
    s32 count;
    u32 max;
} GeometryHandler;

void *Allocate(u32 size, s32 type, const char *name);

s32 Geometry_Create(void) {
    extern GeometryHandler *g_pGeometryHandler;
    u32 i;

    g_pGeometryHandler = Allocate(sizeof(GeometryHandler), 0, "");
    if (g_pGeometryHandler == NULL) {
        return 0;
    }
    g_pGeometryHandler->max = 0x800;
    g_pGeometryHandler->count = 0;
    for (i = 0; i < g_pGeometryHandler->max; i++) {
        g_pGeometryHandler->entries[i].geometry = NULL;
        g_pGeometryHandler->entries[i].refCount = 0;
    }
    return 1;
}

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

void Geometry_FreeGeometry(void *geometry) {
    void Deallocate(void *);
    extern u8 *g_pGeometryHandler;
    u32 i;

    if (geometry != NULL) {
        for (i = 0; i < M2C_FIELD(g_pGeometryHandler, u32 *, 0x24004); i++) {
            if (*(void **)(g_pGeometryHandler + i * 0x48) == geometry) {
                (*(s16 *)(g_pGeometryHandler + i * 0x48 + 0x44))--;
                if (*(s16 *)(g_pGeometryHandler + i * 0x48 + 0x44) < 1) {
                    Deallocate(geometry);
                    *(void **)(g_pGeometryHandler + i * 0x48) = NULL;
                    *(u8 *)(g_pGeometryHandler + i * 0x48 + 4) = 0;
                    M2C_FIELD(g_pGeometryHandler, s32 *, 0x24000)--;
                }
            }
        }
    }
}

