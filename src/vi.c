#include "common.h"

volatile u16 __VIRegs[59] : 0xCC002000;

typedef struct VITiming {
    u8 equ;
    u16 acv;
    u16 prbOdd;
    u16 prbEven;
    u16 psbOdd;
    u16 psbEven;
    u8 unkC[0xC];
    u16 nhlines;
    u16 hlw;
} VITiming;

typedef struct VIPositionInfo {
    u16 DispPosX;
    u16 DispPosY;
    u16 DispSizeX;
    u16 DispSizeY;
    u16 AdjustedDispPosX;
    u16 AdjustedDispPosY;
    u16 AdjustedDispSizeY;
    u16 AdjustedPanPosY;
    u16 AdjustedPanSizeY;
    u16 FBSizeX;
    u16 FBSizeY;
    u16 PanPosX;
    u16 PanPosY;
    u16 PanSizeX;
    u16 PanSizeY;
    s32 FBMode;
    u32 nonInter;
    u32 tv;
    u8 wordPerLine;
    u8 std;
    u8 wpl;
    u32 bufAddr;
    u32 tfbb;
    u32 bfbb;
    u8 xof;
    s32 black;
    s32 threeD;
    u32 rbufAddr;
    u32 rtfbb;
    u32 rbfbb;
    VITiming *timing;
} VIPositionInfo;

static u16 regs[59];
static u16 shdwRegs[59];
static VIPositionInfo HorVer;

static inline void GetCurrentDisplayPosition(u32 *hct, u32 *vct) {
    u32 hcount;
    u32 vcount0;
    u32 vcount;

    vcount = __VIRegs[22] & 0x7FF;
    do {
        vcount0 = vcount;
        hcount = __VIRegs[23] & 0x7FF;
        vcount = __VIRegs[22] & 0x7FF;
    } while (vcount0 != vcount);
    *hct = hcount;
    *vct = vcount;
}

static inline u32 getCurrentHalfLine(void) {
    extern VITiming *CurrTiming;
    u32 hcount;
    u32 vcount;

    GetCurrentDisplayPosition(&hcount, &vcount);
    return ((vcount - 1) << 1) + ((hcount - 1) / CurrTiming->hlw);
}

static inline s32 cntlzd(u64 bit) {
    u32 hi;
    u32 lo;
    s32 value;

    hi = (u32)(bit >> 32);
    lo = (u32)(bit & 0xFFFFFFFF);
    value = __cntlzw(hi);
    if (value < 32) {
        return value;
    }
    return 32 + __cntlzw(lo);
}

static u8 timing[0x130] = {
    0x06, 0x00, 0x00, 0xF0, 0x00, 0x18, 0x00, 0x19, 0x00, 0x03, 0x00, 0x02, 0x0C, 0x0D, 0x0C, 0x0D, 0x02, 0x08, 0x02, 0x07,
    0x02, 0x08, 0x02, 0x07, 0x02, 0x0D, 0x01, 0xAD, 0x40, 0x47, 0x69, 0xA2, 0x01, 0x75, 0x7A, 0x00, 0x01, 0x9C,
    0x06, 0x00, 0x00, 0xF0, 0x00, 0x18, 0x00, 0x18, 0x00, 0x04, 0x00, 0x04, 0x0C, 0x0C, 0x0C, 0x0C, 0x02, 0x08, 0x02, 0x08,
    0x02, 0x08, 0x02, 0x08, 0x02, 0x0E, 0x01, 0xAD, 0x40, 0x47, 0x69, 0xA2, 0x01, 0x75, 0x7A, 0x00, 0x01, 0x9C,
    0x05, 0x00, 0x01, 0x1F, 0x00, 0x23, 0x00, 0x24, 0x00, 0x01, 0x00, 0x00, 0x0D, 0x0C, 0x0B, 0x0A, 0x02, 0x6B, 0x02, 0x6A,
    0x02, 0x69, 0x02, 0x6C, 0x02, 0x71, 0x01, 0xB0, 0x40, 0x4B, 0x6A, 0xAC, 0x01, 0x7C, 0x85, 0x00, 0x01, 0xA4,
    0x05, 0x00, 0x01, 0x1F, 0x00, 0x21, 0x00, 0x21, 0x00, 0x02, 0x00, 0x02, 0x0D, 0x0B, 0x0D, 0x0B, 0x02, 0x6B, 0x02, 0x6D,
    0x02, 0x6B, 0x02, 0x6D, 0x02, 0x70, 0x01, 0xB0, 0x40, 0x4B, 0x6A, 0xAC, 0x01, 0x7C, 0x85, 0x00, 0x01, 0xA4,
    0x06, 0x00, 0x00, 0xF0, 0x00, 0x18, 0x00, 0x19, 0x00, 0x03, 0x00, 0x02, 0x10, 0x0F, 0x0E, 0x0D, 0x02, 0x06, 0x02, 0x05,
    0x02, 0x04, 0x02, 0x07, 0x02, 0x0D, 0x01, 0xAD, 0x40, 0x4E, 0x70, 0xA2, 0x01, 0x75, 0x7A, 0x00, 0x01, 0x9C,
    0x06, 0x00, 0x00, 0xF0, 0x00, 0x18, 0x00, 0x18, 0x00, 0x04, 0x00, 0x04, 0x10, 0x0E, 0x10, 0x0E, 0x02, 0x06, 0x02, 0x08,
    0x02, 0x06, 0x02, 0x08, 0x02, 0x0E, 0x01, 0xAD, 0x40, 0x4E, 0x70, 0xA2, 0x01, 0x75, 0x7A, 0x00, 0x01, 0x9C,
    0x0C, 0x00, 0x01, 0xE0, 0x00, 0x30, 0x00, 0x30, 0x00, 0x06, 0x00, 0x06, 0x18, 0x18, 0x18, 0x18, 0x04, 0x0E, 0x04, 0x0E,
    0x04, 0x0E, 0x04, 0x0E, 0x04, 0x1A, 0x01, 0xAD, 0x40, 0x47, 0x69, 0xA2, 0x01, 0x75, 0x7A, 0x00, 0x01, 0x9C,
    0x0C, 0x00, 0x01, 0xE0, 0x00, 0x2C, 0x00, 0x2C, 0x00, 0x0A, 0x00, 0x0A, 0x18, 0x18, 0x18, 0x18, 0x04, 0x0E, 0x04, 0x0E,
    0x04, 0x0E, 0x04, 0x0E, 0x04, 0x1A, 0x01, 0xAD, 0x40, 0x47, 0x69, 0xA8, 0x01, 0x7B, 0x7A, 0x00, 0x01, 0x9C,
};

s32 VISetPostRetraceCallback(s32 cb) {
    extern s32 PostCB;
    s32 OSDisableInterrupts(void);
    void OSRestoreInterrupts(void);
    s32 old;

    old = PostCB;
    OSDisableInterrupts();
    PostCB = cb;
    OSRestoreInterrupts();
    return old;
}

u8 *getTiming(u32 mode) {
    switch (mode) {
    case 0:
        return &timing[0x00];
    case 1:
        return &timing[0x26];
    case 2:
        return &timing[0x4C];
    case 3:
        return &timing[0x72];
    case 20:
        return &timing[0x00];
    case 21:
        return &timing[0x26];
    case 4:
        return &timing[0x98];
    case 5:
        return &timing[0xBE];
    case 6:
        return &timing[0xE4];
    case 7:
        return &timing[0x10A];
    case 8:
        return &timing[0x4C];
    case 9:
        return &timing[0x72];
    }
    return NULL;
}

void VIWaitForRetrace(void) {
    extern u32 retraceCount;
    extern u8 retraceQueue[8];
    s32 OSDisableInterrupts(void);
    void OSRestoreInterrupts(s32 level);
    void OSSleepThread(void *queue);
    s32 enabled;
    u32 count;

    enabled = OSDisableInterrupts();
    count = retraceCount;
    do {
        OSSleepThread(retraceQueue);
    } while (count == retraceCount);
    OSRestoreInterrupts(enabled);
}

static void setFbbRegs(VIPositionInfo *hv, u32 *tfbb, u32 *bfbb, u32 *rtfbb, u32 *rbfbb);
void VISetNextFrameBuffer(void *fb) {
    extern s32 FBSet;
    s32 OSDisableInterrupts(void);
    void OSRestoreInterrupts(s32 level);
    s32 enabled;

    enabled = OSDisableInterrupts();
    HorVer.bufAddr = (u32)fb;
    FBSet = 1;
    setFbbRegs(&HorVer, &HorVer.tfbb, &HorVer.bfbb, &HorVer.rtfbb, &HorVer.rbfbb);
    OSRestoreInterrupts(enabled);
}

static void setVerticalRegs(u16 dispPosY, u16 dispSizeY, u8 equ, u16 acv, u16 prbOdd, u16 prbEven, u16 psbOdd, u16 psbEven, s32 black);
void VISetBlack(s32 black) {
    s32 OSDisableInterrupts(void);
    void OSRestoreInterrupts(s32 level);
    s32 enabled;
    VITiming *tm;

    enabled = OSDisableInterrupts();
    HorVer.black = black;
    tm = HorVer.timing;
    setVerticalRegs(HorVer.AdjustedDispPosY, HorVer.DispSizeY, tm->equ, tm->acv, tm->prbOdd, tm->prbEven, tm->psbOdd, tm->psbEven, HorVer.black);
    OSRestoreInterrupts(enabled);
}

static u32 getCurrentFieldEvenOdd(void) {
    extern VITiming *CurrTiming;
    u32 hct;
    u32 vct;

    GetCurrentDisplayPosition(&hct, &vct);
    return ((((vct - 1) << 1) + ((hct - 1) / CurrTiming->hlw)) < CurrTiming->nhlines) ? 1 : 0;
}

u32 VIGetNextField(void) {
    extern VITiming *CurrTiming;
    s32 OSDisableInterrupts(void);
    void OSRestoreInterrupts(s32 level);
    s32 enabled;
    u32 field;
    u32 hct;
    u32 vct;

    enabled = OSDisableInterrupts();
    GetCurrentDisplayPosition(&hct, &vct);
    field = ((((vct - 1) << 1) + ((hct - 1) / CurrTiming->hlw)) < CurrTiming->nhlines) ? 1 : 0;
    OSRestoreInterrupts(enabled);
    return (field ^ 1) ^ (HorVer.AdjustedDispPosY & 1);
}

u32 VIGetCurrentLine(void) {
    extern VITiming *CurrTiming;
    s32 OSDisableInterrupts(void);
    void OSRestoreInterrupts(s32 level);
    u32 halfLine;
    VITiming *tm;
    s32 enabled;
    u32 hct;
    u32 vct;

    tm = CurrTiming;
    enabled = OSDisableInterrupts();
    GetCurrentDisplayPosition(&hct, &vct);
    halfLine = ((vct - 1) << 1) + ((hct - 1) / CurrTiming->hlw);
    OSRestoreInterrupts(enabled);
    if (halfLine >= tm->nhlines) {
        halfLine -= tm->nhlines;
    }
    return halfLine >> 1;
}

u32 VIGetTvFormat(void) {
    extern u32 CurrTvMode;
    s32 OSDisableInterrupts(void);
    void OSRestoreInterrupts(s32 level);
    u32 format;
    s32 enabled;

    enabled = OSDisableInterrupts();
    switch (CurrTvMode) {
    case 0:
    case 3:
        format = 0;
        break;
    case 1:
    case 4:
        format = 1;
        break;
    case 5:
    case 2:
        format = CurrTvMode;
        break;
    }
    OSRestoreInterrupts(enabled);
    return format;
}

