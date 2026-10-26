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

