#include "common.h"

typedef struct SoundSequence {
    u8 pad0[0x74];
    u32 flags;    /* 0x74 */
    u8 pad78[0x38];
    u8 *params;   /* 0xB0 */
} SoundSequence;

u32 ThingManager_GetActiveSoundSequences(u8 **list, u32 max);

s32 SoundSeq_Invalidate(SoundSequence *seq, u32 start, u32 end)
{
    if ((u32)seq->params >= start && (u32)seq->params < end) {
        return 1;
    }
    return 0;
}

s32 SoundSequence_GetTotalElementsInQueue(void)
{
    u8 *list[128];
    u32 count;
    s32 total;

    total = 0;
    count = ThingManager_GetActiveSoundSequences(list, 128);
    while (count != 0) {
        count--;
        if (((SoundSequence *)list[count])->flags & 0x40000000) {
            total++;
        }
    }
    return total;
}

