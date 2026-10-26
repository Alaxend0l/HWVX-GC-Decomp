#include "common.h"


void BulletHole_Init(void) {
    extern u8 g_bulletData[512][0x60];
    extern s32 g_nextBullet;
    s32 i;

    for (i = 0; i < 512; i++) {
        *(s32 *)(g_bulletData[i] + 0x50) = 0;
    }
    g_nextBullet = 1;
}
