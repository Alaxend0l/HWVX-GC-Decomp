#include "common.h"


void ExitStateGamePlay(s32 arg0) {
    extern s8 g_bExitStateGamePlay;
    extern s32 g_targetGameState;
    g_targetGameState = arg0;
    g_bExitStateGamePlay = 1;
}

void InterfaceInputAnimator_Release(void) {
    static const char name[] = "InterfaceInput";
    ProcessManager_Release((char *)name);
}

void InterfaceInputAnimator_SetCounter(s32 which, f32 value) {
    extern f32 g_LR_Counter;
    extern f32 g_UD_Counter;
    switch (which) {
    case 0:
        g_LR_Counter = value;
        break;
    case 1:
        g_UD_Counter = value;
        break;
    }
}

void InterfaceInputAnimator_Count(s32 arg0, s32 arg1, f32 farg0) {
    extern s32 g_LR_Direction;
    extern f32 g_LR_Max_Min;
    extern s32 g_UD_Direction;
    extern f32 g_UD_Max_Min;
    switch (arg0) {                                 /* irregular */
    case 0:
        g_LR_Max_Min = farg0;
        g_LR_Direction = arg1;
        return;
    case 1:
        g_UD_Max_Min = farg0;
        g_UD_Direction = arg1;
        return;
    }
}

f32 InterfaceInputAnimator_GetLRCounter(void) {
    extern f32 g_LR_Counter;
    return g_LR_Counter;
}

f32 InterfaceInputAnimator_GetUDCounter(void) {
    extern f32 g_LR_Counter;
    return g_LR_Counter;
}

