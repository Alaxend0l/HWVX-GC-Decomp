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
