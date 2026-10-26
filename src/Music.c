#include "common.h"

/* main/Music.  Song state: the two raw volumes are combined into the one the
 * player uses, and every per-track hook starts out as AudioVoid. */
extern volatile u32 g_musicVol1;
extern volatile u32 g_musicVol2;
extern u32 g_musicVol;
extern u8 g_musicTitle[0x20];

extern s32 (*SetMusicState)(void);
extern s32 (*DestroyMusic)(void);
extern s32 (*SetMusicVolume)(void);
extern s32 (*SetMusicPitch)(void);
extern s32 (*SetMusicTempo)(void);
extern s32 (*SetMusicOrder)(void);
extern s32 (*SetMusicTrackPriority)(void);
extern s32 (*SetMusicTrackOn)(void);
extern s32 (*SetMusicTrackOff)(void);
extern s32 (*SetMusicTrackVol)(void);
s32 AudioVoid(void);

#pragma push
#pragma optimization_level 1
s32 AudioMusic_Init(void)
{
    void *memset(void *dst, s32 c, u32 n);

    SetMusicState = AudioVoid;
    DestroyMusic = AudioVoid;
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

void SetMusicVol(u32 vol) { u32 p; g_musicVol1 = vol; p = g_musicVol2 * g_musicVol1; g_musicVol = p >> 15; SetMusicVolume(); }

void SetMusicGVol(u32 vol) { u32 p; g_musicVol2 = vol; p = g_musicVol1 * g_musicVol2; g_musicVol = p >> 15; SetMusicVolume(); }

s32 AudioVoid(void)
{
	return 0;
}
