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

static SIPacket Packet[4];
static OSAlarm Alarm[4];

BOOL __SITransfer(s32 chan, void *output, u32 outputBytes, void *input, u32 inputBytes, void *callback);

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

