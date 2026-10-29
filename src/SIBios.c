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

BOOL __SITransfer(s32 chan, void *output, u32 outputBytes, void *input, u32 inputBytes, void *callback);

BOOL SIBusy(void) {
    return Si.chan != -1 ? TRUE : FALSE;
}

void SISetCommand(s32 chan, u32 command) {
    __SIRegs[3 * chan] = command;
}

#pragma peephole off
void SITransferCommands(void) {
    *(volatile u32 *)0xCC006438 = 0x80000000;
}
#pragma peephole reset

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

