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
