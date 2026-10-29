// Code of main that had no source file yet: decompdecomp gave it this one.
// split_file divides it into the game's real files (file_status shows its range).
#include "common.h"

typedef struct TRKCPURegs {
    u32 gpr[32];
    u32 pc;
} TRKCPURegs;

typedef struct TRKCPUState {
    TRKCPURegs regs;
    u8 pad_84[0x98 - 0x84];
    u64 fpr[32];
    u8 pad_198[0x1A8 - 0x198];
    u32 extended1[0x14]; /* 0x1A8 */
    u32 stepMode; /* 0x1F8: MSR, 0x400 = single-step trace */
    u8 pad_1FC[0x278 - 0x1FC];
    u32 field_278;
    u8 pad_27C[0x2F8 - 0x27C];
    u32 field_2F8;
} TRKCPUState;

typedef struct TRKState {
    u8 pad_0[0x98];
    s32 field_98;
    s32 field_9C;
    void *inputPendingPtr;
} TRKState;

typedef struct TRKStepStatus {
    s32 active;
    u8 type;        /* 0: count, 1: out of range, 0x10: over */
    u8 pad_5[3];
    u32 count;
    u32 rangeStart;
    u32 rangeEnd;
} TRKStepStatus;

typedef struct TRKExceptionStatus {
    u32 field_0;
    u8 pad_4[4];
    u16 field_8;
    u8 pad_A[2];
    u8 inTRK;             /* 0xC */
    u8 exceptionDetected; /* 0xD */
    u8 pad_E[2];
} TRKExceptionStatus;

typedef struct memRange {
    u8 *start;
    u8 *end;
    s32 readable;
    s32 writeable;
} memRange;

extern const memRange gTRKMemMap[1];
extern TRKCPUState gTRKCPUState;
extern TRKState gTRKState;
extern TRKStepStatus gTRKStepStatus_800DA0B4;
extern TRKExceptionStatus gTRKExceptionStatus_800DA0A4;

s32 TRKTargetAccessMemory(void *data, u32 start, u32 *length, s32 space, s32 read);

typedef void (*TRKAccessFunc)(void *, void *);

extern u8 TRKvalue128_temp[16];
void TRK_flush_cache(u32 start, u32 length);

static inline s32 TRKPPCAccessSpecialReg(void *value, u32 *access_func, s32 read) {
    TRKAccessFunc asm_access;

    access_func[9] = 0x4E800020;
    asm_access = (TRKAccessFunc)access_func;
    TRK_flush_cache((u32)access_func, sizeof(u32) * 10);
    (*asm_access)(value, TRKvalue128_temp);
    return 0;
}

static inline s32 TRKPPCAccessSPR(void *value, u32 spr_register_num, s32 read) {
    u32 access_func[10] = {
        0x60000000, 0x60000000, 0x60000000, 0x60000000, 0x60000000,
        0x60000000, 0x60000000, 0x60000000, 0x60000000, 0x60000000,
    };

    if (read) {
        access_func[0] = 0x7C8002A6 | ((spr_register_num & 0x1F) << 16) | ((spr_register_num & 0x3E0) << 6);
        access_func[1] = 0x90830000;
    } else {
        access_func[0] = 0x80830000;
        access_func[1] = 0x7C8003A6 | ((spr_register_num & 0x1F) << 16) | ((spr_register_num & 0x3E0) << 6);
    }
    return TRKPPCAccessSpecialReg(value, access_func, read);
}

static inline s32 TRKPPCAccessPairedSingleRegister(void *srcDestPtr, u32 psr, s32 read) {
    u32 instructionData[10] = {
        0x60000000, 0x60000000, 0x60000000, 0x60000000, 0x60000000,
        0x60000000, 0x60000000, 0x60000000, 0x60000000, 0x60000000,
    };

    if (read) {
        instructionData[0] = 0xF0030000 | (psr << 21);
    } else {
        instructionData[0] = 0xE0030000 | (psr << 21);
    }
    return TRKPPCAccessSpecialReg(srcDestPtr, instructionData, read);
}
s32 TRKAppendBuffer1_ui32(void *buffer, u32 data);
s32 TRKAppendBuffer1_ui16(void *buffer, u16 data);
s32 TRKReadBuffer1_ui64(void *buffer, u64 *data);

void TRKTargetSetInputPendingPtr(void *ptr) {
    gTRKState.inputPendingPtr = ptr;
}

s32 TRKPPCAccessFPRegister(void *srcDestPtr, u32 fpr, s32 read) {
    s32 error = 0;
    u32 instructionData1[10] = {
        0x60000000, 0x60000000, 0x60000000, 0x60000000, 0x60000000,
        0x60000000, 0x60000000, 0x60000000, 0x60000000, 0x60000000,
    };

    if (fpr < 0x20) {
        if (read) {
            instructionData1[0] = 0xD8030000 | (fpr << 21);
        } else {
            instructionData1[0] = 0xC8030000 | (fpr << 21);
        }
        error = TRKPPCAccessSpecialReg(srcDestPtr, instructionData1, read);
    } else if (fpr == 0x20) {
        *(u64 *)srcDestPtr &= 0xFFFFFFFF;
    } else if (fpr == 0x21) {
        if (!read) {
            *(u32 *)srcDestPtr = *((u32 *)srcDestPtr + 1);
        }
        error = TRKPPCAccessSPR(srcDestPtr, 1022, read);
        if (read) {
            *(u64 *)srcDestPtr = (*(u64 *)srcDestPtr >> 32) & 0xFFFFFFFF;
        }
    }
    return error;
}

s32 TRKTargetStop(void) {
    gTRKState.field_98 = 1;
    return 0;
}

void TRKTargetSetStopped(s32 stopped) {
    gTRKState.field_98 = stopped;
}

s32 TRKTargetStopped(void) {
    return gTRKState.field_98;
}

s32 TRKTargetFlushCache(u8 space, u32 start, u32 end) {
    void TRK_flush_cache(u32 start, u32 length);

    if (start < end) {
        TRK_flush_cache(start, end - start);
        return 0;
    }
    return 0x700;
}

s32 HandleOpenFileSupportRequest(const char *path, u8 replyError, u32 *param, u8 *ioResult);
s32 HandleCloseFileSupportRequest(u32 fileHandle, u8 *ioResult);
s32 HandlePositionFileSupportRequest(u32 fileHandle, u32 *position, u8 mode, u8 *ioResult);
s32 TRKSuppAccessFile(u8 file, u8 *data, u32 *count, u8 *ioResult, BOOL needReply, BOOL read);
#pragma use_lmw_stmw on
s32 TRKTargetSupportRequest(void) {
    s32 err;
    u32 position;
    u8 ioResult;
    u32 command;
    u8 event[0x10];
    u32 *length;

    command = (u8)gTRKCPUState.regs.gpr[3];
    if (command != 0xD1 && command != 0xD0 && command != 0xD2 && command != 0xD3 && command != 0xD4) {
        TRKConstructEvent(event, 4);
        TRKPostEvent(event);
        return 0;
    }
    if (command == 0xD2) {
        err = HandleOpenFileSupportRequest((const char *)gTRKCPUState.regs.gpr[4], gTRKCPUState.regs.gpr[5],
                                           (u32 *)gTRKCPUState.regs.gpr[6], &ioResult);
        if (ioResult == 0 && err != 0) {
            ioResult = 1;
        }
        gTRKCPUState.regs.gpr[3] = ioResult;
    } else if (command == 0xD3) {
        err = HandleCloseFileSupportRequest(gTRKCPUState.regs.gpr[4], &ioResult);
        if (ioResult == 0 && err != 0) {
            ioResult = 1;
        }
        gTRKCPUState.regs.gpr[3] = ioResult;
    } else if (command == 0xD4) {
        position = *(u32 *)gTRKCPUState.regs.gpr[5];
        err = HandlePositionFileSupportRequest(gTRKCPUState.regs.gpr[4], &position, gTRKCPUState.regs.gpr[6], &ioResult);
        if (ioResult == 0 && err != 0) {
            ioResult = 1;
        }
        gTRKCPUState.regs.gpr[3] = ioResult;
        *(u32 *)gTRKCPUState.regs.gpr[5] = position;
    } else {
        err = TRKSuppAccessFile(gTRKCPUState.regs.gpr[4], (u8 *)gTRKCPUState.regs.gpr[6], length = (u32 *)gTRKCPUState.regs.gpr[5], &ioResult, 1, command == 0xD1);
        if (ioResult == 0 && err != 0) {
            ioResult = 1;
        }
        gTRKCPUState.regs.gpr[3] = ioResult;
        if (command == 0xD1) {
            TRK_flush_cache(gTRKCPUState.regs.gpr[6], *length);
        }
    }
    gTRKCPUState.regs.pc += 4;
    return err;
}
#pragma use_lmw_stmw off

u32 TRKTargetGetPC(void) {
    return gTRKCPUState.regs.pc;
}

s32 TRKTargetStepOutOfRange(u32 rangeStart, u32 rangeEnd, s32 stepOver) {
    if (stepOver != 0) {
        return 0x703;
    }
    gTRKStepStatus_800DA0B4.type = 1;
    gTRKStepStatus_800DA0B4.rangeStart = rangeStart;
    gTRKStepStatus_800DA0B4.rangeEnd = rangeEnd;
    gTRKStepStatus_800DA0B4.active = 1;
    gTRKCPUState.stepMode |= 0x400;
    if (gTRKStepStatus_800DA0B4.type == 0 || gTRKStepStatus_800DA0B4.type == 0x10) {
        gTRKStepStatus_800DA0B4.count--;
    }
    gTRKState.field_98 = 0;
    return 0;
}

s32 TRKTargetSingleStep(u32 count, s32 stepOver) {
    if (stepOver != 0) {
        return 0x703;
    }
    gTRKStepStatus_800DA0B4.type = 0;
    gTRKStepStatus_800DA0B4.count = count;
    gTRKStepStatus_800DA0B4.active = 1;
    gTRKCPUState.stepMode |= 0x400;
    if (gTRKStepStatus_800DA0B4.type == 0 || gTRKStepStatus_800DA0B4.type == 0x10) {
        gTRKStepStatus_800DA0B4.count--;
    }
    gTRKState.field_98 = 0;
    return 0;
}

s32 TRKTargetAddExceptionInfo(void *b) {
    s32 err;
    u32 inst;
    u32 size;

    err = TRKAppendBuffer1_ui32(b, gTRKExceptionStatus_800DA0A4.field_0);
    if (err == 0) {
        size = 4;
        err = TRKTargetAccessMemory(&inst, gTRKExceptionStatus_800DA0A4.field_0, &size, 0, 1);
        if (err == 0 && size != 4) {
            err = 0x700;
        }
    }
    if (err == 0) {
        err = TRKAppendBuffer1_ui32(b, inst);
    }
    if (err == 0) {
        err = TRKAppendBuffer1_ui16(b, gTRKExceptionStatus_800DA0A4.field_8);
    }
    return err;
}

s32 TRKAppendBuffer1_ui64(void *buffer, u64 data);
s32 TRKTargetAddStopInfo(void *b) {
    s32 err;
    u32 inst;
    u32 size;
    s32 i;

    err = TRKAppendBuffer1_ui32(b, gTRKCPUState.regs.pc);
    if (err == 0) {
        size = 4;
        err = TRKTargetAccessMemory(&inst, gTRKCPUState.regs.pc, &size, 0, 1);
        if (err == 0 && size != 4) {
            err = 0x700;
        }
    }
    if (err == 0) {
        err = TRKAppendBuffer1_ui32(b, inst);
    }
    if (err == 0) {
        err = TRKAppendBuffer1_ui16(b, gTRKCPUState.field_2F8);
    }
    if (err == 0) {
        for (i = 0; i < 32; i++) {
            err = TRKAppendBuffer1_ui32(b, (u16)gTRKCPUState.regs.gpr[i]);
        }
        for (i = 0; i < 32; i++) {
            err = TRKAppendBuffer1_ui64(b, (u16)gTRKCPUState.fpr[i]);
        }
    }
    return err;
}

s32 TRKTargetInterrupt(TRKEvent *event) {
    s32 TRKDoNotifyStopped(u8 cmd);
    s32 result = 0;
    s32 done;

    switch (event->eventType) {
    case 3:
    case 4:
        if (gTRKStepStatus_800DA0B4.active) {
            gTRKCPUState.stepMode &= ~0x400;
            done = 1;
            if (gTRKStepStatus_800DA0B4.active && (u16)gTRKCPUState.field_2F8 == 0xD00) {
                switch (gTRKStepStatus_800DA0B4.type) {
                case 0:
                    if (gTRKStepStatus_800DA0B4.count > 0) {
                        done = 0;
                    }
                    break;
                case 1:
                    if (gTRKCPUState.regs.pc >= gTRKStepStatus_800DA0B4.rangeStart &&
                        gTRKCPUState.regs.pc <= gTRKStepStatus_800DA0B4.rangeEnd) {
                        done = 0;
                    }
                    break;
                }
            }
            if (done) {
                gTRKStepStatus_800DA0B4.active = 0;
            } else {
                gTRKStepStatus_800DA0B4.active = 1;
                gTRKCPUState.stepMode |= 0x400;
                if (gTRKStepStatus_800DA0B4.type == 0 || gTRKStepStatus_800DA0B4.type == 0x10) {
                    gTRKStepStatus_800DA0B4.count--;
                }
                gTRKState.field_98 = 0;
            }
        }
        if (gTRKStepStatus_800DA0B4.active == 0) {
            gTRKState.field_98 = 1;
            result = TRKDoNotifyStopped(0x90);
        }
        break;
    }
    return result;
}

void TRKPostInterruptEvent(void)
{
    void TRKConstructEvent(void *event, s32 eventType);
    s32 TRKPostEvent(void *event);
    s32 eventType;
    u32 inst;
    u32 size;
    u8 event[0x10];

    if (gTRKState.field_9C)
    {
        gTRKState.field_9C = 0;
    }
    else
    {
        switch (gTRKCPUState.field_2F8 & 0xFFFF)
        {
        case 0xD00:
        case 0x700:
            size = 4;
            TRKTargetAccessMemory(&inst, gTRKCPUState.regs.pc, &size, 0, 1);
            if (inst == 0x0FE00000)
            {
                eventType = 5;
            }
            else
            {
                eventType = 3;
            }
            break;
        default:
            eventType = 4;
            break;
        }
        TRKConstructEvent(event, eventType);
        TRKPostEvent(event);
    }
}

s32 TRKTargetSupportMask(u8 *mask)
{
    mask[0] = 0x7A;
    mask[1] = 0;
    mask[2] = 0x4F;
    mask[3] = 7;
    mask[4] = 0;
    mask[5] = 0;
    mask[6] = 0;
    mask[7] = 0;
    mask[8] = 0;
    mask[9] = 0;
    mask[0xA] = 0;
    mask[0xB] = 0;
    mask[0xC] = 0;
    mask[0xD] = 0;
    mask[0xE] = 0;
    mask[0xF] = 0;
    mask[0x10] = 1;
    mask[0x11] = 0;
    mask[0x12] = 3;
    mask[0x13] = 0;
    mask[0x14] = 0;
    mask[0x15] = 0;
    mask[0x16] = 0;
    mask[0x17] = 0;
    mask[0x18] = 0;
    mask[0x19] = 0;
    mask[0x1A] = 3;
    mask[0x1B] = 0;
    mask[0x1C] = 0;
    mask[0x1D] = 0;
    mask[0x1E] = 0;
    mask[0x1F] = 0x80;
    return 0;
}

s32 TRKTargetVersions(u8 *targetVersions)
{
    targetVersions[0] = 0;
    targetVersions[1] = 0xA;
    targetVersions[2] = 1;
    targetVersions[3] = 0xA;
    return 0;
}

#pragma use_lmw_stmw on
s32 TRKTargetAccessExtended2(u32 firstRegister, u32 lastRegister, void *b, u32 *registersLengthPtr, s32 read)
{
    TRKExceptionStatus savedException;
    u32 value_buf0[1];
    u32 value_buf[2];
    s32 error;
    u32 i;

    if (lastRegister > 0x1F) {
        return 0x701;
    }

    savedException = gTRKExceptionStatus_800DA0A4;
    gTRKExceptionStatus_800DA0A4.exceptionDetected = 0;

    TRKPPCAccessSPR(value_buf0, 920, 1);
    value_buf0[0] |= 0xA0000000;
    TRKPPCAccessSPR(value_buf0, 920, 0);
    value_buf0[0] = 0;
    TRKPPCAccessSPR(value_buf0, 912, 0);

    *registersLengthPtr = 0;
    error = 0;

    for (i = firstRegister; i <= lastRegister && error == 0; i++) {
        if (read) {
            TRKPPCAccessPairedSingleRegister(value_buf, i, read);
            error = TRKAppendBuffer1_ui64(b, *(u64 *)value_buf);
        } else {
            TRKReadBuffer1_ui64(b, (u64 *)value_buf);
            error = TRKPPCAccessPairedSingleRegister(value_buf, i, read);
        }
        *registersLengthPtr += sizeof(u64);
    }

    if (gTRKExceptionStatus_800DA0A4.exceptionDetected) {
        error = 0x702;
        *registersLengthPtr = 0;
    }
    gTRKExceptionStatus_800DA0A4 = savedException;
    return error;
}
#pragma use_lmw_stmw off

extern u8 gTRKRestoreFlags[9];
s32 TRKTargetAccessExtended1(u32 firstRegister, u32 lastRegister, void *b, u32 *registersLengthPtr, s32 read)
{
    s32 TRKAppendBuffer_ui32(void *buffer, u32 *data, s32 count);
    s32 TRKReadBuffer_ui32(void *buffer, u32 *data, s32 count);
    s32 error;
    s32 count;
    u32 *data;
    TRKExceptionStatus savedException;

    if (lastRegister > 0x60) {
        return 0x701;
    }

    savedException = gTRKExceptionStatus_800DA0A4;
    gTRKExceptionStatus_800DA0A4.exceptionDetected = 0;
    *registersLengthPtr = 0;

    if (firstRegister <= lastRegister) {
        data = (u32 *)&gTRKCPUState.extended1 + firstRegister;
        count = (lastRegister - firstRegister) + 1;
        *registersLengthPtr += count * sizeof(u32);

        if (read) {
            error = TRKAppendBuffer_ui32(b, data, count);
        } else {
            if (data <= &gTRKCPUState.extended1[17] && data + count - 1 >= &gTRKCPUState.extended1[16]) {
                gTRKRestoreFlags[0] = 1;
            }
            if (data <= &gTRKCPUState.field_278 && data + count - 1 >= &gTRKCPUState.field_278) {
                gTRKRestoreFlags[1] = 1;
            }
            error = TRKReadBuffer_ui32(b, data, count);
        }
    }

    if (gTRKExceptionStatus_800DA0A4.exceptionDetected) {
        *registersLengthPtr = 0;
        error = 0x702;
    }
    gTRKExceptionStatus_800DA0A4 = savedException;
    return error;
}

#pragma use_lmw_stmw on
s32 TRKTargetAccessFP(u32 firstRegister, u32 lastRegister, void *b, u32 *registersLengthPtr, s32 read)
{
    u32 __TRK_get_MSR(void);
    void __TRK_set_MSR(u32 msr);
    s32 TRKPPCAccessFPRegister(void *srcDestPtr, u32 fpr, s32 read);
    s32 TRKReadBuffer1_ui64(void *buffer, u64 *data);
    u64 temp;
    TRKExceptionStatus savedException;
    s32 error;
    u32 current;

    if (lastRegister > 0x21) {
        return 0x701;
    }

    savedException = gTRKExceptionStatus_800DA0A4;
    gTRKExceptionStatus_800DA0A4.exceptionDetected = 0;

    __TRK_set_MSR(__TRK_get_MSR() | 0x2000);

    *registersLengthPtr = 0;
    error = 0;

    for (current = firstRegister; current <= lastRegister && error == 0; current++, *registersLengthPtr += sizeof(u64)) {
        if (read) {
            TRKPPCAccessFPRegister(&temp, current, read);
            error = TRKAppendBuffer1_ui64(b, temp);
        } else {
            TRKReadBuffer1_ui64(b, &temp);
            error = TRKPPCAccessFPRegister(&temp, current, read);
        }
    }

    if (gTRKExceptionStatus_800DA0A4.exceptionDetected) {
        *registersLengthPtr = 0;
        error = 0x702;
    }
    gTRKExceptionStatus_800DA0A4 = savedException;
    return error;
}
#pragma use_lmw_stmw off
s32 TRKTargetAccessDefault(u32 firstRegister, u32 lastRegister, void *b, u32 *registersLengthPtr, s32 read)
{
    s32 TRKAppendBuffer_ui32(void *buffer, u32 *data, s32 count);
    s32 TRKReadBuffer_ui32(void *buffer, u32 *data, s32 count);
    s32 error;
    u32 count;
    u32 *data;
    TRKExceptionStatus savedException;

    if (lastRegister > 0x24) {
        return 0x701;
    }

    savedException = gTRKExceptionStatus_800DA0A4;
    gTRKExceptionStatus_800DA0A4.exceptionDetected = 0;

    data = gTRKCPUState.regs.gpr + firstRegister;
    count = (lastRegister - firstRegister) + 1;
    *registersLengthPtr = count * sizeof(u32);

    if (read) {
        error = TRKAppendBuffer_ui32(b, data, count);
    } else {
        error = TRKReadBuffer_ui32(b, data, count);
    }

    if (gTRKExceptionStatus_800DA0A4.exceptionDetected) {
        *registersLengthPtr = 0;
        error = 0x702;
    }
    gTRKExceptionStatus_800DA0A4 = savedException;
    return error;
}

s32 TRKValidMemory32(const void *addr, u32 length, s32 readWriteable);
#pragma use_lmw_stmw on
s32 TRKTargetAccessMemory(void *data, u32 start, u32 *length, s32 space, s32 read)
{
    u32 TRKTargetTranslate(u32 addr);
    u32 __TRK_get_MSR(void);
    void TRK_ppc_memcpy(void *dest, const void *src, u32 n, u32 fromMSR, u32 toMSR);
    void TRK_flush_cache(u32 start, u32 length);
    s32 error;
    u32 trans;
    u32 oldMSR;
    u32 newMSR;
    TRKExceptionStatus savedException;

    savedException = gTRKExceptionStatus_800DA0A4;
    gTRKExceptionStatus_800DA0A4.exceptionDetected = 0;

    trans = TRKTargetTranslate(start);
    error = TRKValidMemory32((void *)trans, *length, read == 0);
    if (error != 0) {
        *length = 0;
    } else {
        oldMSR = __TRK_get_MSR();
        newMSR = oldMSR | (gTRKCPUState.stepMode & 0x10);
        if (read) {
            TRK_ppc_memcpy(data, (void *)trans, *length, oldMSR, newMSR);
        } else {
            TRK_ppc_memcpy((void *)trans, data, *length, newMSR, oldMSR);
            TRK_flush_cache(trans, *length);
            if (start != trans) {
                TRK_flush_cache(start, *length);
            }
        }
    }

    if (gTRKExceptionStatus_800DA0A4.exceptionDetected) {
        *length = 0;
        error = 0x702;
    }
    gTRKExceptionStatus_800DA0A4 = savedException;
    return error;
}
#pragma use_lmw_stmw off

#pragma dont_inline on
s32 TRKValidMemory32(const void *addr, u32 length, s32 readWriteable) {
    s32 err = 0x700;
    const u8 *start;
    const u8 *end;
    s32 i;

    start = (const u8 *)addr;
    end = (const u8 *)addr + (length - 1);

    if (end < start) {
        return 0x700;
    }

    for (i = 0; i < (s32)(sizeof(gTRKMemMap) / sizeof(memRange)); i++) {
        if (start <= gTRKMemMap[i].end && end >= gTRKMemMap[i].start) {
            if (((u8)readWriteable == 0 && !gTRKMemMap[i].readable) ||
                ((u8)readWriteable == 1 && !gTRKMemMap[i].writeable)) {
                err = 0x700;
            } else {
                err = 0;
                if (start < gTRKMemMap[i].start) {
                    err = TRKValidMemory32(start, (u32)(gTRKMemMap[i].start - start), readWriteable);
                }
                if (err == 0 && end > gTRKMemMap[i].end) {
                    err = TRKValidMemory32(gTRKMemMap[i].end, (u32)(end - gTRKMemMap[i].end), readWriteable);
                }
            }
            break;
        }
    }
    return err;
}
#pragma dont_inline off
void fn_80094FB0(void) {
    int a = 0;
    a = 1 / a;
}
