#include "common.h"

typedef struct AXVPB {
    u8 unk0[0x1C];
    u32 sync;
    u32 depop;
    u32 updateMS;
    u32 updateCounter;
    u32 updateTotal;
    u16 *updateWrite;
    u16 updateData[128];
    void *itdBuffer;
    u8 pb_unk138[0xE];
    u16 state;
    u8 pb_unk148[0x26];
    u16 itdFlag;
    u8 pb_unk170[0xC];
    u16 pb_unk17C;
    u16 pb_unk17E;
    u16 pb_unk180;
    u16 pb_unk182;
    u16 pb_unk184;
} AXVPB;


s32 __AXGetNumVoices(void) {
    extern s32 __AXNumVoices;
    return __AXNumVoices;
}

void *__AXGetPBs(void) {
    extern u8 __AXPB[];
    return __AXPB;
}

void __AXSetPBDefault(AXVPB *p)
{
    p->state = 0;
    p->itdFlag = 0;
    p->sync = 0xA4;
    p->updateCounter = 0;
    p->updateMS = 0;
    p->updateWrite = p->updateData;
    p->pb_unk184 = 0;
    p->pb_unk182 = 0;
    p->pb_unk180 = 0;
    p->pb_unk17E = 0;
    p->pb_unk17C = 0;
}

void __AXVPBQuit(void) {
}

