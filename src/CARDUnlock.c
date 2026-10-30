#include "common.h"

typedef struct CARDControl {
    u8 unk0[0x14];
    s32 latency;     // 0x14
    u8 unk18[0x30 - 0x18];
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

s32 EXISelect(s32 chan, u32 dev, u32 freq);
s32 EXIImmEx(s32 chan, void *buf, s32 len, u32 mode);
s32 EXIDeselect(s32 chan);
void *memset(void *dst, int c, u32 n);

#pragma push
#pragma use_lmw_stmw on
static s32 ReadArrayUnlock(s32 chan, u32 data, void *rbuf, s32 rlen, s32 mode) {
    CARDControl *card;
    BOOL err;
    u8 cmd[5];

    card = &__CARDBlock[chan];
    if (!EXISelect(chan, 0, 4)) {
        return -3;
    }

    data &= 0xFFFFF000;
    memset(cmd, 0, 5);
    cmd[0] = 0x52;
    if (mode == 0) {
        cmd[1] = (u8)((data & 0x60000000) >> 29);
        cmd[2] = (u8)((data & 0x1FE00000) >> 21);
        cmd[3] = (u8)((data & 0x00180000) >> 19);
        cmd[4] = (u8)((data & 0x0007F000) >> 12);
    } else {
        cmd[1] = (u8)(data >> 24);
        cmd[2] = (u8)((data & 0x00FF0000) >> 16);
    }

    err = FALSE;
    err |= !EXIImmEx(chan, cmd, 5, 1);
    err |= !EXIImmEx(chan, (u8 *)card->workArea + 0x200, card->latency, 1);
    err |= !EXIImmEx(chan, rbuf, rlen, 0);
    err |= !EXIDeselect(chan);
    if (err) {
        return -3;
    }
    return 0;
}
#pragma pop

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
