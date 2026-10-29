#include "common.h"


void F23(u32 arg0) {
    typedef struct GBAControl {
        u8 _pad0[0xa8];
        u32 realLength;
        u8 _padAC[0x4c];
        u32 xferLength;
        u8 _padFC[4];
    } GBAControl;
    extern GBAControl __GBA[4];
    u32 DSPCheckMailToDSP(void);
    void DSPSendMailToDSP(u32 mail);
    void OSPanic(const char *file, s32 line, const char *msg, ...);

    GBAControl *gba;
    u32 idx;
    u64 pad;

    (void)&pad;
    gba = __GBA;
    if ((u32)&gba->realLength == arg0) {
        idx = 0;
    } else if ((u32)&(++gba)->realLength == arg0) {
        idx = 1;
    } else if ((u32)&(++gba)->realLength == arg0) {
        idx = 2;
    } else if ((u32)&(++gba)->realLength == arg0) {
        idx = 3;
    } else {
        OSPanic("GBAKey.c", 169, "GBA - unexpected dsp call");
        idx = -1;
    }

    gba = &__GBA[idx];
    DSPSendMailToDSP(0xABBA0000);
    while (DSPCheckMailToDSP()) {
    }
    DSPSendMailToDSP(gba->xferLength);
    while (DSPCheckMailToDSP()) {
    }
}



void F25(u32 arg0) {
    typedef struct GBAControl {
        u8 _pad0[0xa8];
        u32 realLength;
        u8 _padAC[0x4c];
        u32 xferLength;
        u8 _padFC[4];
    } GBAControl;
    extern GBAControl __GBA[4];
    void __GBAX01(s32 chan, s32 ret);
    void OSPanic(const char *file, s32 line, const char *msg, ...);

    GBAControl *gba;
    s32 idx;
    s32 chan;

    gba = __GBA;
    if ((u32)&gba->realLength == arg0) {
        idx = 0;
    } else if ((u32)&(++gba)->realLength == arg0) {
        idx = 1;
    } else if ((u32)&(++gba)->realLength == arg0) {
        idx = 2;
    } else if ((u32)&(++gba)->realLength == arg0) {
        idx = 3;
    } else {
        OSPanic("GBAKey.c", 169, "GBA - unexpected dsp call");
        idx = -1;
    }

    chan = idx;
    __GBAX01(chan, 0);
}

