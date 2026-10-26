#include "common.h"

extern s8 g_bRoundEnd;
extern u32 g_playerNumThatWon;


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
