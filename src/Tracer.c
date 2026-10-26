#include "common.h"

struct TracerEntry {
    u8 pad0[0x4CE];
    s16 active;
    u8 pad4D0[0x4E0 - 0x4D0];
};
extern struct TracerEntry tracers[32];

struct Thing {
    u8 pad0[0xB0];
    s32 tracer;
    s32 tracerIndex;
};


void Tracer_Release(struct Thing *thing) {
    void Thing_Release(void *);

    tracers[thing->tracerIndex].active = 0;
    thing->tracer = 0;
    Thing_Release(thing);
}

s32 Tracer_Invalidate(void *arg0, u32 arg1, u32 arg2) {
    u32 temp_r0;

    temp_r0 = M2C_FIELD(arg0, u32 *, 0xB0);
    if ((temp_r0 >= arg1) && (temp_r0 < arg2)) {
        return 1;
    }
    return 0;
}
