#include "common.h"

#pragma readonly_strings on

typedef struct AXVPB {
    struct AXVPB *next;
    struct AXVPB *prev;
    struct AXVPB *next1;
    u32 priority;
    void *callback;
    void *userContext;
    u32 index;
    u32 sync;
    u8 unk20[0x146 - 0x20];
    u16 state;
    u8 unk148[0x1B2 - 0x148];
    u16 currentAddressHi;
    u16 currentAddressLo;
    u8 unk1B6[0x1DE - 0x1B6];
    u16 ratioHi;
    u16 ratioLo;
} AXVPB;

typedef struct VoiceHW {
    u8 kind;
    u8 id;
    u8 unk2[2];
    struct VoiceHW *next;
    u8 unk8[0xC];
    u8 priority;
    u8 state;
    u8 unk16[0xA];
    u32 addr;
    u8 unk24[4];
    u32 length;
    u8 unk2C[4];
    u32 rate;
    u32 pitch;
    u8 unk38[0x10];
    s32 unk48;
    u8 unk4C[4];
    AXVPB *axv;
    u8 unk54;
    u8 unk55;
    u8 unk56;
    u8 unk57;
    u8 unk58[2];
    s16 unk5A;
    u8 unk5C[8];
    s16 unk64;
    u8 unk66[6];
    s16 unk6C;
    u8 unk6E[2];
} VoiceHW;

extern void (*DestroySongVoice)(VoiceHW *voice);
extern VoiceHW *g_pFree;

typedef struct AXFX_DELAY {
    u8 data[0x60];
} AXFX_DELAY;

extern AXFX_DELAY g_reverb[2];

void AXRegisterAuxACallback(void *callback, void *context);
void AXRegisterAuxBCallback(void *callback, void *context);
s32 AXFXDelayShutdown(AXFX_DELAY *delay);

s32 OSDisableInterrupts(void);
void OSRestoreInterrupts(s32 level);
void AXSetVoicePriority(AXVPB *axv, u32 priority);


void AudioVoice_Destroy_(void) {
    AXRegisterAuxBCallback(NULL, NULL);
    AXRegisterAuxACallback(NULL, NULL);
    AXFXDelayShutdown(&g_reverb[1]);
    AXFXDelayShutdown(&g_reverb[0]);
}

void OSReport(const char *msg, ...);
AXVPB *AXAcquireVoice(u32 priority, void (*callback)(AXVPB *), u32 userContext);
void DestroyVoice_(AXVPB *axv);
#pragma optimization_level 1
VoiceHW *GetVoice_(u8 kind, u32 priority) {
    VoiceHW *voice;
    AXVPB *axv;
    s32 level;

    if (g_pFree == NULL) {
        OSReport("g_pFree NULL\n");
        return NULL;
    }
    level = OSDisableInterrupts();
    axv = AXAcquireVoice(priority, DestroyVoice_, (u32)g_pFree);
    if (axv == NULL) {
        return NULL;
    }
    voice = g_pFree;
    g_pFree = g_pFree->next;
    voice->next = NULL;
    voice->axv = axv;
    voice->kind = kind;
    voice->state = 3;
    voice->priority = priority;
    voice->unk54 = 1;
    voice->unk5A = -1;
    voice->unk64 = -1;
    voice->unk6C = -1;
    voice->unk55 = 0;
    voice->unk56 = 0;
    voice->unk57 = 0;
    voice->unk48 = 0x7FFF;
    OSRestoreInterrupts(level);
    return voice;
}
void SetVoicePriority_(VoiceHW *voice) {
    s32 level;

    level = OSDisableInterrupts();
    AXSetVoicePriority(voice->axv, voice->priority);
    OSRestoreInterrupts(level);
}

#pragma optimization_level 1
void DestroyVoice_(AXVPB *axv) {
    VoiceHW *voice;
    s32 level;

    level = OSDisableInterrupts();
    voice = axv->userContext;
    if (voice->kind == 0) {
        DestroySongVoice(voice);
    }
    voice->state = 0;
    voice->id = 0xFD;
    voice->axv = NULL;
    voice->next = g_pFree;
    g_pFree = voice;
    OSRestoreInterrupts(level);
}
void SetVoiceVol_(u8 *v, u32 vol) {
    extern s32 g_speakers_803C789C;
    extern const u16 g_panTab[128];
    u8 *axv;
    s32 pan;
    u32 amp;
    s32 l;
    s32 r;
    s16 dl;
    s16 dr;

    axv = *(u8 **)(v + 0x50);
    if (axv == (u8 *)0xFFFFFFFF) {
        return;
    }
    pan = 0x40;
    *(u32 *)(axv + 0x1C) &= ~0x10;
    v[0x56] = 0;
    v[0x57] = 0;
    if (g_speakers_803C789C >= 1) {
        pan = ((*(u32 *)(v + 0x40) >> 9) & 0x7F) ^ 0x40;
    }
    l = g_panTab[0x7F - pan];
    r = g_panTab[pan];
    vol = *(u32 *)(v + 0x3C) * vol;
    amp = vol >> 15;
    l = (s32)(l * amp) >> 17;
    r = (s32)(r * amp) >> 17;
    if (l != *(s16 *)(v + 0x64) || r != *(s16 *)(v + 0x66)) {
        *(u32 *)(axv + 0x1C) |= 0x12;
        if (*(s16 *)(v + 0x64) < 0) {
            dl = 0;
            v[0x56] = 1;
            dr = 0;
        } else {
            dl = (l - *(s16 *)(v + 0x64)) / 160;
            dr = (r - *(s16 *)(v + 0x66)) / 160;
        }
        *(s16 *)(v + 0x60) = l;
        *(s16 *)(v + 0x62) = r;
        if (dl | dr) {
            v[0x56] = 1;
            *(s16 *)(axv + 0x14C) = dl;
            *(s16 *)(axv + 0x150) = dr;
            *(u16 *)(axv + 0x144) |= 8;
        } else {
            *(s16 *)(axv + 0x14A) = l;
            *(s16 *)(axv + 0x14E) = r;
            *(u16 *)(axv + 0x144) &= ~8;
        }
    }
    if (v[0] == 1) {
        return;
    }
    l = (u32)(l * *(s32 *)(v + 0x48)) >> 15;
    r = (u32)(r * *(s32 *)(v + 0x48)) >> 15;
    if (l == *(s16 *)(v + 0x6C) && r == *(s16 *)(v + 0x6E)) {
        return;
    }
    *(u32 *)(axv + 0x1C) |= 0x12;
    if (*(s16 *)(v + 0x6C) < 0) {
        dl = 0;
        v[0x57] = 1;
        dr = 0;
    } else {
        dl = (l - *(s16 *)(v + 0x6C)) / 160;
        dr = (r - *(s16 *)(v + 0x6E)) / 160;
    }
    *(s16 *)(v + 0x68) = l;
    *(s16 *)(v + 0x6A) = r;
    if (dl | dr) {
        v[0x57] = 1;
        if (v[0]) {
            *(s16 *)(axv + 0x15C) = dl;
            *(s16 *)(axv + 0x160) = dr;
            *(u16 *)(axv + 0x144) |= 0x800;
        } else {
            *(s16 *)(axv + 0x154) = dl;
            *(s16 *)(axv + 0x158) = dr;
            *(u16 *)(axv + 0x144) |= 0x40;
        }
    } else if (v[0]) {
        *(s16 *)(axv + 0x15A) = l;
        *(s16 *)(axv + 0x15E) = r;
        *(u16 *)(axv + 0x144) &= ~0x800;
    } else {
        *(s16 *)(axv + 0x152) = l;
        *(s16 *)(axv + 0x156) = r;
        *(u16 *)(axv + 0x144) &= ~0x40;
    }
}

#pragma optimization_level 1
s8 SetVoiceRate_(VoiceHW *voice) {
    u32 rate = (1.0f / 48000.0f) * ((f32)voice->rate * (f32)voice->pitch);

    if (rate <= 0x2EE00 && rate >= 0x60) {
        *(u32 *)&voice->axv->ratioHi = rate;
        voice->axv->sync |= 0x80000;
        return 1;
    }
    return 0;
}
s32 SetVoiceOffset_(VoiceHW *voice, u32 offset) {
    AXVPB *axv;

    if (offset < voice->length) {
        axv = voice->axv;
        *(u32 *)&axv->currentAddressHi = voice->addr + offset;
        axv->sync |= 0x1000;
        return 1;
    }
    return 0;
}

