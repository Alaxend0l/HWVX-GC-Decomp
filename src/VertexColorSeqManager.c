#include "common.h"


s32 ActionVertexColorSeqChangeMode(s32 *data) {
    u32 count = data[1];
    u32 i;

    for (i = 0; i < count; i++) {
        u8 *seq = (u8 *)data[i + 2];
        switch (data[0]) {
        case 0:
            *(u32 *)(seq + 8) = 0;
            break;
        case 1:
            *(u32 *)(seq + 8) = 1;
            break;
        case 2:
            if (*(u32 *)(seq + 8) == 0) {
                *(u32 *)(seq + 8) = 1;
            } else {
                *(u32 *)(seq + 8) = 0;
            }
            break;
        }
    }
    return 1;
}

void VertexColorSeqManager_Create(void) {

}

void VertexColorSeqManager_Destroy(void) {
    extern s32 g_iTotalVertexColorSeq;
    extern s32 g_iTotalVertexColorTiles;
    g_iTotalVertexColorSeq = 0;
    g_iTotalVertexColorTiles = 0;
}

void VertexColorSeqManager_AddVertices(void *vertices);
void VertexColorSeqManager_Init(void) {
    typedef struct VertexColorTile {
        u8 *light;
        u8 key[0x2C];
        u8 active;
        u8 unk31[3];
    } VertexColorTile;
    typedef struct VCSZone {
        u8 unk0[0x14];
        void *vertices;
        u8 unk18[0x4];
        u32 unk1C;
        u8 unk20[0x30];
    } VCSZone;
    typedef struct VCSWorld {
        u8 unk0[0x20];
        u32 numZones;
        VCSZone *zones;
        u8 unk28[0x38];
        u32 numLights;
        u8 *lights;
    } VCSWorld;
    extern VertexColorTile g_VertexColorTiles[64];
    extern s32 g_iTotalVertexColorSeq;
    extern s32 g_iTotalVertexColorTiles;
    VCSWorld *world;
    u32 i;

    g_iTotalVertexColorSeq = 0;
    g_iTotalVertexColorTiles = 0;
    world = g_worldData.world;
    if (world != NULL) {
        for (i = 0; i < world->numLights; i++) {
            g_VertexColorTiles[i].light = world->lights + i * 0x10;
            g_VertexColorTiles[i].active = 1;
            g_iTotalVertexColorTiles++;
        }
        for (i = 0; i < ((VCSWorld *)g_worldData.world)->numZones; i++) {
            if (((VCSWorld *)g_worldData.world)->zones[i].unk1C == 0) {
                if (((VCSWorld *)g_worldData.world)->zones[i].vertices != NULL) {
                    VertexColorSeqManager_AddVertices(((VCSWorld *)g_worldData.world)->zones[i].vertices);
                }
                break;
            }
        }
    }
}

void VertexColorSeqManager_Heartbeat(void) {
    typedef struct VertexColorTile {
        u8 *light;
        u8 key[0x30];
    } VertexColorTile;
    extern VertexColorTile g_VertexColorTiles[64];
    extern s32 g_iTotalVertexColorTiles;
    extern f32 g_fDeltaTimeMS;
    f32 dt;
    u32 i;

    dt = g_fDeltaTimeMS;
    for (i = 0; i < g_iTotalVertexColorTiles; i++) {
        ExtractColorKey(M2C_FIELD(g_VertexColorTiles[i].light, void **, 0x4), g_VertexColorTiles[i].key, dt);
    }
}

void VertexColorSeqManager_AddVertices(void *vertices) {
    extern s32 g_iTotalVertexColorSeq;
    extern void *g_pVertexColorSeqList[64];
    if (vertices != NULL) {
        g_pVertexColorSeqList[g_iTotalVertexColorSeq++] = vertices;
    }
}

void VertexColorSeqManager_RemoveVertices(void *vertices) {
    extern s32 g_iTotalVertexColorSeq;
    extern void *g_pVertexColorSeqList[64];
    u32 i;

    for (i = 0; i < g_iTotalVertexColorSeq; i++) {
        if (g_pVertexColorSeqList[i] == vertices) {
            g_iTotalVertexColorSeq--;
            g_pVertexColorSeqList[i] = g_pVertexColorSeqList[g_iTotalVertexColorSeq];
            return;
        }
    }
}

s32 VertexColorSeqManager_GetLightIndex(void *light) {
    typedef struct VertexColorTile {
        u8 *light;
        u8 key[0x30];
    } VertexColorTile;
    extern VertexColorTile g_VertexColorTiles[64];
    extern s32 g_iTotalVertexColorTiles;
    u32 i;

    for (i = 0; i < g_iTotalVertexColorTiles; i++) {
        if (light == g_VertexColorTiles[i].light) {
            return i;
        }
    }
    return -1;
}

void VertexColorSeqManager_GetLightColor(u32 index, u32 *color) {
    typedef struct VertexColorTile {
        u8 *light;
        u8 key[0x24];
        u32 color;
        u8 unk2C[0x8];
    } VertexColorTile;
    extern VertexColorTile g_VertexColorTiles[64];
    extern s32 g_iTotalVertexColorTiles;

    if (index < g_iTotalVertexColorTiles) {
        *color = g_VertexColorTiles[index].color;
    }
}

