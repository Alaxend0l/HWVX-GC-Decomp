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
