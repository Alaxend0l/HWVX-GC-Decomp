#include "common.h"


void StateProgressionManager_SetProgressionID(s32 id, s32 subID) {
    extern s32 g_GameProgressionID;
    extern s32 g_GameProgressionState[8];
    g_GameProgressionID = id;
    g_GameProgressionState[2] = subID;
}

void StateProgressionManager_GetProgressionID(s32 *id, s32 *subID) {
    extern s32 g_GameProgressionID;
    extern s32 g_GameProgressionState[8];
    *id = g_GameProgressionID;
    *subID = g_GameProgressionState[2];
}

void StateProgressionManager_RestartCurrentLevel(void) {
    extern u8 g_bRestartCurrentLevel;
    void ExitStateGamePlay(s32 state);
    g_bRestartCurrentLevel = 1;
    ExitStateGamePlay(0xB);
}

