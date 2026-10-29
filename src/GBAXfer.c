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

void *OSGetCurrentContext(void);
void OSClearContext(void *context);
void OSSetCurrentContext(void *context);
void __OSReschedule(void);
s32 SITransfer(s32 chan, void *output, s32 outputBytes, void *input, s32 inputBytes, void *callback, s64 delay);
#pragma peephole on
void __GBASyncCallback(s32 chan, s32 ret)
{
	void OSWakeupThread(void *queue);

	OSWakeupThread(__GBA[chan].threadQueue);
}

#pragma peephole on
s32 __GBASync(s32 chan)
{
	s32 OSDisableInterrupts(void);
	s32 OSRestoreInterrupts(s32);
	void OSSleepThread(void *queue);
	GBAControl *gba = &__GBA[chan];
	s32 enabled;
	s32 ret;

	enabled = OSDisableInterrupts();
	while (gba->callback)
	{
		OSSleepThread(gba->threadQueue);
	}
	ret = gba->ret;
	OSRestoreInterrupts(enabled);
	return ret;
}
static void TypeAndStatusCallback(s32 chan, u32 type)
{
	GBAControl *gba;
	void *context;
	GBACallback callback;
	GBAProc proc;
	u8 exceptionContext[0x2C8];

	gba = &__GBA[chan];
	if (__GBAReset != 0)
	{
		return;
	}

	if ((type & 0xFF) != 0 || (type & 0xFFFF0000) != 0x40000)
	{
		gba->ret = 1;
	}
	else
	{
		if (SITransfer(chan, gba->output, gba->outputBytes, gba->input, gba->inputBytes, __GBAHandler, gba->delay))
		{
			return;
		}
		gba->ret = 2;
	}

	if (gba->proc)
	{
		proc = gba->proc;
		gba->proc = NULL;
		proc(chan);
	}

	if (gba->callback)
	{
		context = OSGetCurrentContext();
		OSClearContext(exceptionContext);
		OSSetCurrentContext(exceptionContext);
		callback = gba->callback;
		gba->callback = NULL;
		callback(chan, gba->ret);
		OSClearContext(exceptionContext);
		OSSetCurrentContext(context);
		__OSReschedule();
	}
}

s64 __GBASetDelay(s32 chan, s64 delay)
{
	GBAControl *gba = &__GBA[chan];
	s64 oldDelay = gba->delay;

	gba->delay = delay;

 return oldDelay;
}
