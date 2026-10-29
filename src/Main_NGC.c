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

void TextureManager_ReleaseCache(void) {

}

