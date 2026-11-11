#include "common.h"

typedef struct MIDISample_ {
    u32 addr;
    u32 loop;
    s32 end;
    u8 unkC[5];
    u8 bits;
    u8 unk12[0xE];
} MIDISample_;

typedef struct MIDIHeader_ {
    u32 unk0;
    u32 size;
    u32 unk8;
    u32 unkC;
    u32 numSamples;
    u32 sampleData;
    u32 unk18;
    u32 unk1C;
    MIDISample_ samples[1];
} MIDIHeader_;

u32 HAllocA_(s32 size);
void MemCpyA_(u32 dst, u32 src, s32 size);
void DCFlushRange(void *addr, u32 nBytes);
void FreeA_(u32 addr);
void OSFreeToHeap(s32 heap, void *ptr);
extern s32 __OSCurrHeap;
extern u32 g_aheap;

#pragma push
#pragma optimization_level 1
s32 CreateMIDI_(MIDIHeader_ *hdr) {
    s32 size;
    u32 i;
    MIDISample_ *smp;

    size = hdr->size - ((u8 *)&hdr->samples[hdr->numSamples] - (u8 *)hdr - 8);
    g_aheap = HAllocA_(size);
    if (g_aheap == 0) {
        return 0;
    }
    DCFlushRange(&hdr->samples[hdr->numSamples], size);
    MemCpyA_(g_aheap, hdr->sampleData, size);
    smp = hdr->samples;
    for (i = 0; i < hdr->numSamples; i++) {
        smp->addr += g_aheap;
        if (smp->bits == 0x10) {
            smp->addr >>= 1;
            smp->loop >>= 1;
            smp->end >>= 1;
        }
        smp++;
    }
    return 1;
}
#pragma pop

void DestroyMIDI_(void *data) {
    OSFreeToHeap(__OSCurrHeap, data);
    FreeA_(g_aheap);
}

/* A MIDI variable-length number at *pp (7 bits a byte, high bit: more),
 * *pp moved past it. Hand-written assembly in the original. */
asm u32 ReadVar_(u8 **pp) {
    fralloc
    lwz r7, 0(r3)
    addi r7, r7, -1
    li r4, 0
@next:
    lbzu r6, 1(r7)
    slwi r4, r4, 7
    andi. r5, r6, 127
    or r4, r4, r5
    cmplw r5, r6
    bne @next
    addi r7, r7, 1
    stw r7, 0(r3)
    mr r3, r4
    frfree
    blr
}

extern const u32 g_pitchTab[];

/* g_pitchTab[note] raised to the power count (2.30 fixed point), >> 14. With
 * count 0 it still multiplies once. Hand-written assembly in the original. */
asm u32 GetPitchWheel_(u8 note, u8 count) {
    fralloc
    lis r5, g_pitchTab@ha
    slwi r3, r3, 2
    addi r5, r5, g_pitchTab@l
    addic. r4, r4, -1
    lwzx r3, r3, r5
    beq @done
    mr r5, r3
@loop:
    mulhwu r6, r3, r5
    mullw r3, r3, r5
    slwi r6, r6, 2
    srwi r3, r3, 30
    addic. r4, r4, -1
    or r3, r3, r6
    bgt @loop
@done:
    srwi r3, r3, 14
    frfree
    blr
}

