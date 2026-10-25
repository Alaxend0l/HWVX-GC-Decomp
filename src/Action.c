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

