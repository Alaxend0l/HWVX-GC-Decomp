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

void BulletHole_Heartbeat(void) {
    extern u8 g_bulletData[512][0x60];
    extern s32 g_iDeltaTimeMS;
    u8 *bullet;
    u32 alpha;
    s32 i;
    s32 delta;

    delta = g_iDeltaTimeMS;
    for (i = 0; i < 512; i++) {
        bullet = g_bulletData[i];
        if (*(s32 *)(bullet + 0x50) > 0) {
            *(s32 *)(bullet + 0x50) -= delta;
            if (*(s32 *)(bullet + 0x50) <= 0) {
                *(s32 *)(bullet + 0x50) = 0;
            } else if (*(s32 *)(bullet + 0x50) < 2000) {
                alpha = (*(s32 *)(bullet + 0x50) << 7) / 2000;
                if (alpha < *(u32 *)(bullet + 0xC)) {
                    *(u32 *)(bullet + 0xC) = alpha;
                }
            }
        }
    }
}

