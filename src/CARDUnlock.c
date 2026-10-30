#include "common.h"

typedef struct CARDControl {
    u8 unk0[0x14];
    s32 latency;     // 0x14
    u8 unk18[0x2C - 0x18];
    u32 scramble;    // 0x2C
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

static u32 bitrev(u32 data) {
    u32 wk;
    u32 i;
    u32 k = 0;
    u32 j = 1;

    wk = 0;
    for (i = 0; i < 32; i++) {
        if (i > 15) {
            if (i == 31) {
                wk |= (((data & (0x01 << 31)) >> 31) & 0x01);
            } else {
                wk |= ((data & (0x01 << i)) >> j);
                j += 2;
            }
        } else {
            wk |= ((data & (0x01 << i)) << (31 - i - k));
            k++;
        }
    }
    return wk;
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
        cmd[1] = (u8)((data >> 29) & 3);
        cmd[2] = (u8)(data >> 21);
        cmd[3] = (u8)((data >> 19) & 3);
        cmd[4] = (u8)((data >> 12) & 0x7F);
    } else {
        cmd[1] = (u8)(data >> 24);
        cmd[2] = (u8)(data >> 16);
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

static u32 exnor(u32 data, u32 lshift) {
    u32 wk;
    u32 work;
    u32 i;

    work = data;
    for (i = 0; i < lshift; i++) {
        wk = ~(work ^ (work << 7) ^ (work << 15) ^ (work << 23));
        work = (work << 1) | ((wk >> 30) & 0x00000002);
    }
    return work;
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

s32 EXIUnlock(s32 chan);
s32 EXIProbe(s32 chan);
void __CARDMountCallback(s32 chan, s32 result);
s32 __CARDReadStatus(s32 chan, u8 *status);
static void DoneCallback(void *_task) {
    u8 rbuf[64];
    u8 status;
    u8 *output;
    s32 chan;
    u8 *input;
    u32 data;
    u32 dummy;
    u32 rlen;
    u32 rshift;
    u32 wk, wk1, Ans2;
    CARDControl *card;
    void *task;
    s32 result;

    task = _task;
    for (chan = 0; chan < 2; ++chan) {
        card = &__CARDBlock[chan];
        if ((void *)card->task == task) {
            break;
        }
    }

    input = (u8 *)(((u32)card->workArea + 0x10 + 0x1F) & ~0x1F);
    output = input + 32;
    Ans2 = *(u32 *)output;
    dummy = DummyLen();
    rlen = dummy;
    data = ((Ans2 ^ card->scramble) & 0xFFFF0000);
    if (ReadArrayUnlock(chan, data, rbuf, rlen, 1) < 0) {
        EXIUnlock(chan);
        __CARDMountCallback(chan, -3);
        return;
    }

    rshift = (u32)((dummy + 4 + card->latency) * 8 + 1);
    wk = exnor(card->scramble, rshift);
    wk1 = ~(wk ^ (wk << 7) ^ (wk << 15) ^ (wk << 23));
    card->scramble = (wk | ((wk1 >> 31) & 0x00000001));

    dummy = DummyLen();
    rlen = dummy;
    data = (((Ans2 << 16) ^ card->scramble) & 0xFFFF0000);
    if (ReadArrayUnlock(chan, data, rbuf, rlen, 1) < 0) {
        EXIUnlock(chan);
        __CARDMountCallback(chan, -3);
        return;
    }

    result = __CARDReadStatus(chan, &status);
    if (!EXIProbe(chan)) {
        EXIUnlock(chan);
        __CARDMountCallback(chan, -3);
        return;
    }
    if (result == 0 && !(status & 0x40)) {
        EXIUnlock(chan);
        result = -5;
    }
    __CARDMountCallback(chan, result);
}

