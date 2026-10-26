#include "common.h"
extern u8 g_playerManager[0x20E0];

void PlayerManager_Create(void) {

}

void PlayerManager_Destroy(void) {

}

void *PlayerManager_NewHumanPlayer(void) {
    extern u8 g_playerManager[0x20E0];
    u8 *player;

    for (player = M2C_FIELD(g_playerManager, u8 **, 0x20DC); player != NULL; player = M2C_FIELD(player, u8 **, 0xB0)) {
        if (Player_IsHuman(player)) {
            if (M2C_FIELD(player, u8 **, 0xB4) == NULL) {
                M2C_FIELD(g_playerManager, u8 **, 0x20DC) = M2C_FIELD(player, u8 **, 0xB0);
            } else {
                M2C_FIELD(M2C_FIELD(player, u8 **, 0xB4), u8 **, 0xB0) = M2C_FIELD(player, u8 **, 0xB0);
            }
            if (M2C_FIELD(player, u8 **, 0xB0) != NULL) {
                M2C_FIELD(M2C_FIELD(player, u8 **, 0xB0), u8 **, 0xB4) = M2C_FIELD(player, u8 **, 0xB4);
                M2C_FIELD(player, u8 **, 0xB0) = NULL;
            }
            M2C_FIELD(player, u8 **, 0xB4) = NULL;
            break;
        }
    }
    if (player == NULL) {
        return NULL;
    }
    M2C_FIELD(player, void (**)(void *, s32), 0x24)(player, 0);
    M2C_FIELD(player, u8 **, 0xB0) = M2C_FIELD(g_playerManager, u8 **, 0x20D8);
    M2C_FIELD(player, u8 **, 0xB4) = NULL;
    if (M2C_FIELD(player, u8 **, 0xB0) != NULL) {
        M2C_FIELD(M2C_FIELD(player, u8 **, 0xB0), u8 **, 0xB4) = player;
    }
    M2C_FIELD(g_playerManager, u8 **, 0x20D8) = player;
    return player;
}

u8 PlayerManager_PlayerHumanCount(void) {
    extern u8 g_playerManager[0x20E0];
    u8 count = 0;
    void *player;

    for (player = M2C_FIELD(g_playerManager, void **, 0x20D8); player != NULL; player = M2C_FIELD(player, void **, 0xB0)) {
        if (Player_IsHuman(player)) {
            count++;
        }
    }
    return count;
}

u8 PlayerManager_PlayerCount(void) {
    extern u8 g_playerManager[0x20E0];
    u8 count = 0;
    void *player;

    for (player = M2C_FIELD(g_playerManager, void **, 0x20D8); player != NULL; player = M2C_FIELD(player, void **, 0xB0)) {
        count++;
    }
    return count;
}

void PlayerManager_Render_DEBUG(void) {

}
