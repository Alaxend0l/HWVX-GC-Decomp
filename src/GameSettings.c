#include "common.h"

extern GeneralGameSettings g_generalGameSettings;


extern HotWheelsGameData g_HotWheelsGameData;



extern CodeVariables g_codeVariables;
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
        g_HotWheelsGameData.unk0 = 1;
        g_HotWheelsGameData.unk1 = 1;
        g_HotWheelsGameData.unk2 = 1;
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

ControllerSettings *GameSettings_GetControllerSettings(s32 player) {
    return &g_generalGameSettings.controllers[player - 1];
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
    extern s8 g_DDRescueGameData[];
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
