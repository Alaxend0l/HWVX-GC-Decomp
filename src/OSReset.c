#include "common.h"


typedef struct OSResetFunctionInfo {
    BOOL (*func)(BOOL final);
    u32 priority;
    struct OSResetFunctionInfo *next;
    struct OSResetFunctionInfo *prev;
} OSResetFunctionInfo;

extern OSResetFunctionInfo *ResetFunctionQueue;

BOOL __OSSyncSram(void);
void OSCancelThread(void *thread);

static BOOL __OSCallResetFunctions(BOOL final) {
    OSResetFunctionInfo *info;
    s32 err = 0;

    for (info = ResetFunctionQueue; info; info = info->next) {
        err |= !info->func(final);
    }
    err |= !__OSSyncSram();
    if (err) {
        return FALSE;
    }
    return TRUE;
}

static inline void KillThreads(void) {
    u8 *thread;
    u8 *next;

    for (thread = *(u8 **)0x800000DC; thread; thread = next) {
        next = *(u8 **)(thread + 0x2FC);
        switch (*(u16 *)(thread + 0x2C8)) {
        case 1:
        case 4:
            OSCancelThread(thread);
            break;
        }
    }
}

#pragma peephole off
volatile u32 __PIRegs[12] : 0xCC003000;
volatile u16 __VIRegs[59] : 0xCC002000;
void __OSDoHotReset(u32 resetCode) {
    s32 OSDisableInterrupts(void);
    void ICFlashInvalidate(void);
    void Reset(u32);
    OSDisableInterrupts();
    __VIRegs[1] = 0;
    ICFlashInvalidate();
    Reset(resetCode * 8);
}

BOOL OSDisableInterrupts(void);
s32 OSDisableScheduler(void);
s32 OSEnableScheduler(void);
void __OSStopAudioSystem(void);
BOOL __PADDisableRecalibration(BOOL disable);
BOOL __OSSyncSram(void);
u8 *__OSLockSram(void);
BOOL __OSUnlockSram(BOOL commit);
void LCDisable(void);
void OSCancelThread(void *thread);
void __OSReboot(u32 resetCode, BOOL forceMenu);
void OSResetSystem(s32 reset, u32 resetCode, BOOL forceMenu) {
    BOOL rc;
    BOOL enabled;
    BOOL disableRecalibration;
    u8 *thread;
    u8 *next;

    OSDisableScheduler();
    __OSStopAudioSystem();
    if (reset == 2) {
        disableRecalibration = __PADDisableRecalibration(TRUE);
    }
    while (!__OSCallResetFunctions(FALSE)) {
    }
    if (reset == 1 && forceMenu) {
        u8 *sram = __OSLockSram();
        sram[0x13] |= 0x40;
        __OSUnlockSram(TRUE);
        while (!__OSSyncSram()) {
        }
    }
    enabled = OSDisableInterrupts();
    rc = __OSCallResetFunctions(TRUE);
    LCDisable();
    if (reset == 1) {
        __OSDoHotReset(resetCode);
    } else if (reset == 0) {
        for (thread = *(u8 **)0x800000DC; thread; thread = next) {
            next = *(u8 **)(thread + 0x2FC);
            switch (*(u16 *)(thread + 0x2C8)) {
            case 1:
            case 4:
                OSCancelThread(thread);
                break;
            }
        }
        OSEnableScheduler();
        __OSReboot(resetCode, forceMenu);
    }
    KillThreads();
    memset((void *)0x80000040, 0, 0x8C);
    memset((void *)0x800000D4, 0, 0x14);
    memset((void *)0x800000F4, 0, 4);
    memset((void *)0x80003000, 0, 0xC0);
    memset((void *)0x800030C8, 0, 0xC);
    memset((void *)0x800030E2, 0, 1);
    __PADDisableRecalibration(disableRecalibration);
}

u32 OSGetResetCode(void) {
    if (*(u8 *)0x800030E2 != 0) {
        return 0x80000000;
    }
    return (__PIRegs[9] & ~7) >> 3;
}
