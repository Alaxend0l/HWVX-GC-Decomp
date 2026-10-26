#include "common.h"


void FlashHolder_Init(void) {
    extern u8 g_pFlash[16][0x80];
    u32 i;

    for (i = 0; i < 16; i++) {
        *(void **)(g_pFlash[i] + 0x5C) = NULL;
    }
}
