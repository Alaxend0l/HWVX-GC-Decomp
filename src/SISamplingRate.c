#include "common.h"

typedef struct XY {
    u16 line;
    u8 count;
} XY;

static u32 SamplingRate;

static XY XYNTSC[12] = {
    { 246, 2 }, { 15, 18 }, { 30, 9 }, { 44, 6 }, { 52, 5 }, { 65, 4 },
    { 87, 3 }, { 87, 3 }, { 87, 3 }, { 131, 2 }, { 131, 2 }, { 131, 2 },
};

static XY XYPAL[12] = {
    { 296, 2 }, { 15, 21 }, { 29, 11 }, { 45, 7 }, { 52, 6 }, { 63, 5 },
    { 78, 4 }, { 104, 3 }, { 104, 3 }, { 104, 3 }, { 104, 3 }, { 156, 2 },
};

s32 OSDisableInterrupts(void);
s32 OSRestoreInterrupts(s32 level);
u32 VIGetTvFormat(void);
void OSReport(const char *msg, ...);
void SISetXY(u32 x, u32 y);
