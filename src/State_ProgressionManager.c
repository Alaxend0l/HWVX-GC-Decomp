#include "common.h"

typedef struct LWItem {
    u8 pad0[0x1C];
    void *script;   /* 0x1C */
    u8 pad20[0x10];
    u16 id;         /* 0x30 */
    u8 pad32[4];
    u8 lo;          /* 0x36 */
    u8 hi;          /* 0x37 */
    u8 pad38[8];
} LWItem;

typedef struct LWItemList {
    u8 pad0[0x14];
    u32 count;      /* 0x14 */
    LWItem *items;  /* 0x18 */
} LWItemList;

typedef struct LWQuad {
    u8 pad0[0x1C];
    void *bsp;           /* 0x1C */
    u32 numNeighbors;    /* 0x20 */
    u32 *neighbors;      /* 0x24 */
    u8 pad28[8];
    LWItemList *items;   /* 0x30 */
    u8 pad34[0x1C];
} LWQuad;

typedef struct LWCar {
    u8 pad0[0x24];
    void *script;   /* 0x24 */
    u8 pad28[8];
    u32 id;         /* 0x30 */
    u8 pad34;
    u8 type;        /* 0x35 */
    u8 pad36;
    u8 hi;          /* 0x37 */
    u8 pad38[0x18];
    void *script2;  /* 0x50 */
    void *script3;  /* 0x54 */
    u8 pad58[5];
    u8 pack2;       /* 0x5D */
    u8 pack3;       /* 0x5E */
    u8 pad5F;
} LWCar;

typedef struct LWPacks {
    u8 pad0[0xA0];
    char *aiPack;   /* 0xA0 */
    char *itemPack; /* 0xA4 */
} LWPacks;

typedef struct LWWorld {
    void *unk0;
    u32 pad4;
    u32 numCars;     /* 0x08 */
    LWCar *cars;     /* 0x0C */
    u8 pad10[0x10];
    u32 numQuads;    /* 0x20 */
    LWQuad *quads;   /* 0x24 */
    u8 pad28[0x54];
    char *pack5Name; /* 0x7C */
    s32 pack5ID;     /* 0x80 */
    char *pack6Name; /* 0x84 */
    s32 pack6ID;     /* 0x88 */
    u8 pad8C[0x10];
    LWPacks *packs;  /* 0x9C */
} LWWorld;

typedef struct LWMCP {
    u8 flag;
    u8 pad1[0xF];
    char *name;      /* 0x10 */
} LWMCP;

s32 FileStreaming_LoadDataPack(char *name, s32 arg1, s32 id, s32 arg3);
void *FileStreaming_InitializeDataPack(s32 pack, s32 arg1, s32 arg2);
s32 FileStreaming_GetPackID(char *name, s32 arg1, s32 id);
void FileStreaming_InitializeAllQuads(void);
u32 FileStreaming_GetNumAvailableQuadPacks(void);
void FileStreaming_LoadQuadPacks(LWQuad **quads, s32 count, s32 arg2);
void *DataPack_GetScript(void *pack, s32 id);
void DataPack_RefreshPtrs(void *pack);
s8 IsLoadingThreadRunning(void);
u8 *GameSettings_GetMCP(void);
s32 GameSettings_GetGameType(void);
s8 GameSettings_IsCheatEnabled(s32 cheat);
s8 BSPN0_CollideWithVector(void *bsp, void *pos);
u32 RandomNumberGenerator_GetRandomInteger(u32 lo, u32 hi);
void ShaderHolder_Init(void);
void World_Init(void);
void World_Refresh(void);


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

s32 StateProgressionManager_Heartbeat(void) {
    typedef struct {
        char *name;
        s32 id;
    } ProgPack;
    typedef struct {
        u8 type;
        u8 pad[3];
        void *data;
    } ProgStep;
    typedef struct {
        u32 count;
        ProgStep *steps;
        u16 advance;
    } ProgSequence;
    typedef struct {
        u32 count;
        ProgSequence *seqs;
    } ProgSeqList;
    typedef struct {
        u32 count;
        ProgStep *steps;
        ProgPack *pack;
        char *world;
        ProgSeqList win;
        ProgSeqList lose;
    } ProgLevel;
    typedef struct {
        s32 count;
        ProgLevel *levels;
    } ProgScript;
    typedef struct {
        ProgScript *script;
        s32 phase;
        s32 index;
        u32 step;
        s8 loaded;
        u8 pad[3];
        s32 worldState;
    } ProgState;
    extern ProgState g_GameProgressionState;
    extern u32 g_GameProgressionEndState[2];
    extern u8 g_bLoadingDemoLand;
    static s32 loadingInterfaceID;
    u32 GameSettings_GetCodeVariable(s32, s32);
    void StateManager_GotoState(s32 state);
    void StateManager_QueueChange(s32 state);
    void StateMovie_SetMovieToPlay(void *movie);
    s32 FileStreaming_LoadDataPack(char *name, s32 arg1, s32 id, s32 arg3);
    void *FileStreaming_InitializeDataPack(s32 pack, s32 arg1, s32 arg2);
    s32 FileStreaming_GetPackID(char *name, s32 arg1, s32 id);
    void FileStreaming_ReleaseDataPack(s32 pack, s32 arg1);
    void FileStreaming_HeartBeat(void);
    void *DataPack_GetScript(void *pack, s32 id);
    void DataPack_EnableLoadingSounds(s32 enable);
    void DataPack_CloseFiles(void);
    s32 InterfaceManager_Create(void *script, s32 arg1, s32 arg2);
    u32 InterfaceManager_GetTotalActiveInterfaces(void);
    void InterfaceManager_EndInterface(s32 id);
    void SoundManager_Init(void);
    void ShowStats_StartFrame(void);
    void ShowStats_BeginScene(void);
    void ShowStats_EndScene(void);
    void ViewManager_PrepForRender(void);
    void ProcessManager_Heartbeat(s32 activeOnly);
    void ProcessManager_Render(s32 arg0);
    void ProcessManager_Release(char *name);
    s32 LoadWorldProcess(char *world, u8 loaded);
    int sprintf(char *buf, const char *fmt, ...);
    char buf[512];
    char *world;
    ProgLevel *level;
    ProgSequence *seq;
    ProgPack *pack;
    void *dataPack;
    s32 id;
    s32 phase;

    level = NULL;
    if (g_GameProgressionState.script != NULL) {
        level = &g_GameProgressionState.script->levels[g_GameProgressionState.index];
        if (g_GameProgressionState.index >= g_GameProgressionState.script->count || g_GameProgressionState.index < 0) {
            if (GameSettings_GetCodeVariable(0x198, 0) != 0) {
                StateManager_GotoState(0xC);
            } else {
                StateManager_GotoState(2);
            }
            return 1;
        }
        world = level->world;
    } else {
        world = "DemoLand";
        g_bLoadingDemoLand = 1;
    }
    phase = g_GameProgressionState.phase;
    switch (phase) {
    case 0:
    case 2:
        if (phase == 0) {
            seq = (ProgSequence *)level;
        } else if (g_GameProgressionEndState[0] == 4) {
            if (g_GameProgressionEndState[1] > level->win.count) {
                StateManager_GotoState(2);
                return 1;
            }
            seq = &level->win.seqs[g_GameProgressionEndState[1]];
        } else {
            if (g_GameProgressionEndState[1] > level->lose.count) {
                StateManager_GotoState(2);
                return 1;
            }
            seq = &level->lose.seqs[g_GameProgressionEndState[1]];
        }
        if (g_GameProgressionState.step >= seq->count) {
            if (phase == 0) {
                g_GameProgressionState.phase = 1;
            } else {
                g_GameProgressionState.index += (g_GameProgressionEndState[0] == 4 ? &level->win : &level->lose)->seqs[g_GameProgressionEndState[1]].advance;
                g_GameProgressionState.phase = 0;
                g_GameProgressionState.worldState = 0;
                g_GameProgressionState.step = 0;
            }
        } else {
            switch (seq->steps[g_GameProgressionState.step].type) {
            case 0:
                StateMovie_SetMovieToPlay(seq->steps[g_GameProgressionState.step].data);
                StateManager_QueueChange(3);
                break;
            case 1:
                if (!g_GameProgressionState.loaded) {
                    pack = seq->steps[g_GameProgressionState.step].data;
                    GameSettings_GetMCP();
                    dataPack = FileStreaming_InitializeDataPack(FileStreaming_LoadDataPack(pack->name, 1, pack->id, 0), 1, 1);
                    if (dataPack == NULL) {
                        sprintf(buf, "Could not load data pack %s (%d)XXXXXXXXXXXXXXXXX\n", pack->name, pack->id);
                    }
                    InterfaceManager_Create(DataPack_GetScript(dataPack, pack->id), 0, 0);
                    g_GameProgressionState.loaded = 1;
                } else if (InterfaceManager_GetTotalActiveInterfaces() == 0) {
                    pack = seq->steps[g_GameProgressionState.step].data;
                    g_GameProgressionState.loaded = 0;
                    g_GameProgressionState.step++;
                    FileStreaming_ReleaseDataPack(FileStreaming_GetPackID(pack->name, 1, pack->id), 1);
                    FileStreaming_HeartBeat();
                    SoundManager_Init();
                } else {
                    ShowStats_StartFrame();
                    ViewManager_PrepForRender();
                    ProcessManager_Heartbeat(0);
                    ShowStats_BeginScene();
                    ProcessManager_Render(1);
                    ShowStats_EndScene();
                }
                break;
            }
        }
        break;
    case 1:
        if (g_GameProgressionState.script != NULL && !g_GameProgressionState.loaded && level->pack->name != NULL) {
            ProcessManager_Release("XXXXXXX");
            DataPack_EnableLoadingSounds(0);
            id = FileStreaming_LoadDataPack(level->pack->name, 1, level->pack->id, 0);
            DataPack_EnableLoadingSounds(1);
            loadingInterfaceID = InterfaceManager_Create(DataPack_GetScript(FileStreaming_InitializeDataPack(id, 1, 1), level->pack->id), 0, 0);
            g_GameProgressionState.loaded = 1;
        }
        ShowStats_StartFrame();
        ProcessManager_Heartbeat(0);
        ShowStats_BeginScene();
        ProcessManager_Render(1);
        ShowStats_EndScene();
        if (g_GameProgressionState.worldState != 7) {
            g_GameProgressionState.worldState = LoadWorldProcess(world, g_GameProgressionState.loaded);
            switch (g_GameProgressionState.worldState) {
            case 8:
                g_GameProgressionState.worldState = 7;
                break;
            case 7:
                if (g_GameProgressionState.loaded) {
                    InterfaceManager_EndInterface(loadingInterfaceID);
                }
                break;
            }
        } else if (g_GameProgressionState.worldState == 7 && InterfaceManager_GetTotalActiveInterfaces() == 0) {
            if (g_GameProgressionState.loaded) {
                FileStreaming_ReleaseDataPack(FileStreaming_GetPackID(level->pack->name, 1, level->pack->id), 1);
                g_GameProgressionState.loaded = 0;
            }
            DataPack_CloseFiles();
            StateManager_QueueChange(5);
        }
        break;
    }
    return 1;
}

s32 StateProgressionManager_Shutdown(void) {
    M2C_UNK FileStreaming_GetPackID(s32, M2C_UNK, s8);
    M2C_UNK FileStreaming_ReleaseDataPack(M2C_UNK);
    M2C_UNK PlayerManager_Release();
    extern M2C_UNK g_GameProgressionState;
    u32 var_r31;

    PlayerManager_Release();
    if ((s8) M2C_FIELD(&g_GameProgressionState, u8 *, 0x10) != 0) {
        FileStreaming_GetPackID(M2C_FIELD(&g_GameProgressionState, s32 *, 0x1C), 1, (s8) M2C_FIELD(&g_GameProgressionState, u8 *, 0x10));
        FileStreaming_ReleaseDataPack(1);
        M2C_FIELD(&g_GameProgressionState, u8 *, 0x10) = 0U;
    }
    FileStreaming_ReleaseAllDataPacks(1, 1);
    if ((s32) M2C_FIELD(&g_GameProgressionState, s32 *, 0x14) != 7) {
        var_r31 = 0U;
        do {
            if (var_r31 != 3U) {
                FileStreaming_ReleaseAllDataPacks(1, 1);
            }
            var_r31 += 1;
        } while (var_r31 < 0xBU);
    }
    return 1;
}

