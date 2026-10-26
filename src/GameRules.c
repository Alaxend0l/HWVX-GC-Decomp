#include "common.h"

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
