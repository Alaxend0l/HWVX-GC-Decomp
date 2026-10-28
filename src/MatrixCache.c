#include "common.h"


void MatrixCache_Init(void) {

}

void MatrixCache_Destroy(void) {
    M2C_UNK Deallocate(s32);
    extern s32 g_pMatrixCacheData;
    Deallocate(g_pMatrixCacheData);
    g_pMatrixCacheData = 0;
}

