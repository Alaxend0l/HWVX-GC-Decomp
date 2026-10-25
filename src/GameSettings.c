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

s32 FileStreaming_LoadDataPack(void *name, s32 type, u32 id, s8 threaded);
void *FileStreaming_InitializeDataPack(void *, s32, s32);
u8 *DataPack_GetScript(void *, s32);
s32 PlayerManager_PlayerHumanCount(void);

void GameSettings_GameBoyConnected(void) {
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
        g_HotWheelsGameData.unk0 = 1;
        g_HotWheelsGameData.unk1 = 1;
        g_HotWheelsGameData.unk2 = 1;
        break;
    case 1:
        break;
    }
}
