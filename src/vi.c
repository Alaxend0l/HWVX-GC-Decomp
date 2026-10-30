#include "common.h"

volatile u16 __VIRegs[59] : 0xCC002000;

typedef struct VITiming {
    u8 equ;
    u16 acv;
    u16 prbOdd;
    u16 prbEven;
    u16 psbOdd;
    u16 psbEven;
    u8 bs1;
    u8 bs2;
    u8 bs3;
    u8 bs4;
    u16 be1;
    u16 be2;
    u16 be3;
    u16 be4;
    u16 nhlines;
    u16 hlw;
    u8 hsy;
    u8 hcs;
    u8 hce;
    u8 hbe640;
    u16 hbs640;
    u8 hbeCCIR656;
    u16 hbsCCIR656;
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

typedef struct OSContext {
    u32 gpr[32];
    u32 cr;
    u32 lr;
    u32 ctr;
    u32 xer;
    f64 fpr[32];
    u32 fpscr_pad;
    u32 fpscr;
    u32 srr0;
    u32 srr1;
    u16 mode;
    u16 state;
    u32 gqr[8];
    u32 psf_pad;
    f64 psf[32];
} OSContext;

static u32 getCurrentFieldEvenOdd(void);

static inline BOOL VISetRegs(void) {
    extern volatile u32 shdwChangeMode;
    extern volatile u64 shdwChanged;
    extern VITiming *CurrTiming;
    extern u32 CurrTvMode;
    s32 regIndex;

    if (!((shdwChangeMode == 1) && (getCurrentFieldEvenOdd() == 0))) {
        while (shdwChanged) {
            regIndex = cntlzd(shdwChanged);
            __VIRegs[regIndex] = shdwRegs[regIndex];
            shdwChanged &= ~((u64)1 << (63 - regIndex));
        }
        shdwChangeMode = 0;
        CurrTiming = HorVer.timing;
        CurrTvMode = HorVer.tv;
        return TRUE;
    }
    return FALSE;
}

static void __VIRetraceHandler(s16 interrupt, OSContext *context) {
    extern u32 retraceCount;
    extern volatile u32 flushFlag;
    extern void (*PreCB)(u32 retraceCount);
    extern void (*PostCB)(u32 retraceCount);
    extern u8 retraceQueue[8];
    void OSClearContext(OSContext *context);
    void OSSetCurrentContext(OSContext *context);
    void OSWakeupThread(void *queue);
    void SIRefreshSamplingRate(void);
    OSContext exceptionContext;
    u16 reg;
    u32 inter = 0;

    reg = __VIRegs[24];
    if (reg & 0x8000) {
        __VIRegs[24] = (u16)(reg & ~0x8000);
        inter |= 1;
    }
    reg = __VIRegs[26];
    if (reg & 0x8000) {
        __VIRegs[26] = (u16)(reg & ~0x8000);
        inter |= 2;
    }
    reg = __VIRegs[28];
    if (reg & 0x8000) {
        __VIRegs[28] = (u16)(reg & ~0x8000);
        inter |= 4;
    }
    reg = __VIRegs[30];
    if (reg & 0x8000) {
        __VIRegs[30] = (u16)(reg & ~0x8000);
        inter |= 8;
    }
    if ((inter & 4) || (inter & 8)) {
        OSSetCurrentContext(context);
        return;
    }
    retraceCount++;
    OSClearContext(&exceptionContext);
    OSSetCurrentContext(&exceptionContext);
    if (PreCB) {
        (*PreCB)(retraceCount);
    }
    if (flushFlag) {
        if (VISetRegs()) {
            flushFlag = 0;
            SIRefreshSamplingRate();
        }
    }
    if (PostCB) {
        OSClearContext(&exceptionContext);
        (*PostCB)(retraceCount);
    }
    OSWakeupThread(retraceQueue);
    OSClearContext(&exceptionContext);
    OSSetCurrentContext(context);
}

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

#pragma push
#pragma dont_inline on
void __VIInit(s32 mode) {
    VITiming *tm;
    u32 nonInter;
    volatile u32 a;
    u32 tv, tvForReg;
    u32 hct, vct;

    nonInter = mode & 2;
    tv = (u32)mode >> 2;
    *(u32 *)0x800000CC = tv;
    tm = (VITiming *)getTiming(mode);
    __VIRegs[1] = 2;
    for (a = 0; a < 1000; a++) {
    }
    __VIRegs[1] = 0;
    __VIRegs[3] = tm->hlw;
    __VIRegs[2] = (u16)(tm->hce | (tm->hcs << 8));
    __VIRegs[5] = (u16)(tm->hsy | (tm->hbe640 << 7));
    __VIRegs[4] = (u16)((tm->hbe640 >> 9) | (tm->hbs640 << 1));
    __VIRegs[0] = tm->equ;
    __VIRegs[7] = (u16)(tm->prbOdd + tm->acv * 2 - 2);
    __VIRegs[6] = (u16)(tm->psbOdd + 2);
    __VIRegs[9] = (u16)(tm->prbEven + tm->acv * 2 - 2);
    __VIRegs[8] = (u16)(tm->psbEven + 2);
    __VIRegs[11] = (u16)(tm->bs1 | (tm->be1 << 5));
    __VIRegs[10] = (u16)(tm->bs3 | (tm->be3 << 5));
    __VIRegs[13] = (u16)(tm->bs2 | (tm->be2 << 5));
    __VIRegs[12] = (u16)(tm->bs4 | (tm->be4 << 5));
    __VIRegs[36] = 0x2828;
    __VIRegs[27] = 1;
    __VIRegs[26] = 0x1001;
    hct = tm->hlw + 1;
    vct = tm->nhlines / 2 + 1;
    __VIRegs[25] = (u16)hct;
    __VIRegs[24] = (u16)(vct | 0x1000);
    if (mode != 2 && mode != 3) {
        tvForReg = tv;
        __VIRegs[1] = (u16)((nonInter << 2) | 1 | (tvForReg << 8));
        __VIRegs[54] = 0;
    } else {
        __VIRegs[1] = (u16)((tv << 8) | 5);
        __VIRegs[54] = 1;
    }
}
#pragma pop

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

static void setVerticalRegs(u16 dispPosY, u16 dispSizeY, u8 equ, u16 acv, u16 prbOdd, u16 prbEven, u16 psbOdd, u16 psbEven, s32 black) {
    extern volatile u64 changed;
    u16 actualPrbOdd;
    u16 actualPrbEven;
    u16 actualPsbOdd;
    u16 actualPsbEven;
    u16 actualAcv;
    u16 c;
    u16 d;

    if (equ >= 10) {
        c = 1;
        d = 2;
    } else {
        c = 2;
        d = 1;
    }

    if (dispPosY % 2 == 0) {
        actualPrbOdd = prbOdd + d * dispPosY;
        actualPsbOdd = psbOdd + d * ((c * acv - dispSizeY) - dispPosY);
        actualPrbEven = prbEven + d * dispPosY;
        actualPsbEven = psbEven + d * ((c * acv - dispSizeY) - dispPosY);
    } else {
        actualPrbOdd = prbEven + d * dispPosY;
        actualPsbOdd = psbEven + d * ((c * acv - dispSizeY) - dispPosY);
        actualPrbEven = prbOdd + d * dispPosY;
        actualPsbEven = psbOdd + d * ((c * acv - dispSizeY) - dispPosY);
    }

    actualAcv = dispSizeY / c;

    if (black) {
        actualPrbOdd += 2 * actualAcv - 2;
        actualPrbEven += 2 * actualAcv - 2;
        actualPsbOdd += 2;
        actualPsbEven += 2;
        actualAcv = 0;
    }

    regs[0] = (u16)(equ | (actualAcv << 4));
    changed |= (u64)1 << 63;
    regs[7] = actualPrbOdd;
    changed |= (u64)1 << 56;
    regs[6] = actualPsbOdd;
    changed |= (u64)1 << 57;
    regs[9] = actualPrbEven;
    changed |= (u64)1 << 54;
    regs[8] = actualPsbEven;
    changed |= (u64)1 << 55;
}

void VIFlush(void) {
    extern volatile u32 changeMode;
    extern volatile u64 changed;
    extern volatile u32 flushFlag;
    extern volatile u32 shdwChangeMode;
    extern volatile u64 shdwChanged;
    s32 OSDisableInterrupts(void);
    void OSRestoreInterrupts(s32 level);
    s32 enabled;
    s32 regIndex;

    enabled = OSDisableInterrupts();
    shdwChangeMode |= changeMode;
    changeMode = 0;
    shdwChanged |= changed;
    while (changed) {
        regIndex = cntlzd(changed);
        shdwRegs[regIndex] = regs[regIndex];
        changed &= ~((u64)1 << (63 - regIndex));
    }
    flushFlag = 1;
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

