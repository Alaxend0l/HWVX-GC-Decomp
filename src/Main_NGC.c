#include "common.h"

#pragma use_lmw_stmw on


void ForceIPLReset(void) {
    extern s32 m_ilpreset;
    extern s32 m_reset;
    m_ilpreset = 1;
    m_reset = 1;
}

s32 Init_Display(void) {
    extern s32 g_viewID;
    s32 Gfx_CreateView(s32 a, s32 w, s32 h, s32 d, s32 e, s32 f);

    g_viewID = Gfx_CreateView(0, 640, 480, 16, 0, 0);
    return g_viewID >= 0;
}

void Main_Shutdown_NGC(void) {

}

s32 RunThread(void *(*func)(void *)) {
    s32 OSCreateThread(void *thread, void *(*func)(void *), void *param, void *stack, u32 stackSize, s32 priority, u16 attr);
    s32 OSDisableInterrupts(void);
    s32 OSRestoreInterrupts(s32 level);
    s32 OSResumeThread(void *thread);
    extern u8 Thread[0x310];
    extern u8 ThreadStack[0x5000];
    extern s8 NGC_Thread_On;
    s32 enabled;

    OSCreateThread(Thread, func, 0, ThreadStack + 0x5000, 0x5000, 0x10, 1);
    enabled = OSDisableInterrupts();
    OSResumeThread(Thread);
    OSRestoreInterrupts(enabled);
    NGC_Thread_On = 1;
    return 1;
}

s8 isInThread(void) {
    void *OSGetCurrentThread(void);
    extern u8 Thread[0x310];

    return (s8)(Thread == OSGetCurrentThread());
}

void EndThread(void) {
    void OSExitThread(void *val);
    extern s8 NGC_Thread_On;
    s32 val;

    OSExitThread(&val);
    NGC_Thread_On = 0;
}

void TextureManager_ReleaseCache(void) {

}

void *OSGetArenaLo(void);
void *OSGetArenaHi(void);
void OSSetArenaLo(void *addr);
void *OSInitAlloc(void *arenaStart, void *arenaEnd, s32 maxHeaps);
s32 OSCreateHeap(void *start, void *end);
s32 OSSetCurrentHeap(s32 heap);
void InitMem(void) {
    typedef struct {
        u32 viTVmode;
        u16 fbWidth;
        u16 efbHeight;
        u16 xfbHeight;
    } RenderMode;
    extern RenderMode *rmode;
    extern void *g_frameBuffer1;
    extern void *g_frameBuffer2;
    extern void *g_currentBuffer;
    void *arenaLo;
    void *arenaHi;
    u32 fbSize;

    arenaLo = OSGetArenaLo();
    arenaHi = OSGetArenaHi();
    fbSize = (u16)(((u16)rmode->fbWidth + 15) & ~15) * rmode->xfbHeight * 2;
    g_frameBuffer1 = (void *)(((u32)arenaLo + 0x1F) & ~0x1F);
    g_frameBuffer2 = (void *)(((u32)g_frameBuffer1 + fbSize + 0x1F) & ~0x1F);
    g_currentBuffer = g_frameBuffer2;
    arenaLo = (void *)(((u32)g_frameBuffer2 + fbSize + 0x1F) & ~0x1F);
    OSSetArenaLo(arenaLo);

    arenaLo = OSGetArenaLo();
    arenaHi = OSGetArenaHi();
    arenaLo = OSInitAlloc(arenaLo, arenaHi, 1);
    OSSetArenaLo(arenaLo);
    arenaLo = (void *)(((u32)arenaLo + 0x1F) & ~0x1F);
    arenaHi = (void *)((u32)arenaHi & ~0x1F);
    OSSetCurrentHeap(OSCreateHeap(arenaLo, arenaHi));
    OSSetArenaLo(arenaHi);
}

void InitRenderMode(void *mode) {
    extern void *rmode;
    extern u8 GXNtsc480IntDf[];
    extern u8 GXPal528IntDf[];
    extern u8 GXMpal480IntDf[];
    extern u8 rmodeobj[];
    void OSPanic(const char *file, s32 line, const char *msg, ...);
    s32 VIGetTvFormat(void);
    void GXAdjustForOverscan(void *rmin, void *rmout, u16 hor, u16 ver);
    void VIConfigure(void *rm);
    void InitVI(void);

    if (mode != NULL) {
        rmode = mode;
    } else {
        switch (VIGetTvFormat()) {
        case 0:
            rmode = GXNtsc480IntDf;
            break;
        case 1:
            rmode = GXPal528IntDf;
            break;
        case 2:
            rmode = GXMpal480IntDf;
            break;
        default:
            OSPanic("Main_NGC.c", 366, "DEMOInit: invalid TV format\n");
            break;
        }
        GXAdjustForOverscan(rmode, rmodeobj, 0, 16);
        rmode = rmodeobj;
    }
    VIConfigure(rmode);
    InitVI();
}

void InitVI(void) {
    void VIFlush(void);
    void VISetNextFrameBuffer(void *fb);
    void VIWaitForRetrace(void);
    extern void *g_currentBuffer;
    extern void *g_frameBuffer1;
    extern void *g_frameBuffer2;
    extern u32 *rmode;

    VISetNextFrameBuffer(g_frameBuffer1);
    g_currentBuffer = g_frameBuffer2;
    VIFlush();
    VIWaitForRetrace();
    if (*rmode & 1) {
        VIWaitForRetrace();
    }
}

void GetSystemDate(s32 *year, s32 *month, s32 *day, s32 *hour, s32 *minute) {
    s64 OSGetTime(void);
    void OSTicksToCalendarTime(s64 ticks, void *td);
    s32 td[10];

    OSTicksToCalendarTime(OSGetTime(), td);
    *year = td[5];
    *month = td[4] + 1;
    *day = td[3];
    *hour = td[2];
    *minute = td[1];
}
