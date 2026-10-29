#include "common.h"

struct BootInfo {
    u32 field_0[10];
    u32 field_28;
    u32 field_2C;
    u32 field_30;
    u32 field_34;
    u8 pad_38[0xBC];
    u32 field_F4;
    u8 pad_F8[0x30E6 - 0xF8];
    u16 field_30E6;
    u8 field_30E8;
    u8 field_30E9;
};

struct BootInfo *BootInfo;
u32 *BI2DebugFlag;
u32 BI2DebugFlagHolder;
BOOL __OSIsGcam;
static BOOL AreWeInitialized;
void **OSExceptionTable;
u32 __OSSavedRegionEnd;
u32 __OSSavedRegionStart;
BOOL __OSInIPL;
s64 __OSStartTime;

static u32 DriveInfo[8] __attribute__((aligned(32)));
static u32 DriveBlock[12];

extern u32 __DVDLongFileNameFlag;
extern u32 __PADSpec;
extern u8 __ArenaLo[];
extern u8 __ArenaHi[];
extern u8 _stack_addr[];


void ClearArena(void)
{
  extern u32 __OSSavedRegionStart;
  extern u32 __OSSavedRegionEnd;
  extern u32 OSGetArenaHi(void);
  extern u32 OSGetArenaLo(void);
  extern s32 OSGetResetCode(void);
  extern void memset();
  u32 regionEnd;
  u32 regionStart;
  u32 *saved;
  if ((((u32) OSGetResetCode()) + 0x80000000u) != 0)
  {
    __OSSavedRegionStart = 0;
    __OSSavedRegionEnd = 0;
    memset((void *) OSGetArenaLo(), 0, OSGetArenaHi() - OSGetArenaLo());
    return;
  }
  saved = (u32 *) 0x812FDFEC;
  regionStart = saved[1];
  regionEnd = saved[0];
  __OSSavedRegionStart = regionStart;
  __OSSavedRegionEnd = regionEnd;
  if (regionStart == 0)
  {
    memset((void *) OSGetArenaLo(), 0, OSGetArenaHi() - OSGetArenaLo());
    return;
  }
  if (OSGetArenaLo() >= __OSSavedRegionStart)
  {
    return;
  }
  if (OSGetArenaHi() <= __OSSavedRegionStart)
  {
    memset((void *) OSGetArenaLo(), 0, OSGetArenaHi() - OSGetArenaLo());
    return;
  }
  memset((void *) OSGetArenaLo(), 0, __OSSavedRegionStart - OSGetArenaLo());
  if (OSGetArenaHi() <= __OSSavedRegionEnd)
  {
    return;
  }
  memset((void *) __OSSavedRegionEnd, 0, OSGetArenaHi() - __OSSavedRegionEnd);
}
