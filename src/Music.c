#include "common.h"

/* main/Music.  Song state: the two raw volumes are combined into the one the
 * player uses, and every per-track hook starts out as AudioVoid. */
extern u32 g_musicVol1;
extern u32 g_musicVol2;
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
