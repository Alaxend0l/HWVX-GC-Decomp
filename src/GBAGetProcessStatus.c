#include "common.h"

typedef void (*GBACallback)(s32 chan, s32 ret);

typedef struct GBASecParam
{
	u8 readbuf[4];
	s32 paletteColor;
	s32 paletteSpeed;
	s32 length;
	u32 *out;
	u8 _padding0[12];
	u32 keyA;
	s32 keyB;
	u8 _padding1[24];
} GBASecParam;

typedef struct GBADSPTaskInfo
{
	u32 state;
	u32 priority;
	u32 flags;
	void *iram_mmem_addr;
	u32 iram_length;
	u32 iram_addr;
	void *dram_mmem_addr;
	u32 dram_length;
	u32 dram_addr;
	u16 dsp_init_vector;
	u16 dsp_resume_vector;
	void *init_cb;
	void *res_cb;
	void *done_cb;
	void *req_cb;
	u8 _padding38[0x50 - 0x38];
} GBADSPTaskInfo;

typedef struct GBABootInfo
{
	s32 paletteColor;
	s32 paletteSpeed;
	u8 *programp;
	s32 length;
	u8 *status;
	GBACallback callback;
	u8 readbuf[4];
	u8 writebuf[4];
	s32 i;
	u32 start;
	s64 begin;
	s32 firstXfer;
	s32 curOffset;
	u32 crc;
	u32 dummyWord[7];
	u32 keyA;
	u32 keyB;
	u32 initialCode;
	s32 realLength;
} GBABootInfo;

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
	void *proc;
	u8 _padding3C[4];
	GBABootInfo bootInfo;
	GBADSPTaskInfo task;
	GBASecParam *param;
	u8 _paddingFC[4];
} GBAControl;

extern GBAControl __GBA[4];

s32 OSDisableInterrupts(void);
s32 OSRestoreInterrupts(s32 level);
s64 OSGetTime(void);

u32 __OSBusClock : 0x800000F8;
