#include "common.h"

typedef struct OSAlarm {
    void (*handler)(struct OSAlarm *, void *);
    u32 tag;
    s64 fire;
    struct OSAlarm *prev;
    struct OSAlarm *next;
    s64 period;
    s64 start;
} OSAlarm;

volatile u32 __SIRegs[64] : 0xCC006400;

static SIControl Si = {-1, 0, 0, NULL, NULL};
static SIPacket Packet[4];
static OSAlarm Alarm[4];
static u32 Type[4] = {8, 8, 8, 8};
static s64 TypeTime[4];
static s64 XferTime[4];
static void *TypeCallback[4][4];
static void *RDSTHandler[4];
static BOOL InputBufferValid[4];
static u32 InputBuffer[4][2];
static volatile u32 InputBufferVcount[4];

u32 SIGetType(s32 chan);
BOOL __SITransfer(s32 chan, void *output, u32 outputBytes, void *input, u32 inputBytes, void *callback);

BOOL SIBusy(void) {
    return Si.chan != -1 ? TRUE : FALSE;
}

BOOL SIIsChanBusy(s32 chan) {
    return Packet[chan].chan != -1 || Si.chan == chan;
}

BOOL SIEnablePollingInterrupt(BOOL enable) {
    BOOL enabled;
    BOOL rc;
    u32 reg;
    int i;

    enabled = OSDisableInterrupts();
    reg = __SIRegs[13];
    rc = (reg & 0x08000000) ? TRUE : FALSE;
    if (enable) {
        reg |= 0x08000000;
        for (i = 0; i < 4; i++) {
            InputBufferVcount[i] = 0;
        }
    } else {
        reg &= ~0x08000000;
    }
    reg &= ~0x80000001;
    __SIRegs[13] = reg;
    OSRestoreInterrupts(enabled);
    return rc;
}

BOOL SIRegisterPollingHandler(void *handler) {
    BOOL enabled;
    int i;

    enabled = OSDisableInterrupts();
    for (i = 0; i < 4; ++i) {
        if (RDSTHandler[i] == handler) {
            OSRestoreInterrupts(enabled);
            return TRUE;
        }
    }
    for (i = 0; i < 4; ++i) {
        if (RDSTHandler[i] == 0) {
            RDSTHandler[i] = handler;
            SIEnablePollingInterrupt(TRUE);
            OSRestoreInterrupts(enabled);
            return TRUE;
        }
    }
    OSRestoreInterrupts(enabled);
    return FALSE;
}

void SISetSamplingRate(u32 msec);
void *__OSSetInterruptHandler(s16 interrupt, void *handler);
u32 __OSUnmaskInterrupts(u32 mask);
void SIInterruptHandler(s16 interrupt, void *context);
void SIInit(void) {
    Packet[0].chan = Packet[1].chan = Packet[2].chan = Packet[3].chan = -1;
    Si.poll = 0;
    SISetSamplingRate(0);
    while (__SIRegs[13] & 1)
        ;
    __SIRegs[13] = 0x80000000;
    __OSSetInterruptHandler(0x14, SIInterruptHandler);
    __OSUnmaskInterrupts(0x800);
    SIGetType(0);
    SIGetType(1);
    SIGetType(2);
    SIGetType(3);
}

u32 SIGetStatus(s32 chan) {
    BOOL enabled;
    u32 sr;
    int chanShift;

    enabled = OSDisableInterrupts();
    sr = __SIRegs[14];
    chanShift = 8 * (3 - chan);
    sr >>= chanShift;
    if (sr & 8) {
        if (!(Type[chan] & 0x80)) {
            Type[chan] = 8;
        }
    }
    OSRestoreInterrupts(enabled);
    return sr;
}

void SISetCommand(s32 chan, u32 command) {
    __SIRegs[3 * chan] = command;
}

#pragma peephole on
void SITransferCommands(void) {
    __SIRegs[14] = 0x80000000;
}

BOOL OSDisableInterrupts(void);
BOOL OSRestoreInterrupts(BOOL level);
u32 SISetXY(u32 x, u32 y) {
    u32 poll;
    BOOL enabled;

    poll = x << 16;
    poll |= y << 8;
    enabled = OSDisableInterrupts();
    Si.poll &= ~(0x03FF0000 | 0x0000FF00);
    Si.poll |= poll;
    poll = Si.poll;
    __SIRegs[12] = poll;
    OSRestoreInterrupts(enabled);
    return (u32)poll;
}

u32 SIEnablePolling(u32 poll) {
    BOOL enabled;
    u32 en;

    if (poll == 0) {
        return Si.poll;
    }
    enabled = OSDisableInterrupts();
    poll >>= 24;
    en = poll & 0xF0;
    en >>= 4;
    Si.poll &= ~en;
    poll &= 0x03FFFFF0 | en;
    poll &= ~0x03FFFF00;
    Si.poll |= poll;
    poll = Si.poll;
    __SIRegs[14] = 0x80000000;
    __SIRegs[12] = poll;
    OSRestoreInterrupts(enabled);
    return poll;
}

u32 SIDisablePolling(u32 poll) {
    BOOL enabled;

    if (poll == 0) {
        return Si.poll;
    }
    enabled = OSDisableInterrupts();
    poll >>= 24;
    poll &= 0xF0;
    poll = Si.poll & ~poll;
    __SIRegs[12] = poll;
    Si.poll = poll;
    OSRestoreInterrupts(enabled);
    return poll;
}

BOOL SIGetResponseRaw(s32 chan) {
    u32 sr;

    sr = SIGetStatus(chan);
    if (sr & 0x20) {
        InputBuffer[chan][0] = __SIRegs[3 * chan + 1];
        InputBuffer[chan][1] = __SIRegs[3 * chan + 2];
        InputBufferValid[chan] = TRUE;
        return TRUE;
    }
    return FALSE;
}

BOOL SIGetResponse(s32 chan, void *data) {
    BOOL rc;
    BOOL enabled;

    enabled = OSDisableInterrupts();
    SIGetResponseRaw(chan);
    rc = InputBufferValid[chan];
    InputBufferValid[chan] = FALSE;
    if (rc) {
        ((u32 *)data)[0] = InputBuffer[chan][0];
        ((u32 *)data)[1] = InputBuffer[chan][1];
    }
    OSRestoreInterrupts(enabled);
    return rc;
}

static void AlarmHandler(OSAlarm *alarm, void *context) {
    s32 chan;
    SIPacket *packet;

    chan = alarm - Alarm;
    packet = &Packet[chan];
    if (packet->chan != -1) {
        if (__SITransfer(packet->chan, packet->output, packet->outputBytes, packet->input, packet->inputBytes, packet->callback)) {
            packet->chan = -1;
        }
    }
}

s32 SIDecodeType(s32 type) {
    u32 error;
    s32 kind;

    error = type & 0xFF;
    kind = type & ~0xFF;
    if (error & 0x08) {
        return 0x08;
    }
    if (error & 0x47) {
        return 0x40;
    }
    if (error) {
        return 0x80;
    }
    if ((kind & 0x18000000) == 0) {
        switch (kind & 0xFFFF0000) {
        case 0x00040000:
        case 0x00020000:
        case 0x00010000:
        case 0x05000000:
        case 0x02000000:
            return kind & 0xFFFF0000;
        }
        return 0x40;
    }
    if ((kind & 0x18000000) != 0x08000000) {
        return 0x40;
    }
    switch (kind & 0xFFFF0000) {
    case 0x09000000:
    case 0x08000000:
        return kind & 0xFFFF0000;
    }
    if ((kind & 0xFFE00000) == 0x08200000) {
        return 0x08200000;
    }
    if ((kind & 0x80000000) && !(kind & 0x04000000)) {
        if ((kind & 0x8B100000) == 0x8B100000) {
            return 0x8B100000;
        }
        if (!(kind & 0x02000000)) {
            return 0x88000000;
        }
    }
    if ((kind & 0x09000000) == 0x09000000) {
        kind = 0x09000000;
        return kind;
    }
    return 0x40;
}

u32 SIGetType(s32 chan);
u32 SIProbe(s32 chan) {
    return SIDecodeType(SIGetType(chan));
}

