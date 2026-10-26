#include "common.h"


s32 StateGameManager_Heartbeat(void) {
    typedef struct { s32 name; s32 script; } PackRef;
    typedef struct { u8 type; void *movie; PackRef *pack; } Step;
    typedef struct { u8 pad0[4]; s32 menuName; u8 pad8[0x20]; s32 menuFile; u32 numSteps; Step *steps; } MCP;
    extern struct { MCP *mcp; u32 step; } g_GameManagerState;
    extern s8 g_bInterfacePlaying;
    extern s32 g_interfacePackID;
    void StateManager_GotoState(s32);
    void StateMovie_SetMovieToPlay(void *);
    s32 FileStreaming_LoadDataPack(s32, s32, s32, s32);
    void *FileStreaming_InitializeDataPack(s32, s32, s32);
    void *DataPack_GetScript(void *, s32);
    void InterfaceManager_Create(void *, s32, void (*)(void *));
    void StateGameManager_InterfaceHandler(void *);
    void ShowStats_StartFrame(void);
    void ViewManager_PrepForRender(void);
    void ProcessManager_Heartbeat(s32);
    void ShowStats_BeginScene(void);
    void ProcessManager_Render(s32);
    void ShowStats_EndScene(void);
    u32 InterfaceManager_GetTotalActiveInterfaces(void);
    void FileStreaming_ReleaseDataPack(s32, s32);
    void GameSettings_LoadDefaultMemoryCardData(void);
    u32 GameSettings_GetCodeVariable(s32, s32);
    void StateTitleScreen_SetMenuFile(s32, s32);
    s8 loadDefaults = 0;
    Step *step;
    void *pack;

    if (g_GameManagerState.mcp == NULL) {
        StateManager_GotoState(0xB);
        return 1;
    }
    if (g_GameManagerState.step < g_GameManagerState.mcp->numSteps) {
        step = &g_GameManagerState.mcp->steps[g_GameManagerState.step];
        switch (step->type) {
        case 0:
            if (step->movie != NULL) {
                StateMovie_SetMovieToPlay(step->movie);
                StateManager_QueueChange(3);
            }
            break;
        case 1:
            loadDefaults = 1;
        case 4:
            if (!g_bInterfacePlaying) {
                g_interfacePackID = FileStreaming_LoadDataPack(step->pack->name, 1, step->pack->script, 0);
                if ((pack = FileStreaming_InitializeDataPack(g_interfacePackID, 1, 1)) != NULL) {
                    InterfaceManager_Create(DataPack_GetScript(pack, step->pack->script), 0, StateGameManager_InterfaceHandler);
                    g_bInterfacePlaying = 1;
                }
                if (g_GameManagerState.step == g_GameManagerState.mcp->numSteps - 1) {
                    FileStreaming_LoadDataPack(g_GameManagerState.mcp->menuName, 1, g_GameManagerState.mcp->menuFile, 0);
                }
            }
            ShowStats_StartFrame();
            ViewManager_PrepForRender();
            ProcessManager_Heartbeat(0);
            ShowStats_BeginScene();
            ProcessManager_Render(1);
            ShowStats_EndScene();
            if (!g_bInterfacePlaying && InterfaceManager_GetTotalActiveInterfaces() == 0) {
                FileStreaming_ReleaseDataPack(g_interfacePackID, 1);
                g_interfacePackID = -1;
                if (loadDefaults) {
                    GameSettings_LoadDefaultMemoryCardData();
                }
            } else {
                g_bInterfacePlaying = 1;
            }
            break;
        }
        if (!g_bInterfacePlaying && GameSettings_GetCodeVariable(0x198, 0) == 0) {
            if (loadDefaults) {
                GameSettings_LoadDefaultMemoryCardData();
            }
            g_GameManagerState.step++;
        }
    } else {
        StateTitleScreen_SetMenuFile(g_GameManagerState.mcp->menuName, g_GameManagerState.mcp->menuFile);
        StateManager_QueueChange(2);
    }
    return 1;
}
