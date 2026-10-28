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
u32 PauseFlag;
u32 PausingFlag;
u32 AutoFinishing;
s32 FatalErrorFlag;
u32 CurrCommand;
u32 Canceling;
u32 CancelCallback;
u32 ResumeFromHere;
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
