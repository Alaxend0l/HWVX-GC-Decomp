#include "common.h"

#pragma use_lmw_stmw on

void *Allocate(u32 size, u32 flags, const char *name);
void Deallocate(void *p);

MatrixCacheData *g_pMatrixCacheData;

void MatrixCache_Create(void) {
    u8 *buf;
    u32 i;

    g_pMatrixCacheData = Allocate(0x2A720, 0, "");
    buf = (u8 *)g_pMatrixCacheData + sizeof(MatrixCacheData);
    for (i = 0; i < 0x60; i++) {
        g_pMatrixCacheData->small[i].key = (u32)buf;
        g_pMatrixCacheData->small[i].mtx = NULL;
        buf += 0x100;
    }
    for (i = 0; i < 0x80; i++) {
        g_pMatrixCacheData->medium[i].key = (u32)buf;
        g_pMatrixCacheData->medium[i].mtx = NULL;
        buf += 0x400;
    }
    for (i = 0; i < 4; i++) {
        g_pMatrixCacheData->large[i].key = (u32)buf;
        g_pMatrixCacheData->large[i].mtx = NULL;
        buf += 0x1000;
    }
}

void MatrixCache_Init(void) {

}

void MatrixCache_Destroy(void) {
    Deallocate(g_pMatrixCacheData);
    g_pMatrixCacheData = NULL;
}

