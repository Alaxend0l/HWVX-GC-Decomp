#include "common.h"


void AI_Reset(void *arg0) {
    M2C_FIELD(arg0, s32 *, 0xC8) = 1;
    M2C_FIELD(arg0, s8 *, 0xCC) = 0;
}

void AI_Init(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

