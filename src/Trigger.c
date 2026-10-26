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

