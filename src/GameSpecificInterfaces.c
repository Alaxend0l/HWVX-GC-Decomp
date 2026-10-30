#include "common.h"

extern VehicleInterfaceData g_vehicleInterfaceData[4];

void GSI_VehicleItemInit(void);

void GSI_Init(void)
{
    GSI_VehicleItemInit();
}

void GSI_VehicleItemInit(void)
{
    void PlayerManager_Init(void);
    void *memset(void *dst, s32 c, u32 n);
    u32 i;

    for (i = 0; i < 4; i++) {
        memset(&g_vehicleInterfaceData[i], 0, sizeof(VehicleInterfaceData));
        g_vehicleInterfaceData[i].iface = -1;
        g_vehicleInterfaceData[i].packIndex = -1;
        g_vehicleInterfaceData[i].packID = -1;
        g_vehicleInterfaceData[i].itemID = -1;
        g_vehicleInterfaceData[i].selection = -1;
        memset(g_vehicleInterfaceData[i].objects, 0xFF, 8);
    }
    PlayerManager_Init();
}

void GSI_VehicleSelectionInfo(u8 *info)
{
    u32 i;
    u8 *vehicle;

    for (i = 0; i < 4; i++) {
        if (*(u32 *)info == g_vehicleInterfaceData[i].iface && g_vehicleInterfaceData[i].cars[0] != NULL) {
            vehicle = *(u8 **)(g_vehicleInterfaceData[i].cars[0] + 0x3BC);
            info[0xC9] = *(u8 *)(*(u8 **)(vehicle + 0x84) + 0x34);
            info[0xCA] = *(u8 *)(*(u8 **)(vehicle + 0x84) + 0x35);
            info[0xCB] = *(u8 *)(*(u8 **)(vehicle + 0x84) + 0x36);
            info[0xCC] = *(u8 *)(*(u8 **)(vehicle + 0x84) + 0x37);
            return;
        }
    }
}

void GSI_VehicleItemSelection(void *arg) {
    typedef struct InterfaceMsg {
        u32 iface;
        u16 type;
        u16 sub;
        u8 unk8[0x20 - 0x8];
        u32 result;
        u8 unk24[0x30 - 0x24];
        u32 selection;
        u32 objects[2];
        u32 *list;
        u16 nextState;
    } InterfaceMsg;
    typedef struct CreateCarParams {
        Vector4 pos;
        Vector4 rot;
        Vector4 vel;
        void *script;
        u8 unk44[0x4C - 0x44];
        void *player;
    } CreateCarParams;
    extern Vector4 g_zeroVector;
    u8 *GameSettings_GetMCP(void);
    void GameSettings_SetCodeVariable(s32 id, s32 index, s32 value);
    void Input_Init(void);
    void *PlayerManager_GetHumanPlayerByIndex(u8 index);
    s32 FileStreaming_LoadDataPack(void *name, s32 type, u32 id, s8 threaded);
    void *FileStreaming_InitializeDataPack(u32 index, s32 type, s8 refresh);
    s32 FileStreaming_GetPackID(void *name, s32 type, u32 id);
    s32 FileStreaming_ReleaseDataPack(u32 index, s32 type);
    void FileStreaming_ReleaseAllDataPacks(s32 type, s32 arg);
    void DataPack_EnableLoadingSounds(s32 enable);
    u8 *DataPack_GetScript(void *pack, s32 id);
    u8 *GameSettings_GetControllerSettings(s32 port);
    void Input_SetForceFeedbackActive(s32 chan, u8 active);
    void Gfx_SetCameraMatrix(void *a, void *b, void *c, void *d);
    void Gfx_SetProjectionMatrix(f32 fov);
    void CameraManager_ResetAllCameras(void);
    void CameraManager_Pause(s32 pause);
    void Gfx_SetEnvmap(u16 a, u16 b, s8 flag, f32 x, f32 y);
    void Gfx_SetEnviroMapMode(u8 mode);
    void InterfaceManager_SetInfoCallBackRoutine(u32 index, void (*cb)(u8 *));
    void GSI_VehicleSelectionInfo(u8 *info);
    void InterfaceManager_LinkThingToObject(s32 index, u32 id, void *thing);
    void *ThingManagerList_GetByType(void *list, s32 type);
    void WildCardRack_JettisonAll(void *rack);
    void WildCardRack_Add(void *rack, void *item, s32 arg);
    void CarA_Release(void *car);
    void ParticleManager_Init(void);
    void Control_ClearControl(void *player);
    s8 IsLoadingThreadRunning(void);
    s32 GameSettings_GetGameType(void);
    s8 GameSettings_IsCheatEnabled(s32 cheat);
    void *ThingManager_DARYL_CreateThing(s32 type, void *params);
    void Player_SetThing(u8 *player, u8 *thing);
    static s8 bReleaseThings = 1;
    static s8 bReleaseAllPacks = 1;
    static u32 lockedID = 0xFFFFFFFF;
    static void *s_pColGrid;
    InterfaceMsg *msg = arg;
    VehicleInterfaceData *data;
    u8 *mcp;
    u8 *car;
    u8 *rack;
    u8 *pack;
    u8 *script;
    u32 id;
    u32 i;
    u32 j;
    s8 busy;
    CreateCarParams params;

    data = NULL;
    if (msg->type != 3) {
        for (i = 0; i < 4; i++) {
            if (msg->iface == g_vehicleInterfaceData[i].iface) {
                data = &g_vehicleInterfaceData[i];
            }
        }
        if (data == NULL && msg->sub != 2) {
            return;
        }
        if (data == NULL) {
            return;
        }
    }
    switch (msg->type) {
    case 3:
        mcp = GameSettings_GetMCP();
        if (msg->sub == 2) {
            for (i = 1; i <= 4; i++) {
                GameSettings_SetCodeVariable(0xCB, i, -1);
            }
            for (i = 0; i < 4; i++) {
                if (g_vehicleInterfaceData[i].iface == -1) {
                    data = &g_vehicleInterfaceData[i];
                    Input_Init();
                    for (j = 0; j < 2; j++) {
                        data->players[j] = PlayerManager_GetHumanPlayerByIndex(0);
                    }
                    data->iface = msg->iface;
                    break;
                }
            }
            bReleaseAllPacks = 1;
            if (data == NULL && msg->sub != 2) {
                return;
            }
            if (data == NULL) {
                return;
            }
            data->active = 1;
            if (mcp != NULL) {
                if (*(void **)(mcp + 0x10) != NULL) {
                    FileStreaming_LoadDataPack(*(void **)(mcp + 0x10), 8, 1, 0);
                }
                if (*(void **)(mcp + 0x8) != NULL) {
                    FileStreaming_LoadDataPack(*(void **)(mcp + 0x8), 8, 2, 0);
                }
            }
            lockedID = -1;
        } else if (msg->sub == 3) {
            data = &g_vehicleInterfaceData[0];
            if (g_vehicleInterfaceData[0].iface != -1 && g_vehicleInterfaceData[1].iface == -1) {
                data->iface = msg->iface;
            } else {
                if (g_vehicleInterfaceData[0].iface != -1) {
                    return;
                }
                data = &g_vehicleInterfaceData[1];
                if (g_vehicleInterfaceData[1].iface == -1) {
                    return;
                }
                data->iface = msg->iface;
            }
            data->active = 0;
            for (i = 0; i < 2; i++) {
                if (data->objects[i] != 0xFF && data->cars[i] != NULL) {
                    InterfaceManager_LinkThingToObject(msg->iface, data->objects[i], data->cars[i]);
                }
            }
            bReleaseAllPacks = 1;
            bReleaseThings = 1;
            if (mcp != NULL && *(void **)(mcp + 0xC) != NULL) {
                FileStreaming_LoadDataPack(*(void **)(mcp + 0xC), 8, 3, 0);
            }
            lockedID = -1;
        } else {
            return;
        }
        Input_SetForceFeedbackActive(0, *GameSettings_GetControllerSettings(1));
        if (g_worldData.world != NULL) {
            s_pColGrid = *(void **)g_worldData.world;
            *(void **)g_worldData.world = NULL;
        } else {
            s_pColGrid = NULL;
        }
        Gfx_SetCameraMatrix(NULL, NULL, NULL, NULL);
        Gfx_SetProjectionMatrix(90.0f);
        CameraManager_ResetAllCameras();
        CameraManager_Pause(1);
        if (mcp != NULL) {
            Gfx_SetEnvmap(*(u32 *)(mcp + 0x14), *(u32 *)(mcp + 0x18), 1, 100.0f, 100.0f);
            Gfx_SetEnviroMapMode(1);
        }
        InterfaceManager_SetInfoCallBackRoutine(msg->iface, GSI_VehicleSelectionInfo);
        break;
    case 5:
        if (!bReleaseThings) {
            break;
        }
        if (bReleaseAllPacks) {
            for (i = 0; i < 2; i++) {
                if (data->cars[i] != NULL) {
                    WildCardRack_JettisonAll(ThingManagerList_GetByType(data->cars[i] + 0x344, 8));
                    CarA_Release(data->cars[i]);
                }
            }
            memset(data, 0, sizeof(VehicleInterfaceData));
            memset(data->objects, 0xFF, 8);
        } else {
            for (i = 0; i < 2; i++) {
                if (data->cars[i] != NULL) {
                    WildCardRack_JettisonAll(ThingManagerList_GetByType(data->cars[i] + 0x344, 8));
                    CarA_Release(data->cars[i]);
                    data->cars[i] = NULL;
                }
            }
        }
        CameraManager_Pause(0);
        data->iface = -1;
        data->selection = -1;
        data->itemID = -1;
        ParticleManager_Init();
        if (bReleaseAllPacks) {
            data->packIndex = -1;
            data->packID = -1;
            FileStreaming_ReleaseAllDataPacks(0, 0);
            FileStreaming_ReleaseAllDataPacks(2, 0);
            FileStreaming_ReleaseAllDataPacks(8, 0);
        }
        if (g_worldData.world != NULL) {
            *(void **)g_worldData.world = s_pColGrid;
        }
        break;
    case 0:
        switch (msg->sub) {
        case 100:
            for (i = 0; i < 2; i++) {
                if (lockedID != -1) {
                    data->locked = 1;
                    break;
                }
                if (data->cars[i] != NULL) {
                    rack = ThingManagerList_GetByType(data->cars[i] + 0x344, 8);
                    if (rack != NULL) {
                        *(u16 *)(rack + rack[0x470] * 0x50 + 0xF0) = 10000;
                        *(s32 *)(data->cars[i] + 0x78) = 10000;
                        (*(u8 **)(data->cars[i] + 0x94))[0x1B] = 0xFF;
                    }
                }
            }
            break;
        }
        break;
    case 12:
        for (i = 0; i < 2; i++) {
            car = data->cars[i];
            if (car != NULL) {
                if (!data->locked) {
                    Control_ClearControl(*(void **)(car + 0x94));
                }
                *(s32 *)(car + 0x460) = 0;
                *(f32 *)(car + 0x458) = 10.0f;
                ((f32 *)data->cars[i])[0] = 0.0f;
                ((f32 *)data->cars[i])[1] = 0.0f;
                ((f32 *)data->cars[i])[2] = 0.0f;
                *(Vector4 *)(car + 0x390) = g_zeroVector;
                *(Vector4 *)(car + 0x380) = g_zeroVector;
                *(Vector4 *)(car + 0x410) = g_zeroVector;
            }
        }
        data->locked = 0;
        break;
    case 13:
        id = msg->list[msg->selection];
        if (IsLoadingThreadRunning()) {
            break;
        }
        data->selection = msg->selection;
        if (id != data->packID || (data->cars[0] == NULL && lockedID == -1)) {
            for (i = 0; i < 2; i++) {
                if (data->cars[i] != NULL) {
                    rack = ThingManagerList_GetByType(data->cars[i] + 0x344, 8);
                    if (rack != NULL) {
                        WildCardRack_JettisonAll(rack);
                    }
                    CarA_Release(data->cars[i]);
                    data->cars[i] = NULL;
                    if (msg->objects[i] != 0xFF) {
                        InterfaceManager_LinkThingToObject(msg->iface, msg->objects[i], NULL);
                    }
                }
            }
            if (IsLoadingThreadRunning()) {
                break;
            }
            if (lockedID != -1 && lockedID != data->iface) {
                break;
            }
            lockedID = data->iface;
            ParticleManager_Init();
            if (data->packIndex != -1 && id != data->packID) {
                FileStreaming_ReleaseDataPack(data->packIndex, 0);
                data->packIndex = -1;
                break;
            }
            mcp = GameSettings_GetMCP();
            if (mcp == NULL || id == data->packID) {
                break;
            }
            FileStreaming_InitializeDataPack(FileStreaming_GetPackID(*(void **)(mcp + 0x8), 8, 2), 8, 1);
            DataPack_EnableLoadingSounds(0);
            data->packIndex = FileStreaming_LoadDataPack(*(void **)(mcp + 0x8), 0, id, 1);
            if (data->packIndex != -1) {
                data->packID = id;
            }
            break;
        }
        if (IsLoadingThreadRunning() || (lockedID != -1 && lockedID != data->iface)) {
            if (!IsLoadingThreadRunning()) {
                lockedID = -1;
            }
            break;
        }
        j = 0;
        if (msg->objects[0] == 0xFF) {
            break;
        }
        if (data->cars[0] == NULL) {
            pack = FileStreaming_InitializeDataPack(data->packIndex, 0, 1);
            if (pack == NULL) {
                lockedID = -1;
                break;
            }
            DataPack_EnableLoadingSounds(1);
            script = DataPack_GetScript(pack, data->packID);
            if (script == NULL) {
                break;
            }
            params.pos.x = 0.0f;
            params.pos.y = -7.0f;
            params.pos.z = 30.0f;
            params.player = data->players[0];
            params.rot.x = 0.0f;
            params.rot.y = 0.0f;
            params.rot.z = 0.0f;
            params.rot.w = 1.0f;
            params.script = script;
            params.vel.x = 0.0f;
            params.vel.y = 0.0f;
            params.vel.z = 0.0f;
            for (i = 0; i < *(u32 *)(script + 0xD0); i++) {
                *(f32 *)(*(u8 **)(script + 0xD4) + i * 0x50 + 0xC) = 0.3f;
            }
            data->cars[0] = ThingManager_DARYL_CreateThing(9, &params);
            *(u32 *)(data->cars[0] + 0x90) = 0;
            data->objects[0] = msg->objects[0];
            Player_SetThing(data->players[0], data->cars[0]);
            *(u32 *)(data->players[0] + 0xA8) = 0;
            *(u32 *)(data->cars[0] + 0x74) |= 0x60200;
            if (j == 1) {
                *(u32 *)(data->cars[0] + 0x74) |= 2;
            }
        } else {
            InterfaceManager_LinkThingToObject(msg->iface, msg->objects[0], data->cars[0]);
            lockedID = -1;
        }
        break;
    case 14:
        id = msg->list[msg->selection];
        if (IsLoadingThreadRunning()) {
            break;
        }
        if (GameSettings_GetGameType() == 0 && id == 3 && GameSettings_IsCheatEnabled(8)) {
            id = 0x4D;
        }
        if (id != data->itemID) {
            if (lockedID != -1 && lockedID != data->iface) {
                break;
            }
            lockedID = data->iface;
            mcp = GameSettings_GetMCP();
            if (mcp != NULL) {
                data->itemPack = FileStreaming_LoadDataPack(*(void **)(mcp + 0xC), 2, id, 0);
                if (data->itemPack != -1) {
                    data->itemID = id;
                }
            }
            Control_ClearControl(*(void **)(data->cars[0] + 0x94));
            data->locked = 0;
        }
        if (lockedID == -1 || lockedID != data->iface) {
            break;
        }
        lockedID = -1;
        pack = FileStreaming_InitializeDataPack(data->itemPack, 2, 1);
        if (pack == NULL) {
            break;
        }
        script = DataPack_GetScript(pack, data->itemID);
        if (script == NULL || data->cars[0] == NULL) {
            break;
        }
        rack = ThingManagerList_GetByType(data->cars[0] + 0x344, 8);
        if (rack == NULL) {
            break;
        }
        WildCardRack_JettisonAll(rack);
        WildCardRack_Add(rack, script, *(s32 *)(*(u8 **)(script + 0x30) + 0x20));
        break;
    case 15:
        busy = 0;
        for (i = 0; i < 2 && !busy; i++) {
            if (data->cars[i] != NULL || data->packID == -1) {
                busy = 1;
            }
        }
        if (!busy || IsLoadingThreadRunning() ||
            (data->selection != -1 && msg->list[data->selection] != data->packID && data->active)) {
            msg->result = 0xB;
        }
        if ((msg->nextState == 3 || msg->nextState == 8) && msg->result != 0xB) {
            bReleaseAllPacks = 0;
        }
        if (msg->nextState == 3 && msg->result != 0xB) {
            bReleaseThings = 0;
        }
        ParticleManager_Init();
        break;
    }
}
