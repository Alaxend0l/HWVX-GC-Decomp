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
s32 NextCommandNumber;

extern s32 Read(s32, s32, s32, s32);
extern s32 DVDLowSeek(s32, s32);

void AlarmHandler(void)
{
    DVDCommand *cmd;

    cmd = &CommandList[NextCommandNumber];
    if (cmd->command == 1)
    {
        NextCommandNumber++;
        Read(cmd->readArg1, cmd->readArg2, cmd->seekLba, cmd->seekSectors);
    }
    else if (cmd->command == 2)
    {
        NextCommandNumber++;
        DVDLowSeek(cmd->seekLba, cmd->seekSectors);
    }
}
