#include "common.h"

/* The per-GBA-emu channel state; only the fields used here are named, the rest is left
   opaque (see src/GBAWrite.c for the full layout).  Kept 0x100 bytes wide so __GBA[chan]
   indexes with the original's stride. */
typedef struct GBAControl
{
	u8 output[5];
	u8 input[5];
	s32 outputBytes;
	s32 inputBytes;
	u8 *status;
	u8 _padding18[8];
	s32 ret;
	u8 _padding24[0xDC];
} GBAControl;

extern GBAControl __GBA[4];
extern s32 __GBAReset;

void ShortCommandProc(s32 chan)
{
	GBAControl *gba = &__GBA[chan];

	if (gba->ret != 0)
	{
		return;
	}
	if (gba->input[0] != 0 || gba->input[1] != 4)
	{
		gba->ret = 1;
		return;
	}
	*gba->status = gba->input[2] & 0x3A;
}

s32 OnReset(void)
{
	__GBAReset = 1;

 return 1;
}
