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
    void (*callback)(s32 result);
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
u32 AutoFinishing;
s32 FatalErrorFlag;
u32 CurrCommand;
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
        executing = (DVDCommandBlock *)(tmpBuffer + 0x80);
        if (finished->callback)
        {
            (*finished->callback)(-1);
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
                executing = (DVDCommandBlock *)(tmpBuffer + 0x80);
                if (finished->callback)
                {
                    (*finished->callback)(-4);
                }
                stateReady();
                break;
            default:
                DVDReset();
                OSCreateAlarm((OSAlarm *)(tmpBuffer + 0xb0));
                OSSetAlarm((OSAlarm *)(tmpBuffer + 0xb0),
                           (*(u32 *)(0x80000000 + 0xf8) / 4) / 1000 * 1150,
                           AlarmHandler);
                break;
            }
            break;
        case 7:
            executing->state = -1;
            __DVDStoreErrorCode(CancelLastError);
            DVDLowStopMotor(cbForStateError);
            ResumeFromHere = 0;
            return;
        }
    }
    else
    {
        executing->state = 1;
        stateBusy(executing);
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
