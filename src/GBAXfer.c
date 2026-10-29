#include "common.h"

/* The per-GBA-emu channel state (see src/GBAWrite.c for the full layout).  Kept 0x100
   bytes wide so __GBA[chan] indexes with the original's stride. */
typedef void (*GBACallback)(s32 chan, s32 ret);
typedef void (*GBAProc)(s32 chan);

typedef struct GBAControl
{
	u8 output[5];
	u8 input[5];
	s32 outputBytes;
	s32 inputBytes;
	u8 *status;
	u8 *ptr;
	GBACallback callback;
	s32 ret;
	void *threadQueue[2];
	u8 _padding2C[4];
	s64 delay;
	GBAProc proc;
	u8 _padding3C[0xC4];
} GBAControl;

extern GBAControl __GBA[4];
extern s32 __GBAReset;

void __GBAHandler(s32 chan, u32 error, void *context);

s64 __GBASetDelay(s32 chan, s64 delay)
{
	GBAControl *gba = &__GBA[chan];
	s64 oldDelay = gba->delay;

	gba->delay = delay;

 return oldDelay;
}
