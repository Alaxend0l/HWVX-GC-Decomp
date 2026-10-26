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

