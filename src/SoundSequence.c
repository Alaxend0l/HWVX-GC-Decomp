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
