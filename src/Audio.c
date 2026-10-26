#include "common.h"

/* The sound system's update hooks, installed by the create calls below. */
void (*SongUpdate)(void);
void (*VoiceUpdate)(void);

/* Set once Audio_Create() has brought everything up; it makes later calls no-ops. */
s8 g_init;

s8 AudioMusic_Init(void);
s8 AudioIT_Init(void);
s8 AudioMIDI_Init(void);
s8 AudioVoice_Init(void);
s8 Audio_Init_(void);

s8 AudioMusic_Create(void);
s8 AudioIT_Create(void);
s8 AudioMIDI_Create(void);
s8 AudioVoice_Create(void);
s8 Audio_Create_(void);
