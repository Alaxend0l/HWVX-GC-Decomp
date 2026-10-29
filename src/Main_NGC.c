#include "common.h"


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
