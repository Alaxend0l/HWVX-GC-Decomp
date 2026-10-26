#include "common.h"

/* main/Music.  Song state: the two raw volumes are combined into the one the
 * player uses, and every per-track hook starts out as AudioVoid. */
extern u32 g_musicVol1;
extern u32 g_musicVol2;
extern u32 g_musicVol;
u8 g_musicTitle[0x20];

u32 g_musicVol1;
u32 g_musicVol2;
u32 g_musicVol;
u32 g_trkOn;
u32 g_trkOff;
s32 g_musicState;
s32 (*DestroyMusic)(void);
s32 (*SetMusicState)(void);
s32 (*SetMusicVolume)(void);
s32 (*SetMusicPitch)(void);
s32 (*SetMusicTempo)(void);
s32 (*SetMusicOrder)(void);
s32 (*SetMusicTrackPriority)(void);
s32 (*SetMusicTrackOn)(void);
s32 (*SetMusicTrackOff)(void);
s32 (*SetMusicTrackVol)(void);
s32 AudioVoid(void);

#pragma push
#pragma optimization_level 1
s32 AudioMusic_Create(void)
{
    extern s32 g_musicState;

    g_musicVol = 0x7FFF;
    g_musicVol1 = 0x7FFF;
    g_musicVol2 = 0x7FFF;
    g_musicState = 0;
    g_musicTitle[0x1F] = 0;
    return 1;
}

s32 AudioMusic_Init(void)
{
    void *memset(void *dst, s32 c, u32 n);

    DestroyMusic = AudioVoid;
    SetMusicState = AudioVoid;
    SetMusicVolume = AudioVoid;
    SetMusicPitch = AudioVoid;
    SetMusicTempo = AudioVoid;
    SetMusicOrder = AudioVoid;
    SetMusicTrackPriority = AudioVoid;
    SetMusicTrackOn = AudioVoid;
    SetMusicTrackOff = AudioVoid;
    SetMusicTrackVol = AudioVoid;
    memset(g_musicTitle, 0, 0x20);
    return 1;
}
#pragma pop

#pragma push
#pragma optimization_level 1
void SetMusicVol(u32 vol)
{
    g_musicVol1 = vol;
    g_musicVol = (g_musicVol2 * g_musicVol1) >> 15;
    SetMusicVolume();
}
void SetMusicGVol(u32 vol)
{
    g_musicVol2 = vol;
    g_musicVol = (g_musicVol1 * g_musicVol2) >> 15;
    SetMusicVolume();
}
#pragma pop

s32 AudioVoid(void)
{
	return 0;
}

#pragma push
#pragma optimization_level 1
void CreateMusic(u8 *data, u32 size)
{
    s8 CmpType_(u8 *chunk, s32 type);
    s8 CmpID_(u8 *chunk, s32 id);
    void CreateMIDI(u8 *data, u32 size);
    void CreateIT(u8 *data, u32 size);
    extern s32 DestroyMIDI();
    extern s32 SetMIDIState();
    extern s32 SetMIDIVol();
    extern s32 SetMIDIPitch();
    extern s32 SetMIDITempo();
    extern s32 SetMIDIMarker();
    extern s32 SetMIDITrackPriority();
    extern s32 SetMIDITrackOn();
    extern s32 SetMIDITrackOff();
    extern s32 SetMIDITrackVol();
    extern s32 DestroyIT();
    extern s32 SetITState();
    extern s32 SetITVol();
    extern s32 SetITPitch();
    extern s32 SetITTempo();
    extern s32 SetITOrder();
    extern s32 SetITTrackPriority();
    extern s32 SetITTrackOn();
    extern s32 SetITTrackOff();
    extern s32 SetITTrackVol();

    if (data && size) {
        if (CmpType_(data, 'MIDI')) {
            DestroyMusic = DestroyMIDI;
            SetMusicState = SetMIDIState;
            SetMusicVolume = SetMIDIVol;
            SetMusicPitch = SetMIDIPitch;
            SetMusicTempo = SetMIDITempo;
            SetMusicOrder = SetMIDIMarker;
            SetMusicTrackPriority = SetMIDITrackPriority;
            SetMusicTrackOn = SetMIDITrackOn;
            SetMusicTrackOff = SetMIDITrackOff;
            SetMusicTrackVol = SetMIDITrackVol;
            CreateMIDI(data, size);
        }
        else if (CmpID_(data, 'IMPM')) {
            DestroyMusic = DestroyIT;
            SetMusicState = SetITState;
            SetMusicVolume = SetITVol;
            SetMusicPitch = SetITPitch;
            SetMusicTempo = SetITTempo;
            SetMusicOrder = SetITOrder;
            SetMusicTrackPriority = SetITTrackPriority;
            SetMusicTrackOn = SetITTrackOn;
            SetMusicTrackOff = SetITTrackOff;
            SetMusicTrackVol = SetITTrackVol;
            CreateIT(data, size);
        }
    }
}
#pragma pop

