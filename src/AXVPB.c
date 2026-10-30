#include "common.h"

typedef struct AXPB {
    u16 nextHi;          /* 0x00 */
    u16 nextLo;          /* 0x02 */
    u16 currHi;          /* 0x04 */
    u16 currLo;          /* 0x06 */
    u16 srcSelect;       /* 0x08 */
    u16 coefSelect;      /* 0x0A */
    u16 mixerCtrl;       /* 0x0C */
    u16 state;           /* 0x0E */
    u8 unk10[0x26];
    u16 itdFlag;         /* 0x36 */
    u16 itdHi;           /* 0x38 */
    u16 itdLo;           /* 0x3A */
    u8 unk3C[8];
    u16 unk44;
    u16 unk46;
    u16 unk48;
    u16 unk4A;
    u16 unk4C;
    u16 updHi;           /* 0x4E */
    u16 updLo;           /* 0x50 */
    u8 unk52[0xA6 - 0x52];
    u16 ratioHi;         /* 0xA6 */
    u8 unkA8[0xEC - 0xA8];
} AXPB;

typedef struct AXVPB {
    u8 unk0[0xC];
    u32 priority;        /* 0x0C */
    u8 unk10[8];
    u32 index;           /* 0x18 */
    u32 sync;
    u32 depop;
    u32 updateMS;
    u32 updateCounter;
    u32 updateTotal;
    u16 *updateWrite;
    u16 updateData[128];
    void *itdBuffer;
    AXPB pb;             /* 0x138 */
} AXVPB;

static AXPB __AXPB[64];
static u32 __AXITD[64][16];
static u16 __AXUpdates[64][128];
static AXVPB __AXVPB[64];
static u32 __AXMaxDspCycles;
static u32 __AXRecDspCycles;
s32 __AXNumVoices;

static u32 __AXSrcCycles[5] = { 0xDF8, 0xF78, 0x14B8, 0x19F8, 0x19F8 };
static u32 __AXMainMixCycles[16] = {
    0x000, 0x2F8, 0x2F8, 0x5BE, 0x2F8, 0x5F0, 0x5F0, 0x8B6,
    0x000, 0x4F1, 0x4F1, 0x9A6, 0x4F1, 0x9E2, 0x9E2, 0xE97,
};
static u32 __AXAuxMixCycles[32] = {
    0x000, 0x2F8, 0x2F8, 0x5BE, 0x000, 0x4F1, 0x4F1, 0x9A6,
    0x2F8, 0x5F0, 0x5F0, 0x8B6, 0x2F8, 0x7E9, 0x7E9, 0xC9E,
    0x000, 0x2F8, 0x2F8, 0x5BE, 0x000, 0x4F1, 0x4F1, 0x9A6,
    0x4F1, 0x7E9, 0x7E9, 0xAAF, 0x4F1, 0x9E2, 0x9E2, 0xE97,
};

s32 __AXGetNumVoices(void) {
    return __AXNumVoices;
}

void DCInvalidateRange(void *addr, u32 nBytes);
void __AXServiceVPB(AXVPB *pvpb);
u32 __AXGetCommandListCycles(void);
AXVPB *__AXGetStackHead(u32 priority);
void __AXDepopVoice(AXPB *ppb);
void __AXPushCallbackStack(AXVPB *pvpb);
void __AXSyncPBs(u32 lessDspCycles) {
    u32 cycles;
    u32 i;
    AXVPB *pvpb;

    __AXNumVoices = 0;
    DCInvalidateRange(__AXPB, sizeof(__AXPB));
    DCInvalidateRange(__AXITD, sizeof(__AXITD));
    cycles = __AXGetCommandListCycles() + 0xAA10 + lessDspCycles;
    for (i = 31; i; i--) {
        pvpb = __AXGetStackHead(i);
        while (pvpb) {
            if (pvpb->depop) {
                __AXDepopVoice(&__AXPB[pvpb->index]);
            }
            if (pvpb->pb.state == 1 || pvpb->updateCounter) {
                if (pvpb->pb.srcSelect != 2) {
                    cycles += __AXSrcCycles[pvpb->pb.ratioHi];
                }
                cycles += __AXMainMixCycles[pvpb->pb.mixerCtrl & 0xF] + __AXAuxMixCycles[(pvpb->pb.mixerCtrl >> 4) & 0x1F] + __AXAuxMixCycles[(pvpb->pb.mixerCtrl >> 9) & 0x1F] + 0x8C;
                if (__AXMaxDspCycles > cycles) {
                    __AXServiceVPB(pvpb);
                } else {
                    AXPB *ppb = &__AXPB[pvpb->index];

                    if (ppb->state == 1) {
                        __AXDepopVoice(&__AXPB[pvpb->index]);
                    }
                    ppb->unk4C = 0;
                    ppb->unk4A = 0;
                    ppb->unk48 = 0;
                    ppb->unk46 = 0;
                    ppb->unk44 = 0;
                    ppb->state = 0;
                    pvpb->pb.state = 0;
                    __AXPushCallbackStack(pvpb);
                }
            } else {
                __AXServiceVPB(pvpb);
            }
            pvpb->sync = 0;
            pvpb->depop = 0;
            pvpb->updateCounter = 0;
            pvpb->updateMS = 0;
            pvpb->updateWrite = pvpb->updateData;
            pvpb = *(AXVPB **)pvpb;
        }
    }
    __AXRecDspCycles = cycles;
    pvpb = __AXGetStackHead(0);
    while (pvpb) {
        if (pvpb->depop) {
            __AXDepopVoice(&__AXPB[pvpb->index]);
        }
        pvpb->depop = 0;
        __AXPB[pvpb->index].unk4C = 0;
        __AXPB[pvpb->index].unk4A = 0;
        __AXPB[pvpb->index].unk48 = 0;
        __AXPB[pvpb->index].unk46 = 0;
        __AXPB[pvpb->index].unk44 = 0;
        __AXPB[pvpb->index].state = 0;
        pvpb = *(AXVPB **)pvpb;
    }
    DCFlushRange(__AXPB, sizeof(__AXPB));
    DCFlushRange(__AXITD, sizeof(__AXITD));
    DCFlushRange(__AXUpdates, sizeof(__AXUpdates));
}

void *__AXGetPBs(void) {
    return __AXPB;
}

void __AXSetPBDefault(AXVPB *p)
{
    p->pb.state = 0;
    p->pb.itdFlag = 0;
    p->sync = 0xA4;
    p->updateCounter = 0;
    p->updateMS = 0;
    p->updateWrite = p->updateData;
    p->pb.unk4C = 0;
    p->pb.unk4A = 0;
    p->pb.unk48 = 0;
    p->pb.unk46 = 0;
    p->pb.unk44 = 0;
}

void __AXVPBInit(void) {
    void __AXPushFreeStack(AXVPB *p);
    void DCFlushRange(void *addr, u32 nBytes);
    u32 i;
    AXPB *ppb;
    u32 *pitd;
    u16 *pupd;
    AXVPB *pvpb;

    __AXMaxDspCycles = *(u32 *)0x800000F8 / 400;
    __AXRecDspCycles = 0;
    for (i = 0; i < sizeof(__AXPB); i += 4) {
        *(u32 *)((u8 *)__AXPB + i) = 0;
    }
    for (i = 0; i < sizeof(__AXITD); i += 4) {
        *(u32 *)((u8 *)__AXITD + i) = 0;
    }
    for (i = 0; i < sizeof(__AXVPB); i += 4) {
        *(u32 *)((u8 *)__AXVPB + i) = 0;
    }
    for (i = 0; i < 64; i++) {
        ppb = &__AXPB[i];
        pitd = __AXITD[i];
        pupd = __AXUpdates[i];
        pvpb = &__AXVPB[i];
        pvpb->index = i;
        pvpb->updateWrite = pvpb->updateData;
        pvpb->itdBuffer = pitd;
        __AXSetPBDefault(pvpb);
        if (i == 63) {
            ppb->nextLo = 0;
            ppb->nextHi = 0;
            pvpb->pb.nextLo = 0;
            pvpb->pb.nextHi = 0;
        } else {
            pvpb->pb.nextHi = (u16)((u32)(ppb + 1) >> 16);
            pvpb->pb.nextLo = (u16)((u32)(ppb + 1));
            ppb->nextHi = (u16)((u32)(ppb + 1) >> 16);
            ppb->nextLo = (u16)((u32)(ppb + 1));
        }
        pvpb->pb.currHi = (u16)((u32)ppb >> 16);
        pvpb->pb.currLo = (u16)((u32)ppb);
        ppb->currHi = (u16)((u32)ppb >> 16);
        ppb->currLo = (u16)((u32)ppb);
        pvpb->pb.itdHi = (u16)((u32)pitd >> 16);
        pvpb->pb.itdLo = (u16)((u32)pitd);
        ppb->itdHi = (u16)((u32)pitd >> 16);
        ppb->itdLo = (u16)((u32)pitd);
        pvpb->pb.updHi = (u16)((u32)pupd >> 16);
        pvpb->pb.updLo = (u16)((u32)pupd);
        ppb->updHi = (u16)((u32)pupd >> 16);
        ppb->updLo = (u16)((u32)pupd);
        pvpb->priority = 1;
        __AXPushFreeStack(pvpb);
    }
    DCFlushRange(__AXPB, sizeof(__AXPB));
}

void __AXVPBQuit(void) {
}

