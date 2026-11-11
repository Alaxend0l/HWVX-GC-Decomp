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

#pragma optimization_level 1
s32 CreateIT_(ITHeader *hdr, ITSample **smp) {
    u32 i;
    u32 size;

    i = hdr->smpNum - 1;
    size = smp[i]->data + (smp[i]->length << ((smp[i]->flags & 2) != 0));
    g_aheap = HAllocA_(size);
    if (g_aheap == 0) {
        return 0;
    }
    DCFlushRange((void *)smp[0]->data, size);
    MemCpyA_(g_aheap, smp[0]->data, size);
    smp[0]->data = 0;
    for (i = 0; i < hdr->smpNum; i++) {
        smp[i]->data += g_aheap;
        if (smp[i]->flags & 2) {
            smp[i]->data >>= 1;
        }
    }
    return 1;
}
#pragma optimization_level reset

void DestroyIT_(void *data) {
    OSFreeToHeap(__OSCurrHeap, data);
    FreeA_(g_aheap);
    g_aheap = 0;
}

#pragma optimization_level 1
/* A track's volume through the music and channel volumes:
 * ((trk->vol * g_musicVol >> 7) * (ch->vol * ch->gvol >> 6)) >> 15.
 * Hand-written assembly in the original. */
asm u32 GetITGVol_(ITTrack *trk, ITChannel *ch) {
    fralloc
    lwz r5, g_musicVol
    lwz r3, 24(r3)
    lwz r6, 40(r4)
    lwz r4, 28(r4)
    mullw r3, r3, r5
    mullw r4, r4, r6
    srwi r3, r3, 7
    srwi r4, r4, 6
    mullw r3, r3, r4
    srwi r3, r3, 15
    frfree
    blr
}
#pragma optimization_level reset

