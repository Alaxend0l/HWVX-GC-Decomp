#include "common.h"

typedef struct THPPlayer {
    u8 fileInfo[0x3C];
    u8 tag[4];
    u32 version;
    u8 pad44[0x18];
    u32 dataOffset;
    u8 pad60[0xC];
    u32 numComponents;
    u8 components[0x80 - 0x70];
    u8 videoInfo[8];
    u8 audioInfo[0xC];
    u8 pad94[4];
    BOOL open;
    u8 state;
    u8 unk9D;
    u8 playFlag;
    u8 audioExist;
    u8 padA0[5];
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

BOOL THPPlayerOpen(char *fileName, BOOL onMemory) {
    s32 offset;
    u32 i;

    if (!Initialized) {
        return FALSE;
    }
    if (ActivePlayer.open) {
        return FALSE;
    }
    memset(&ActivePlayer.videoInfo, 0, 8);
    memset(&ActivePlayer.audioInfo, 0, 0xC);
    if (!DVDOpen(fileName, &ActivePlayer)) {
        return FALSE;
    }
    DVDReadAsyncPrio(&ActivePlayer, WorkBuffer, 0x40, 0, 2);
    do {
        m_cd_drive_status = DVDGetDriveStatus();
        switch (m_cd_drive_status) {
        case -1:
        case 4:
        case 5:
        case 6:
        case 11:
            Render_BeginScene();
            Render_EndScene();
            Render_FlipBuffers();
            break;
        }
        if (OSGetResetButtonState()) {
            m_reset = 1;
        }
        if (m_reset && !OSGetResetButtonState()) {
            VISetBlack(1);
            VIWaitForRetrace();
            VIFlush();
            VIWaitForRetrace();
            VIFlush();
            VIWaitForRetrace();
            VISetBlack(1);
            VIWaitForRetrace();
            VIFlush();
            VIWaitForRetrace();
            VIFlush();
            VIWaitForRetrace();
            if ((OSGetTime() - g_resetTicks) / (*(u32 *)0x800000F8 / 4) < 8) {
                OSResetSystem(1, 0, 0);
            } else {
                OSResetSystem(0, 0, 0);
            }
        }
    } while (m_cd_drive_status != 0);
    memcpy(ActivePlayer.tag, WorkBuffer, 0x30);
    if (strcmp((char *)ActivePlayer.tag, "THP") != 0) {
        DVDClose(&ActivePlayer);
        return FALSE;
    }
    if (ActivePlayer.version != 0x10000) {
        DVDClose(&ActivePlayer);
        return FALSE;
    }
    offset = ActivePlayer.dataOffset;
    if (DVDReadPrio(&ActivePlayer, WorkBuffer, 0x20, offset, 2) < 0) {
        DVDClose(&ActivePlayer);
        return FALSE;
    }
    memcpy(&ActivePlayer.numComponents, WorkBuffer, 0x14);
    offset += 0x14;
    ActivePlayer.audioExist = 0;
    for (i = 0; i < ActivePlayer.numComponents; i++) {
        switch (ActivePlayer.components[i]) {
        case 0:
            if (DVDReadPrio(&ActivePlayer, WorkBuffer, 0x20, offset, 2) < 0) {
                DVDClose(&ActivePlayer);
                return FALSE;
            }
            memcpy(&ActivePlayer.videoInfo, WorkBuffer, 8);
            offset += 8;
            break;
        case 1:
            if (DVDReadPrio(&ActivePlayer, WorkBuffer, 0x20, offset, 2) < 0) {
                DVDClose(&ActivePlayer);
                return FALSE;
            }
            memcpy(&ActivePlayer.audioInfo, WorkBuffer, 0xC);
            ActivePlayer.audioExist = 1;
            offset += 0xC;
            break;
        default:
            return FALSE;
        }
    }
    ActivePlayer.unk9D = 0;
    ActivePlayer.state = 0;
    ActivePlayer.playFlag = 0;
    ActivePlayer.onMemory = onMemory;
    ActivePlayer.open = TRUE;
    return TRUE;
}
