#include "common.h"

typedef struct CARDControl {
    u8 unk0[0x30];
    u8 task[0x50];   // 0x30 DSPTaskInfo
    void *workArea;  // 0x80
    u8 unk84[0x110 - 0x84];
} CARDControl;

extern CARDControl __CARDBlock[2];

void DSPSendMailToDSP(u32 mail);
u32 DSPCheckMailToDSP(void);
u32 OSGetTick(void);

static u32 next = 1;

static int CARDRand(void) {
    next = next * 1103515245 + 12345;
    return (int)((u32)(next / 65536) % 32768);
}

static void CARDSrand(unsigned int seed) {
    next = seed;
}

static u32 DummyLen(void) {
    u32 tick;
    u32 wk;
    int tmp;
    u32 max;

    wk = 1;
    max = 0;
    tick = OSGetTick();
    CARDSrand(tick);
    tmp = CARDRand();
    tmp &= 0x1f;
    tmp += 1;
    while ((tmp < 4) && (max < 10)) {
        tick = OSGetTick();
        tick <<= wk;
        wk++;
        if (wk > 16) {
            wk = 1;
        }
        CARDSrand(tick);
        tmp = CARDRand();
        tmp &= 0x1f;
        tmp += 1;
        max++;
    }
    if (tmp < 4) {
        tmp = 4;
    }
    return tmp;
}

static void InitCallback(void *task) {
    s32 chan;
    CARDControl *card;
    void *data;

    for (chan = 0; chan < 2; ++chan) {
        card = &__CARDBlock[chan];
        if ((void *)card->task == task) {
            break;
        }
    }
    data = card->workArea;
    DSPSendMailToDSP(0xFF000000);
    while (DSPCheckMailToDSP()) {
    }
    DSPSendMailToDSP((u32)data);
    while (DSPCheckMailToDSP()) {
    }
}
