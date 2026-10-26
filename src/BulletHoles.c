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

void BulletHole_Render(void) {
    extern u8 g_bulletData[512][0x60];
    void *Gfx_Begin_GroupQuadFlat(u16 texture, s32 arg1, s32 arg2);
    void Gfx_End_GroupTri(u32 count);
    void MemCpy32(void *dst, void *src, u32 size);
    u8 *bullet;
    u32 i;
    u16 texture;
    u32 count;
    u8 *dst;

    count = 0;
    texture = 0xFFFF;
    dst = NULL;
    for (i = 0; i < 512; i++) {
        bullet = g_bulletData[i];
        if (*(s32 *)(bullet + 0x50) > 0) {
            if (texture != *(u16 *)(bullet + 0x5C)) {
                if (count != 0) {
                    Gfx_End_GroupTri(count);
                    count = 0;
                }
                dst = Gfx_Begin_GroupQuadFlat(*(u16 *)(bullet + 0x5C), 7, 0);
                texture = *(u16 *)(bullet + 0x5C);
            }
            MemCpy32(dst, bullet, 0x50);
            dst += 0x50;
            count++;
        }
    }
    if (count != 0) {
        Gfx_End_GroupTri(count);
    }
}

void BulletHole_AddSegment(Matrix4 *mtx, s16 texture, GXColor color, s32 life, f32 size) {
    typedef struct BulletHole {
        u32 r;
        u32 g;
        u32 b;
        u32 a;
        f32 v[16];
        s32 life;
        u8 unk54[0x5C - 0x54];
        s16 texture;
        u8 unk5E[0x60 - 0x5E];
    } BulletHole;
    extern BulletHole g_bulletData[512];
    extern u32 g_nextBullet;
    s32 i;

    i = 512;
    while (i-- > 0) {
        if (g_nextBullet == 512) {
            g_nextBullet = 0;
        }
        if (g_bulletData[g_nextBullet++].life == 0) {
            i = g_nextBullet - 1;
            break;
        }
    }
    if (i == 0) {
        return;
    }
    g_bulletData[i].life = life;
    g_bulletData[i].texture = texture;
    g_bulletData[i].r = color.r;
    g_bulletData[i].g = color.g;
    g_bulletData[i].b = color.b;
    g_bulletData[i].a = color.a;
    g_bulletData[i].v[3] = 0.0f;
    g_bulletData[i].v[7] = 1.0f;
    g_bulletData[i].v[0] = mtx->v[3].x + mtx->v[0].x * size + mtx->v[1].x * size;
    g_bulletData[i].v[1] = mtx->v[3].y + mtx->v[0].y * size + mtx->v[1].y * size;
    g_bulletData[i].v[2] = mtx->v[3].z + mtx->v[0].z * size + mtx->v[1].z * size;
    g_bulletData[i].v[4] = mtx->v[3].x - mtx->v[0].x * size + mtx->v[1].x * size;
    g_bulletData[i].v[5] = mtx->v[3].y - mtx->v[0].y * size + mtx->v[1].y * size;
    g_bulletData[i].v[6] = mtx->v[3].z - mtx->v[0].z * size + mtx->v[1].z * size;
    g_bulletData[i].v[12] = mtx->v[3].x - mtx->v[0].x * size - mtx->v[1].x * size;
    g_bulletData[i].v[13] = mtx->v[3].y - mtx->v[0].y * size - mtx->v[1].y * size;
    g_bulletData[i].v[14] = mtx->v[3].z - mtx->v[0].z * size - mtx->v[1].z * size;
    g_bulletData[i].v[8] = mtx->v[3].x + mtx->v[0].x * size - mtx->v[1].x * size;
    g_bulletData[i].v[9] = mtx->v[3].y + mtx->v[0].y * size - mtx->v[1].y * size;
    g_bulletData[i].v[10] = mtx->v[3].z + mtx->v[0].z * size - mtx->v[1].z * size;
}

