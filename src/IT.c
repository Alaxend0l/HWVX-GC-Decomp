

#include "common.h"

typedef struct IT_VOICE
{
    u8 byte0;
    u8 track;
    u8 pad_2_3[2];
    struct IT_VOICE *next;
    u8 pad_8_3B[0x34];
    s32 vol;
    s32 pan;
    u8 pad_44_6F[0x2C];
} IT_VOICE;

typedef struct IT_TRACK
{
    struct IT_VOICE *head;
    struct IT_VOICE *active;
    u8 pad_8_10[9];
    u8 state;
    u8 pad_12_19[8];
    s16 gvol;
    u8 pad_1C_27[0xC];
    s32 vol;
    s32 pan;
    u8 pad_30_6F[0x40];
} IT_TRACK;

IT_TRACK g_trk[0x20];

s32 g_song[0xC];

void SetVoiceState_(IT_VOICE *voice, s32 state);
void SetVoiceVol_(IT_VOICE *voice, s32 vol);
void StopActive(IT_TRACK *trk);
void StartVoice_(void *voice);
void *GetVoice_(u8 type, u32 priority);
u32 GetNoteRate_(u32 rate, u8 note);

void AudioIT_Destroy(void) {

}

#pragma push
#pragma optimization_level 1
void DestroyIT(void)
{
    extern void *g_pHdr;
    extern u8 g_musicTitle[0x20];
    void SetITState(s32 state);
    void DestroyIT_(void *hdr);

    if (g_pHdr == NULL)
    {
        return;
    }
    SetITState(3);
    DestroyIT_(g_pHdr);
    g_pHdr = NULL;
    memset(g_musicTitle, 0, 0x20);
}
#pragma pop

#pragma push
#pragma optimization_level 1
void SetITPitch(s32 pitch)
{
    extern IT_VOICE g_voice[0x20];
    void SetVoiceRate_(IT_VOICE *voice);
    IT_VOICE *voice;
    s32 i;

    g_song[7] = pitch;
    for (voice = g_voice, i = 0x20; i != 0; voice++, i--)
    {
        if (voice->pad_8_3B[0x0D] == 0)
        {
            continue;
        }
        if (voice->byte0 != 0)
        {
            continue;
        }
        *(s32 *)((u8 *)voice + 0x34) = pitch;
        SetVoiceRate_(voice);
    }
}

#pragma push
#pragma optimization_level 1
void SetITTempo(u32 tempo)
{
    if (tempo > 0x1FFFF)
    {
        tempo = 0x1FFFF;
    }
    g_song[0xA] = tempo;
    g_song[0] = ((u32)g_song[0xA] << 0xF) / (0x1C9C380U / ((u32)g_song[9] * 0x18));
}
#pragma pop

#pragma pop

#pragma push
#pragma optimization_level 1
void SetITOrder(u8 order, s8 deferred)
{
    extern void *g_pHdr;

    if (g_pHdr == NULL)
    {
        return;
    }
    if (order < *(u16 *)((u8 *)g_pHdr + 0x20))
    {
        *((u8 *)g_song + 0x15) = order;
        if (deferred == 0)
        {
            g_song[4] = 1;
        }
    }
}
#pragma pop

void SetITTrackOn(u32 mask, s8 deferred)
{
    extern u32 g_trkOn;
    IT_TRACK *trk;

    if (deferred)
    {
        g_trkOn |= mask;
        return;
    }
    for (trk = g_trk; mask != 0; trk++)
    {
        trk->state |= mask & 1;
        mask >>= 1;
    }
}

#pragma push
#pragma optimization_level 1
#pragma dont_inline
void SetITTrackOff(u32 mask, s8 deferred)
{
    extern u32 g_trkOff;
    IT_TRACK *trk;
    IT_VOICE *voice;
    IT_VOICE *next;

    if (deferred)
    {
        g_trkOff |= mask;
        return;
    }
    for (trk = g_trk; mask != 0; trk++)
    {
        if (mask & 1)
        {
            trk->state = 0;
            voice = trk->head;
            trk->head = NULL;
            trk->active = NULL;
            while (voice)
            {
                next = voice->next;
                SetVoiceState_(voice, 3);
                voice = next;
            }
        }
        mask >>= 1;
    }
}

s32 PlayNote(IT_TRACK *trk)
{
    u32 rate;
    u8 stolen;
    u8 *voice;
    u8 *song;

    stolen = 0;
    rate = GetNoteRate_(*(u32 *)(*(u8 **)trk->pad_8_10 + 0x3C), trk->pad_12_19[7]);
    if (rate > 0x2EE00 || rate < 0x60)
    {
        if (!stolen && trk->active != NULL)
        {
            StopActive(trk);
        }
        return 0;
    }
    if (trk->active == NULL)
    {
        trk->active = (IT_VOICE *)GetVoice_(0, trk->pad_12_19[0]);
        if (trk->active == NULL)
        {
            return 0;
        }
        trk->active->track = trk->pad_8_10[8];
        trk->active->pad_2_3[0] = trk->pad_12_19[2];
        trk->active->next = trk->head;
        trk->head = trk->active;
    }
    voice = (u8 *)trk->active;
    *(u32 *)(voice + 0x10) = *(u32 *)trk->pad_8_10;
    song = *(u8 **)trk->pad_8_10;
    *(u32 *)(voice + 0x20) = *(u32 *)(song + 0x48);
    *(s32 *)(voice + 0x24) = (song[0x12] & 0x10) ? *(s32 *)(song + 0x34) : -1;
    *(s32 *)(voice + 0x28) = (song[0x12] & 0x10) ? *(s32 *)(song + 0x38) : *(s32 *)(song + 0x30);
    voice[0x2C] = (song[0x12] & 2) ? 0x10 : 8;
    *(u32 *)(voice + 0x30) = rate;
    *(s32 *)(voice + 0x34) = g_song[7];
    *(u32 *)(voice + 0x38) = *(u32 *)(trk->pad_30_6F + 0x14);
    *(s32 *)(voice + 0x3C) = song[0x11] << 9;
    *(s32 *)(voice + 0x40) = trk->pan;
    *(s32 *)(voice + 0x44) = -0x7FFF;
    StartVoice_((void *)voice);
    SetVoiceVol_((IT_VOICE *)voice, trk->gvol);
    return 1;
}

#pragma pop
void SetTrackVol(IT_TRACK *trk)
{
    IT_VOICE *voice;

    for (voice = trk->head; voice != NULL; voice = voice->next)
    {
        SetVoiceVol_(voice, trk->gvol);
    }
}

void SetTrackPan(IT_TRACK *trk)
{
    IT_VOICE *voice;

    for (voice = trk->head; voice != NULL; voice = voice->next)
    {
        voice->pan = trk->pan;
        SetVoiceVol_(voice, trk->gvol);
    }
}

void StopActive(IT_TRACK *trk)
{
    trk->head = trk->head->next;
    SetVoiceState_(trk->active, 3);
    trk->active = NULL;
}

#pragma push
#pragma optimization_level 1
#pragma dont_inline
void DestroyITVoice(IT_VOICE *voice)
{
    IT_TRACK *trk;
    IT_VOICE *v;

    if (voice->track > 0x20)
    {
        return;
    }
    trk = &g_trk[voice->track];
    if (trk->active == voice)
    {
        trk->active = NULL;
        trk->head = voice->next;
        return;
    }
    v = trk->head;
    while (v->next != voice)
    {
        v = v->next;
    }
    v->next = voice->next;
}
#pragma pop
