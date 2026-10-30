#include "common.h"

char g_saveFileName[11];
char g_fileNames[8][41];
u8 g_DDRsavedGameData[0xE83C];
s8 g_DDRescueGameData[0x1CDC];
u8 g_HWsavedGameData[0x8FC];
HotWheelsGameData g_HotWheelsGameData;
GeneralGameSettings g_generalGameSettings;
CodeVariables g_codeVariables;
extern s8 g_bValidFileName[8];
extern s8 g_bRefreshMC;
extern s8 g_bRefreshMC_FileList;
extern s8 g_bRefreshMC_FileInfo;
u8 FileManager_FormatDevice(char *device);

static s8 bFirstTime = 1;
static u8 *pMCP;

s32 FileStreaming_LoadDataPack(void *name, s32 type, u32 id, s8 threaded);
u8 *GameSettings_GetMCP(void);
void *FileStreaming_InitializeDataPack(void *, s32, s32);
u8 *DataPack_GetScript(void *, s32);
s32 PlayerManager_PlayerHumanCount(void);

static inline u8 *GameSettings_LoadMCP(void)
{
    static s8 bFirstTime = 1;
    static u8 *pMCP;
    void *pack;

    if (bFirstTime)
    {
        bFirstTime = 0;
        pack = FileStreaming_InitializeDataPack((void *)FileStreaming_LoadDataPack("mcp", 3, 0, 0), 3, 1);
        if (pack == NULL)
        {
            return NULL;
        }
        pMCP = DataPack_GetScript(pack, 0);
    }
    return pMCP;
}

void GameSetting_Init(void) {
    void GameSettings_SetDefaultControllerState(void);
    void GameSettings_ResetPlayerVehicleItem(void);
    void GameSettings_ResetGame(void);
    void GameSettings_ResetControllerConfig(s32 player);
    s32 GameSettings_GetGameType(void);
    void GameSettings_SetCodeVariable(s32 var, s32 value, s32 arg2);
    u8 OSGetSoundMode(void);
    void *memset(void *dst, s32 c, u32 n);
    extern GameInfo g_gameInfo[2];
    extern s8 g_bGameSettingsInitialized;
    extern u8 *g_pGameData;
    extern u8 *g_pSavedGameData;
    u32 i;

    g_bGameSettingsInitialized = 1;
    memset(&g_codeVariables, 0, sizeof(CodeVariables));
    g_codeVariables.device = (char *)1;
    GameSettings_SetDefaultControllerState();
    GameSettings_ResetPlayerVehicleItem();
    GameSettings_ResetGame();
    memset(&g_generalGameSettings, 0, sizeof(GeneralGameSettings));
    g_generalGameSettings.difficulty = 0;
    g_generalGameSettings.audio.speakerMode = OSGetSoundMode();
    g_generalGameSettings.audio.musicVolume = 100;
    g_generalGameSettings.audio.sfxVolume = 100;
    for (i = 1; i <= 4; i++) {
        GameSettings_ResetControllerConfig(i);
        g_generalGameSettings.controllers[i - 1].data[0] = 1;
    }
    switch (GameSettings_GetGameType()) {
    case 0:
        g_gameInfo[0].unk14 = 0x8F0;
        g_gameInfo[0].unk18 = 0xF4;
        g_pGameData = (u8 *)&g_HotWheelsGameData;
        g_pSavedGameData = g_HWsavedGameData;
        memset(&g_HotWheelsGameData, 0, sizeof(HotWheelsGameData));
        g_HotWheelsGameData.cars[3] = 1;
        g_HotWheelsGameData.cars[4] = 1;
        g_HotWheelsGameData.cars[5] = 1;
        g_HotWheelsGameData.unk93[0] = 1;
        g_HotWheelsGameData.unk93[1] = 1;
        g_HotWheelsGameData.unk93[2] = 1;
        g_HotWheelsGameData.tracks[0] = 1;
        g_HotWheelsGameData.tracks[5] = 1;
        g_HotWheelsGameData.tracks[0x13] = 1;
        g_HotWheelsGameData.tracks[0x14] = 1;
        g_HotWheelsGameData.tracks[0x15] = 1;
        g_HotWheelsGameData.tracks[0x16] = 1;
        g_HotWheelsGameData.tracks[0x1D] = 1;
        g_HotWheelsGameData.tracks[0x1E] = 1;
        g_HotWheelsGameData.tracks[0x1F] = 1;
        g_HotWheelsGameData.tracks[0x20] = 1;
        g_HotWheelsGameData.tracks[0x27] = 1;
        g_HotWheelsGameData.unkAA[0] = 1;
        g_HotWheelsGameData.unkAA[1] = 1;
        g_HotWheelsGameData.unkAA[4] = 1;
        break;
    case 1:
        g_gameInfo[1].unk14 = 0xE830;
        g_gameInfo[1].unk18 = 0x1CDC;
        g_pGameData = (u8 *)g_DDRescueGameData;
        g_pSavedGameData = g_DDRsavedGameData;
        memset(g_DDRescueGameData, 0, sizeof(g_DDRescueGameData));
        g_DDRescueGameData[0] = 1;
        g_DDRescueGameData[0x14] = 1;
        GameSettings_SetCodeVariable(0x6A, 0xA, 1);
        GameSettings_SetCodeVariable(0x6B, 0xA, 1);
        break;
    }
}

void GameSettings_ResetGame(void)
{
    s32 i;

    g_codeVariables.unk30 = 1;
    for (i = 0; i < 4; i++)
    {
        g_codeVariables.unk34[i] = 0;
        g_codeVariables.unk44[i] = 0;
        g_codeVariables.unk54[i] = 0;
        g_codeVariables.unk64[i] = 0;
    }
    g_codeVariables.unkE4 = 0;
    g_codeVariables.unkE8 = 0;
    g_codeVariables.unkEC = 300000;
    g_codeVariables.unkF0 = 10;
    g_codeVariables.unkF4 = 1000;
    g_codeVariables.unkF8 = 3;
    g_codeVariables.unkFC = 0;
    g_codeVariables.unk100 = 3;
    g_codeVariables.unk104 = 1;
    g_codeVariables.unk108 = 1;
    g_codeVariables.unk111 = 1;
}

void GameSettings_SetDefaultControllerState(void)
{
    s32 i;

    for (i = 0; i < 4; i++)
    {
        g_codeVariables.controllerState[i] = 1;
    }
}

void GameSettings_ResetPlayerVehicleItem(void) {
    M2C_FIELD(&g_codeVariables, s32 *, 0) = -1;
    M2C_FIELD(&g_codeVariables, s32 *, 0x10) = -1;
    M2C_FIELD(&g_codeVariables, s32 *, 4) = -1;
    M2C_FIELD(&g_codeVariables, s32 *, 0x14) = -1;
    M2C_FIELD(&g_codeVariables, s32 *, 8) = -1;
    M2C_FIELD(&g_codeVariables, s32 *, 0x18) = -1;
    M2C_FIELD(&g_codeVariables, s32 *, 0xC) = -1;
    M2C_FIELD(&g_codeVariables, s32 *, 0x1C) = -1;
}

u8 GameSettings_GetGameType(void)
{
    u8 *mcp;

    mcp = GameSettings_GetMCP();
    if (mcp != NULL)
    {
        return mcp[1];
    }
    return 0;
}

u8 *GameSettings_GetMCP(void)
{
    void *pack;

    if (bFirstTime)
    {
        bFirstTime = 0;
        pack = FileStreaming_InitializeDataPack((void *)FileStreaming_LoadDataPack("mcp", 3, 0, 0), 3, 1);
        if (pack == NULL)
        {
            return NULL;
        }
        pMCP = DataPack_GetScript(pack, 0);
    }
    return pMCP;
}

#pragma push
#pragma use_lmw_stmw off
void GameSettings_ResetControllerConfig(s32 player)
{
    void *FileStreaming_InitializeDataPack(void *, s32, s32);
    u8 *DataPack_GetScript(void *, s32);
    ControllerSettings *GameSettings_GetControllerSettings(s32 player);
    u8 *mcp;
    void *pack;
    ControllerSettings *settings;
    u8 *src;

    if (bFirstTime)
    {
        bFirstTime = 0;
        pack = FileStreaming_InitializeDataPack((void *)FileStreaming_LoadDataPack("mcp", 3, 0, 0), 3, 1);
        if (pack == NULL)
        {
            mcp = NULL;
            goto done;
        }
        pMCP = DataPack_GetScript(pack, 0);
    }
    mcp = pMCP;
done:
    settings = GameSettings_GetControllerSettings(player);
    if (mcp == NULL || *(u32 *)((char *)mcp + 0x3c) == 0)
    {
        memset(&settings->data[2], 0, 0x10);
    }
    else
    {
        src = (u8 *)*(u32 *)((char *)*(u32 *)((char *)*(u32 *)((char *)*(u32 *)((char *)mcp + 0x40) + 0) + 0) + 4);
        memcpy(&settings->data[2], src, 0x10);
    }
}

#pragma use_lmw_stmw on
u32 GameSettings_GetCodeVariable(s32 var, u32 arg) {
    typedef struct WorldEntry {
        u32 unk0;
        u8 *thing;
    } WorldEntry;
    typedef struct World {
        u8 pad0[0x58];
        u32 numEntries;
        WorldEntry *entries;
    } World;
    void GameSetting_Init(void);
    s8 GameSettings_IsCheatEnabled(u32 cheat);
    s32 Input_GetContollersPluggedIn(void);
    s32 StateManager_CurrState(void);
    u8 PlayerManager_PlayerCount(void);
    u8 *PlayerManager_GetPlayerByIndex(u8 index);
    u8 *PlayerManager_GetHumanPlayerByIndex(u8 index);
    s8 Player_IsHuman(void *player);
    s32 GameSettings_GetDefaultMemoryCardStatus(s32 *device, s8 bForce);
    s32 GameSettings_GetMemoryCardStatus(s32 device, s32 arg1);
    extern s8 g_bGameSettingsInitialized;
    static s8 bFirstTime = 1;
    static u8 *pMCP;
    s32 type;
    u8 *mcp;
    void *pack;
    u32 i;
    u8 count;
    u8 *player;
    s32 plugged;
    s32 port;
    s32 n;
    s32 device;
    s32 status;
    World *world;

    if (bFirstTime) {
        bFirstTime = 0;
        pack = FileStreaming_InitializeDataPack((void *)FileStreaming_LoadDataPack("mcp", 3, 0, 0), 3, 1);
        if (pack == NULL) {
            mcp = NULL;
            goto done;
        }
        pMCP = DataPack_GetScript(pack, 0);
    }
    mcp = pMCP;
done:
    if (mcp != NULL) {
        type = mcp[1];
    } else {
        type = 0;
    }
    if (!g_bGameSettingsInitialized) {
        GameSetting_Init();
    }

    switch (var) {
    case 0x64:
        if (GameSettings_IsCheatEnabled(1)) {
            return 1;
        }
        switch (type) {
        case 0:
            if (arg < 0x21) {
                return g_HotWheelsGameData.cars[arg];
            }
            break;
        case 1:
            if (arg < 0x14) {
                return g_DDRescueGameData[arg];
            }
            break;
        }
        break;
    case 0x65:
        if (GameSettings_IsCheatEnabled(3)) {
            return 1;
        }
        switch (type) {
        case 0:
            if (arg < 0x39) {
                return g_HotWheelsGameData.tracks[arg];
            }
            break;
        case 1:
            if (arg < 0xA) {
                return g_DDRescueGameData[0x14 + arg];
            }
            break;
        }
        break;
    case 0x66:
        if (GameSettings_IsCheatEnabled(2)) {
            return 1;
        }
        switch (type) {
        case 0:
            if (arg < 0xC) {
                return g_HotWheelsGameData.unk93[arg];
            }
            break;
        }
        break;
    case 0x68:
        if (GameSettings_IsCheatEnabled(4)) {
            return 1;
        }
        switch (type) {
        case 0:
            if (arg < 5) {
                return g_HotWheelsGameData.unkAA[arg];
            }
            break;
        case 1:
            if (arg < 5) {
                return g_DDRescueGameData[0x29 + arg];
            }
            break;
        }
        break;
    case 0x69:
        return GameSettings_IsCheatEnabled(9);
    case 0x6A:
        switch (type) {
        case 0:
            return g_HotWheelsGameData.unk9F[arg] || g_HotWheelsGameData.unkA9;
        case 1:
            return g_DDRescueGameData[0x1E + arg] || g_DDRescueGameData[0x28];
        }
        break;
    case 0x6B:
        return GameSettings_IsCheatEnabled(arg);
    case 0x6C:
        return g_codeVariables.unk128[arg];
    case 0xC8:
        return g_generalGameSettings.difficulty;
    case 0xC9:
        if ((1 << (arg - 1)) & Input_GetContollersPluggedIn()) {
            return g_codeVariables.controllerState[arg - 1];
        }
        g_codeVariables.playerVehicle[arg - 1] = -1;
        g_codeVariables.controllerState[arg - 1] = 1;
        return 0;
    case 0xCA:
        switch (type) {
        case 0:
            if (arg < 0x1E) {
                return g_HotWheelsGameData.unkB0[arg];
            }
            break;
        case 1:
            if (arg < 0x1E) {
                return ((u16 *)g_DDRescueGameData)[0x17 + arg];
            }
            break;
        }
        break;
    case 0xCB:
        if (StateManager_CurrState() == 5) {
            u8 numPlayers = PlayerManager_PlayerCount();
            u32 index;

            for (index = 0; index < numPlayers; index++) {
                player = PlayerManager_GetPlayerByIndex(index);
                if (*(u32 *)(player + 0x60) == arg && *(u8 **)(player + 0xA8) != NULL) {
                    return *(u32 *)(*(u8 **)(player + 0xA8) + 0x30);
                }
            }
        }
        if (arg == 1 && g_codeVariables.unkE8 == 0) {
            switch (type) {
            case 0:
                return g_HotWheelsGameData.unkEC;
            case 1:
                return *(s32 *)&g_DDRescueGameData[0x1CD4];
            }
            break;
        }
        return g_codeVariables.playerVehicle[arg - 1];
    case 0xCC:
        return g_codeVariables.playerItem[arg - 1];
    case 0xCD:
        n = 0;
        for (port = 0; port < 4; port++) {
            if (g_codeVariables.playerVehicle[port] != -1) {
                n++;
            }
        }
        return n;
    case 0xCE:
        if (arg <= 4) {
            return g_generalGameSettings.controllers[arg - 1].data[1];
        }
        return 0;
    case 0xD1:
        if (arg <= 4) {
            return (s8)g_generalGameSettings.controllers[arg - 1].data[0];
        }
        return 0;
    case 0xD2:
        if (arg <= 4) {
            return g_codeVariables.playerVehicle[arg - 1] != -1;
        }
        return 0;
    case 0xCF:
        return g_generalGameSettings.audio.musicVolume;
    case 0xD0:
        return g_generalGameSettings.audio.sfxVolume;
    case 0xD6:
        return g_generalGameSettings.audio.speakerMode;
    case 0xD3:
        plugged = Input_GetContollersPluggedIn();
        n = 0;
        for (port = 0; port < 4; port++) {
            if ((1 << port) & plugged) {
                n++;
            }
        }
        return n;
    case 0xD4:
        plugged = Input_GetContollersPluggedIn();
        count = PlayerManager_PlayerHumanCount();
        for (i = 0; i < count; i++) {
            port = *(s32 *)(PlayerManager_GetHumanPlayerByIndex(i) + 0xC8);
            if (!(plugged & (1 << port))) {
                return port + 1;
            }
        }
        return 0;
    case 0x19A:
        world = g_worldData.world;
        if (world != NULL) {
            n = 0;
            for (port = 0; port < world->numEntries; port++) {
                if (world->entries[port].thing[1] == 0x17) {
                    n++;
                }
            }
            return n;
        }
        break;
    case 0xD5:
        return g_codeVariables.unkE0;
    case 0x12C:
        return g_codeVariables.unkE4;
    case 0x12D:
        return g_codeVariables.unkE8;
    case 0x12E:
        return g_codeVariables.unkEC;
    case 0x12F:
        return g_codeVariables.unkF0;
    case 0x130:
        return g_codeVariables.unkF4;
    case 0x131:
        return g_codeVariables.unkF8;
    case 0x132:
        return g_codeVariables.unkFC;
    case 0x133:
        return g_codeVariables.unk100;
    case 0x134:
        return g_codeVariables.unk104;
    case 0x136:
        return g_codeVariables.unk108;
    case 0x135:
        return g_codeVariables.unk109[7];
    case 0x190:
        return g_codeVariables.unk30;
    case 0x191:
        if (arg <= 4) {
            return g_codeVariables.unk34[arg - 1];
        }
        break;
    case 0x192:
        if (arg <= 4) {
            return g_codeVariables.unk44[arg - 1];
        }
        break;
    case 0x193:
        if (arg <= 4) {
            return g_codeVariables.unk54[arg - 1];
        }
        break;
    case 0x194:
        if (arg <= 4) {
            return g_codeVariables.unk64[arg - 1];
        }
        break;
    case 0x195:
        switch (type) {
        case 0:
            if (GameSettings_IsCheatEnabled(3)) {
                return 0xD;
            }
            for (n = 0x12; n > 4; n--) {
                if (g_HotWheelsGameData.tracks[n]) {
                    return n - 5;
                }
            }
            break;
        }
        break;
    case 0x199:
        switch (type) {
        case 0:
            if (GameSettings_IsCheatEnabled(3)) {
                return 0x11;
            }
            for (n = 0x38; n > 0x26; n--) {
                if (g_HotWheelsGameData.tracks[n]) {
                    return n - 0x27;
                }
            }
            break;
        case 1:
            break;
        }
        break;
    case 0x196:
        count = PlayerManager_PlayerCount();
        for (i = 0; i < count; i++) {
            player = PlayerManager_GetPlayerByIndex(i);
            if (*(u32 *)(player + 0x60) == arg) {
                if (Player_IsHuman(player)) {
                    return *(u32 *)(player + 0x60);
                }
                return 5;
            }
        }
        return 0;
    case 0x197:
        return g_codeVariables.unk109[0];
    case 0x198:
        return g_codeVariables.unk109[1];
    case 0x1F4:
        return GameSettings_GetDefaultMemoryCardStatus(&device, 0);
    case 0x1F5:
        GameSettings_GetDefaultMemoryCardStatus(&status, 0);
        return status;
    case 0x1F6:
        return GameSettings_GetMemoryCardStatus(arg, 0);
    case 0x1F7:
        return (u32)g_codeVariables.device;
    case 0x1F8:
        return g_codeVariables.fileIndex;
    case 0x1F9:
        return g_codeVariables.unk11C[0];
    case 0x1FA:
        return g_codeVariables.unk11C[1];
    case 0x1FB:
        return g_codeVariables.bOverwritingFile;
    case 0x1FC:
        return g_codeVariables.unk124;
    case 0x1FD:
        return g_codeVariables.unk11C[2];
    case 0x1FE:
        return g_codeVariables.unk120[0];
    case 0x258:
        return 2;
    case 0x259:
        return 1;
    case 0x19B:
        return g_codeVariables.unk111;
    case 0x19C:
        if (arg < 0x1E) {
            return g_codeVariables.unk68[arg];
        }
        break;
    }
    return 0;
}
void GameSettings_GameBoyConnected(void) {
    u8 *mcp;
    void *pack;
    s32 state;

    if (bFirstTime) {
        bFirstTime = 0;
        pack = FileStreaming_InitializeDataPack((void *)FileStreaming_LoadDataPack("mcp", 3, 0, 0), 3, 1);
        if (pack == NULL) {
            mcp = NULL;
            goto done;
        }
        pMCP = DataPack_GetScript(pack, 0);
    }
    mcp = pMCP;
done:
    if (mcp != NULL) {
        state = mcp[1];
    } else {
        state = 0;
    }
    switch (state) {
    case 0:
        g_HotWheelsGameData.cars[0] = 1;
        g_HotWheelsGameData.cars[1] = 1;
        g_HotWheelsGameData.cars[2] = 1;
        break;
    case 1:
        break;
    }
}

s32 GameSettings_GetDifficulty(void) {
    return g_generalGameSettings.difficulty;
}

u8 GameSettings_GetNumOfPlayers(void) {
    return PlayerManager_PlayerHumanCount();
}

s8 GameSettings_IsCheatEnabled(s32 cheat) {
    static s8 bFirstTime = 1;
    static u8 *pMCP;
    u8 *mcp;
    void *pack;
    s32 type;
    s8 *cheats;
    u32 i;
    u8 result;

    if (bFirstTime) {
        bFirstTime = 0;
        pack = FileStreaming_InitializeDataPack((void *)FileStreaming_LoadDataPack("mcp", 3, 0, 0), 3, 1);
        if (pack == NULL) {
            mcp = NULL;
            goto done;
        }
        pMCP = DataPack_GetScript(pack, 0);
    }
    mcp = pMCP;
done:
    if (mcp != NULL) {
        type = mcp[1];
    } else {
        type = 0;
    }
    switch (type) {
    case 0:
        cheats = g_HotWheelsGameData.unk9F;
        break;
    case 1:
        cheats = &g_DDRescueGameData[0x1E];
        break;
    }
    if (cheat == 9) {
        for (i = 0; i < 11; i++) {
            if (i != 9 && cheats[i]) {
                return 1;
            }
        }
        if (i == 11) {
            return 0;
        }
    }
    if (((u32)(cheat - 1) <= 2 || cheat == 4) && cheats[10] && g_codeVariables.unk128[10]) {
        return 1;
    }
    result = 0;
    if ((cheats[10] || cheats != NULL) && g_codeVariables.unk128[cheat]) {
        result = 1;
    }
    return result;
}

ControllerSettings *GameSettings_GetControllerSettings(s32 player) {
    return &g_generalGameSettings.controllers[player - 1];
}

void GameSettings_GetGameInfo(void *info) {
    void *FileStreaming_InitializeDataPack(void *, s32, s32);
    u8 *DataPack_GetScript(void *, s32);
    extern GameInfo g_gameInfo[2];
    static s8 bFirstTime = 1;
    static u8 *pMCP;
    u8 *mcp;
    void *pack;

    if (bFirstTime) {
        bFirstTime = 0;
        pack = FileStreaming_InitializeDataPack((void *)FileStreaming_LoadDataPack("mcp", 3, 0, 0), 3, 1);
        if (pack == NULL) {
            mcp = NULL;
            goto done;
        }
        pMCP = DataPack_GetScript(pack, 0);
    }
    mcp = pMCP;
done:
    if (mcp != NULL) {
        ((GameInfo *)info)->name = g_gameInfo[mcp[1]].name;
        ((GameInfo *)info)->id = g_gameInfo[mcp[1]].id;
        ((GameInfo *)info)->title = g_gameInfo[mcp[1]].title;
        ((GameInfo *)info)->fileName = g_gameInfo[mcp[1]].fileName;
        ((GameInfo *)info)->icon = g_gameInfo[mcp[1]].icon;
        ((GameInfo *)info)->unk14 = g_gameInfo[mcp[1]].unk14;
        ((GameInfo *)info)->unk18 = g_gameInfo[mcp[1]].unk18;
    } else {
        ((GameInfo *)info)->name = "XXXXXXXXXXXXXXXXXXX";
        ((GameInfo *)info)->id = "SLUS_001.00";
        ((GameInfo *)info)->title = "XXXXXXXXXXX";
        ((GameInfo *)info)->fileName = "saveFile";
        ((GameInfo *)info)->icon = "mm.ico";
        ((GameInfo *)info)->unk14 = 0;
        ((GameInfo *)info)->unk18 = 0;
    }
}

AudioSettings *GameSettings_GetAudioSettings(void) {
    return &g_generalGameSettings.audio;
}

void SetPlayerControls(void);
void SetMusicGVol(u32);
void SetSFXVol(u32);
void SetSpeakerMode(u8);
void OSSetSoundMode(u32);
u8 *GameSettings_GetMCP(void);
void GameSettings_ApplyDefaultGameData(void)
{
    SetPlayerControls();
    SetMusicGVol((u32)((f32)(g_generalGameSettings.audio.musicVolume * 0x7FFF) / 100.0f));
    SetSFXVol((u32)((f32)(g_generalGameSettings.audio.sfxVolume * 0x7FFF) / 100.0f));
    SetSpeakerMode(g_generalGameSettings.audio.speakerMode);
    OSSetSoundMode(g_generalGameSettings.audio.speakerMode);
}

void GameSettings_GetLoadedProgression(s32 *arg0, s32 *arg1)
{
    void GameSettings_SetCodeVariable(s32 var, s32 value, s32 arg2);
    u8 *mcp;
    void *pack;
    s32 state;

    if (bFirstTime)
    {
        bFirstTime = 0;
        pack = FileStreaming_InitializeDataPack((void *)FileStreaming_LoadDataPack("mcp", 3, 0, 0), 3, 1);
        if (pack == NULL)
        {
            mcp = NULL;
            goto done;
        }
        pMCP = DataPack_GetScript(pack, 0);
    }
    mcp = pMCP;
done:
    if (mcp != NULL)
    {
        state = mcp[1];
    }
    else
    {
        state = 0;
    }
    switch (state)
    {
    case 0:
        *arg0 = 0;
        *arg1 = 0;
        break;
    case 1:
        *arg0 = *(s32 *)&g_DDRescueGameData[0x6c];
        *arg1 = *(s32 *)&g_DDRescueGameData[0x70];
        GameSettings_SetCodeVariable(0xcb, 1, *(s32 *)&g_DDRescueGameData[0x1cd4]);
        break;
    }
}

void GameSettings_RestorePlayerStats(void) {
    void *FileStreaming_InitializeDataPack(void *, s32, s32);
    u8 *DataPack_GetScript(void *, s32);
    u8 PlayerManager_PlayerHumanCount(void);
    void *PlayerManager_GetHumanPlayerByIndex(u8 index);
    void *memcpy(void *, const void *, u32);
    static s8 bFirstTime = 1;
    static u8 *pMCP;
    u8 *mcp;
    void *pack;
    s32 state;

    if (bFirstTime) {
        bFirstTime = 0;
        pack = FileStreaming_InitializeDataPack((void *)FileStreaming_LoadDataPack("mcp", 3, 0, 0), 3, 1);
        if (pack == NULL) {
            mcp = NULL;
            goto done;
        }
        pMCP = DataPack_GetScript(pack, 0);
    }
    mcp = pMCP;
done:
    if (mcp != NULL) {
        state = mcp[1];
    } else {
        state = 0;
    }
    switch (state) {
    case 0:
        break;
    case 1:
        if (g_DDRescueGameData[0x74] && PlayerManager_PlayerHumanCount() == 1) {
            memcpy((u8 *)PlayerManager_GetHumanPlayerByIndex(0) + 0x70, &g_DDRescueGameData[0x78], 0x38);
        }
        break;
    }
}

void GameSettings_RestoreGameData(void) {
    typedef struct DDRescueGameData {
        u8 unk0[0x74];
        s8 bValid;
        u8 unk75[0xB0 - 0x75];
        u8 actions[0x880 - 0xB0];
        f32 unk880[50];
        u8 unk948[50][2];
        f32 unk9AC[50];
        u8 unkA74[50][2];
        u8 unkAD8[200];
        u8 unkBA0[1000];
        u8 unkF88[2000];
        u8 unk1758[500];
        u8 unk194C[50];
        f32 unk1980[50][3];
        f32 unk1BD8[50];
        u8 unk1CA0[50];
        s32 vehicle;
        s32 item;
    } DDRescueGameData;
    void *FileStreaming_InitializeDataPack(void *, s32, s32);
    u8 *DataPack_GetScript(void *, s32);
    void Trigger_RestoreActions(void *actions, s32 count);
    static s8 bFirstTime = 1;
    static u8 *pMCP;
    u8 *mcp;
    void *pack;
    s32 state;
    u8 *group;
    u32 i;
    u32 j;
    u32 nTriggers;
    u32 nSwitches;

    nTriggers = 0;
    nSwitches = 0;
    if (bFirstTime) {
        bFirstTime = 0;
        pack = FileStreaming_InitializeDataPack((void *)FileStreaming_LoadDataPack("mcp", 3, 0, 0), 3, 1);
        if (pack == NULL) {
            mcp = NULL;
            goto done;
        }
        pMCP = DataPack_GetScript(pack, 0);
    }
    mcp = pMCP;
done:
    if (mcp != NULL) {
        state = mcp[1];
    } else {
        state = 0;
    }
    switch (state) {
    case 0:
        break;
    case 1:
        if (((DDRescueGameData *)g_DDRescueGameData)->bValid) {
            Trigger_RestoreActions(((DDRescueGameData *)g_DDRescueGameData)->actions, 500);
            for (i = 0; i < *(u32 *)((u8 *)g_worldData.world + 0x50) && i < 50; i++) {
                *(f32 *)((*(u8 ***)((u8 *)g_worldData.world + 0x54))[i] + 0x8) = ((DDRescueGameData *)g_DDRescueGameData)->unk880[i];
                (*(u8 **)((*(u8 ***)((u8 *)g_worldData.world + 0x54))[i] + 0xC))[0] = ((DDRescueGameData *)g_DDRescueGameData)->unk948[i][0];
                (*(u8 **)((*(u8 ***)((u8 *)g_worldData.world + 0x54))[i] + 0xC))[1] = ((DDRescueGameData *)g_DDRescueGameData)->unk948[i][1];
            }
            for (i = 0; i < *(u32 *)((u8 *)g_worldData.world + 0x48) && i < 50; i++) {
                *(f32 *)((*(u8 ***)((u8 *)g_worldData.world + 0x4C))[i] + 0x4) = ((DDRescueGameData *)g_DDRescueGameData)->unk9AC[i];
                if (*(u8 **)((*(u8 ***)((u8 *)g_worldData.world + 0x4C))[i] + 0xC) != NULL) {
                    (*(u8 **)((*(u8 ***)((u8 *)g_worldData.world + 0x4C))[i] + 0xC))[0] = ((DDRescueGameData *)g_DDRescueGameData)->unkA74[i][0];
                    (*(u8 **)((*(u8 ***)((u8 *)g_worldData.world + 0x4C))[i] + 0xC))[1] = ((DDRescueGameData *)g_DDRescueGameData)->unkA74[i][1];
                }
            }
            for (i = 0; i < *(u32 *)((u8 *)g_worldData.world + 0x40) && i < 200; i++) {
                *(s32 *)(*(u8 **)((u8 *)g_worldData.world + 0x44) + i * 0x40 + 0x20) = ((DDRescueGameData *)g_DDRescueGameData)->unkAD8[i];
            }
            for (i = 0; i < *(u32 *)((u8 *)g_worldData.world + 0x20); i++) {
                group = *(u8 **)((u8 *)g_worldData.world + 0x24) + i * 0x50;
                for (j = 0; j < *(u32 *)(*(u8 **)(group + 0x30) + 0x14) && nTriggers < 1000; j++) {
                    *(s32 *)(*(u8 **)(*(u8 **)(group + 0x30) + 0x18) + j * 0x40 + 0x28) = ((DDRescueGameData *)g_DDRescueGameData)->unkBA0[nTriggers++];
                }
                for (j = 0; j < *(u32 *)(*(u8 **)(group + 0x30) + 0x4) && nSwitches < 2000; j++) {
                    *(s32 *)(*(u8 **)(*(u8 **)(group + 0x30) + 0x8) + j * 0x40 + 0x30) = ((DDRescueGameData *)g_DDRescueGameData)->unkF88[nSwitches++];
                }
            }
            for (i = 0; i < *(u32 *)((u8 *)g_worldData.world + 0x10) && i < 500; i++) {
                *(s32 *)(*(u8 **)((u8 *)g_worldData.world + 0x14) + i * 0x20 + 0x14) = ((DDRescueGameData *)g_DDRescueGameData)->unk1758[i];
            }
            for (i = 0; i < *(u32 *)((u8 *)g_worldData.world + 0x8) && i < 50; i++) {
                *(s32 *)(*(u8 **)((u8 *)g_worldData.world + 0xC) + i * 0x60 + 0x2C) = ((DDRescueGameData *)g_DDRescueGameData)->unk194C[i];
                *(f32 *)(*(u8 **)((u8 *)g_worldData.world + 0xC) + i * 0x60 + 0x0) = ((DDRescueGameData *)g_DDRescueGameData)->unk1980[i][0];
                *(f32 *)(*(u8 **)((u8 *)g_worldData.world + 0xC) + i * 0x60 + 0x4) = ((DDRescueGameData *)g_DDRescueGameData)->unk1980[i][1];
                *(f32 *)(*(u8 **)((u8 *)g_worldData.world + 0xC) + i * 0x60 + 0x8) = ((DDRescueGameData *)g_DDRescueGameData)->unk1980[i][2];
                *(f32 *)(*(u8 **)((u8 *)g_worldData.world + 0xC) + i * 0x60 + 0x58) = ((DDRescueGameData *)g_DDRescueGameData)->unk1BD8[i];
                *(u8 *)(*(u8 **)((u8 *)g_worldData.world + 0xC) + i * 0x60 + 0x5F) = ((DDRescueGameData *)g_DDRescueGameData)->unk1CA0[i];
            }
            g_codeVariables.playerItem[0] = ((DDRescueGameData *)g_DDRescueGameData)->item;
            g_codeVariables.playerVehicle[0] = ((DDRescueGameData *)g_DDRescueGameData)->vehicle;
        }
        break;
    }
}

s32 GameSettings_GetDefaultMemoryCardStatus(s32 *device, s8 bForce)
{
    static u32 s_updateCntr = 10;
    static s32 s_curStatus;
    static s32 s_curDevice;
    s32 GameSettings_GetMemoryCardStatus(s32 device, s32 arg1);
    s32 status1;
    s32 status2;

    if (++s_updateCntr < 10 && !bForce && !g_bRefreshMC)
    {
        *device = s_curDevice;
        return s_curStatus;
    }
    s_updateCntr = 0;
    s_curStatus = 2;
    g_bRefreshMC = 0;
    status1 = GameSettings_GetMemoryCardStatus(1, 1);
    status2 = GameSettings_GetMemoryCardStatus(2, 1);
    if (status1 == 1 || status1 == 0)
    {
        s_curStatus = status1;
        s_curDevice = 1;
    }
    else if (status1 == 2 && status2 != 2)
    {
        s_curStatus = status2;
        s_curDevice = 2;
    }
    else
    {
        s_curStatus = status1;
        s_curDevice = 1;
    }
    *device = s_curDevice;
    return s_curStatus;
}

void GameSettings_DetermineIfOverwritingFile(void)
{
    if (g_bValidFileName[g_codeVariables.fileIndex - 1])
    {
        g_codeVariables.bOverwritingFile = 1;
    }
    else
    {
        g_codeVariables.bOverwritingFile = 0;
    }
}

#pragma push
#pragma readonly_strings on
void GameSettings_GetSaveGameName(s32 *letters) {
    char *strncpy(char *dst, const char *src, u32 n);
    int sprintf(char *buf, const char *fmt, ...);
    u8 *mcp;
    char name[12];
    s32 i;

    if (g_bValidFileName[g_codeVariables.fileIndex - 1]) {
        strncpy(name, g_fileNames[g_codeVariables.fileIndex - 1], 11);
    } else {
        mcp = GameSettings_LoadMCP();
        switch (mcp != NULL ? mcp[1] : 0) {
        case 0:
            sprintf(name, "HW VEL X %d", g_codeVariables.fileIndex);
            break;
        case 1:
            sprintf(name, "DD RESCUE%d", g_codeVariables.fileIndex);
            break;
        default:
            sprintf(name, "SAVE GAME%d", g_codeVariables.fileIndex);
            break;
        }
    }
    for (i = 0; i < 10; i++) {
        if (name[i] >= 'A' && name[i] <= 'Z') {
            letters[i] = name[i] - 'A';
        } else if (name[i] >= '0' && name[i] <= '9') {
            letters[i] = name[i] - '0' + 26;
        } else {
            letters[i] = 36;
        }
    }
}
#pragma pop

#pragma push
#pragma readonly_strings on
void GameSettings_SetSaveGameName(u32 *letters) {
    s32 File_Open(char *dev, const char *name, u32 flags, s32 *size);
    u32 File_Read(s32 handle, u32 size, void *buf);
    void File_Close(s32 handle);
    extern u8 *g_pSavedGameData;
    extern s32 g_saveFileCheckSum;
    extern GameInfo g_gameInfo[2];
    char *device;
    GameInfo info;
    u8 *data;
    s32 index;
    u8 *mcp;
    s32 handle;
    s32 sum;
    s32 i;

    device = g_codeVariables.device;
    for (i = 0; i < 10; i++) {
        if (letters[i] < 26) {
            g_saveFileName[i] = (s8)letters[i] + 'A';
        } else if (letters[i] < 36) {
            g_saveFileName[i] = letters[i] - 26 + '0';
        } else {
            g_saveFileName[i] = ' ';
        }
    }
    g_saveFileName[10] = 0;
    mcp = GameSettings_LoadMCP();
    if (mcp != NULL) {
        info.fileName = g_gameInfo[mcp[1]].fileName;
        index = g_gameInfo[mcp[1]].unk18;
    } else {
        info.fileName = "saveFile";
        index = 0;
    }
    data = g_pSavedGameData;
    handle = File_Open(device + 0xA, info.fileName, 0, 0);
    if (handle != -1) {
        File_Read(handle, index * 8 + 0x15C, data);
        File_Close(handle);
        sum = *(s32 *)(data + 8);
    } else {
        sum = -1;
    }
    g_saveFileCheckSum = sum;
}
#pragma pop

char *GameSettings_GetActiveMemoryCardLevelDetails(void) {
    s32 GameSettings_GetMemoryCardStatus(s32 device, s32 arg1);
    int sprintf(char *buf, const char *fmt, ...);
    extern u8 *g_pSavedGameData;
    extern s8 g_bRefreshMC_FileInfo_803C6F7A;
    typedef struct SaveSlot {
        s8 valid;
        u8 unk1[0x1F];
    } SaveSlot;
    typedef struct SaveData {
        u8 header[0x5C];
        SaveSlot slots[8];
    } SaveData;
    static s32 cachedStatus = -1;
    static char *cachedMemorySlot = (char *)-1;
    static u32 cachedFileSlot = -1;
    static char levelDetails[256];
    char *slot;
    s32 status;
    u32 fileSlot;
    SaveData *data;
    u8 *mcp;
    s8 *save;
    u32 i;
    u32 mission;
    u32 challenge;
    u32 cars;
    u32 items;
    u32 battles;
    u32 races;

    slot = g_codeVariables.device - 1;
    status = GameSettings_GetMemoryCardStatus((s32)g_codeVariables.device, 0);
    fileSlot = g_codeVariables.fileIndex - 1;
    if (cachedMemorySlot != slot || status != cachedStatus) {
        g_bRefreshMC_FileInfo_803C6F7A = 1;
    }
    if (cachedFileSlot != fileSlot || g_bRefreshMC_FileInfo_803C6F7A) {
        cachedMemorySlot = slot;
        cachedFileSlot = fileSlot;
        cachedStatus = status;
        if (fileSlot >= 8 || status != 1) {
            return NULL;
        }
        data = (SaveData *)g_pSavedGameData;
        mcp = GameSettings_GetMCP();
        switch (mcp != NULL ? mcp[1] : 0) {
        case 0:
            save = (s8 *)(g_pSavedGameData + cachedFileSlot * 0xF4 + 0x15C);
            if (data->slots[cachedFileSlot].valid) {
                mission = 1;
                for (i = 5; i < 19; i++) {
                    if (save[0x21 + i]) {
                        mission = i;
                    }
                }
                challenge = 1;
                for (i = 39; i < 57; i++) {
                    if (save[0x21 + i]) {
                        challenge = i;
                    }
                }
                cars = 0;
                for (i = 0; i < 33; i++) {
                    if (save[i]) {
                        cars++;
                    }
                }
                items = 0;
                for (i = 0; i < 9; i++) {
                    if (save[0x96 + i]) {
                        items++;
                    }
                }
                races = 0;
                for (i = 0; i < 2; i++) {
                    if (save[0x42 + i]) {
                        races++;
                    }
                }
                battles = 0;
                for (i = 0; i < 4; i++) {
                    if (save[0x38 + i]) {
                        battles++;
                    }
                }
                sprintf(levelDetails, "Mission %d|Challenge %d|Available Cars %d|Available Items %d|Available Races %d|Available Battles %d|Completed %d%%",
                        mission - 4, challenge - 38, cars, items + 3, races + 4, battles + 4,
                        (u32)(100.0f * ((f32)(cars + items + (mission - 4) + (challenge - 38) + (races + 4) + (battles + 4) - 13) / 75.0f)));
                g_codeVariables.unk124 = mission - 5;
            } else {
                levelDetails[0] = 0;
                g_codeVariables.unk124 = 0;
            }
            break;
        case 1:
            levelDetails[0] = 0;
            g_codeVariables.unk124 = 0;
            break;
        }
    } else {
        levelDetails[0] = 0;
        g_codeVariables.unk124 = 0;
    }
    return levelDetails;
}

#pragma push
#pragma readonly_strings on
#pragma use_lmw_stmw on
u8 GameSettings_LoadGame(void) {
    s32 File_Open(char *dev, const char *name, u32 flags, s32 *size);
    u32 File_Read(s32 handle, u32 size, void *buf);
    void File_Close(s32 handle);
    u32 FileManager_GetChecksum(u32 size, void *data);
    void *memcpy(void *dst, const void *src, u32 n);
    extern u8 *g_pSavedGameData;
    extern u8 *g_pGameData;
    extern GameInfo g_gameInfo[2];
    s32 slot;
    char *device;
    s32 handle;
    s8 ok;
    u8 *data;
    u32 version;
    u8 *saved;
    u32 size;
    GameInfo info;
    u8 *mcp;
    u32 i;

    device = g_codeVariables.device;
    slot = g_codeVariables.fileIndex - 1;
    mcp = GameSettings_LoadMCP();
    if (mcp != NULL) {
        info.fileName = g_gameInfo[mcp[1]].fileName;
        version = g_gameInfo[mcp[1]].unk14;
        size = g_gameInfo[mcp[1]].unk18;
    } else {
        info.fileName = "saveFile";
        version = 0;
        size = version;
    }
    data = g_pSavedGameData;
    ok = 0;
    if ((handle = File_Open(device + 0xA, info.fileName, 0, NULL)) != -1) {
        File_Read(handle, size * 8 + 0x15C, data);
        if (((u32 *)data)[0] == 4 && ((u32 *)data)[1] == version &&
            ((u32 *)data)[2] == FileManager_GetChecksum(version, data + 0xC)) {
            ok = 1;
        }
        File_Close(handle);
    }
    if (!ok) {
        g_codeVariables.unk11C[1] = 0;
        return 0;
    }
    saved = g_pSavedGameData;
    memcpy(g_pGameData, &saved[slot * size + 0x15C], size);
    memcpy(&g_generalGameSettings, saved + 0xC, 0x150);
    g_codeVariables.unk11C[1] = 1;
    g_bRefreshMC = 1;
    g_bRefreshMC_FileList = 1;
    g_bRefreshMC_FileInfo = 1;
    for (i = 0; i < 11; i++) {
        g_codeVariables.unk128[i] = 0;
    }
    return g_codeVariables.unk11C[1];
}
#pragma pop

s32 GameSettings_FormatMemoryCard(void)
{
    u8 result;

    result = FileManager_FormatDevice(g_codeVariables.device + 0xA);
    g_codeVariables.unk11C[2] = result;
    g_bRefreshMC = 1;
    g_bRefreshMC_FileList = 1;
    g_bRefreshMC_FileInfo = 1;
    return result;
}

#pragma push
#pragma readonly_strings on
#pragma use_lmw_stmw off
s32 GameSettings_DeleteGameFile(void) {
    u8 File_Delete(char *device, const char *fileName);
    extern GameInfo g_gameInfo[];
    static s8 bFirstTime = 1;
    static u8 *pMCP;
    char *device;
    u8 *mcp;
    void *pack;
    const char *name;
    u8 result;

    device = g_codeVariables.device + 0xA;
    if (bFirstTime) {
        bFirstTime = 0;
        pack = FileStreaming_InitializeDataPack((void *)FileStreaming_LoadDataPack("mcp", 3, 0, 0), 3, 1);
        if (pack == NULL) {
            mcp = NULL;
            goto done;
        }
        pMCP = DataPack_GetScript(pack, 0);
    }
    mcp = pMCP;
done:
    name = (mcp != NULL) ? g_gameInfo[mcp[1]].fileName : "saveFile";
    result = File_Delete(device, name);
    g_bRefreshMC = 1;
    g_codeVariables.unk11C[2] = result;
    g_bRefreshMC_FileList = 1;
    g_bRefreshMC_FileInfo = 1;
    return result;
}
#pragma pop

