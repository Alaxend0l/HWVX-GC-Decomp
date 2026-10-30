#include "common.h"

// DVD command queue (see DVDLow): queued commands executed one at a time.
typedef struct DVDCommand
{
    s32 command;
    s32 readArg1;
    s32 readArg2;
    s32 seekLba;
    s32 seekSectors;
} DVDCommand;

static DVDCommand CommandList[3];
static volatile s32 NextCommandNumber;
static volatile s32 StopAtNextInt;
static volatile s32 Breaking;

typedef void (*DVDLowCallback)(u32 intType);
static DVDLowCallback Callback;
static volatile s32 WaitingCoverClose;
static u32 WorkAroundType;
static u32 WorkAroundSeekLocation;
static volatile s32 ResetOccurred;
static s64 LastResetEnd;

s64 __OSGetSystemTime(void);
volatile u32 __PIRegs[12] : 0xCC003000;

s32 OSDisableInterrupts(void);
s32 OSRestoreInterrupts(s32 level);

volatile u32 __DIRegs[16] : 0xCC006000;
u32 __OSBusClock : 0x800000F8;

typedef struct OSAlarm OSAlarm;
typedef void (*OSAlarmHandler)(OSAlarm *alarm, void *context);
struct OSAlarm {
    OSAlarmHandler handler;
    u32 tag;
    s64 fire;
    OSAlarm *prev;
    OSAlarm *next;
    s64 period;
    s64 start;
};

static OSAlarm AlarmForTimeout;

void OSCreateAlarm(OSAlarm *alarm);
void OSSetAlarm(OSAlarm *alarm, s64 tick, OSAlarmHandler handler);

static inline void SetTimeoutAlarm(OSAlarmHandler handler, s64 timeout)
{
    OSCreateAlarm(&AlarmForTimeout);
    OSSetAlarm(&AlarmForTimeout, timeout, handler);
}

static volatile s32 LastCommandWasRead;
static s64 LastReadIssued;
static u32 LastLength;

extern s32 Read(s32, s32, s32, s32);
extern s32 DVDLowSeek(s32, s32);

void __DVDLowSetWAType(u32 type, u32 location);
void OSInitAlarm(void);
void __DVDInitWA(void)
{
    NextCommandNumber = 0;
    CommandList[0].command = -1;
    __DVDLowSetWAType(0, 0);
    OSInitAlarm();
}

void AlarmHandler(void)
{
    s32 n;

    n = NextCommandNumber;
    if (CommandList[n].command == 1)
    {
        NextCommandNumber++;
        Read(CommandList[n].readArg1, CommandList[n].readArg2, CommandList[n].seekLba, CommandList[n].seekSectors);
    }
    else if (CommandList[n].command == 2)
    {
        NextCommandNumber++;
        DVDLowSeek(CommandList[n].seekLba, CommandList[n].seekSectors);
    }
}

void __OSMaskInterrupts(u32 mask);
void OSClearContext(void *context);
void OSSetCurrentContext(void *context);
void AlarmHandlerForTimeout(void *alarm, void *context)
{
    u8 tmpContext[0x2C8];
    DVDLowCallback callback;

    __OSMaskInterrupts(0x400);
    OSClearContext(tmpContext);
    OSSetCurrentContext(tmpContext);
    callback = Callback;
    Callback = NULL;
    if (callback) {
        callback(0x10);
    }
    OSClearContext(tmpContext);
    OSSetCurrentContext(context);
}

void SeekTwiceBeforeRead(s32 addr, s32 length, s32 offset, s32 callback)
{
    s32 offsetToSeek;
    s32 tmp;

    tmp = offset & ~0x7FFF;
    if (tmp == 0) {
        offsetToSeek = 0;
    } else {
        offsetToSeek = tmp + WorkAroundSeekLocation;
    }
    CommandList[0].command = 2;
    CommandList[0].seekLba = offsetToSeek;
    CommandList[0].seekSectors = callback;
    CommandList[1].command = 1;
    CommandList[1].readArg1 = addr;
    CommandList[1].readArg2 = length;
    CommandList[1].seekLba = offset;
    CommandList[1].seekSectors = callback;
    CommandList[2].command = -1;
    NextCommandNumber = 0;
    DVDLowSeek(offsetToSeek, callback);
}

s32 DVDLowSeek(s32 offset, s32 callback)
{
    Callback = (DVDLowCallback)callback;
    StopAtNextInt = 0;
    __DIRegs[2] = 0xAB000000;
    __DIRegs[3] = (u32)offset >> 2;
    __DIRegs[7] = 1;
    SetTimeoutAlarm((OSAlarmHandler)AlarmHandlerForTimeout, (__OSBusClock / 4) * 10);
    return 1;
}

s32 DVDLowWaitCoverClose(DVDLowCallback callback)
{
    Callback = callback;
    WaitingCoverClose = 1;
    StopAtNextInt = 0;
    __DIRegs[1] = 2;
    return 1;
}

s32 DVDLowReadDiskID(void *diskID, DVDLowCallback callback)
{
    Callback = callback;
    StopAtNextInt = 0;
    __DIRegs[2] = 0xA8000040;
    __DIRegs[3] = 0;
    __DIRegs[4] = 0x20;
    __DIRegs[5] = (u32)diskID;
    __DIRegs[6] = 0x20;
    __DIRegs[7] = 3;
    SetTimeoutAlarm((OSAlarmHandler)AlarmHandlerForTimeout, (__OSBusClock / 4) * 10);
    return 1;
}

s32 DVDLowStopMotor(DVDLowCallback callback)
{
    Callback = callback;
    StopAtNextInt = 0;
    __DIRegs[2] = 0xE3000000;
    __DIRegs[7] = 1;
    SetTimeoutAlarm((OSAlarmHandler)AlarmHandlerForTimeout, (__OSBusClock / 4) * 10);
    return 1;
}

s32 DVDLowRequestError(DVDLowCallback callback)
{
    Callback = callback;
    StopAtNextInt = 0;
    __DIRegs[2] = 0xE0000000;
    __DIRegs[7] = 1;
    SetTimeoutAlarm((OSAlarmHandler)AlarmHandlerForTimeout, (__OSBusClock / 4) * 10);
    return 1;
}

s32 DVDLowInquiry(void *info, DVDLowCallback callback)
{
    Callback = callback;
    StopAtNextInt = 0;
    __DIRegs[2] = 0x12000000;
    __DIRegs[4] = 0x20;
    __DIRegs[5] = (u32)info;
    __DIRegs[6] = 0x20;
    __DIRegs[7] = 3;
    SetTimeoutAlarm((OSAlarmHandler)AlarmHandlerForTimeout, (__OSBusClock / 4) * 10);
    return 1;
}

s32 DVDLowAudioStream(u32 subcmd, u32 length, u32 offset, DVDLowCallback callback)
{
    Callback = callback;
    StopAtNextInt = 0;
    __DIRegs[2] = subcmd | 0xE1000000;
    __DIRegs[3] = offset >> 2;
    __DIRegs[4] = length;
    __DIRegs[7] = 1;
    SetTimeoutAlarm((OSAlarmHandler)AlarmHandlerForTimeout, (__OSBusClock / 4) * 10);
    return 1;
}

s32 DVDLowRequestAudioStatus(u32 subcmd, DVDLowCallback callback)
{
    Callback = callback;
    StopAtNextInt = 0;
    __DIRegs[2] = subcmd | 0xE2000000;
    __DIRegs[7] = 1;
    SetTimeoutAlarm((OSAlarmHandler)AlarmHandlerForTimeout, (__OSBusClock / 4) * 10);
    return 1;
}

s32 DVDLowAudioBufferConfig(s32 enable, u32 size, DVDLowCallback callback)
{
    u32 bufSize;

    Callback = callback;
    StopAtNextInt = 0;
    bufSize = enable ? 0x10000 : 0;
    __DIRegs[2] = size | (0xE4000000 | bufSize);
    __DIRegs[7] = 1;
    SetTimeoutAlarm((OSAlarmHandler)AlarmHandlerForTimeout, (__OSBusClock / 4) * 10);
    return 1;
}

#pragma push
#pragma use_lmw_stmw on
void DVDLowReset(void)
{
    u32 reg;
    s64 resetStart;

    __DIRegs[1] = 2;
    reg = __PIRegs[9];
    __PIRegs[9] = (reg & ~4) | 1;
    resetStart = __OSGetSystemTime();
    while ((__OSGetSystemTime() - resetStart) < ((12 * ((__OSBusClock / 4) / 125000)) / 8))
        ;
    __PIRegs[9] = reg | 5;
    ResetOccurred = 1;
    LastResetEnd = __OSGetSystemTime();
}
#pragma pop

s32 DVDLowBreak(void)
{
    StopAtNextInt = 1;
    Breaking = 1;
    return 1;
}

DVDLowCallback DVDLowClearCallback(void)
{
    DVDLowCallback old;

    __DIRegs[1] = 0;
    old = Callback;
    Callback = NULL;
    return old;
}

void __DVDLowSetWAType(u32 type, u32 seekLoc)
{
    s32 enabled;

    enabled = OSDisableInterrupts();
    WorkAroundType = type;
    WorkAroundSeekLocation = seekLoc;
    OSRestoreInterrupts(enabled);
}

