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
