#include "common.h"


s8 TriggerSoundSeqQueueEnd(void) {
    s32 SoundSequence_GetTotalElementsInQueue();
    return (s8) (SoundSequence_GetTotalElementsInQueue() == 0);
}

s32 TriggerMoveableEnd(void) {
    return 1;
}

s32 TriggerCreaturePickedUp(void) {
    return 1;
}

s32 TriggerTrue(void) {
    return 1;
}

s32 TriggerGameRoundsComplete(void) {
    return 1;
}

s32 TriggerGameRoundXComplete(void) {
    return 1;
}

s32 TriggerItemDropped(void) {
    return 1;
}

void Trigger_Create(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

void Trigger_Destroy(void) {
    extern s32 m_triggerData_803C7120;
    Deallocate(m_triggerData_803C7120);
    m_triggerData_803C7120 = 0;
}

void Trigger_Init(void) {
    extern s32 m_triggerData_803C7120;
    extern u32 TriggerStats_800D3C58[][3];
    extern u32 **m_Triggers_802C1890[];
    extern u32 m_interfaces_802C1900[][2];
    extern s8 g_bTACSystemPaused_803C7124;
    u32 i;
    u32 offset = 0;
    u32 total = 0;

    if ((u32)m_triggerData_803C7120 != 0) {
        Deallocate(m_triggerData_803C7120);
        m_triggerData_803C7120 = 0;
    }
    for (i = 0; i < 28; i++) {
        TriggerStats_800D3C58[i][0] = 0;
        total += TriggerStats_800D3C58[i][1];
    }
    m_triggerData_803C7120 = (s32)Allocate(total * 4, 0, "");
    for (i = 0; i < 28; i++) {
        m_Triggers_802C1890[i] = (u32 **)(m_triggerData_803C7120 + offset * 4);
        offset += TriggerStats_800D3C58[i][1];
    }
    for (i = 0; i < 10; i++) {
        m_interfaces_802C1900[i][0] = 0;
    }
    g_bTACSystemPaused_803C7124 = 0;
}

void Trigger_SaveActions(s32 *saved, u32 count) {
    typedef struct { u32 pad[4]; u32 value; u32 pad2; } TrigAction;
    extern u32 TriggerStats_800D3C58[][3];
    extern u32 **m_Triggers_802C1890[];
    u32 type;
    u32 i;
    u32 j;
    u32 n;

    n = 0;
    for (type = 0; type < 28; type++) {
        for (i = 0; i < TriggerStats_800D3C58[type][0]; i++) {
            for (j = 0; j < m_Triggers_802C1890[type][i][7]; j++) {
                if (n >= count) {
                    return;
                }
                if (((TrigAction *)m_Triggers_802C1890[type][i][5])[j].value != 0xFFFFFFFF) {
                    saved[n++] = ((TrigAction *)m_Triggers_802C1890[type][i][5])[j].value;
                }
            }
        }
    }
}

void Trigger_RestoreActions(s32 *saved, u32 count) {
    typedef struct { u32 pad[4]; u32 value; u32 pad2; } TrigAction;
    extern u32 TriggerStats_800D3C58[][3];
    extern u32 **m_Triggers_802C1890[];
    u32 type;
    u32 i;
    u32 j;
    u32 n;

    n = 0;
    for (type = 0; type < 28; type++) {
        for (i = 0; i < TriggerStats_800D3C58[type][0]; i++) {
            for (j = 0; j < m_Triggers_802C1890[type][i][7] && n < count; j++) {
                if (((TrigAction *)m_Triggers_802C1890[type][i][5])[j].value != 0xFFFFFFFF) {
                    ((TrigAction *)m_Triggers_802C1890[type][i][5])[j].value = saved[n++];
                }
            }
            if (type == 8) {
                m_Triggers_802C1890[type][i][9] = 1;
            } else {
                m_Triggers_802C1890[type][i][9] = 0x10;
            }
        }
    }
}

void Trigger_PauseTACSystem(s8 arg0) {
    extern s8 g_bTACSystemPaused_803C7124;
    g_bTACSystemPaused_803C7124 = arg0;
}

u8 Trigger_IsTACSystemPaused(void) {
    extern u8 g_bTACSystemPaused_803C7124;
    return g_bTACSystemPaused_803C7124;
}

void Trigger_Heartbeat(void) {
    extern u32 TriggerStats_800D3C58[][3];
    extern u32 **m_Triggers_802C1890[];
    extern s8 g_bTACSystemPaused_803C7124;
    extern s8 (*pTriggerFuncArray_800D3DA8[])(void *param);
    extern s32 m_ReleasedInterface_803C7128;
    s8 TriggerCheckConditions(u8, s32, s32);
    s32 Action_AddAction(s32);
    u32 type;
    u32 i;
    u32 j;
    u32 actions;
    u32 *trigger;

    if (!g_bTACSystemPaused_803C7124) {
        for (type = 0; type < 28; type++) {
            if (*(s8 *)&TriggerStats_800D3C58[type][2]) {
                continue;
            }
            for (i = 0; i < TriggerStats_800D3C58[type][0]; i++) {
                trigger = m_Triggers_802C1890[type][i];
                if ((trigger[9] & 1) && pTriggerFuncArray_800D3DA8[trigger[0]]((void *)trigger[1])) {
                    trigger = m_Triggers_802C1890[type][i];
                    if (TriggerCheckConditions(((u8 *)trigger)[0x28], trigger[4], trigger[3])) {
                        actions = 0;
                        trigger = m_Triggers_802C1890[type][i];
                        if ((s32)trigger[0] == 0x12 && ((u32 *)trigger[1])[2] == 0) {
                            for (j = 0; j < m_Triggers_802C1890[type][i][8]; j++) {
                                actions += Action_AddAction((s32)((u8 *)m_Triggers_802C1890[type][i][6] + j * 0x18));
                            }
                        } else {
                            for (j = 0; j < m_Triggers_802C1890[type][i][7]; j++) {
                                actions += Action_AddAction((s32)((u8 *)m_Triggers_802C1890[type][i][5] + j * 0x18));
                            }
                        }
                        if (actions == 0) {
                            m_Triggers_802C1890[type][i][9] = 2;
                        } else {
                            m_Triggers_802C1890[type][i][9] = 0x10;
                        }
                    } else {
                        m_Triggers_802C1890[type][i][9] = 0x10;
                    }
                }
                trigger = m_Triggers_802C1890[type][i];
                if ((trigger[9] & 0x10) && !pTriggerFuncArray_800D3DA8[trigger[0]]((void *)trigger[1])) {
                    m_Triggers_802C1890[type][i][9] |= 1;
                    m_Triggers_802C1890[type][i][9] &= ~0x10;
                }
            }
        }
        m_ReleasedInterface_803C7128 = -1;
    }
}

s32 Trigger_AddTrigger(s32 *trigger) {
    extern u32 TriggerStats_800D3C58[][3];
    extern char *triggerLabelsArray_800D3BE8[];
    extern s32 **m_Triggers_802C1890[];
    extern s8 g_bTACSystemPaused_803C7124;
    int sprintf(char *buf, const char *fmt, ...);
    char buf[0x200];
    s32 type = trigger[0];

    if (TriggerStats_800D3C58[type][0] == TriggerStats_800D3C58[type][1]) {
        sprintf(buf, "Could not add the trigger: \"%s\", please notify Peter.", triggerLabelsArray_800D3BE8[type]);
        return 0;
    }
    m_Triggers_802C1890[type][TriggerStats_800D3C58[type][0]++] = trigger;
    if (trigger[0] == 8) {
        trigger[9] = 1;
    } else {
        trigger[9] = 0x10;
    }
    g_bTACSystemPaused_803C7124 = 0;
    return 1;
}

s32 Trigger_ReleaseTrigger(s32 *trigger) {
    extern u32 TriggerStats_800D3C58[][3];
    extern s32 **m_Triggers_802C1890[];
    u32 i;

    for (i = 0; i < TriggerStats_800D3C58[trigger[0]][0]; i++) {
        if (m_Triggers_802C1890[trigger[0]][i] == trigger) {
            TriggerStats_800D3C58[trigger[0]][0]--;
            m_Triggers_802C1890[trigger[0]][i] = m_Triggers_802C1890[trigger[0]][TriggerStats_800D3C58[trigger[0]][0]];
            return 1;
        }
    }
    return 0;
}

s8 TriggerCheckConditions(u8 mode, s32 count, s32 conds) {
    typedef struct TriggerCondition {
        u32 type;
        void *param;
        u8 unk8[0x2C - 0x8];
    } TriggerCondition;
    extern s8 (*pTriggerFuncArray_800D3DA8[])(void *param);
    TriggerCondition *cond = (TriggerCondition *)conds;
    u32 i;

    if (count == 0) {
        return 1;
    }
    switch (mode) {
    case 0:
        for (i = 0; i < count; i++) {
            if (!pTriggerFuncArray_800D3DA8[cond[i].type](cond[i].param)) {
                return 0;
            }
        }
        return 1;
    case 1:
        for (i = 0; i < count; i++) {
            if (pTriggerFuncArray_800D3DA8[cond[i].type](cond[i].param)) {
                return 1;
            }
        }
        return 0;
    case 2:
        for (i = 0; i < count; i++) {
            if (!pTriggerFuncArray_800D3DA8[cond[i].type](cond[i].param)) {
                return 1;
            }
        }
        return 0;
    case 3:
        for (i = 0; i < count; i++) {
            if (pTriggerFuncArray_800D3DA8[cond[i].type](cond[i].param)) {
                return 0;
            }
        }
        return 1;
    }
    return 0;
}

s32 TriggerLevelStart(void) {
    return 1;
}

