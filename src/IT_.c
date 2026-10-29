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

