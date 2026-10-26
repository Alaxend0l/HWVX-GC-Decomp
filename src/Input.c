#include "Input.h"

s32 Input_GetPlayerDevice(s32 player) {
    s16 i;

    player--;
    if ((u32)player > 4) {
        for (i = 0; i < 4; i++) {
            if ((s8)g_VirtualPad[i].unk0[1] == 0) {
                g_VirtualPad[i].unk0[1] = 1;
                g_VirtualPad[i].unk0[2] = 1;
                return i;
            }
        }
        return -2;
    }
    g_VirtualPad[player].unk0[1] = 1;
    g_VirtualPad[player].unk0[2] = 1;
    return player;
}

void Input_SetForceFeedbackActive(s32 chan, u8 active) {
    g_VirtualPad[chan].forceFeedbackActive = active;
}
