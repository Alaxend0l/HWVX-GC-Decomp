#include "common.h"

typedef struct OSAlarm {
    void *(*handler)(void *, void *);
    u32 tag;
    s64 fire;
    struct OSAlarm *prev;
    struct OSAlarm *next;
    s64 fire2;
    s64 last;
} OSAlarm;

typedef struct DVDCommandBlock {
    s32 unk0;
    s32 unk4;
    u32 command;
    s32 state;
    s32 offset;
    s32 length;
    void *address;
    s32 currTransferSize;
    s32 transferredSize;
    s32 unk24;
    void (*callback)(s32 result, struct DVDCommandBlock *block);
    s32 unk2c;
} DVDCommandBlock;

u8 tmpBuffer[0x80];
DVDCommandBlock DummyCommandBlock;

OSAlarm ResetAlarm;

DVDCommandBlock *executing;
u32 currID;
u32 bootInfo;
s32 PauseFlag;
s32 PausingFlag;
s32 AutoFinishing;
s32 FatalErrorFlag;
volatile u32 CurrCommand;
volatile u32 Canceling;
void (*CancelCallback)(s32 result, DVDCommandBlock *block);
volatile u32 ResumeFromHere;
u32 CancelLastError;
u32 LastError;
volatile s32 NumInternalRetry;
u32 ResetRequired;
u32 FirstTimeInBootrom;
s32 DVDInitialized;
u32 LastState;

typedef struct DVDDIRegs {
    u32 command;
    u32 status;
} DVDDIRegs;

volatile u32 __DIRegs[16] : 0xCC006000;

extern u32 __DVDThreadQueue[2];
BOOL DVDCancelAsync(DVDCommandBlock *block, void (*callback)(s32, DVDCommandBlock *));
void cbForCancelSync(void);
void OSSleepThread(void *queue);

extern s32 autoInvalidation;
void DCInvalidateRange(void *addr, u32 nBytes);
BOOL OSDisableInterrupts(void);
BOOL OSRestoreInterrupts(BOOL level);
BOOL __DVDPushWaitingQueue(s32 prio, DVDCommandBlock *block);
void stateReady(void);

static inline BOOL CheckCancel(u32 resume) {
    DVDCommandBlock *finished;

    if (Canceling) {
        ResumeFromHere = resume;
        Canceling = FALSE;
        finished = executing;
        executing = &DummyCommandBlock;

        finished->state = 10;
        if (finished->callback) {
            (*finished->callback)(-3, finished);
        }
        if (CancelCallback) {
            (CancelCallback)(0, finished);
        }
        stateReady();
        return TRUE;
    }
    return FALSE;
}

static inline BOOL issueCommand(s32 prio, DVDCommandBlock *block) {
    BOOL level;
    BOOL result;

    if (autoInvalidation &&
        (block->command == 1 || block->command == 4 || block->command == 5 || block->command == 14)) {
        DCInvalidateRange(block->address, block->length);
    }

    level = OSDisableInterrupts();

    block->state = 2;
    result = __DVDPushWaitingQueue(prio, block);

    if ((executing == (DVDCommandBlock *)NULL) && (PauseFlag == FALSE)) {
        stateReady();
    }

    OSRestoreInterrupts(level);

    return result;
}

void DVDReset(void);
void stateGoToRetry(void);
void cbForUnrecoveredErrorRetry(u32 intType);

#define DVD_SET_DI_REG()    \
    {                        \
        __DIRegs[1] = __DIRegs[1];   \
    }


void cbForStateReadingFST(u32 intType);
void DVDInit(void) {
    void __DVDInterruptHandler(s16 interrupt, void *context);
    void *__OSSetInterruptHandler(s16 interrupt, void *handler);
    u32 __OSUnmaskInterrupts(u32 mask);
    void OSInitThreadQueue(void *queue);
    void OSInitAlarm(void);
    void __DVDFSInit(void);
    void __DVDClearWaitingQueue(void);
    void __DVDInitWA(void);
    void __fstLoad(void);
    void OSReport(const char *msg, ...);

    if (DVDInitialized) {
        return;
    }

    OSInitAlarm();
    DVDInitialized = TRUE;
    __DVDFSInit();
    __DVDClearWaitingQueue();
    __DVDInitWA();

    bootInfo = 0x80000000;
    currID = 0x80000000;

    __OSSetInterruptHandler(21, __DVDInterruptHandler);
    __OSUnmaskInterrupts(0x400);

    OSInitThreadQueue(__DVDThreadQueue);

    __DIRegs[0] = 0x2A;
    __DIRegs[1] = 0;

    if (*(u32 *)(bootInfo + 0x20) == 0xE5207C22) {
        OSReport("app booted via JTAG\n");
        OSReport("load fst\n");
        __fstLoad();
    } else if (*(u32 *)(bootInfo + 0x20) == 0x0D15EA5E) {
        OSReport("app booted from bootrom\n");
    } else {
        FirstTimeInBootrom = TRUE;
        OSReport("bootrom\n");
    }
}

void stateReadingFST(void) {
    LastState = (u32)stateReadingFST;
    DVDLowRead(*(void **)(bootInfo + 0x38), (((u32 *)tmpBuffer)[2] + 31) & ~31, ((u32 *)tmpBuffer)[1], cbForStateReadingFST);
}

void stateTimeout(void);
void stateReady(void);
void stateGettingError(void);
void cbForStateReadingFST(u32 intType) {
    DVDCommandBlock *finished;

    if (intType == 16) {
        executing->state = -1;
        stateTimeout();
        return;
    }

    if (intType & 1) {
        NumInternalRetry = 0;
        finished = executing;
        executing = &DummyCommandBlock;
        finished->state = 0;
        if (finished->callback) {
            (finished->callback)(0, finished);
        }
        stateReady();
    } else {
        stateGettingError();
    }
}

void cbForStateError(u32 intType) {
    DVDCommandBlock *finished;

    if (intType == 16) {
        executing->state = -1;
        stateTimeout();
        return;
    }

    FatalErrorFlag = 1;
    finished = executing;
    executing = &DummyCommandBlock;
    if (finished->callback) {
        (*finished->callback)(-1, finished);
    }
    if (Canceling) {
        Canceling = 0;
        if (CancelCallback) {
            (*CancelCallback)(0, finished);
        }
    }
    stateReady();
}

void stateTimeout(void) {
    void __DVDStoreErrorCode(u32 code);
    DVDCommandBlock *finished;

    __DVDStoreErrorCode(0x1234568);
    DVDReset();
    FatalErrorFlag = 1;
    finished = executing;
    executing = &DummyCommandBlock;
    if (finished->callback) {
        (*finished->callback)(-1, finished);
    }
    if (Canceling) {
        Canceling = 0;
        if (CancelCallback) {
            (*CancelCallback)(0, finished);
        }
    }
    stateReady();
}

void stateGettingError(void)
{
    void DVDLowRequestError(void (*cb)(u32));
    void cbForStateGettingError(u32 code);

    DVDLowRequestError(cbForStateGettingError);
}

s32 CategorizeError(u32 error) {
    if (error == 0x20400) {
        LastError = error;
        return 1;
    }

    error &= 0xFFFFFF;

    if ((error == 0x62800) || (error == 0x23A00) || (error == 0xB5A01)) {
        return 0;
    }

    ++NumInternalRetry;
    if (NumInternalRetry == 2) {
        if (error == LastError) {
            LastError = error;
            return 1;
        } else {
            LastError = error;
            return 2;
        }
    } else {
        LastError = error;

        if ((error == 0x31100) || (executing->command == 5)) {
            return 2;
        } else {
            return 3;
        }
    }
}

void cbForUnrecoveredError(u32 intType) {
    void __DVDStoreErrorCode(u32 code);
    void cbForStateError(u32 code);
    void DVDLowRequestError(void (*cb)(u32));

    if (intType == 16) {
        executing->state = -1;
        __DVDStoreErrorCode(0x1234568);
        DVDReset();
        cbForStateError(0);
        return;
    }

    if (intType & 1) {
        stateGoToRetry();
        return;
    }

    DVDLowRequestError(cbForUnrecoveredErrorRetry);
}

void cbForUnrecoveredErrorRetry(u32 intType) {
    void __DVDStoreErrorCode(u32 code);
    void cbForStateError(u32 code);
    void DVDLowStopMotor(void (*cb)(u32));

    if (intType == 16) {
        executing->state = -1;
        __DVDStoreErrorCode(0x1234568);
        DVDReset();
        cbForStateError(0);
        return;
    }

    executing->state = -1;

    if (intType & 2) {
        __DVDStoreErrorCode(0x1234567);
        DVDLowStopMotor(cbForStateError);
    } else {
        __DVDStoreErrorCode(__DIRegs[8]);
        DVDLowStopMotor(cbForStateError);
    }
}

void stateGoToRetry(void)
{
    void DVDLowStopMotor(void (*cb)(u32));
    void cbForStateGoToRetry(u32 code);

    DVDLowStopMotor(cbForStateGoToRetry);
}

void stateCheckID(void) {
    int memcmp(const void *a, const void *b, u32 n);
    void *memcpy(void *dst, const void *src, u32 n);
    void DVDLowStopMotor(void (*cb)(u32));
    void cbForStateCheckID1(u32 intType);
    void stateCheckID2(DVDCommandBlock *block);
    void stateCheckID3(DVDCommandBlock *block);

    switch (CurrCommand) {
    case 3:
        if (memcmp(tmpBuffer, (void *)executing->unk24, 0x1C)) {
            DVDLowStopMotor(cbForStateCheckID1);
            return;
        }
        memcpy((void *)currID, tmpBuffer, 0x20);
        executing->state = 1;
        DCInvalidateRange(tmpBuffer, 0x20);
        LastState = (u32)stateCheckID2;
        stateCheckID2(executing);
        break;
    default:
        if (memcmp(tmpBuffer, (void *)currID, 0x20)) {
            DVDLowStopMotor(cbForStateCheckID1);
            return;
        }
        LastState = (u32)stateCheckID3;
        stateCheckID3(executing);
        break;
    }
}

void cbForStateCheckID3(u32 intType);
void stateCheckID3(DVDCommandBlock *block) {
    DVDLowAudioBufferConfig(((u8 *)currID)[8], 10, cbForStateCheckID3);
}

void cbForStateCheckID2(u32 intType);
void stateCheckID2(DVDCommandBlock *block) {
    DVDLowRead(tmpBuffer, 0x20, 0x420, cbForStateCheckID2);
}

void stateMotorStopped(void);
void cbForStateCheckID1(u32 intType) {
    void __DVDStoreErrorCode(u32 code);
    void cbForStateError(u32 intType);
    void DVDLowStopMotor(void (*cb)(u32));

    if (intType == 16) {
        executing->state = -1;
        __DVDStoreErrorCode(0x1234568);
        DVDReset();
        cbForStateError(0);
        return;
    }

    if (intType & 2) {
        executing->state = -1;
        __DVDStoreErrorCode(0x1234567);
        DVDLowStopMotor(cbForStateError);
        return;
    }

    NumInternalRetry = 0;
    if (!CheckCancel(1)) {
        executing->state = 6;
        stateMotorStopped();
    }
}

void cbForStateCheckID2(u32 intType) {
    void __DVDStoreErrorCode(u32 code);
    void cbForStateError(u32 code);
    void cbForStateGettingError(u32 code);
    void DVDLowRequestError(void (*cb)(u32));

    if (intType == 16) {
        executing->state = -1;
        __DVDStoreErrorCode(0x1234568);
        DVDReset();
        cbForStateError(0);
        return;
    }

    if (intType & 1) {
        NumInternalRetry = 0;
        stateReadingFST();
    } else {
        DVDLowRequestError(cbForStateGettingError);
    }
}

void cbForStateCheckID3(u32 intType) {
    void __DVDStoreErrorCode(u32 code);
    void cbForStateError(u32 intType);
    void cbForStateGettingError(u32 code);
    void DVDLowRequestError(void (*cb)(u32));
    void stateBusy(DVDCommandBlock *block);
    DVDCommandBlock *finished;
    BOOL canceled;

    if (intType == 16) {
        executing->state = -1;
        __DVDStoreErrorCode(0x1234568);
        DVDReset();
        cbForStateError(0);
        return;
    }

    if (intType & 1) {
        NumInternalRetry = 0;
        canceled = FALSE;
        if (Canceling) {
            ResumeFromHere = 0;
            finished = executing;
            Canceling = FALSE;
            executing = &DummyCommandBlock;
            finished->state = 10;
            if (finished->callback) {
                (*finished->callback)(-3, finished);
            }
            if (CancelCallback) {
                (CancelCallback)(0, finished);
            }
            stateReady();
            canceled = TRUE;
        }
        if (!canceled) {
            executing->state = 1;
            stateBusy(executing);
        }
    } else {
        DVDLowRequestError(cbForStateGettingError);
    }
}

void DCInvalidateRange(void *addr, u32 nBytes);
void stateCoverClosed_CMD(DVDCommandBlock *block);
void AlarmHandler(void *alarm, void *context)
{
    DVDReset();
    DCInvalidateRange(tmpBuffer, 0x20);
    LastState = (u32)stateCoverClosed_CMD;
    stateCoverClosed_CMD(executing);
}

void stateCoverClosed(void) {
    void __DVDClearWaitingQueue(void);
    void OSCreateAlarm(OSAlarm *alarm);
    void OSSetAlarm(OSAlarm *alarm, s64 time, void (*handler)(void *, void *));
    DVDCommandBlock *finished;

    switch (CurrCommand) {
    case 4:
    case 5:
    case 13:
    case 15:
        __DVDClearWaitingQueue();
        finished = executing;
        executing = &DummyCommandBlock;
        if (finished->callback) {
            (*finished->callback)(-4, finished);
        }
        stateReady();
        break;
    default:
        DVDReset();
        OSCreateAlarm(&ResetAlarm);
        OSSetAlarm(&ResetAlarm, (*(u32 *)(0x80000000 + 0xf8) / 4) / 1000 * 1150, AlarmHandler);
        break;
    }
}

void cbForStateCoverClosed(u32 intType);
void stateCoverClosed_CMD(DVDCommandBlock *block) {
    DVDLowReadDiskID(tmpBuffer, cbForStateCoverClosed);
}

void cbForStateCoverClosed(u32 intType) {
    void __DVDStoreErrorCode(u32 code);
    void cbForStateError(u32 code);
    void cbForStateGettingError(u32 code);
    void DVDLowRequestError(void (*cb)(u32));
    void stateCheckID(void);

    if (intType == 16) {
        executing->state = -1;
        __DVDStoreErrorCode(0x1234568);
        DVDReset();
        cbForStateError(0);
        return;
    }

    if (intType & 1) {
        NumInternalRetry = 0;
        stateCheckID();
    } else {
        DVDLowRequestError(cbForStateGettingError);
    }
}

void stateMotorStopped(void)
{
    void DVDLowWaitCoverClose(void (*cb)(u32));
    void cbForStateMotorStopped(u32 code);

    DVDLowWaitCoverClose(cbForStateMotorStopped);
}

void cbForStateMotorStopped(u32 intType) {
    void __DVDClearWaitingQueue(void);
    void OSCreateAlarm(OSAlarm *alarm);
    void OSSetAlarm(OSAlarm *alarm, s64 time, void (*handler)(void *, void *));
    DVDCommandBlock *finished;

    __DIRegs[1] = 0;
    executing->state = 3;
    switch (CurrCommand) {
    case 4:
    case 5:
    case 13:
    case 15:
        __DVDClearWaitingQueue();
        finished = executing;
        executing = &DummyCommandBlock;
        if (finished->callback) {
            (*finished->callback)(-4, finished);
        }
        stateReady();
        break;
    default:
        DVDReset();
        OSCreateAlarm(&ResetAlarm);
        OSSetAlarm(&ResetAlarm, (*(u32 *)(0x80000000 + 0xf8) / 4) / 1000 * 1150, AlarmHandler);
        break;
    }
}

void stateReady(void)
{
    s32 __DVDCheckWaitingQueue(void);
    DVDCommandBlock *__DVDPopWaitingQueue(void);
    void __DVDClearWaitingQueue(void);
    void __DVDStoreErrorCode(u32 code);
    void stateBusy(DVDCommandBlock *block);
    void DVDLowWaitCoverClose(void (*cb)(u32));
    void DVDLowStopMotor(void (*cb)(u32));
    void DVDReset(void);
    void OSCreateAlarm(OSAlarm *alarm);
    void OSSetAlarm(OSAlarm *alarm, s64 time, void (*handler)(void *, void *));
    void cbForStateMotorStopped(u32 code);
    void cbForStateError(u32 code);
    void AlarmHandler(void *alarm, void *context);
    DVDCommandBlock *finished;

    if (!__DVDCheckWaitingQueue())
    {
        executing = NULL;
        return;
    }
    if (PauseFlag)
    {
        PausingFlag = 1;
        executing = NULL;
        return;
    }
    executing = __DVDPopWaitingQueue();
    if (FatalErrorFlag)
    {
        executing->state = -1;
        finished = executing;
        executing = &DummyCommandBlock;
        if (finished->callback)
        {
            (*finished->callback)(-1, finished);
        }
        stateReady();
        return;
    }
    CurrCommand = executing->command;
    if (ResumeFromHere != 0)
    {
        switch (ResumeFromHere)
        {
        case 1:
            executing->state = 6;
            DVDLowWaitCoverClose(cbForStateMotorStopped);
            break;
        case 2:
            executing->state = 0xb;
            DVDLowWaitCoverClose(cbForStateMotorStopped);
            break;
        case 3:
            executing->state = 4;
            DVDLowWaitCoverClose(cbForStateMotorStopped);
            break;
        case 4:
            executing->state = 7;
            DVDLowWaitCoverClose(cbForStateMotorStopped);
            break;
        case 5:
            executing->state = 5;
            DVDLowWaitCoverClose(cbForStateMotorStopped);
            break;
        case 6:
            executing->state = 3;
            switch (CurrCommand)
            {
            case 4:
            case 5:
            case 13:
            case 15:
                __DVDClearWaitingQueue();
                finished = executing;
                executing = &DummyCommandBlock;
                if (finished->callback)
                {
                    (*finished->callback)(-4, finished);
                }
                stateReady();
                break;
            default:
                DVDReset();
                OSCreateAlarm(&ResetAlarm);
                OSSetAlarm(&ResetAlarm,
                           (*(u32 *)(0x80000000 + 0xf8) / 4) / 1000 * 1150,
                           AlarmHandler);
                break;
            }
            break;
        case 7:
            executing->state = -1;
            __DVDStoreErrorCode(CancelLastError);
            DVDLowStopMotor(cbForStateError);
            break;
        }
        ResumeFromHere = 0;
        return;
    }
    executing->state = 1;
    stateBusy(executing);
}

void stateBusy(DVDCommandBlock *block)
{
    void cbForStateBusy(u32 intType);
    void DVDLowReadDiskID(void *diskID, void (*callback)(u32));
    void DVDLowRead(void *addr, u32 length, u32 offset, void (*callback)(u32));
    void DVDLowSeek(u32 offset, void (*callback)(u32));
    void DVDLowStopMotor(void (*callback)(u32));
    void DVDLowRequestAudioStatus(u32 subcmd, void (*callback)(u32));
    void DVDLowAudioStream(u32 subcmd, u32 length, u32 offset, void (*callback)(u32));
    void DVDLowAudioBufferConfig(u32 enable, u32 size, void (*callback)(u32));
    void DVDLowInquiry(void *info, void (*callback)(u32));

    LastState = (u32)stateBusy;
    switch (block->command)
    {
    case 5:
        DVD_SET_DI_REG();
        block->currTransferSize = 0x20;
        DVDLowReadDiskID(block->address, cbForStateBusy);
        break;
    case 1:
    case 4:
        DVD_SET_DI_REG();
        block->currTransferSize = ((u32)(block->length - block->transferredSize) > 0x80000) ? 0x80000 : (block->length - block->transferredSize);
        DVDLowRead((u8 *)block->address + block->transferredSize, block->currTransferSize, block->offset + block->transferredSize, cbForStateBusy);
        break;
    case 2:
        DVD_SET_DI_REG();
        DVDLowSeek(block->offset, cbForStateBusy);
        break;
    case 3:
        DVDLowStopMotor(cbForStateBusy);
        break;
    case 15:
        DVDLowStopMotor(cbForStateBusy);
        break;
    case 6:
        DVD_SET_DI_REG();
        if (AutoFinishing) { executing->currTransferSize = 0; DVDLowRequestAudioStatus(0, cbForStateBusy); }
        else { executing->currTransferSize = 1; DVDLowAudioStream(0, block->length, block->offset, cbForStateBusy); }
        break;
    case 7:
        DVD_SET_DI_REG();
        DVDLowAudioStream(0x10000, 0, 0, cbForStateBusy);
        break;
    case 8:
        DVD_SET_DI_REG();
        AutoFinishing = 1;
        DVDLowAudioStream(0, 0, 0, cbForStateBusy);
        break;
    case 9:
        DVD_SET_DI_REG();
        DVDLowRequestAudioStatus(0, cbForStateBusy);
        break;
    case 10:
        DVD_SET_DI_REG();
        DVDLowRequestAudioStatus(0x10000, cbForStateBusy);
        break;
    case 11:
        DVD_SET_DI_REG();
        DVDLowRequestAudioStatus(0x20000, cbForStateBusy);
        break;
    case 12:
        DVD_SET_DI_REG();
        DVDLowRequestAudioStatus(0x30000, cbForStateBusy);
        break;
    case 13:
        DVD_SET_DI_REG();
        DVDLowAudioBufferConfig(block->offset, block->length, cbForStateBusy);
        break;
    case 14:
        DVD_SET_DI_REG();
        block->currTransferSize = 0x20;
        DVDLowInquiry(block->address, cbForStateBusy);
        break;
    }
}

BOOL DVDReadAbsAsyncPrio(DVDCommandBlock *block, void *addr, s32 length, s32 offset, void (*callback)(s32, DVDCommandBlock *), s32 prio) {
    BOOL idle;

    block->command = 1;
    block->address = addr;
    block->length = length;
    block->offset = offset;
    block->transferredSize = 0;
    block->callback = callback;

    idle = issueCommand(prio, block);
    return idle;
}

BOOL DVDReadAbsAsyncForBS(DVDCommandBlock *block, void *addr, s32 length, s32 offset, void (*callback)(s32, DVDCommandBlock *)) {
    BOOL idle;

    block->command = 4;
    block->address = addr;
    block->length = length;
    block->offset = offset;
    block->transferredSize = 0;
    block->callback = callback;

    idle = issueCommand(2, block);
    return idle;
}

BOOL DVDReadDiskID(DVDCommandBlock *block, void *diskID, void (*callback)(s32, DVDCommandBlock *)) {
    BOOL idle;

    block->command = 5;
    block->address = diskID;
    block->length = 0x20;
    block->offset = 0;
    block->transferredSize = 0;
    block->callback = callback;

    idle = issueCommand(2, block);
    return idle;
}

BOOL DVDInquiryAsync(DVDCommandBlock *block, void *addr, void (*callback)(s32, DVDCommandBlock *)) {
    BOOL idle;

    block->command = 14;
    block->address = addr;
    block->length = 0x20;
    block->transferredSize = 0;
    block->callback = callback;

    idle = issueCommand(2, block);
    return idle;
}

void DVDLowReset(void);
void DVDReset(void) {
    DVDLowReset();
    __DIRegs[0] = 0x2A;
    __DIRegs[1] = __DIRegs[1];
    ResetRequired = 0;
    ResumeFromHere = 0;
}

BOOL OSDisableInterrupts(void);
BOOL OSRestoreInterrupts(BOOL level);
s32 DVDGetCommandBlockStatus(const DVDCommandBlock *block) {
    BOOL enabled;
    s32 retVal;

    enabled = OSDisableInterrupts();
    if (block->state == 3) {
        retVal = 1;
    } else {
        retVal = block->state;
    }
    OSRestoreInterrupts(enabled);
    return retVal;
}

s32 DVDGetDriveStatus(void) {
    BOOL enabled;
    s32 retVal;

    enabled = OSDisableInterrupts();

    if (FatalErrorFlag) {
        retVal = -1;
    } else if (PausingFlag) {
        retVal = 8;
    } else {
        if (executing == (DVDCommandBlock *)NULL) {
            retVal = 0;
        } else if (executing == &DummyCommandBlock) {
            retVal = 0;
        } else {
            retVal = DVDGetCommandBlockStatus(executing);
        }
    }

    OSRestoreInterrupts(enabled);
    return retVal;
}

s32 DVDSetAutoInvalidation(s32 arg0) {
    extern s32 autoInvalidation;
    s32 temp_r0;

    temp_r0 = autoInvalidation;
    autoInvalidation = arg0;
    return temp_r0;
}

s32 DVDCancel(volatile DVDCommandBlock *block) {
    BOOL result;
    s32 state;
    u32 command;
    BOOL enabled;

    result = DVDCancelAsync((DVDCommandBlock *)block, (void (*)(s32, DVDCommandBlock *))cbForCancelSync);
    if (result == FALSE) {
        return -1;
    }

    enabled = OSDisableInterrupts();
    for (;;) {
        state = block->state;
        if ((state == 0) || (state == -1) || (state == 10)) {
            break;
        }
        if (state == 3) {
            command = block->command;
            if ((command == 4) || (command == 5) || (command == 13) || (command == 15)) {
                break;
            }
        }
        OSSleepThread(__DVDThreadQueue);
    }
    OSRestoreInterrupts(enabled);
    return 0;
}

void cbForCancelSync(void) {
    void OSWakeupThread(void *queue);
    extern u32 __DVDThreadQueue[2];

    OSWakeupThread(__DVDThreadQueue);
}

s32 DVDGetCurrentDiskID(void) {
    return 0x80000000;
}

BOOL DVDCheckDisk(void) {
    BOOL enabled;
    s32 retVal;
    s32 state;
    u32 coverReg;

    enabled = OSDisableInterrupts();

    if (FatalErrorFlag) {
        state = -1;
    } else if (PausingFlag) {
        state = 8;
    } else {
        if (executing == (DVDCommandBlock *)NULL) {
            state = 0;
        } else if (executing == &DummyCommandBlock) {
            state = 0;
        } else {
            state = executing->state;
        }
    }

    switch (state) {
    case 1:
    case 9:
    case 10:
    case 2:
        retVal = TRUE;
        break;
    case -1:
    case 11:
    case 7:
    case 3:
    case 4:
    case 5:
    case 6:
        retVal = FALSE;
        break;
    case 0:
    case 8:
        coverReg = __DIRegs[1];
        if (((coverReg >> 2) & 1) || (coverReg & 1)) {
            retVal = FALSE;
        } else {
            retVal = TRUE;
        }
    }

    OSRestoreInterrupts(enabled);
    return retVal;
}

