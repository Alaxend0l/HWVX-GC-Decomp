/* Functions from 0x80097BE8 to 0x800CC320, found by decomp-toolkit. */
#include "common.h"

void fwrite(void) {
    M2C_UNK __fwrite();
    __fwrite();
}

void fseek(void) {
    M2C_UNK fn_8009A088();
    fn_8009A088();
}

void srand(s32 arg0) {
    extern s32 next_803C6EA0;
    next_803C6EA0 = arg0;
}

void acos(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

void atan2(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

void pow(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

f32 tanf(void) {
    f64 tan();
    return (f32) tan();
}

f32 sinf(void) {
    f64 sin();
    return (f32) sin();
}

f32 cosf(void) {
    f64 cos();
    return (f32) cos();
}

void fn_8009C888(s32 arg0) {
    extern s32 lbl_803C6EA0;
    lbl_803C6EA0 = arg0;
}

void fn_8009FAD8(void) {
    M2C_UNK fn_8009CE30();
    fn_8009CE30();
}

void fn_8009FAF8(void) {
    M2C_UNK fn_8009D140();
    fn_8009D140();
}

void fn_8009FB18(void) {
    M2C_UNK fn_8009D3D0();
    fn_8009D3D0();
}

f32 fn_8009FB38(void) {
    f64 fn_8009FA60();
    return (f32) fn_8009FA60();
}

f32 fn_8009FB5C(void) {
    f64 fn_8009F988();
    return (f32) fn_8009F988();
}

f32 fn_8009FB80(void) {
    f64 fn_8009F420();
    return (f32) fn_8009F420();
}

void EXI2_Init(void) {

}

void EXI2_EnableInterrupts(void) {

}

s32 EXI2_Poll(void) {
    return 0;
}

s32 EXI2_ReadN(void) {
    return 0;
}

s32 EXI2_WriteN(void) {
    return 0;
}

void EXI2_Reserve(void) {

}

void EXI2_Unreserve(void) {

}

s32 AMC_IsStub(void) {
    return 1;
}

s32 Hu_IsStub(void) {
    return 0;
}

void DBClose(void) {

}

void DBOpen(void) {

}

f32 ArcTan(void) {
    f64 atan2();
    return (f32) atan2();
}

void ForceIPLReset(void) {
    extern s32 m_ilpreset_803C7620;
    extern s32 m_reset;
    m_ilpreset_803C7620 = 1;
    m_reset = 1;
}

void Main_Shutdown_NGC(void) {

}

void EndThread(void) {
    M2C_UNK OSExitThread(M2C_UNK *);
    extern s8 NGC_Thread_On;
    M2C_UNK sp8;

    OSExitThread(&sp8);
    NGC_Thread_On = 0;
}

void TextureManager_ReleaseCache(void) {

}

void MemCpy32(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

void HeapDump(void) {

}

f32 Distance_Plane(void *arg0, void *arg1) {
    return M2C_FIELD(arg0, f32 *, 0xC) + ((M2C_FIELD(arg0, f32 *, 8) * M2C_FIELD(arg1, f32 *, 8)) + ((M2C_FIELD(arg0, f32 *, 0) * M2C_FIELD(arg1, f32 *, 0)) + (M2C_FIELD(arg0, f32 *, 4) * M2C_FIELD(arg1, f32 *, 4))));
}

s8 Timer_Interrupt300(void) {
    extern u32 g_timer300;
    return (s8) ((u32) g_timer300 >> 0x1FU);
}

void Performance_Start(void) {

}

void Performance_End(void) {

}

void AnimExtractKeyFrame_NonCompound(void) {

}

void FontManager_Create(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

void FontManager_Destroy(void) {

}

void Gfx_AnimExtractKeyFrame(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

void Gfx_AnimTraverseHeirarchyToMatrices(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

void Gfx_DisplayList_Destroy(void) {
    extern s32 g_pDL;
    extern s32 g_pGDL;
    Deallocate(g_pGDL);
    Deallocate(g_pDL);
}

void Gfx_Add2DRectangleBack(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

void Gfx_Destroy(void) {
    M2C_UNK Gfx_DisplayList_Destroy();
    M2C_UNK Gfx_TextureHandler_Destroy();
    M2C_UNK Render_Destroy();
    M2C_UNK FontManager_Destroy();
    FontManager_Destroy();
    Gfx_DisplayList_Destroy();
    Gfx_TextureHandler_Destroy();
    Render_Destroy();
}

void Gfx_CreateView(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

void Gfx_EndScene(void) {
    M2C_UNK Render_EndScene();
    extern s8 g_screenTextX;
    extern s8 g_screenTextY;
    Render_EndScene();
    g_screenTextY = 0;
    g_screenTextX = 0;
}

s32 Gfx_GetRenderWindowIndex(void) {
    extern s32 g_currentWindow;
    return g_currentWindow;
}

void Gfx_ClearScreen(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

void Gfx_LoadPalettes_RAM(void) {

}

void MovieManager_Destroy(void) {

}

s32 MovieManager_GetTextureIndex(void) {
    return 0;
}

void Render_ClearScreen(void) {

}

void Render_Destroy(void) {

}

void Gfx_GetViewport(s32 *arg0, s32 *arg1, s32 *arg2, s32 *arg3) {
    *arg0 = 0;
    *arg1 = 0;
    *arg2 = 0x280;
    *arg3 = 0x1E0;
}

f32 Gfx_GetProjectionMatrix(void) {
    void *GetRenderWindow();
    return M2C_FIELD(GetRenderWindow(), f32 *, 0x270);
}

void Gfx_SetTextColor(s8 arg0, s8 arg1, s8 arg2, s8 arg3) {
    extern M2C_UNK g_textcolor;
    M2C_FIELD(&g_textcolor, s8 *, 0) = arg0;
    M2C_FIELD(&g_textcolor, s8 *, 1) = arg1;
    M2C_FIELD(&g_textcolor, s8 *, 2) = arg2;
    M2C_FIELD(&g_textcolor, s8 *, 3) = arg3;
}

void Mpeg_Create(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

void Mpeg_Render(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

void Mpeg_Destroy(void) {

}

void THPPlayerQuit(void) {
    extern s32 Initialized_803C7758;
    LCDisable();
    Initialized_803C7758 = 0;
}

void AudioDecoder(void) {
    M2C_UNK AudioDecode();
    s32 PopReadedBuffer();
    M2C_UNK PushReadedBuffer2(s32);
    s32 temp_r31;

loop_1:
    temp_r31 = PopReadedBuffer();
    AudioDecode();
    PushReadedBuffer2(temp_r31);
    goto loop_1;
}

void DestroyMIDI_(s32 arg0) {
    M2C_UNK FreeA_(s32);
    M2C_UNK OSFreeToHeap(s32, s32);
    extern s32 __OSCurrHeap;
    extern s32 g_aheap;
    OSFreeToHeap(__OSCurrHeap, arg0);
    FreeA_(g_aheap);
}

void Audio_Destroy_(void) {
    M2C_UNK AIReset();
    M2C_UNK ARQReset();
    M2C_UNK ARReset();
    M2C_UNK AXQuit();
    AXQuit();
    AIReset();
    ARQReset();
    ARReset();
}

void Audio_Update_(void) {
    M2C_UNK Audio_Update(M2C_UNK);
    Audio_Update(0xD05);
}

void MemCpyDone(void) {
    extern s32 g_arqWait_803C78B0;
    g_arqWait_803C78B0 = 0;
}

void AudioVoice_Destroy(void) {
    M2C_UNK AudioVoice_Destroy_();
    extern s32 VoiceUpdate;
    VoiceUpdate = 0;
    AudioVoice_Destroy_();
}

void SetSpeakerMode(s8 arg0) {
    extern s8 g_speakers;
    g_speakers = arg0;
}

void ProcessVoice(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

void AudioIT_Destroy(void) {

}

s32 AudioMIDI_Init(void) {
    M2C_UNK DestroyMIDI();
    extern u32 g_pHdr_803C78F4;
    if ((u32) g_pHdr_803C78F4 != 0U) {
        DestroyMIDI();
    }
    return 1;
}

void AudioMIDI_Destroy(void) {

}

void SetMIDITrackPriority(void) {

}

void SetMIDITrackVol(void) {

}

s32 AudioVoid(void) {
    return 0;
}

void FileManager_Destroy(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

void Input_SetAlternativeForceFeedback(void) {

}

void Input_ForceFeedbackPause(void) {

}

s32 Input_GetContollersPluggedIn(void) {
    return 0xF;
}

void Wheel_PlayAllAirborne(void) {

}

void GBAConnectivityLoop__Fv(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

s32 __ct__8ConstantFv(s32 arg0) {
    M2C_UNK __ct__5ForceFv();
    __ct__5ForceFv();
    return arg0;
}

s32 __ct__5ForceFv(s32 arg0) {
    M2C_UNK InitVars__5ForceFv();
    InitVars__5ForceFv();
    return arg0;
}

void InitLGLibrary__8LGWheelsFv(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

void IsOpened__8LGWheelsFl(s32 arg0) {
    M2C_UNK IsOpened__6WheelsFl(s32);
    IsOpened__6WheelsFl(arg0 + 0x82C);
}

void IsConnected__8LGWheelsFl(s32 arg0) {
    M2C_UNK IsConnected__6WheelsFl(s32);
    IsConnected__6WheelsFl(arg0 + 0x82C);
}

void PedalsConnected__8LGWheelsFl(s32 arg0) {
    M2C_UNK PedalsConnected__6WheelsFl(s32);
    PedalsConnected__6WheelsFl(arg0 + 0x82C);
}

s32 __ct__8PeriodicFv(s32 arg0) {
    M2C_UNK __ct__5ForceFv();
    __ct__5ForceFv();
    return arg0;
}

s32 __ct__4RampFv(s32 arg0) {
    M2C_UNK __ct__5ForceFv();
    __ct__5ForceFv();
    return arg0;
}

s32 __ct__9ConditionFv(s32 arg0) {
    M2C_UNK __ct__5ForceFv();
    __ct__5ForceFv();
    return arg0;
}

f32 fn_800A08D4(void) {
    f64 fn_8009FAF8();
    return (f32) fn_8009FAF8();
}

void fn_800A2450(void) {
    extern s32 lbl_803C7620;
    extern s32 lbl_803C7624;
    lbl_803C7620 = 1;
    lbl_803C7624 = 1;
}

void fn_800A24A4(void) {

}

void fn_800A2588(void) {

}

void fn_800A2864(void) {
    M2C_UNK memcpy();
    memcpy();
}

void fn_800A28AC(void) {

}

s8 fn_800A2F84(void) {
    extern u32 lbl_803C6EC0;
    return (s8) ((u32) lbl_803C6EC0 >> 0x1FU);
}

void fn_800A3184(void) {

}

void fn_800A3188(void) {

}

void fn_800A324C(void) {

}

void fn_800A3C74(void) {
    M2C_UNK fn_800A3C98();
    fn_800A3C98();
}

void fn_800A3C94(void) {

}

void fn_800A4C1C(void) {
    M2C_UNK fn_800A3A44();
    fn_800A3A44();
}

void fn_800A4C3C(void) {
    M2C_UNK fn_800A3A98();
    fn_800A3A98();
}

void fn_800A5BEC(void) {
    M2C_UNK fn_800B1E04();
    fn_800B1E04();
}

void fn_800A7CDC(void) {
    M2C_UNK fn_800AA91C();
    fn_800AA91C();
}

s32 fn_800A7FC4(void) {
    extern s32 lbl_803C767C;
    return lbl_803C767C;
}

void fn_800A830C(void) {
    M2C_UNK fn_800AC00C();
    fn_800AC00C();
}

void fn_800A9A84(void) {

}

void fn_800A9B50(void) {

}

s32 fn_800A9B54(void) {
    return 0;
}

void fn_800AC00C(void) {

}

void fn_800AC30C(void) {

}

void fn_800AC310(s32 *arg0, s32 *arg1, s32 *arg2, s32 *arg3) {
    *arg0 = 0;
    *arg1 = 0;
    *arg2 = 0x280;
    *arg3 = 0x1E0;
}

f32 fn_800AF400(void) {
    void *fn_800A7FCC();
    return M2C_FIELD(fn_800A7FCC(), f32 *, 0x270);
}

void fn_800B1F50(s8 arg0, s8 arg1, s8 arg2, s8 arg3) {
    extern M2C_UNK lbl_803C7720;
    M2C_FIELD(&lbl_803C7720, s8 *, 0) = arg0;
    M2C_FIELD(&lbl_803C7720, s8 *, 1) = arg1;
    M2C_FIELD(&lbl_803C7720, s8 *, 2) = arg2;
    M2C_FIELD(&lbl_803C7720, s8 *, 3) = arg3;
}

void fn_800B2C58(void) {
    M2C_UNK fn_800B2C78();
    fn_800B2C78();
}

void fn_800B302C(void) {
    M2C_UNK fn_800ABB90();
    fn_800ABB90();
}

void fn_800B304C(void) {

}

void fn_800B31FC(void) {
    M2C_UNK LCDisable();
    extern s32 lbl_803C7758;
    LCDisable();
    lbl_803C7758 = 0;
}

void fn_800B4948(void) {
    s32 fn_800B46D8();
    M2C_UNK fn_800B47D4(s32);
    M2C_UNK fn_800B4A18();
    s32 temp_r31;

loop_1:
    temp_r31 = fn_800B46D8();
    fn_800B4A18();
    fn_800B47D4(temp_r31);
    goto loop_1;
}

void fn_800BDD14(void) {
    M2C_UNK fn_800C293C(M2C_UNK);
    fn_800C293C(0xD05);
}

void fn_800BE358(void) {
    extern s32 lbl_803C78B0;
    lbl_803C78B0 = 0;
}

void fn_800BE510(void) {
    M2C_UNK fn_800BC868();
    extern s32 lbl_803C7940;
    lbl_803C7940 = 0;
    fn_800BC868();
}

void fn_800BE538(s8 arg0) {
    extern s8 lbl_803C78CC;
    lbl_803C78CC = arg0;
}

void fn_800BE568(void) {
    M2C_UNK fn_800BCEA0();
    fn_800BCEA0();
}

void fn_800BED54(void) {

}

void fn_800C08E0(void) {

}

void fn_800C1020(void) {

}

void fn_800C1108(void) {

}

s32 fn_800C267C(void) {
    return 0;
}

void fn_800C6B00(void) {
    M2C_UNK fn_800C6B20();
    fn_800C6B20();
}

void fn_800C8810(void) {

}

void fn_800C8814(void) {

}

s32 fn_800C8818(void) {
    return 0xF;
}

void fn_800C8B50(void) {

}

void fn_800CA8C8(void) {
    M2C_UNK fn_800CA1D0();
    fn_800CA1D0();
}

void fn_800CAE68(void) {
    M2C_UNK fn_800C2FEC();
    fn_800C2FEC();
}

void fn_800CB9CC(s32 arg0) {
    M2C_UNK fn_800CBEF4(s32);
    fn_800CBEF4(arg0 + 0x82C);
}

void fn_800CB9F0(s32 arg0) {
    M2C_UNK fn_800CBF10(s32);
    fn_800CBF10(arg0 + 0x82C);
}

void fn_800CBA14(s32 arg0) {
    M2C_UNK fn_800CBF78(s32);
    fn_800CBF78(arg0 + 0x82C);
}

