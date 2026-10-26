#include "Input.h"

typedef struct RumbleStatus {
    s32 duration;
    s32 active;
} RumbleStatus;

typedef struct InputUnk108 {
    u16 button;
    u8 unk2[4];
    u8 unk6;
    u8 unk7;
    u8 unk8[2];
    s8 unkA;
    u8 unkB;
} InputUnk108;

static RumbleStatus rumbleStatus[4];
static s32 s_rumbleTime[4];
static u8 s_unk30[0x40];
static u8 s_unk70[0x40];
static u16 s_keyMap[20];
static PADStatus s_unkD8[4];

void Input_Destroy(void) {
    M2C_UNK PADControlMotor(M2C_UNK, M2C_UNK);
    PADControlMotor(0, 0);
    PADControlMotor(1, 0);
    PADControlMotor(2, 0);
    PADControlMotor(3, 0);
}

void Input_SetForceFeedback(s32 chan, f32 strength, f32 duration) {
    void PADControlMotor(s32 chan, u32 command);
    if (g_VirtualPad[chan].forceFeedbackActive) {
        rumbleStatus[chan].duration = 32.0f * duration * strength;
        rumbleStatus[chan].active = 1;
        PADControlMotor(chan, 1);
    }
}

void Input_SetAlternativeForceFeedback(void) {

}

void Input_ForceFeedbackPause(void) {

}

