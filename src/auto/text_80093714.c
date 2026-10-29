// Code of main that had no source file yet: decompdecomp gave it this one.
// split_file divides it into the game's real files (file_status shows its range).
#include "common.h"

typedef struct TRKCPURegs {
    u32 gpr[32];
    u32 pc;
} TRKCPURegs;

typedef struct TRKCPUState {
    TRKCPURegs regs;
    u8 pad_84[0x1F8 - 0x84];
    u32 stepMode; /* 0x1F8: MSR, 0x400 = single-step trace */
    u8 pad_1FC[0x2F8 - 0x1FC];
    u32 field_2F8;
} TRKCPUState;

typedef struct TRKState {
    u8 pad_0[0x98];
    s32 field_98;
    s32 field_9C;
} TRKState;

typedef struct TRKStepStatus {
    u32 active;
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
    u8 pad_A[6];
} TRKExceptionStatus;

extern TRKCPUState gTRKCPUState;
extern TRKState gTRKState;
extern TRKStepStatus gTRKStepStatus_800DA0B4;
extern TRKExceptionStatus gTRKExceptionStatus_800DA0A4;

s32 TRKTargetAccessMemory(void *data, u32 start, u32 *length, s32 space, s32 read);
s32 TRKAppendBuffer1_ui32(void *buffer, u32 data);
s32 TRKAppendBuffer1_ui16(void *buffer, u16 data);

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

void fn_80094FB0(void) {
    M2C_ERROR(/* unknown instruction: twui $r0, 0x0 */);
}
