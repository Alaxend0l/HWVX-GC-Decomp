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
static InputUnk108 s_unk108[4];
extern u16 g_pKeyToDI[20];
extern void *wheels;
u8 IsOpened__8LGWheelsFl(void *wheels, s32 chan);
void PlaySpringForce__8LGWheelsFlScUcs(void *wheels, s32 chan, s8 offset, u8 saturation, s16 coefficient);
void PlayFrontalCollisionForce__8LGWheelsFlUc(void *wheels, s32 chan, u8 force);

void InitTables__Fv(void);
s32 Input_Create(void) {
    void Input_Init(void);
    void SISetSamplingRate(u32 msec);
    void *__nw__FUl(u32 size);
    void *__ct__8LGWheelsFv(void *self);
    void GenerateNonLinValues__8LGWheelsFlUc(void *self, s32 chan, u8 value);
    void PadSamplingCallback__Fv(void);
    void *PADSetSamplingCallback(void *callback);
    void GBAConnectivityInit__Fv(void);
    void *w;

    InitTables__Fv();
    Input_Init();
    SISetSamplingRate(1);
    w = __nw__FUl(0x1430);
    if (w != NULL) {
        w = __ct__8LGWheelsFv(w);
    }
    wheels = w;
    GenerateNonLinValues__8LGWheelsFlUc(wheels, 0, 0x50);
    GenerateNonLinValues__8LGWheelsFlUc(wheels, 1, 0x64);
    GenerateNonLinValues__8LGWheelsFlUc(wheels, 2, 0x64);
    GenerateNonLinValues__8LGWheelsFlUc(wheels, 3, 0x64);
    PADSetSamplingCallback(PadSamplingCallback__Fv);
    GBAConnectivityInit__Fv();
    return 1;
}

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

s32 Input_GetContollersPluggedIn(void) {
    return 0xF;
}

void InitTables__Fv(void) {
    memset(g_pKeyToDI, 0, sizeof(g_pKeyToDI));
    g_pKeyToDI[0] = 0x1000;
    g_pKeyToDI[1] = 0;
    g_pKeyToDI[2] = 1;
    g_pKeyToDI[3] = 2;
    g_pKeyToDI[4] = 8;
    g_pKeyToDI[5] = 4;
    g_pKeyToDI[6] = 0x200;
    g_pKeyToDI[7] = 0x400;
    g_pKeyToDI[8] = 0x800;
    g_pKeyToDI[9] = 0x100;
    g_pKeyToDI[10] = 0;
    g_pKeyToDI[11] = 0x40;
    g_pKeyToDI[12] = 0x10;
    g_pKeyToDI[13] = 0x20;
    g_pKeyToDI[14] = 0;
    g_pKeyToDI[15] = 0;
    g_pKeyToDI[16] = 0;
    g_pKeyToDI[17] = 0;
    g_pKeyToDI[18] = 0;
    g_pKeyToDI[19] = 0;
}

void Wheel_PlaySpringForce(s32 chan, s8 offset, u8 saturation, s16 coefficient) {
    if (IsOpened__8LGWheelsFl(wheels, chan)) {
        PlaySpringForce__8LGWheelsFlScUcs(wheels, chan, offset, saturation, coefficient);
    }
}

void Wheel_PlayDamperForce(s32 chan, s32 coefficient, s32 offset) {
    if (IsOpened__8LGWheelsFl(wheels, chan)) {
        PlayDamperForce__8LGWheelsFlUcs(wheels, chan, coefficient, offset);
    }
}

void Wheel_PlayConstantForce(s32 chan, s32 coefficient, s32 offset) {
    if (IsOpened__8LGWheelsFl(wheels, chan)) {
        PlayConstantForce__8LGWheelsFlsUs(wheels, chan, coefficient, offset);
    }
}

void Wheel_PlayCarAirborne(s32 chan) {
    if (IsOpened__8LGWheelsFl(wheels, chan)) {
        PlayCarAirborne__8LGWheelsFl(wheels, chan);
    }
}

void Wheel_StopCarAirborne(s32 chan) {
    if (IsOpened__8LGWheelsFl(wheels, chan)) {
        StopCarAirborne__8LGWheelsFl(wheels, chan);
    }
}

void Wheel_PlayFrontalCollisionForce(s32 chan, u8 force) {
    if (IsOpened__8LGWheelsFl(wheels, chan)) {
        PlayFrontalCollisionForce__8LGWheelsFlUc(wheels, chan, force);
    }
}

s8 Wheel_IsConnected(s32 arg0) {
    return IsConnected__8LGWheelsFl(wheels, arg0);
}

void Wheel_PlayAllAirborne(void) {

}

void PadSamplingCallback__Fv(void) {
    PADStatus pads[4];
    s32 i;

    PADRead(pads);
    PADClamp(pads);
    for (i = 0; i < 4; i++) {
        if (pads[i].err != -3) {
            memcpy(&ngcPad[i], &pads[i], sizeof(PADStatus));
        }
    }
}

