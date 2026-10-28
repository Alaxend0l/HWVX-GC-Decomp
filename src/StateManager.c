#include "common.h"


void StateManager_Destroy(void) {
    M2C_UNK Deallocate(void *);
    extern void *g_pStateManager;
    if (g_pStateManager != NULL) {
        Deallocate(g_pStateManager);
        g_pStateManager = NULL;
    }
}

s32 StateManager_GotoState(s32 state) {
    typedef struct GameState {
        void (*interrupt)(void);
        void (*heartbeat)(void);
        s8 (*enter)(s32 prev);
        s8 (*exit)(s32 next);
    } GameState;
    typedef struct StateManager {
        GameState states[13];
        s32 current;
    } StateManager;
    extern StateManager *g_pStateManager;
    extern void (*Interrupt_Process)(void);
    extern void (*Heartbeat_Process)(void);
    void State_GetGameStateStr(s32 state, char *str);
    char curStr[64];
    char nextStr[64];

    if (state >= 13 || state < 0) {
        return g_pStateManager->current;
    }
    State_GetGameStateStr(g_pStateManager->current, curStr);
    State_GetGameStateStr(state, nextStr);
    if (g_pStateManager->current != -1 && g_pStateManager->states[g_pStateManager->current].exit != NULL &&
        g_pStateManager->states[g_pStateManager->current].exit(state) != 1) {
        return g_pStateManager->current;
    }
    if (g_pStateManager->states[state].enter != NULL &&
        g_pStateManager->states[state].enter(g_pStateManager->current) != 1) {
        return g_pStateManager->current;
    }
    Interrupt_Process = g_pStateManager->states[state].interrupt ? g_pStateManager->states[state].interrupt : NULL;
    Heartbeat_Process = g_pStateManager->states[state].heartbeat;
    g_pStateManager->current = state;
    return g_pStateManager->current;
}

s32 StateManager_CurrState(void) {
    extern void *g_pStateManager;
    return M2C_FIELD(g_pStateManager, s32 *, 0xD0);
}

s32 StateManager_QueuedState(void) {
    extern void *g_pStateManager;
    s32 state;

    state = M2C_FIELD(g_pStateManager, s32 *, 0xD4);
    M2C_FIELD(g_pStateManager, s32 *, 0xD4) = -1;
    return state;
}

s32 StateManager_QueueChange(s32 state) {
    extern void *g_pStateManager;
    u32 FileManager_GetDeviceStatus(s32);
    if (M2C_FIELD(g_pStateManager, s32 *, 0xD4) == -1) {
        if (state == 8 || FileManager_GetDeviceStatus(0) == 1) {
            M2C_FIELD(g_pStateManager, s32 *, 0xD4) = state;
        }
    }
    return M2C_FIELD(g_pStateManager, s32 *, 0xD4);
}

void State_GetGameStateStr(s32 state, char *str) {
    char *strcpy(char *dst, const char *src);
    switch (state) {
    case 0:
        strcpy(str, "STATE_NONE");
        break;
    case 4:
        strcpy(str, "STATE_XXXXXXXXXXX");
        break;
    case 5:
        strcpy(str, "STATE_XXXXXXXA");
        break;
    case 8:
        strcpy(str, "STATE_XXXXXXXB");
        break;
    case 11:
        strcpy(str, "STATE_XXXXXXXXXXXXXXXXXXXX");
        break;
    case 12:
        strcpy(str, "STATE_XXXX");
        break;
    case -1:
    default:
        strcpy(str, "STATE_NONE");
        break;
    }
}

