#include "common.h"


void ShaderHolder_Destroy(void) {
    M2C_UNK Deallocate(void *);
    extern void *g_pShaderHolder;
    if (g_pShaderHolder != NULL) {
        Deallocate(g_pShaderHolder);
        g_pShaderHolder = NULL;
    }
}

void ShaderHolder_Init(void) {
    extern ShaderHolder *g_pShaderHolder;

    g_pShaderHolder->numShaders = 0;
    g_pShaderHolder->numGX = 0;
}

void ShaderHolder_RemoveGX(s32 gx) {
    extern ShaderHolder *g_pShaderHolder;
    u32 i;

    if (g_pShaderHolder == NULL) {
        return;
    }
    for (i = 0; i < g_pShaderHolder->numGX; i++) {
        if (g_pShaderHolder->gx[i].gx == (void *)gx) {
            g_pShaderHolder->numGX--;
            g_pShaderHolder->gx[i] = g_pShaderHolder->gx[g_pShaderHolder->numGX];
            return;
        }
    }
}

