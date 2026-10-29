#include "common.h"

#pragma use_lmw_stmw on

extern s8 g_bRoundEnd;
extern u32 g_playerNumThatWon;


void GameRules_Init(void) {
    extern s8 g_bRoundEnd_803C71CC;
    extern u32 g_playerNumThatWon_803C71C8;
    extern s8 g_bWaitingForRoundReset;
    extern s8 g_bInit_803C71C4;
    extern s32 g_savedActions_802E12E0[];
    extern GRTrigger **g_pTimer_803C71C0;
    void GameSettings_SetCodeVariable(s32 var, s32 value, s32 arg2);
    u32 GameSettings_GetCodeVariable(s32, s32);
    void Trigger_SaveActions(s32 *saved, u32 count);
    u8 *GameSettings_GetMCP(void);
    u32 j;
    GRZone *zone;
    u32 i;
    u8 *mcp;
    GRTrigger **slot;

    g_bRoundEnd_803C71CC = 0;
    g_playerNumThatWon_803C71C8 = 1;
    g_bWaitingForRoundReset = 0;
    g_bInit_803C71C4 = 1;
    GameSettings_SetCodeVariable(0x197, 0, 0);
    GameSettings_SetCodeVariable(0x190, 0, 1);
    for (i = 1; i < 5; i++) {
        GameSettings_SetCodeVariable(0x191, i, 0);
        GameSettings_SetCodeVariable(0x194, i, 0);
    }
    Trigger_SaveActions(g_savedActions_802E12E0, 100);
    GameSettings_SetCodeVariable(0x190, 0, 1);
    g_pTimer_803C71C0 = NULL;
    switch (GameSettings_GetCodeVariable(0x12D, 0)) {
    case 1:
    case 2:
        for (i = 0; i < ((GRWorld *)g_worldData.world)->numTriggers; i++) {
            slot = &((GRWorld *)g_worldData.world)->triggers[i];
            if ((*slot)->type == 0xC) {
                g_pTimer_803C71C0 = slot;
                (*g_pTimer_803C71C0)->time = GameSettings_GetCodeVariable(0x12E, 0);
                if ((*g_pTimer_803C71C0)->time == 0.0f) {
                    g_pTimer_803C71C0 = NULL;
                }
                break;
            }
        }
        break;
    case 3:
        mcp = GameSettings_GetMCP();
        if (mcp != NULL && GameSettings_GetCodeVariable(0x198, 0) != 0) {
            for (i = 0; i < ((GRWorld *)g_worldData.world)->numTriggers; i++) {
                slot = &((GRWorld *)g_worldData.world)->triggers[i];
                if ((*slot)->type == 0xC) {
                    if ((*slot)->time == 0.0f) {
                        (*slot)->time = (*(u32 **)(mcp + 0x24))[1];
                    }
                    break;
                }
            }
        }
        break;
    }
    for (i = 0; i < ((GRWorld *)g_worldData.world)->numZones; i++) {
        zone = &((GRWorld *)g_worldData.world)->zones[i];
        for (j = 0; j < zone->sub->numA; j++) {
            zone->sub->a[j].unk39 = zone->sub->a[j].unk34;
        }
        for (j = 0; j < zone->sub->numC; j++) {
            zone->sub->c[j].unk39 = zone->sub->c[j].unk28;
            zone->sub->c[j].unk38 = 0;
        }
        for (j = 0; j < zone->sub->numB; j++) {
            zone->sub->b[j].unk36 = zone->sub->b[j].unk30;
        }
    }
}

void GameRules_ForceRoundEnd(u32 arg0) {
    g_bRoundEnd = 1;
    g_playerNumThatWon = arg0;
    if ((arg0 < 1U) && (arg0 > 4U)) {
        g_playerNumThatWon = -1U;
    }
}

void GameRules_SetPlayerOrder(u32 *order, u32 count) {
    void *PlayerManager_GetPlayerByIndex(u8);
    u8 PlayerManager_PlayerCount(void);
    u32 i;
    u32 numPlayers;
    u32 j;
    void *player;

    numPlayers = PlayerManager_PlayerCount();
    for (i = 0; i < numPlayers; i++) {
        player = PlayerManager_GetPlayerByIndex(i);
        for (j = 0; j < count; j++) {
            if (M2C_FIELD(player, u32 *, 0xA8) == order[j]) {
                M2C_FIELD(player, s32 *, 0x60) = j + 1;
                break;
            }
        }
    }
}

void GameRules_ResetRound(s8 resetMotion, s8 resetPosition, s8 jettison) {
    typedef struct SpawnPoint {
        Vector4 pos;
        f32 rotX;
        f32 rotY;
        f32 rotZ;
        u8 pad1C[0x24 - 0x1C];
        u8 *info;
        u8 *thing;
    } SpawnPoint;
    typedef struct CarParams {
        Vector4 pos;
        Vector4 quat;
        Vector4 vel;
        u8 *info;
        u8 pad34[0x3C - 0x34];
        u8 *player;
    } CarParams;
    extern s8 g_bWaitingForRoundReset;
    extern s32 g_savedActions_802E12E0[];
    extern GRTrigger **g_pTimer_803C71C0;
    extern u32 g_attackDelay;
    extern Vector4 g_zeroVector;
    void GameSettings_SetCodeVariable(s32 var, s32 value, s32 arg2);
    u32 GameSettings_GetCodeVariable(s32, s32);
    void FileStreaming_ResetQuadResources(void);
    void Trigger_RestoreActions(s32 *saved, u32 count);
    void InterfaceManager_ReleaseAllInterfaces(void);
    void *PlayerManager_GetPlayerByIndex(u8);
    u8 PlayerManager_PlayerCount(void);
    void SetOrientation(void *m, f32 x, f32 y, f32 z);
    void *ThingManager_DARYL_CreateThing(s32 type, void *params);
    void Player_SetThing(u8 *player, u8 *thing);
    void MotionState_ReleaseSlavesByType(s32 state, u8 type);
    void *ThingManagerList_GetByType(void *list, s32 type);
    s8 Player_IsHuman(void *player);
    void AI_Car_Reset(u8 *car);
    void WildCardRack_JettisonAll(void *rack);
    u32 numPlayers;
    u32 i;
    SpawnPoint *spawn;
    SpawnPoint *sp;
    u8 *player;
    u8 *car;
    void *rack;
    CarParams params;
    Matrix4 mtx;

    GameSettings_SetCodeVariable(0x190, 0, GameSettings_GetCodeVariable(0x190, 0) + 1);
    g_bWaitingForRoundReset = 0;
    FileStreaming_ResetQuadResources();
    Trigger_RestoreActions(g_savedActions_802E12E0, 100);
    InterfaceManager_ReleaseAllInterfaces();
    if (g_pTimer_803C71C0 != NULL) {
        (*g_pTimer_803C71C0)->time = GameSettings_GetCodeVariable(0x12E, 0);
    }
    if (GameSettings_GetCodeVariable(0x12D, 0) >= 3) {
        g_attackDelay = 8000;
    }
    numPlayers = PlayerManager_PlayerCount();
    for (i = 0; i < numPlayers; i++) {
        player = PlayerManager_GetPlayerByIndex(i);
        GameSettings_SetCodeVariable(0x194, *(s32 *)(player + 0x60), 0);
        GameSettings_SetCodeVariable(0x193, *(s32 *)(player + 0x60), 0);
        *(s32 *)(player + 0x78) = 0;
        *(s32 *)(player + 0x7C) = 0;
        spawn = *(SpawnPoint **)(player + 0xA8);
        if (spawn != NULL) {
            if (*(u8 **)(player + 0x2C) == NULL) {
                params.pos = spawn->pos;
                params.pos.y += 10.0f;
                params.player = player;
                params.info = spawn->info;
                params.vel = g_zeroVector;
                SetOrientation(&mtx, spawn->rotY, spawn->rotX, spawn->rotZ);
                SubMatrixToQuaternion(&mtx, &params.quat);
                spawn->thing = ThingManager_DARYL_CreateThing(9, &params);
                Player_SetThing(player, spawn->thing);
                *(u8 **)(spawn->thing + 0x90) = *(u8 **)(player + 0xA8);
            }
            sp = *(SpawnPoint **)(player + 0xA8);
            car = *(u8 **)(player + 0x2C);
            if (resetMotion) {
                *(u32 *)(car + 0x78) = *(u32 *)(*(u8 **)(*(u8 **)(car + 0x3BC) + 0xA0) + 0x14);
                *(u32 *)(car + 0x374) = 0;
                MotionState_ReleaseSlavesByType((s32)(car + 0x330), 2);
            }
            if (resetPosition) {
                car = *(u8 **)(player + 0x2C);
                SetOrientation(car + 0x10, sp->rotY, sp->rotX, sp->rotZ);
                SubMatrixToQuaternion(*(u8 **)(player + 0x2C) + 0x10, (Vector4 *)(*(u8 **)(player + 0x2C) + 0x50));
                *(f32 *)(*(u8 **)(player + 0x2C) + 0x40) = sp->pos.x;
                *(f32 *)(*(u8 **)(player + 0x2C) + 0x44) = 10.0f + sp->pos.y;
                *(f32 *)(*(u8 **)(player + 0x2C) + 0x48) = sp->pos.z;
                *(f32 *)(*(u8 **)(player + 0x2C) + 0x0) = 0.0f;
                *(f32 *)(*(u8 **)(player + 0x2C) + 0x4) = 0.0f;
                *(f32 *)(*(u8 **)(player + 0x2C) + 0x8) = 0.0f;
                *(Vector4 *)(car + 0x390) = g_zeroVector;
                *(Vector4 *)(car + 0x380) = g_zeroVector;
                *(Vector4 *)(car + 0x410) = g_zeroVector;
                *(f32 *)(car + 0x4E4) = 0.0f;
                *(f32 *)(car + 0x458) = 0.0f;
                *(u8 *)(car + 0x4E8) = 1;
                *(s32 *)(car + 0x3B0) = -1;
                *(s32 *)(car + 0x3B4) = 1;
                (*(void (**)(void *))(*(u8 **)(*(u8 **)(player + 0x2C) + 0x8C) + 0xC))(*(u8 **)(player + 0x2C));
                *(u32 *)(*(u8 **)(player + 0x2C) + 0x74) |= 0x800;
            }
            if (jettison || GameSettings_GetCodeVariable(0x135, 0) != 0) {
                rack = ThingManagerList_GetByType(*(u8 **)(player + 0x2C) + 0x344, 8);
                if (rack != NULL) {
                    WildCardRack_JettisonAll(rack);
                }
            }
            if (!Player_IsHuman(player)) {
                AI_Car_Reset(player);
            }
        }
    }
}

