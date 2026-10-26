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
    u8 pad_4_20[0x1C];
} MIDI_CHN;

MIDI_CHN g_chn[0x10];

void SetVoiceState_(MIDI_VOICE *voice, s32 state);

void SetMIDITrackVol(void)
{
}

void DestroyMIDIVoice(MIDI_VOICE *voice)
{
    MIDI_VOICE *v;
    MIDI_VOICE *next;

    next = voice->next;
    v = g_chn[voice->channel].voice;
    if (v == voice)
    {
        g_chn[voice->channel].voice = next;
        return;
    }
    for (; v->next != voice; v = v->next)
    {
    }
    v->next = next;
    voice->state = 0;
    voice->channel = 0xFF;
}

void DestroyVoice(MIDI_VOICE *voice)
{
    MIDI_VOICE *v;
    MIDI_VOICE *next;

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
    voice->state = 3;
    voice->channel = 0xFF;
    SetVoiceState_(voice, 3);
}

