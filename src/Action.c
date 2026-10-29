#include "common.h"

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
