#include "common.h"
#include "CarA.h"

static inline Player *Action_GetCarPlayer(CarA *car) {
    Player *player;

    if (car != NULL) {
        player = car->player;
    } else {
        player = NULL;
    }
    return player;
}
#include "CarA.h"

s32 ActionCameraSetTargets(void) {
    M2C_UNK CameraManager_SetCameraGlobalMode();
    CameraManager_SetCameraGlobalMode();
    return 1;
}

s32 ActionRoundComplete(s32 *arg0) {
    M2C_UNK GameRules_ForceRoundEnd(s32);
    GameRules_ForceRoundEnd(*arg0);
    return 1;
}

s32 ActionResetGameRound(void *arg0) {
    M2C_UNK GameRules_ResetRound(s8, s8, s8);
    GameRules_ResetRound((s8) (u8) M2C_FIELD(arg0, s32 *, 0), (s8) (u8) M2C_FIELD(arg0, s32 *, 4), (s8) (u8) M2C_FIELD(arg0, s32 *, 8));
    return 1;
}

s32 ActionSetPlayerOrder(void *action) {
    GameRules_SetPlayerOrder(action, 4);
    return 1;
}

s32 ActionSpeakerChangeMode(void) {
    return 1;
}

s32 ActionVehicleAISetNamedPath(void) {
    return 1;
}

s32 ActionSetPlayerVehicle(void) {
    return 1;
}

s32 ActionVehicleRemoveCreature(void) {
    return 1;
}

s32 ActionScriptLightChangeMode(void) {
    return 1;
}

void Action_Create(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

void Action_Destroy(void) {

}

void Action_Init(void) {
    extern void *m_Actions_800DBC80[80];
    u32 i;

    for (i = 0; i < 80; i++) {
        m_Actions_800DBC80[i] = NULL;
    }
}

void Action_Heartbeat(void) {
    typedef struct Action {
        s32 type;
        void *data;
        s32 unk8;
        s32 unkC;
        s32 unk10;
        s32 delay;
    } Action;
    extern Action *m_Actions_800DBC80[80];
    extern void (*pActionFuncArray_800CF500[])(void *);
    extern s32 g_iDeltaTimeMS;
    u32 i;

    for (i = 0; i < 80; i++) {
        Action *action = m_Actions_800DBC80[i];
        if (action != NULL && action->delay <= 0) {
            pActionFuncArray_800CF500[action->type](action->data);
            if (m_Actions_800DBC80[i]->type != 0x14 || ((void **)m_Actions_800DBC80[i]->data)[1] == NULL) {
                m_Actions_800DBC80[i] = NULL;
            }
        } else if (action != NULL) {
            action->delay -= g_iDeltaTimeMS;
        }
    }
}

void Action_ReleaseAllActions(void) {
    extern u8 m_Actions_800DBC80[];
    memset(m_Actions_800DBC80, 0, 0x140);
}

#pragma readonly_strings on
void Dprintf(const char *fmt, ...);
s32 Action_AddAction(void *arg0) {
    typedef struct Action {
        s32 type;
        void *data;
        s32 unk8;
        s32 delayInit;
        u32 count;
        s32 delay;
    } Action;
    extern Action *m_Actions_800DBC80[80];
    Action *action = arg0;
    u32 i;

    for (i = 0; i < 80; i++) {
        if (m_Actions_800DBC80[i] == NULL) {
            break;
        }
    }
    if (i >= 80) {
        Dprintf("The max number of actions has been exceeded for this point in the game.  There can only be %d simaltaneous actions\n", 80);
        return 1;
    }
    if (action->count != 0xFFFFFFFF) {
        action->count--;
    }
    m_Actions_800DBC80[i] = action;
    m_Actions_800DBC80[i]->delay = m_Actions_800DBC80[i]->delayInit;
    if (action->count != 0) {
        return 1;
    }
    return 0;
}
#pragma readonly_strings reset

s32 ActionPrintDebugString(u32 *action)
{
    void InterfaceManager_SetPlayerMessage(void *message, u32 player);
    u32 i;
    u8 *obj;

    if (*action == 0)
    {
        if (action[1] == 0)
        {
            action[1] = 0x3c;
        }
        action[1] -= 1;
    }
    else
    {
        action[1] = 0;
        for (i = 0; i < M2C_FIELD(g_worldData.world, u32 *, 8); i++)
        {
            obj = (u8 *)((char *)M2C_FIELD(g_worldData.world, u8 **, 0xC) + i * 0x60);
            if (M2C_FIELD(obj, u8 *, 0x35) == 1)
            {
                switch (*action)
                {
                case 0:
                    break;
                case 1:
                    M2C_FIELD(M2C_FIELD(obj, u8 **, 0x28), u32 *, 0x498) = action[2];
                    break;
                case 2:
                    M2C_FIELD(M2C_FIELD(obj, u8 **, 0x28), u32 *, 0x494) = action[2];
                    break;
                case 3:
                    M2C_FIELD(M2C_FIELD(obj, u8 **, 0x28), u32 *, 0x494) = action[2];
                    break;
                case 4:
                    InterfaceManager_SetPlayerMessage((void *)action[2], 0);
                    break;
                case 5:
                    M2C_FIELD(M2C_FIELD(obj, u8 **, 0x28), u32 *, 0x49C) = action[2];
                    break;
                case 6:
                    M2C_FIELD(M2C_FIELD(obj, u8 **, 0x28), u32 *, 0x4A0) = action[2];
                    break;
                case 7:
                    InterfaceManager_SetPlayerMessage((void *)action[2], 1);
                    break;
                case 8:
                    InterfaceManager_SetPlayerMessage((void *)action[2], 2);
                    break;
                }
            }
        }
    }
    return 1;
}

s8 ActionLoadInterfacePack(s32 *arg0) {
    s32 FileStreaming_LoadDataPack(void *, M2C_UNK, s32, M2C_UNK);
    s32 temp_r3;

    temp_r3 = FileStreaming_LoadDataPack(arg0 + 1, 1, *arg0, 1);
    return (s8) ((u32) ((-1 - temp_r3) | (temp_r3 + 1)) >> 0x1FU);
}

void ActionUnloadInterfacePack(s32 *arg0) {
    s32 FileStreaming_GetPackID(char *name, s32 type, u32 id);
    s32 FileStreaming_ReleaseDataPack(u32 index, s32 type);
    FileStreaming_ReleaseDataPack(FileStreaming_GetPackID((char *)(arg0 + 1), 1, *arg0), 1);
}

s32 ActionCodeVariableModifyValue(u32 *data) {
    f32 GetCodeVariable(s32 var, u8 *thing);
    void SetCodeVariable(s32 var, u8 *obj, f32 value);
    f32 value;
    f32 operand;
    f32 cur;
    u32 i;

    if (data[1] < 100) {
        switch (data[data[2] + 3]) {
        case 0:
            value = *(f32 *)&data[data[2] + 4];
            break;
        case 1:
            value = *(f32 *)(*(u8 **)data[data[2] + 4] + 4);
            break;
        case 2:
            value = GetCodeVariable(data[data[2] + 4], (u8 *)data[data[2] + 5]);
            break;
        }
        for (i = 0; i < data[2]; i++) {
            cur = GetCodeVariable(data[1], (u8 *)data[i + 3]);
            switch (data[0]) {
            case 15:
                cur += value;
                break;
            case 16:
                cur -= value;
                break;
            case 45:
                cur *= value;
                break;
            case 46:
                if (value) {
                    cur /= value;
                }
                break;
            case 17:
                cur = value;
                break;
            }
            SetCodeVariable(data[1], (u8 *)data[i + 3], cur);
        }
    } else {
        value = GetCodeVariable(data[1], (u8 *)data[2]);
        switch (data[3]) {
        case 0:
            operand = *(f32 *)&data[4];
            break;
        case 1:
            operand = *(f32 *)(*(u8 **)data[4] + 4);
            break;
        case 2:
            operand = GetCodeVariable(data[4], (u8 *)data[5]);
            break;
        }
        switch (data[0]) {
        case 15:
            value += operand;
            break;
        case 16:
            value -= operand;
            break;
        case 45:
            value *= operand;
            break;
        case 46:
            if (operand) {
                value /= operand;
            }
            break;
        case 17:
            value = operand;
            break;
        }
        SetCodeVariable(data[1], (u8 *)data[2], value);
    }
    return 1;
}

s32 ActionMissionObjChangeMode(u32 *action) {
    u32 i;

    for (i = 0; i < action[1]; i++) {
        M2C_FIELD(M2C_FIELD(action[i + 2], u8 **, 4), u8 *, 1) = action[0];
    }
    return 1;
}

#pragma readonly_strings on
#pragma push
#pragma use_lmw_stmw on
s32 ActionVehicleChangeMode(u32 *action) {
    typedef struct AIScripts {
        u8 pad0[0x90];
        u32 scripts[4];
    } AIScripts;
    typedef struct ItemParams {
        u8 pad0[0x20];
        s32 count;
    } ItemParams;
    typedef struct TileItem {
        u8 pad0[0x30];
        ItemParams *params;
    } TileItem;
    typedef struct TileCar {
        Vector4 pos;
        f32 rotX;
        f32 rotY;
        f32 rotZ;
        u8 pad1C[0x24 - 0x1C];
        AIScripts *aiScripts;
        CarA *creature;
        u32 mode;
        u8 pad30[0x34 - 0x30];
        u8 unk34;
        u8 unk35;
        u8 pad36[0x38 - 0x36];
        f32 nearDist;
        f32 farDist;
        u32 scripts[4];
        u8 pad50[0x54 - 0x50];
        TileItem *item;
    } TileCar;
    typedef struct CreatureParams {
        Vector4 pos;
        Vector4 quat;
        Vector4 vel;
        AIScripts *aiScripts;
        u8 pad34[0x3C - 0x34];
        void *player;
    } CreatureParams;
    typedef struct AIPlayerDesc {
        u32 script;
        f32 nearSq;
        f32 farSq;
        u8 unkC;
        u8 unkD;
    } AIPlayerDesc;
    typedef struct AIPlayer {
        u8 pad0[0x3C];
        AIScripts *aiScripts;
        u8 pad40[0x6C - 0x40];
        u32 flags;
        u8 pad70[0xA8 - 0x70];
        TileCar *tileCar;
    } AIPlayer;
    extern Vector4 g_zeroVector;
    AIPlayer *PlayerManager_NewAIPlayer(AIPlayerDesc *desc);
    s32 PlayerManager_RemovePlayer(AIPlayer *player);
    void Player_SetName(AIPlayer *player, s32 port, const char *name);
    void Player_SetThing(AIPlayer *player, CarA *thing);
    void SetOrientation(Matrix4 *m, f32 x, f32 y, f32 z);
    void SubMatrixToQuaternion(Matrix4 *m, Vector4 *q);
    s32 GameSettings_GetDifficulty(void);
    void *ThingManager_DARYL_CreateThing(s32 type, void *params);
    void *ThingManagerList_GetByType(void *list, s32 type);
    void WildCardRack_JettisonAll(void *rack);
    void WildCardRack_Add(void *rack, TileItem *item, s32 count);
    s32 Trigger_IsTACSystemPaused(void);
    void Trigger_PauseTACSystem(s32 pause);
    void CarA_Release(CarA *car);
    void Thing_ProcessEndEvent(void *thing, void *event, s32 a, s32 b);
    CreatureParams params;
    Matrix4 mtx;
    AIPlayerDesc desc;
    AIPlayer *player;
    u32 mode;
    u32 count;
    u32 i;
    TileCar *spawn;
    CarA *creature;
    AIPlayer *ai;
    u32 script;
    s32 difficulty;
    s32 paused;
    void *rack;

    mode = action[0];
    count = action[1];
    for (i = 0; i < count; i++) {
        spawn = (TileCar *)action[i + 2];
        creature = spawn->creature;
        player = (AIPlayer *)Action_GetCarPlayer(creature);
        switch (mode) {
        case 0:
            if (creature != NULL) {
                paused = Trigger_IsTACSystemPaused();
                Trigger_PauseTACSystem(1);
                spawn->pos = creature->worldMatrix.v[3];
                CarA_Release(creature);
                PlayerManager_RemovePlayer(player);
                spawn->creature = NULL;
                Trigger_PauseTACSystem(paused);
            }
            break;
        case 9:
            player->flags |= 1;
            break;
        case 3:
            if (creature != NULL) {
                Thing_ProcessEndEvent(creature, creature->vehicleIface->collider, 0, 0);
                CarA_Release(creature);
            }
            break;
        case 1:
        case 10:
            if (creature == NULL && spawn->unk34 == 0) {
                difficulty = GameSettings_GetDifficulty();
                script = 0;
                params.pos = spawn->pos;
                params.pos.y += 10.0f;
                params.aiScripts = spawn->aiScripts;
                params.vel = g_zeroVector;
                SetOrientation(&mtx, spawn->rotY, spawn->rotX, spawn->rotZ);
                SubMatrixToQuaternion(&mtx, &params.quat);
                if (difficulty == 0) {
                    if (spawn->scripts[0] != 0) {
                        script = spawn->scripts[0];
                    } else if (params.aiScripts->scripts[0] != 0) {
                        script = params.aiScripts->scripts[0];
                    }
                } else if (difficulty == 1) {
                    if (spawn->scripts[1] != 0) {
                        script = spawn->scripts[1];
                    } else if (params.aiScripts->scripts[0] != 0) {
                        script = params.aiScripts->scripts[1];
                    }
                } else if (difficulty == 2) {
                    if (spawn->scripts[2] != 0) {
                        script = spawn->scripts[2];
                    } else if (params.aiScripts->scripts[0] != 0) {
                        script = params.aiScripts->scripts[2];
                    }
                } else if (difficulty == 3) {
                    if (spawn->scripts[3] != 0) {
                        script = spawn->scripts[3];
                    } else if (params.aiScripts->scripts[0] != 0) {
                        script = params.aiScripts->scripts[3];
                    }
                } else if (params.aiScripts->scripts[0] != 0) {
                    script = params.aiScripts->scripts[0];
                } else if (params.aiScripts->scripts[1] != 0) {
                    script = params.aiScripts->scripts[1];
                } else {
                    script = params.aiScripts->scripts[2];
                }
                desc.script = script;
                desc.unkD = 0;
                desc.unkC = spawn->unk34;
                desc.nearSq = spawn->nearDist * spawn->nearDist;
                desc.farSq = spawn->farDist * spawn->farDist;
                ai = PlayerManager_NewAIPlayer(&desc);
                if (ai != NULL) {
                    params.player = ai;
                    spawn->creature = ThingManager_DARYL_CreateThing(9, &params);
                    if (spawn->creature != NULL) {
                        spawn->creature->tileData = spawn;
                        Player_SetThing(ai, spawn->creature);
                        Player_SetName(ai, -1, "AI");
                        ai->tileCar = spawn;
                        ai->aiScripts = params.aiScripts;
                        if (spawn->item != NULL) {
                            rack = ThingManagerList_GetByType(&spawn->creature->motionState.slaves, 8);
                            WildCardRack_JettisonAll(rack);
                            WildCardRack_Add(rack, spawn->item, spawn->item->params->count);
                        }
                    } else {
                        PlayerManager_RemovePlayer(ai);
                    }
                }
            }
            if (player != NULL) {
                player->flags &= ~1;
            }
            break;
        }
        spawn->mode = mode;
    }
    return 1;
}
#pragma pop
#pragma readonly_strings reset
s32 ActionNavPointChangeMode(u32 *action) {
    u32 i;

    for (i = 0; i < action[1]; i++) {
        M2C_FIELD(action[i + 2], u8 *, 0x50) = action[0];
    }
    return 1;
}

void Trigger_PauseTACSystem(s8 paused);
s8 Trigger_IsTACSystemPaused(void);
s32 ActionPauseTACSystem(s32 *action) {
    switch (action[0]) {
    case 7:
        Trigger_PauseTACSystem(1);
        break;
    case 8:
        Trigger_PauseTACSystem(0);
        break;
    case 2:
        Trigger_PauseTACSystem(!Trigger_IsTACSystemPaused());
        break;
    }
    return 1;
}

s32 ActionSwapAIScript(u32 *action) {
    s32 GameSettings_GetDifficulty(void);
    void AI_Car_Reset(void *ai);
    u8 *creature;
    u32 i;
    u8 *vehicle;

    for (i = 0; i < action[4]; i++) {
        vehicle = (u8 *)action[i + 5];
        M2C_FIELD(vehicle, u32 *, 0x40) = action[0];
        M2C_FIELD(vehicle, u32 *, 0x44) = action[1];
        M2C_FIELD(vehicle, u32 *, 0x48) = action[2];
        M2C_FIELD(vehicle, u32 *, 0x4C) = action[3];
        creature = M2C_FIELD(vehicle, u8 **, 0x28);
        if (creature != NULL) {
            switch (GameSettings_GetDifficulty()) {
            case 0:
                M2C_FIELD(M2C_FIELD(creature, u8 **, 0x94), u32 *, 0xD0) = action[0];
                break;
            case 1:
                M2C_FIELD(M2C_FIELD(creature, u8 **, 0x94), u32 *, 0xD0) = action[1];
                break;
            case 2:
                M2C_FIELD(M2C_FIELD(creature, u8 **, 0x94), u32 *, 0xD0) = action[2];
                break;
            case 3:
                M2C_FIELD(M2C_FIELD(creature, u8 **, 0x94), u32 *, 0xD0) = action[3];
                break;
            }
            AI_Car_Reset(creature + 0x94);
        }
    }
    return 1;
}

#pragma use_lmw_stmw on
s32 ActionVehicleRemoveItem(u32 *action) {
    typedef struct ItemThing {
        u8 unk0[0x74];
        u32 flags;
        u8 unk78[0x8C - 0x78];
        void (**vtable)(struct ItemThing *);
    } ItemThing;
    typedef struct Vehicle {
        u8 unk0[0x28];
        u8 *creature;
    } Vehicle;
    void *ThingManagerList_GetByType(void *list, s32 type);
    void WildCardRack_JettisonAll(ItemThing *rack);
    void *list;
    u32 i;
    ItemThing *rack;
    ItemThing *item;
    u8 *creature;

    for (i = 0; i < action[1]; i++) {
        creature = ((Vehicle *)action[i + 2])->creature;
        if (creature != NULL) {
            list = creature + 0x344;
            rack = ThingManagerList_GetByType(list, 8);
            switch (action[0]) {
            case 0:
                WildCardRack_JettisonAll(rack);
                break;
            case 1:
                item = ThingManagerList_GetByType(list, 4);
                if (item != NULL) {
                    item->vtable[1](item);
                }
                break;
            }
            rack->flags &= ~0x400200;
        }
    }
    return 1;
}

#pragma push
#pragma use_lmw_stmw on
s32 ActionVehicleGiveItem(void *arg) {
    typedef struct ItemInfo {
        u8 type;
        u8 pad1[0x10 - 0x1];
        f32 height;
        u8 pad14[0x28 - 0x14];
        u32 effect;
    } ItemInfo;
    typedef struct Item {
        Vector4 pos;
        u8 pad10[0x4];
        s32 respawn;
        s32 timer;
        ItemInfo *info;
        f32 value;
        u8 *thing;
        u32 type;
        u8 pad2C[0x34 - 0x2C];
        u8 hasValue;
        u8 subtype;
        u8 pad36[0x38 - 0x36];
        s8 spawned;
        u8 pad39[0x40 - 0x39];
    } Item;
    typedef struct ItemParams {
        Vector4 quat;
        Vector4 pos;
        f32 unk30;
        f32 unk34;
        f32 unk38;
        u32 unk3C;
        ItemInfo *info;
        f32 value;
        u8 subtype;
    } ItemParams;
    typedef struct Action {
        s32 type;
        void *item;
        u32 count;
        u8 *cars[1];
    } Action;
    extern Vector4 g_zeroVector;
    void *ThingManagerList_GetByType(void *list, s32 type);
    void *ThingManager_DARYL_CreateThing(s32 type, void *params);
    void ThingManagerList_Remove(void **arg0, void *arg1);
    void *PlayerManager_GetPlayerByIndex(u8);
    u8 PlayerManager_PlayerCount(void);
    u32 FileStreaming_GetPackID(char *name, s32 type, u32 id);
    void *FileStreaming_InitializeDataPack(u32 index, s32 type, s8 refresh);
    u8 *DataPack_GetScript(void *pack, s32 id);
    s8 CarA_ProcessItemPickup(void *car, void *item);
    Item *item;
    Action *action = arg;
    u32 i;
    u8 *thing = NULL;
    u32 j;
    u8 *car;
    u8 numPlayers;
    struct { Vector4 a; Vector4 b; Vector4 c; Vector4 zero; u32 id; s32 player; f32 f0; f32 f1; } p;
    ItemParams params __attribute__((aligned(16)));
    ItemParams params2;
    ItemInfo *info;
    u8 *t;
    u32 packID;
    void *pack;

    switch (action->type) {
    case 0:
        item = action->item;
        if (item == NULL) {
            return;
        }
        if (item->info->type == 7) {
            numPlayers = PlayerManager_PlayerCount();
            for (j = 0; j < numPlayers; j++) {
                car = *(u8 **)((u8 *)PlayerManager_GetPlayerByIndex(j) + 0x2C);
                if (car != NULL && *(u8 **)(car + 0x8C) == g_carAVTable) {
                    t = ThingManagerList_GetByType(car + 0x344, 4);
                    if (t != NULL && *(Item **)(t + 0x90) == item) {
                        ThingManagerList_Remove((void **)(car + 0x344), t);
                    }
                }
            }
        }
        if (item->thing == NULL) {
            item->timer = 0;
            params.pos = item->pos;
            params.pos.y += item->info->height;
            params.unk30 = 0.0f;
            params.unk34 = 0.0f;
            params.unk38 = 0.0f;
            params.info = item->info;
            params.quat.w = 1.0f;
            params.quat.x = 0.0f;
            params.quat.y = 0.0f;
            params.quat.z = 0.0f;
            params.value = item->hasValue ? item->value : 0.0f;
            params.subtype = item->subtype;
            t = ThingManager_DARYL_CreateThing(4, &params);
            if (t != NULL) {
                item->spawned = 1;
                item->thing = t;
                *(Item **)(item->thing + 0x90) = item;
                *(u32 *)(t + 0x74) |= 1;
                if (item->respawn != 0) {
                    *(Item **)(t + 0xD0) = item;
                }
                if (item->info->effect != 0) {
                    p.id = item->info->effect;
                    p.a = item->pos;
                    p.b = params.quat;
                    p.c.z = 0.0f;
                    p.c.y = 0.0f;
                    p.c.x = 0.0f;
                    p.player = 0;
                    p.f0 = 0.0f;
                    p.f1 = 0.0f;
                    p.zero = g_zeroVector;
                    ThingManager_DARYL_CreateThing(1, &p);
                }
            }
        }
        thing = item->thing;
        break;
    case 1:
        info = NULL;
        if ((u32)action->item < 1000) {
            packID = FileStreaming_GetPackID(*(char **)(*(u8 **)((u8 *)g_worldData.world + 0x9C) + 0xA4), 2, (u32)action->item);
            if (packID < 0xFFFFFFFF) {
                pack = FileStreaming_InitializeDataPack(packID, 2, 1);
                if (pack != NULL) {
                    info = (ItemInfo *)DataPack_GetScript(pack, (s32)action->item);
                }
            }
        } else {
            info = action->item;
        }
        if (info != NULL) {
            params2.pos = g_zeroVector;
            params2.pos.y += info->height;
            params2.unk30 = 0.0f;
            params2.unk34 = 0.0f;
            params2.unk38 = 0.0f;
            params2.info = info;
            params2.quat.w = 1.0f;
            params2.quat.x = 0.0f;
            params2.quat.y = 0.0f;
            params2.quat.z = 0.0f;
            params2.value = 0.0f;
            thing = ThingManager_DARYL_CreateThing(4, &params2);
            if (thing != NULL) {
                *(u32 *)(thing + 0x74) |= 1;
            }
        }
        break;
    }
    if (thing == NULL) {
        return 0;
    }
    for (i = 0; i < action->count; i++) {
        if (*(u8 **)(action->cars[i] + 0x28) != NULL) {
            CarA_ProcessItemPickup(*(u8 **)(action->cars[i] + 0x28), thing);
        }
    }
    return 1;
}
#pragma pop

#pragma use_lmw_stmw off
s32 ActionPlaySoundSequence(void *arg0) {
    void SoundSequence_ClearQueue();
    void SoundSequence_Play(u32, s32);
    void SoundSequence_PlayQueued(u32, s32);
    extern u8 g_soundSeqVTable[];
    u32 id;
    void *thing;

    id = M2C_FIELD(arg0, u32 *, 8);
    if (M2C_FIELD(arg0, u32 *, 0) != 0) {
        if (id != 0) {
            if (M2C_FIELD(arg0, u32 *, 4) != 0) {
                SoundSequence_PlayQueued(id, 0);
            } else {
                SoundSequence_Play(id, 0);
            }
        }
    } else if (M2C_FIELD(arg0, u32 *, 4) != 0) {
        SoundSequence_ClearQueue();
    } else {
        for (thing = g_thingManager_DARYL.activeList; thing != NULL; thing = M2C_FIELD(thing, void **, 0xA4)) {
            if (M2C_FIELD(thing, u8 **, 0x8C) == g_soundSeqVTable && M2C_FIELD(thing, u32 *, 0xB0) == id) {
                ((void (*)(void *))(M2C_FIELD(thing, void ***, 0x8C))[1])(thing);
            }
        }
    }
}

s32 ActionCreatureChangeMode(u32 *action) {
    void Creature_MotionStateInitDeath(u8 *creature);
    void Creature_Release(u32 handle);
    u32 handle;
    u32 i;
    u8 *creature;

    for (i = 0; i < action[1]; i++) {
        creature = (u8 *)action[i + 2];
        handle = M2C_FIELD(creature, u32 *, 0x1C);
        switch (action[0]) {
        case 0:
            M2C_FIELD(creature, u32 *, 0x20) = 0;
            if (handle != 0) {
                M2C_FIELD(creature, u32 *, 0x1C) = 0;
                Creature_Release(handle);
            }
            break;
        case 1:
            M2C_FIELD(creature, u32 *, 0x20) = 1;
            break;
        case 3:
            M2C_FIELD(creature, u32 *, 0x20) = 0;
            if (handle != 0) {
                M2C_FIELD(creature, u32 *, 0x1C) = 0;
                Creature_MotionStateInitDeath((u8 *)handle);
            }
            break;
        case 25:
            M2C_FIELD(creature, u32 *, 0x20) = 0;
            if (handle != 0) {
                M2C_FIELD(creature, u32 *, 0x1C) = 0;
                Creature_Release(handle);
            }
            break;
        case 26:
            M2C_FIELD(creature, u32 *, 0x20) = 1;
            if (handle != 0) {
                M2C_FIELD(creature, u32 *, 0x1C) = 0;
                Creature_MotionStateInitDeath((u8 *)handle);
            }
            break;
        }
    }
    return 1;
}
