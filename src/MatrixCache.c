#include "common.h"

void *Allocate(u32 size, u32 flags, const char *name);
void Deallocate(void *p);

MatrixCacheData *g_pMatrixCacheData;

void MatrixCache_Init(void) {

}

void MatrixCache_Destroy(void) {
    Deallocate(g_pMatrixCacheData);
    g_pMatrixCacheData = NULL;
}

