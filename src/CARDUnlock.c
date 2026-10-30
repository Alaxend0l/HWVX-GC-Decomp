#include "common.h"

typedef struct DSPTaskInfo {
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
    void (*init_cb)(void *task);
    void (*res_cb)(void *task);
    void (*done_cb)(void *task);
    void (*req_cb)(void *task);
    struct DSPTaskInfo *next;
    struct DSPTaskInfo *prev;
    s64 t_context;
    s64 t_task;
} DSPTaskInfo;

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

static u8 CardData[] __attribute__((aligned(32))) = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x21, 0x02, 0xFF, 0x00, 0x21,
    0x13, 0x06, 0x12, 0x03, 0x12, 0x04, 0x13, 0x05, 0x00, 0x92, 0x00, 0xFF, 0x00, 0x88, 0xFF, 0xFF,
    0x00, 0x89, 0xFF, 0xFF, 0x00, 0x8A, 0xFF, 0xFF, 0x00, 0x8B, 0xFF, 0xFF, 0x8F, 0x00, 0x02, 0xBF,
    0x00, 0x88, 0x16, 0xFC, 0xDC, 0xD1, 0x16, 0xFD, 0x00, 0x00, 0x16, 0xFB, 0x00, 0x01, 0x02, 0xBF,
    0x00, 0x8E, 0x25, 0xFF, 0x03, 0x80, 0xFF, 0x00, 0x02, 0x94, 0x00, 0x27, 0x02, 0xBF, 0x00, 0x8E,
    0x1F, 0xDF, 0x24, 0xFF, 0x02, 0x40, 0x0F, 0xFF, 0x00, 0x98, 0x04, 0x00, 0x00, 0x9A, 0x00, 0x10,
    0x00, 0x99, 0x00, 0x00, 0x8E, 0x00, 0x02, 0xBF, 0x00, 0x94, 0x02, 0xBF, 0x86, 0x44, 0x02, 0xBF,
    0x00, 0x88, 0x16, 0xFC, 0xDC, 0xD1, 0x16, 0xFD, 0x00, 0x03, 0x16, 0xFB, 0x00, 0x01, 0x8F, 0x00,
    0x02, 0xBF, 0x00, 0x8E, 0x03, 0x80, 0xCD, 0xD1, 0x02, 0x94, 0x00, 0x48, 0x27, 0xFF, 0x03, 0x80,
    0x00, 0x01, 0x02, 0x95, 0x00, 0x5A, 0x03, 0x80, 0x00, 0x02, 0x02, 0x95, 0x80, 0x00, 0x02, 0x9F,
    0x00, 0x48, 0x00, 0x21, 0x8E, 0x00, 0x02, 0xBF, 0x00, 0x8E, 0x25, 0xFF, 0x02, 0xBF, 0x00, 0x8E,
    0x25, 0xFF, 0x02, 0xBF, 0x00, 0x8E, 0x25, 0xFF, 0x02, 0xBF, 0x00, 0x8E, 0x00, 0xC5, 0xFF, 0xFF,
    0x03, 0x40, 0x0F, 0xFF, 0x1C, 0x9F, 0x02, 0xBF, 0x00, 0x8E, 0x00, 0xC7, 0xFF, 0xFF, 0x02, 0xBF,
    0x00, 0x8E, 0x00, 0xC6, 0xFF, 0xFF, 0x02, 0xBF, 0x00, 0x8E, 0x00, 0xC0, 0xFF, 0xFF, 0x02, 0xBF,
    0x00, 0x8E, 0x20, 0xFF, 0x03, 0x40, 0x0F, 0xFF, 0x1F, 0x5F, 0x02, 0xBF, 0x00, 0x8E, 0x21, 0xFF,
    0x02, 0xBF, 0x00, 0x8E, 0x23, 0xFF, 0x12, 0x05, 0x12, 0x06, 0x02, 0x9F, 0x80, 0xB5, 0x00, 0x21,
    0x27, 0xFC, 0x03, 0xC0, 0x80, 0x00, 0x02, 0x9D, 0x00, 0x88, 0x02, 0xDF, 0x27, 0xFE, 0x03, 0xC0,
    0x80, 0x00, 0x02, 0x9C, 0x00, 0x8E, 0x02, 0xDF, 0x2E, 0xCE, 0x2C, 0xCF, 0x00, 0xF8, 0xFF, 0xCD,
    0x00, 0xF9, 0xFF, 0xC9, 0x00, 0xFA, 0xFF, 0xCB, 0x26, 0xC9, 0x02, 0xC0, 0x00, 0x04, 0x02, 0x9D,
    0x00, 0x9C, 0x02, 0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

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

static u32 GetInitVal(void) {
    u32 tmp;
    u32 tick;

    tick = OSGetTick();
    CARDSrand(tick);
    tmp = 0x7FEC8000;
    tmp |= CARDRand();
    tmp &= 0xFFFFF000;
    return tmp;
}

static u32 exnor_1st(u32 data, u32 rshift) {
    u32 wk;
    u32 work;
    u32 i;

    work = data;
    for (i = 0; i < rshift; i++) {
        wk = ~(work ^ (work >> 7) ^ (work >> 15) ^ (work >> 23));
        work = (work >> 1) | ((wk << 30) & 0x40000000);
    }
    return work;
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

void DCFlushRange(void *addr, u32 len);
void DCInvalidateRange(void *addr, u32 len);
void DSPAddTask(DSPTaskInfo *task);
static void InitCallback(void *task);
static void DoneCallback(void *task);
#pragma push
#pragma use_lmw_stmw on
s32 __CARDUnlock(s32 chan, u8 flashID[12]) {
    u32 init_val;
    u32 data;
    s32 dummy;
    s32 rlen;
    u32 rshift;
    u8 fsts;
    u32 wk, wk1;
    u32 Ans1 = 0;
    u32 Ans2 = 0;
    u32 *dp;
    u8 rbuf[64];
    u32 para1A = 0;
    u32 para1B = 0;
    u32 para2A = 0;
    u32 para2B = 0;
    CARDControl *card;
    DSPTaskInfo *task;
    u32 *param;
    u8 *input;
    u8 *output;

    card = &__CARDBlock[chan];
    task = (DSPTaskInfo *)card->task;
    param = (u32 *)card->workArea;
    input = (u8 *)((u8 *)param + 0x10);
    input = (u8 *)(((u32)input + 0x1F) & ~0x1F);
    output = input + 32;

    fsts = 0;
    init_val = GetInitVal();

    dummy = DummyLen();
    rlen = dummy;
    if (ReadArrayUnlock(chan, init_val, rbuf, rlen, 0) < 0) {
        return -3;
    }

    rshift = (u32)(dummy * 8 + 1);
    wk = exnor_1st(init_val, rshift);
    wk1 = ~(wk ^ (wk >> 7) ^ (wk >> 15) ^ (wk >> 23));
    card->scramble = (wk | ((wk1 << 31) & 0x80000000));
    card->scramble = bitrev(card->scramble);
    dummy = DummyLen();
    rlen = 20 + dummy;
    data = 0;
    if (ReadArrayUnlock(chan, data, rbuf, rlen, 1) < 0) {
        return -3;
    }

    dp = (u32 *)rbuf;
    para1A = *dp++;
    para1B = *dp++;
    Ans1 = *dp++;
    para2A = *dp++;
    para2B = *dp++;
    para1A = (para1A ^ card->scramble);
    rshift = 32;
    wk = exnor(card->scramble, rshift);
    wk1 = ~(wk ^ (wk << 7) ^ (wk << 15) ^ (wk << 23));
    card->scramble = (wk | ((wk1 >> 31) & 0x00000001));
    para1B = (para1B ^ card->scramble);
    rshift = 32;
    wk = exnor(card->scramble, rshift);
    wk1 = ~(wk ^ (wk << 7) ^ (wk << 15) ^ (wk << 23));
    card->scramble = (wk | ((wk1 >> 31) & 0x00000001));
    Ans1 ^= card->scramble;
    rshift = 32;
    wk = exnor(card->scramble, rshift);
    wk1 = ~(wk ^ (wk << 7) ^ (wk << 15) ^ (wk << 23));
    card->scramble = (wk | ((wk1 >> 31) & 0x00000001));
    para2A = (para2A ^ card->scramble);
    rshift = 32;
    wk = exnor(card->scramble, rshift);
    wk1 = ~(wk ^ (wk << 7) ^ (wk << 15) ^ (wk << 23));
    card->scramble = (wk | ((wk1 >> 31) & 0x00000001));
    para2B = (para2B ^ card->scramble);
    rshift = (u32)(dummy * 8);
    wk = exnor(card->scramble, rshift);
    wk1 = ~(wk ^ (wk << 7) ^ (wk << 15) ^ (wk << 23));
    card->scramble = (wk | ((wk1 >> 31) & 0x00000001));
    rshift = 32 + 1;
    wk = exnor(card->scramble, rshift);
    wk1 = ~(wk ^ (wk << 7) ^ (wk << 15) ^ (wk << 23));
    card->scramble = (wk | ((wk1 >> 31) & 0x00000001));

    *(u32 *)&input[0] = para2A;
    *(u32 *)&input[4] = para2B;

    param[0] = (u32)input;
    param[1] = 8;
    param[3] = (u32)output;
    param[2] = 0;

    DCFlushRange(input, 8);
    DCInvalidateRange(output, 4);
    DCFlushRange(param, 0x10);

    task->priority = 255;
    task->iram_mmem_addr = (void *)((u32)CardData - 0x80000000);
    task->iram_length = 0x160;
    task->iram_addr = 0;
    task->dsp_init_vector = 0x10;
    task->init_cb = InitCallback;
    task->res_cb = NULL;
    task->done_cb = DoneCallback;
    task->req_cb = NULL;
    DSPAddTask(task);

    dp = (u32 *)flashID;
    *dp++ = para1A;
    *dp++ = para1B;
    *dp = Ans1;

    return 0;
}
#pragma pop

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

