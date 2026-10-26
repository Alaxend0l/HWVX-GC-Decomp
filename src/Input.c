#include "Input.h"

void Input_SetForceFeedbackActive(s32 chan, u8 active) {
    g_VirtualPad[chan].forceFeedbackActive = active;
}
