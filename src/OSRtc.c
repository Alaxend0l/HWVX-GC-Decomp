#include "common.h"

typedef struct SramControlBlock {
    u8 sram[64];
    u32 offset;
    s32 enabled;
    s32 locked;
    s32 sync;
    void (*callback)(void);
} SramControlBlock;

typedef struct OSSram {
    u16 checkSum;
    u16 checkSumInv;
    u32 ead0;
    u32 ead1;
    u32 counterBias;
    s8 displayOffsetH;
    u8 ntd;
    u8 language;
    u8 flags;
} OSSram;

typedef struct OSSramEx {
    u8 flashID[2][12];
    u32 wirelessKeyboardID;
    u16 wirelessPadID[4];
    u8 dvdErrorCode;
    u8 _padding0;
    u8 flashIDCheckSum[2];
    u16 gbs;
    u16 _padding1;
} OSSramEx;

static SramControlBlock Scb __attribute__((aligned(32)));

void DCInvalidateRange(void *addr, u32 nBytes);
s32 EXILock(s32 chan, u32 dev, void *unlockedCallback);
s32 EXIUnlock(s32 chan);
s32 EXISelect(s32 chan, u32 dev, u32 freq);
s32 EXIDeselect(s32 chan);
s32 EXIImm(s32 chan, void *buf, s32 len, u32 type, void *callback);
s32 EXIImmEx(s32 chan, void *buf, s32 len, u32 mode);
s32 EXIDma(s32 chan, void *buf, s32 len, u32 type, void *callback);
s32 EXISync(s32 chan);
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

static inline s32 ReadSram(void *buffer) {
    s32 err;
    u32 cmd;

    DCInvalidateRange(buffer, 64);
    if (!EXILock(0, 1, 0)) {
        return 0;
    }
    if (!EXISelect(0, 1, 3)) {
        EXIUnlock(0);
        return 0;
    }
    cmd = 0x20000000 | 0x100;
    err = 0;
    err |= !EXIImm(0, &cmd, 4, 1, 0);
    err |= !EXISync(0);
    err |= !EXIDma(0, buffer, 64, 0, 0);
    err |= !EXISync(0);
    err |= !EXIDeselect(0);
    EXIUnlock(0);
    return !err;
}

s32 UnlockSram(s32 commit, u32 offset);
#pragma peephole on
s32 WriteSram(void *buffer, u32 offset, u32 size);
static void WriteSramCallback(s32 chan, void *context) {
    Scb.sync = WriteSram(Scb.sram + Scb.offset, Scb.offset, 64 - Scb.offset);
    if (Scb.sync) {
        Scb.offset = 64;
    }
}

static void WriteSramCallback(s32 chan, void *context);
s32 WriteSram(void *buffer, u32 offset, u32 size) {
    s32 err;
    u32 cmd;

    if (!EXILock(0, 1, WriteSramCallback)) {
        return 0;
    }
    if (!EXISelect(0, 1, 3)) {
        EXIUnlock(0);
        return 0;
    }
    offset <<= 6;
    cmd = 0xA0000000 | 0x100 + offset;
    err = 0;
    err |= !EXIImm(0, &cmd, 4, 1, 0);
    err |= !EXISync(0);
    err |= !EXIImmEx(0, buffer, (s32)size, 1);
    err |= !EXIDeselect(0);
    EXIUnlock(0);
    return !err;
}

void __OSInitSram(void) {
    Scb.locked = Scb.enabled = 0;
    Scb.sync = ReadSram(Scb.sram);
    Scb.offset = 64;
}

void *__OSLockSram(void) {
    return LockSram(0);
}

void *__OSLockSramEx(void) {
    return LockSram(0x14);
}

s32 UnlockSram(s32 commit, u32 offset) {
    u16 *p;

    if (commit) {
        if (offset == 0) {
            OSSram *sram = (OSSram *)Scb.sram;

            if (2u < (sram->flags & 3)) {
                sram->flags &= ~3;
            }
            sram->checkSum = sram->checkSumInv = 0;
            for (p = (u16 *)&sram->counterBias; p < (u16 *)(Scb.sram + sizeof(OSSram)); p++) {
                sram->checkSum += *p;
                sram->checkSumInv += ~*p;
            }
        }
        if (offset < Scb.offset) {
            Scb.offset = offset;
        }
        Scb.sync = WriteSram(Scb.sram + Scb.offset, Scb.offset, 64 - Scb.offset);
        if (Scb.sync) {
            Scb.offset = 64;
        }
    }
    Scb.locked = 0;
    OSRestoreInterrupts(Scb.enabled);
    return Scb.sync;
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

s32 __OSReadROM(void *buffer, s32 length, s32 offset) {
    s32 err;
    u32 cmd;

    DCInvalidateRange(buffer, (u32)length);
    if (!EXILock(0, 1, 0)) {
        return 0;
    }
    if (!EXISelect(0, 1, 3)) {
        EXIUnlock(0);
        return 0;
    }
    cmd = (u32)(offset << 6);
    err = 0;
    err |= !EXIImm(0, &cmd, 4, 1, 0);
    err |= !EXISync(0);
    err |= !EXIDma(0, buffer, length, 0, 0);
    err |= !EXISync(0);
    err |= !EXIDeselect(0);
    EXIUnlock(0);
    return !err;
}

u32 OSGetSoundMode(void) {
    u8 *sram;
    u32 mode;

    sram = __OSLockSram();
    mode = (sram[0x13] & 4) ? 1 : 0;
    __OSUnlockSram(0);
    return mode;
}

void OSSetSoundMode(u32 mode) {
    u8 *sram;

    mode = (mode & 1) << 2;
    sram = __OSLockSram();
    if (mode == (sram[0x13] & 4)) {
        __OSUnlockSram(0);
        return;
    }
    sram[0x13] &= ~4;
    sram[0x13] |= mode;
    __OSUnlockSram(1);
}

u16 OSGetWirelessID(s32 chan) {
    OSSramEx *sram;
    u16 id;

    sram = __OSLockSramEx();
    id = sram->wirelessPadID[chan];
    __OSUnlockSramEx(0);
    return id;
}

void OSSetWirelessID(s32 chan, u16 id) {
    OSSramEx *sram;

    sram = __OSLockSramEx();
    if (sram->wirelessPadID[chan] != id) {
        sram->wirelessPadID[chan] = id;
        __OSUnlockSramEx(1);
    } else {
        __OSUnlockSramEx(0);
    }
}

