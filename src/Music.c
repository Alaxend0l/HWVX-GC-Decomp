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
