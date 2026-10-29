#include "common.h"

void FreeA_(u32 addr);
void OSFreeToHeap(s32 heap, void *ptr);
extern s32 __OSCurrHeap;
extern u32 g_aheap;
extern u32 g_musicVol;

typedef struct ITTrack {
    u8 pad0[0x18];
    u32 vol;
} ITTrack;

typedef struct ITChannel {
    u8 pad0[0x1C];
    u32 vol;
    u8 pad20[0x28 - 0x20];
    u32 gvol;
} ITChannel;

typedef struct ITHeader {
    u8 pad0[0x24];
    u16 smpNum;
} ITHeader;

typedef struct ITSample {
    u8 pad0[0x12];
    u8 flags;
    u8 pad13[0x30 - 0x13];
    u32 length;
    u8 pad34[0x48 - 0x34];
    u32 data;
} ITSample;

u32 HAllocA_(s32 size);
void MemCpyA_(u32 dst, u32 src, s32 size);

void DestroyIT_(void *data) {
    OSFreeToHeap(__OSCurrHeap, data);
    FreeA_(g_aheap);
    g_aheap = 0;
}

#pragma optimization_level 1
u32 GetITGVol_(ITTrack *trk, ITChannel *ch) {
    return ((trk->vol * g_musicVol >> 7) * (ch->vol * ch->gvol >> 6)) >> 15;
}
#pragma optimization_level reset

