#include "common.h"

typedef union PPCWGPipe {
    u8 u8;
    u16 u16;
    u32 u32;
    s32 s32;
    f32 f32;
} PPCWGPipe;

volatile PPCWGPipe GXWGFifo : 0xCC008000;

typedef struct GXData {
    u16 vNumNot;
    u16 bpSentNot;
    u8 pad4[0x4E4 - 0x4];
    s32 perf0;
    s32 perf1;
    u32 perfSel;
} GXData;

extern GXData *gx;
extern volatile u16 *__cpReg;

void GXClearGPMetric(void) {
    __cpReg[2] = 4;
}
