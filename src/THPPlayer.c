#include "common.h"

typedef struct THPPlayer {
    u8 fileInfo[0x3C];
    u8 magic[0x30];
    u32 numComponents;
    u8 components[0x80 - 0x70];
    u8 videoInfo[8];
    u8 audioInfo[0xC];
    u8 state;
    u8 unk9D;
    u8 playFlag;
    u8 audioExist;
    u8 unk9F_pad[3];
    u32 onMemory;
} THPPlayer;

extern THPPlayer ActivePlayer;
extern u8 WorkBuffer[0x40];
static s32 Initialized;
static s32 m_cd_drive_status;
static s32 m_reset;
extern s64 g_resetTicks;

s32 DVDOpen(char *fileName, void *fileInfo);
s32 DVDClose(void *fileInfo);
s32 DVDReadPrio(void *fileInfo, void *addr, s32 length, s32 offset, s32 prio);
void DVDReadAsyncPrio(void *fileInfo, void *addr, s32 length, s64 offset, s32 prio);
s32 DVDGetDriveStatus(void);
void Render_BeginScene(void);
void Render_EndScene(void);
void Render_FlipBuffers(void);
s64 OSGetTime(void);
s32 OSGetResetButtonState(void);
void VISetBlack(s32 black);
void VIWaitForRetrace(void);
void VIFlush(void);
void OSResetSystem(s32 reset, u32 resetCode, s32 forceMenu);
void *memset(void *dst, s32 c, u32 n);
void *memcpy(void *dst, const void *src, u32 n);
s32 strcmp(const char *a, const char *b);
