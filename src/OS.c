#include "common.h"


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
