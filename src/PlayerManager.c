#include "common.h"
extern u8 g_playerManager[0x20E0];
s8 Player_IsHuman(void *player);

typedef struct PMPlayer {
    u8 pad0[0xB0];
    struct PMPlayer *next;
    struct PMPlayer *prev;
} PMPlayer;

typedef struct PlayerManager {
    u8 pad0[0x20D8];
    PMPlayer *active;
    PMPlayer *free;
} PlayerManager;

static inline PMPlayer *PlayerManager_PopActive(PlayerManager *mgr) {
    PMPlayer *player = mgr->active;
    if (player == NULL) {
        return NULL;
    }
    mgr->active = player->next;
    if (player->next != NULL) {
        player->next->prev = NULL;
    }
    player->next = NULL;
    return player;
}

static inline void PlayerManager_PushFree(PlayerManager *mgr, PMPlayer *player) {
    player->next = mgr->free;
    player->prev = NULL;
    if (player->next != NULL) {
        player->next->prev = player;
    }
    mgr->free = player;
}

void PlayerManager_Create(void) {

}

void PlayerManager_Destroy(void) {

}

void PlayerManager_Release(void) {
    void Player_ReleaseInterface(void *player);
    PlayerManager *mgr = (PlayerManager *)g_playerManager;
    PMPlayer *player;

    while ((player = PlayerManager_PopActive(mgr)) != NULL) {
        Player_ReleaseInterface(player);
        PlayerManager_PushFree(mgr, player);
    }
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

void *PlayerManager_GetPlayerByIndex(u8 index) {
    extern u8 *g_pPlayerManager;
    u8 i = 0;
    void *player;

    for (player = M2C_FIELD(g_pPlayerManager, void **, 0x20D8); player != NULL; player = M2C_FIELD(player, void **, 0xB0)) {
        if (i == index) {
            return player;
        }
        i++;
    }
    return NULL;
}

void *PlayerManager_GetHumanPlayerByIndex(u8 index) {
    extern u8 *g_pPlayerManager;
    void *player;
    u8 i = 0;

    for (player = M2C_FIELD(g_pPlayerManager, void **, 0x20D8); player != NULL; player = M2C_FIELD(player, void **, 0xB0)) {
        if (Player_IsHuman(player)) {
            if (i == index) {
                return player;
            }
            i++;
        }
    }
    return NULL;
}

void PlayerManager_Heartbeat(void) {
    extern u8 *g_pPlayerManager;
    extern u32 g_attackDelay;
    extern u32 g_iDeltaTimeMS;
    extern s16 g_AI_FramesSinceAsyncEventProcessed;
    extern u8 g_AI_AsyncProcess[16];
    void *player;
    void *next;

    for (player = M2C_FIELD(g_pPlayerManager, void **, 0x20D8); player != NULL; player = M2C_FIELD(player, void **, 0xB0)) {
        Player_InputHeartbeat(player);
    }
    if (g_attackDelay > g_iDeltaTimeMS) {
        g_attackDelay -= g_iDeltaTimeMS;
    } else {
        g_attackDelay = 0;
    }
    g_AI_FramesSinceAsyncEventProcessed++;
    if (g_worldData.world != NULL) {
        if ((s8)g_AI_AsyncProcess[0xC] == 0) {
            AI_AddEvent(g_AI_AsyncProcess);
        }
        while ((s8)AI_ProcessEvent()) {
        }
    }
    for (player = M2C_FIELD(g_pPlayerManager, void **, 0x20D8); player != NULL; player = next) {
        next = M2C_FIELD(player, void **, 0xB0);
        M2C_FIELD(player, void (**)(void *), 0x28)(player);
    }
}

void PlayerManager_Render_DEBUG(void) {

}

void PlayerManager_ClearAllCameraFlags_PRIVATE(void) {
    extern u8 *g_pPlayerManager;
    void *player;

    for (player = M2C_FIELD(g_pPlayerManager, void **, 0x20D8); player != NULL; player = M2C_FIELD(player, void **, 0xB0)) {
        M2C_FIELD(player, u32 *, 0x6C) &= ~0x10;
    }
}

