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

volatile u32 __DIRegs[16] : 0xCC006000;

extern s32 Read(s32, s32, s32, s32);
extern s32 DVDLowSeek(s32, s32);

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

