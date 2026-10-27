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

extern DVDCommand CommandList[3];
static s32 NextCommandNumber;

extern s32 Read(s32, s32, s32, s32);
extern s32 DVDLowSeek(s32, s32);
