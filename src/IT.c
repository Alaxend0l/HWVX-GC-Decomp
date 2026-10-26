

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

void SetVoiceState_(IT_VOICE *voice, s32 state);
void SetVoiceVol_(IT_VOICE *voice, s32 vol);

void AudioIT_Destroy(void) {

}

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
            while (voice != NULL)
            {
                next = voice->next;
                SetVoiceState_(voice, 3);
                voice = next;
            }
        }
        mask >>= 1;
    }
}

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

