#include "common.h"

typedef struct AXPB {
    u16 nextHi;          /* 0x00 */
    u16 nextLo;          /* 0x02 */
    u16 currHi;          /* 0x04 */
    u16 currLo;          /* 0x06 */
    u8 unk8[6];
    u16 state;           /* 0x0E */
    u8 unk10[0x26];
    u16 itdFlag;         /* 0x36 */
    u16 itdHi;           /* 0x38 */
    u16 itdLo;           /* 0x3A */
    u8 unk3C[8];
    u16 unk44;
    u16 unk46;
    u16 unk48;
    u16 unk4A;
    u16 unk4C;
    u16 updHi;           /* 0x4E */
    u16 updLo;           /* 0x50 */
    u8 unk52[0xEC - 0x52];
} AXPB;

typedef struct AXVPB {
    u8 unk0[0xC];
    u32 priority;        /* 0x0C */
    u8 unk10[8];
    u32 index;           /* 0x18 */
    u32 sync;
    u32 depop;
    u32 updateMS;
    u32 updateCounter;
    u32 updateTotal;
    u16 *updateWrite;
    u16 updateData[128];
    void *itdBuffer;
    AXPB pb;             /* 0x138 */
} AXVPB;

static AXPB __AXPB[64];
static u32 __AXITD[64][16];
static u16 __AXUpdates[64][128];
static AXVPB __AXVPB[64];
static u32 __AXMaxDspCycles;
static u32 __AXRecDspCycles;
s32 __AXNumVoices;

s32 __AXGetNumVoices(void) {
    return __AXNumVoices;
}

void *__AXGetPBs(void) {
    return __AXPB;
}

void __AXSetPBDefault(AXVPB *p)
{
    p->pb.state = 0;
    p->pb.itdFlag = 0;
    p->sync = 0xA4;
    p->updateCounter = 0;
    p->updateMS = 0;
    p->updateWrite = p->updateData;
    p->pb.unk4C = 0;
    p->pb.unk4A = 0;
    p->pb.unk48 = 0;
    p->pb.unk46 = 0;
    p->pb.unk44 = 0;
}

void __AXVPBQuit(void) {
}

