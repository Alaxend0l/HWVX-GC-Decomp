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
    s32 command;
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
u32 PausingFlag;
s32 AutoFinishing;
s32 FatalErrorFlag;
volatile u32 CurrCommand;
u32 Canceling;
u32 CancelCallback;
volatile u32 ResumeFromHere;
u32 CancelLastError;
u32 LastError;
u32 NumInternalRetry;
u32 ResetRequired;
u32 FirstTimeInBootrom;
u32 DVDInitialized;
u32 LastState;

typedef struct DVDDIRegs {
    u32 command;
    u32 status;
} DVDDIRegs;

volatile u32 __DIRegs[16] : 0xCC006000;

void DVDReset(void);

#define DVD_SET_DI_REG()    \
    {                        \
        __DIRegs[1] = __DIRegs[1];   \
    }


void stateGettingError(void)
{
    void DVDLowRequestError(void (*cb)(u32));
    void cbForStateGettingError(u32 code);

    DVDLowRequestError(cbForStateGettingError);
}

void stateGoToRetry(void)
{
    void DVDLowStopMotor(void (*cb)(u32));
    void cbForStateGoToRetry(u32 code);

    DVDLowStopMotor(cbForStateGoToRetry);
}

void cbForStateCheckID3(u32 intType);
void stateCheckID3(DVDCommandBlock *block) {
    DVDLowAudioBufferConfig(((u8 *)currID)[8], 10, cbForStateCheckID3);
}

void cbForStateCheckID2(u32 intType);
void stateCheckID2(DVDCommandBlock *block) {
    DVDLowRead(tmpBuffer, 0x20, 0x420, cbForStateCheckID2);
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

void cbForStateCoverClosed(u32 intType);
void stateCoverClosed_CMD(DVDCommandBlock *block) {
    DVDLowReadDiskID(tmpBuffer, cbForStateCoverClosed);
}

void stateMotorStopped(void)
{
    void DVDLowWaitCoverClose(void (*cb)(u32));
    void cbForStateMotorStopped(u32 code);

    DVDLowWaitCoverClose(cbForStateMotorStopped);
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

s32 DVDSetAutoInvalidation(s32 arg0) {
    extern s32 autoInvalidation;
    s32 temp_r0;

    temp_r0 = autoInvalidation;
    autoInvalidation = arg0;
    return temp_r0;
}

void cbForCancelSync(void) {
    void OSWakeupThread(void *queue);
    extern u32 __DVDThreadQueue[2];

    OSWakeupThread(__DVDThreadQueue);
}

s32 DVDGetCurrentDiskID(void) {
    return 0x80000000;
}
