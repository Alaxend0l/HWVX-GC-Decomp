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

