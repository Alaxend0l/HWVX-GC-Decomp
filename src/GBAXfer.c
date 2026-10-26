#include "common.h"

/* The per-GBA-emu channel state; only the delay field is used here, so the rest is left
   opaque (see src/GBAWrite.c for the full layout).  Kept 0x100 bytes wide so __GBA[chan]
   indexes with the original's stride. */
typedef struct GBAControl
{
	u8 _padding00[0x30];
	s64 delay;
	u8 _padding38[0xC8];
} GBAControl;

extern GBAControl __GBA[4];

s64 __GBASetDelay(s32 chan, s64 delay)
{
	GBAControl *gba = &__GBA[chan];
	s64 oldDelay = gba->delay;

	gba->delay = delay;

 return oldDelay;
}
