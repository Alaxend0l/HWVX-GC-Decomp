#include "common.h"

typedef struct MIDI_VOICE
{
    u8 type;
    u8 channel;
    u8 byte2;
    u8 byte3;
    struct MIDI_VOICE *next;
    u8 pad_8_15[0xD];
    u8 state;
    u8 pad_16_20[0x10];
} MIDI_VOICE;

typedef struct MIDI_CHN
{
    struct MIDI_VOICE *voice;
    u8 pad_4_14[0x10];
    s32 vol;
    s32 pitch;
    u8 count;
    u8 note;
    u8 pad_1E_20[0x2];
} MIDI_CHN;

typedef struct MIDI_INST
{
    u8 pad_0_14[0x14];
    u8 prog;
    u8 pad_15[1];
    u8 lo;
    u8 hi;
    u8 pad_18_20[8];
} MIDI_INST;

typedef struct MIDI_SND
{
    u8 pad_0_10[0x10];
    u32 count;
    u32 size;
    u8 pad_18_20[4];
    MIDI_INST inst[1];
} MIDI_SND;

MIDI_CHN g_chn[0x10];

void SetVoiceState_(MIDI_VOICE *voice, s32 state);

s32 AudioMIDI_Create(void)
{
    extern u8 g_song[0x24];
    extern void *g_pHdr;
    void *memset(void *dst, s32 c, u32 n);

    memset(g_song, 0, 0x24);
    memset(g_chn, 0, 0x200);
    g_pHdr = NULL;
    return 1;
}

s32 AudioMIDI_Init(void)
{
    extern void *g_pHdr;
    s32 DestroyMIDI(void);

    if (g_pHdr != NULL)
    {
        DestroyMIDI();
    }
    return 1;
}

void AudioMIDI_Destroy(void)
{
}

#pragma push
#pragma optimization_level 1
void CreateMIDI(u8 *hdr, u8 *snd)
{
    extern void *g_pHdr;
    extern void *g_pSnd;
    extern void *g_pProg[0x80];
    extern void *g_pDrum[0x80];
    extern s32 g_musicState;
    extern void (*DestroySongVoice)();
    extern s32 __OSCurrHeap;
    void *OSAllocFromHeap(s32 heap, u32 size);
    void FindMarkers(u8 *p);
    s32 CreateMIDI_(void *snd);
    void DestroyMIDIVoice();
    u32 size;
    u32 i;
    s32 j;

    if (hdr == NULL || snd == NULL)
    {
        return;
    }
    size = *(u32 *)(hdr + 4) + 8;
    g_pHdr = OSAllocFromHeap(__OSCurrHeap, size + *(u32 *)(snd + 0x14));
    g_pSnd = (u8 *)g_pHdr + size;
    memcpy(g_pHdr, hdr, size);
    memcpy(g_pSnd, snd, *(u32 *)(snd + 0x14));
    *(u32 *)((u8 *)g_pSnd + 0x14) += (u32)snd;
    FindMarkers((u8 *)g_pHdr + 0x22);
    memset(g_pProg, 0, 0x200);
    memset(g_pDrum, 0, 0x200);
    j = 0x80;
    for (i = 0; i < *(u32 *)((u8 *)g_pSnd + 0x10); i++)
    {
        if (*(u8 *)((u8 *)g_pSnd + (i << 5) + 0x34) != 0x80)
        {
            if (j != *(u8 *)((u8 *)g_pSnd + (i << 5) + 0x34))
            {
                j = *(u8 *)((u8 *)g_pSnd + (i << 5) + 0x34);
                g_pProg[j] = (u8 *)g_pSnd + (i << 5) + 0x20;
            }
        }
        else
        {
            for (j = *(u8 *)((u8 *)g_pSnd + (i << 5) + 0x36);
                j <= *(u8 *)((u8 *)g_pSnd + (i << 5) + 0x37); j++)
            {
                g_pDrum[j] = (u8 *)g_pSnd + (i << 5) + 0x20;
            }
            j = 0x80;
        }
    }
    CreateMIDI_(g_pSnd);
    g_musicState = 0;
    DestroySongVoice = DestroyMIDIVoice;
}
void SetMIDIVol(void)
{
    MIDI_CHN *chn;
    MIDI_VOICE *voice;
    s32 i;
    void SetGVol(void *chn);
    void SetVoiceVol_(void *voice, s32 vol);

    for (chn = g_chn, i = 0x10; i != 0; chn++, i--)
    {
        SetGVol(chn);
        for (voice = chn->voice; voice != NULL; voice = voice->next)
        {
            SetVoiceVol_(voice, chn->vol);
        }
    }
}

#pragma push
#pragma optimization_level 1
#pragma dont_inline on
void SetMIDIPitch(u32 pitch)
{
    u32 i;
    MIDI_CHN *chn;
    MIDI_VOICE *voice;
    extern u8 g_song[0x24];
    s32 Mul16_(s32, s32);
    u32 GetPitchWheel_(u8 note, u8 count);
    s8 SetVoiceRate_(void *voice);

    *(u32 *)(g_song + 0x20) = pitch;
    for (chn = g_chn, i = 0x10; i != 0; chn++, i--)
    {
        chn->pitch = Mul16_(GetPitchWheel_(chn->note, chn->count), pitch);
        for (voice = chn->voice; voice != NULL; voice = voice->next)
        {
            *(s32 *)((u8 *)voice + 0x34) = chn->pitch;
            SetVoiceRate_(voice);
        }
    }
}
#pragma pop

#pragma pop
void SetMIDITempo(u32 tempo)
{
    extern void *g_pHdr;
    extern u8 g_song[0x24];
    u32 inv;

    inv = -1U / tempo;
    *(u32 *)(g_song + 0x10) = inv;
    *(u32 *)(g_song + 4) = (u32)Mul16_(*(u32 *)(g_song + 0xC), inv) / *(u16 *)((u8 *)g_pHdr + 0x18);
}

#pragma push
#pragma optimization_level 1
void SetMIDIMarker(u8 marker, s8 keep)
{
    extern void *g_pHdr;
    extern u32 g_mrk[0x40][2];
    extern u8 g_song[0x24];
    s32 flag;

    if (g_pHdr != NULL && marker < 0x40 && g_mrk[marker][0] != 0)
    {
        flag = !keep;
        g_song[0x1E] = marker;
        g_song[0x1F] = flag;
        return;
    }
    g_song[0x1E] = 0;
    g_song[0x1F] = 0;
}
#pragma pop
void SetMIDITrackPriority(void)
{
}

void SetMIDITrackOn(u32 mask, s8 deferred)
{
    extern u32 g_trkOn;
    extern u32 g_trkOff;
    u8 *chn;

    if (deferred)
    {
        g_trkOff &= ~mask;
        g_trkOn |= mask;
        return;
    }
    for (chn = (u8 *)g_chn; mask != 0; chn += 0x20)
    {
        chn[4] |= mask & 1;
        mask >>= 1;
    }
}

void SetMIDITrackOff(u32 mask, s8 deferred)
{
    extern u32 g_trkOn;
    extern u32 g_trkOff;
    void ChannelOff(void *chn);
    u8 *chn;

    if (deferred)
    {
        g_trkOn &= ~mask;
        g_trkOff |= mask;
        return;
    }
    for (chn = (u8 *)g_chn; mask != 0; chn += 0x20)
    {
        if (mask & 1)
        {
            chn[4] = 0;
            ChannelOff(chn);
        }
        mask >>= 1;
    }
}

void SetMIDITrackVol(void)
{
}

#pragma push
#pragma optimization_level 1
u32 GetEnvRate(u16 arg0)
{
    if (arg0 != 0)
    {
        return -1U / (u32)(arg0 * 0x3E8);
    }
    return -1U;
}

#pragma push
#pragma optimization_level 1
void ReleaseMIDIVoice(void *voice)
{
    void SetVoicePriority_(void *voice);
    u32 rate;

    if (*(u8 *)((u8 *)voice + 0x16) != 4)
    {
        *(u8 *)((u8 *)voice + 0x16) = 4;
        *(u8 *)((u8 *)voice + 0x17) = 0;
        rate = GetEnvRate(*(u16 *)((u8 *)(*(u32 *)((u8 *)voice + 0x10)) + 0x1E));
        if (rate + 0x10000 == 0xFFFF)
        {
            rate = 0;
            *(s32 *)((u8 *)voice + 0x1C) = 0x10000;
        }
        *(u32 *)((u8 *)voice + 0x18) = rate;
        *(u8 *)((u8 *)voice + 0x14) -= 15;
        SetVoicePriority_(voice);
    }
}

#pragma push
#pragma optimization_level 1
void ChannelOff(void *chn)
{
    void *v;

    for (v = *(void **)chn; v != NULL; v = *(void **)((u8 *)v + 4))
    {
        ReleaseMIDIVoice(v);
    }
}
#pragma pop

#pragma pop

#pragma push
#pragma optimization_level 1
void DestroyMIDIVoice(MIDI_VOICE *voice)
{
    MIDI_VOICE *v;

    v = g_chn[voice->channel].voice;
    if (v == voice)
    {
        g_chn[voice->channel].voice = voice->next;
    }
    else
    {
        for (; v->next != voice; v = v->next)
        {
        }
        v->next = voice->next;
    }
    voice->state = 0;
    voice->channel = 0xFF;
}
MIDI_VOICE *DestroyVoice(MIDI_VOICE *voice)
{
    MIDI_VOICE *next;
    MIDI_VOICE *v;

    next = voice->next;
    v = g_chn[voice->channel].voice;
    if (v == voice)
    {
        g_chn[voice->channel].voice = next;
    }
    else
    {
        for (; v->next != voice; v = v->next)
        {
        }
        v->next = next;
    }
    SetVoiceState_(voice, 3);
    return next;
}
#pragma pop
