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

s32 GBAResetAsync(s32 chan, u8 *status, GBACallback callback);

s32 GBAGetStatusAsync(s32 chan, u8 *status, GBACallback callback);

static void F25(s32 chan, s32 ret);
static void F27(s32 chan, s32 ret);

static volatile u8 D54[] = {
	0x18, 0xFC, 0xC0, 0x80, 0x7F, 0x40, 0x3F, 0x01, 0x00, 0x2F, 0x2F, 0x20, 0x43, 0x6F, 0x64, 0x65,
	0x64, 0x20, 0x62, 0x79, 0x20, 0x4B, 0x61, 0x77, 0x61, 0x73, 0x65, 0x64, 0x6F, 0x00, 0x00, 0x02,
	0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0xAC, 0xC4, 0xF8, 0x08, 0x10, 0xBF, 0x18,
};

static void F23(s32 chan, s32 ret) {
    GBAControl *gba;
    GBABootInfo *bootInfo;
    GBACallback callback;

    gba = &__GBA[chan];
    bootInfo = &gba->bootInfo;
    if (ret != 0 || (ret = GBAResetAsync(chan, bootInfo->status, F25)) != 0) {
        bootInfo->begin = 0;
        if (bootInfo->callback) {
            callback = bootInfo->callback;
            bootInfo->callback = NULL;
            callback(chan, ret);
        }
    }
    gba->ret = ret;
}

static void F25(s32 chan, s32 ret) {
    GBAControl *gba;
    GBABootInfo *bootInfo;
    GBACallback callback;

    gba = &__GBA[chan];
    bootInfo = &gba->bootInfo;
    if (ret == 0 && *bootInfo->status != D54[0x25]) {
        ret = 3;
    }
    if (ret != 0 || (ret = GBAGetStatusAsync(chan, bootInfo->status, F27)) != 0) {
        bootInfo->begin = 0;
        if (bootInfo->callback) {
            callback = bootInfo->callback;
            bootInfo->callback = NULL;
            callback(chan, ret);
        }
    }
    gba->ret = ret;
}

static void F29(s32 chan, s32 ret) {
    GBAControl *gba;
    GBABootInfo *bootInfo;
    GBACallback callback;

    gba = &__GBA[chan];
    bootInfo = &gba->bootInfo;
    if (ret == 0) {
        __GBAX02(chan, bootInfo->readbuf);
    } else {
        bootInfo->begin = 0;
        if (bootInfo->callback) {
            callback = bootInfo->callback;
            bootInfo->callback = NULL;
            callback(chan, ret);
        }
    }
    gba->ret = ret;
}

static void F39(s32 chan, s32 ret) {
    GBAControl *gba;
    GBABootInfo *bootInfo;
    GBACallback callback;

    gba = &__GBA[chan];
    bootInfo = &gba->bootInfo;
    if (ret == 0) {
        *bootInfo->status = 0;
    }
    bootInfo->begin = 0;
    if (bootInfo->callback) {
        callback = bootInfo->callback;
        bootInfo->callback = NULL;
        callback(chan, ret);
    }
    gba->ret = ret;
}

