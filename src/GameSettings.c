#include "common.h"

typedef struct AudioSettings {
    u8 sfxVolume;
    u8 musicVolume;
    u8 speakerMode;
    u8 unk3;
} AudioSettings;

typedef struct ControllerSettings {
    u8 data[0x12];
} ControllerSettings;

typedef struct GeneralGameSettings {
    s32 difficulty;
    AudioSettings audio;
    ControllerSettings controllers[18];
    u8 unk14C[4];
} GeneralGameSettings;

extern GeneralGameSettings g_generalGameSettings;

typedef struct HotWheelsGameData {
    u8 unk0;
    u8 unk1;
    u8 unk2;
    u8 unk3[0xF1];
} HotWheelsGameData;

extern HotWheelsGameData g_HotWheelsGameData;

typedef struct CodeVariables {
    s32 playerVehicle[4];
    s32 playerItem[4];
    s32 controllerState[4];
    s32 unk30;
    s32 unk34[4];
    s32 unk44[4];
    s32 unk54[4];
    u8 unk64[4];
    f32 unk68[30];
    s32 unkE0;
    u8 unkE4;
    s32 unkE8;
    s32 unkEC;
    s32 unkF0;
    s32 unkF4;
    s32 unkF8;
    u8 unkFC;
    s32 unk100;
    s32 unk104;
    u8 unk108;
    u8 unk109[0x111 - 0x109];
    u8 unk111;
    u8 unk112[0x114 - 0x112];
    char *device;
    s32 fileIndex;
    u8 unk11C[3];
    s8 bOverwritingFile;
    u8 unk120[0x124 - 0x120];
    s32 unk124;
    u8 unk128[0x134 - 0x128];
} CodeVariables;

extern CodeVariables g_codeVariables;
extern s8 g_bValidFileName[8];

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

