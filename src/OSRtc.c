#include "common.h"

typedef struct SramControlBlock {
    u8 sram[64];
    u32 offset;
    s32 enabled;
    s32 locked;
    s32 sync;
    void (*callback)(void);
} SramControlBlock;

static SramControlBlock Scb __attribute__((aligned(32)));

s32 OSDisableInterrupts(void);
s32 OSRestoreInterrupts(s32 level);

static inline void *LockSram(u32 offset) {
    s32 enabled;

    enabled = OSDisableInterrupts();
    if (Scb.locked != 0) {
        OSRestoreInterrupts(enabled);
        return NULL;
    }
    Scb.enabled = enabled;
    Scb.locked = 1;
    return Scb.sram + offset;
}

s32 UnlockSram(s32 commit, u32 offset);
#pragma peephole on
void *__OSLockSram(void) {
    return LockSram(0);
}

void *__OSLockSramEx(void) {
    return LockSram(0x14);
}

s32 __OSUnlockSram(s32 commit) {
    return UnlockSram(commit, 0);
}

s32 __OSUnlockSramEx(s32 commit) {
    return UnlockSram(commit, 0x14);
}

s32 __OSSyncSram(void)
{
    return Scb.sync;
}
