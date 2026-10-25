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
