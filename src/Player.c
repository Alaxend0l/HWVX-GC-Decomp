#include "common.h"


s8 Player_IsHuman(void *player) {
    extern void Player_DefaultControlReader(void);
    s32 isHuman = 0;

    if (player != NULL && *(void **)((u8 *)player + 0x20) == Player_DefaultControlReader) {
        isHuman = 1;
    }
    return isHuman;
}

void Player_Init(void *player, s32 arg1);
void PlayerHuman_Init(void *player) {
    typedef struct PlayerHuman {
        u8 pad0[0x6C];
        u32 flags;
        u8 pad70[0xC8 - 0x70];
        s32 wheelChannel;
        u8 padCC[0x114 - 0xCC];
        void *bindings;
        u8 bindingData[4];
    } PlayerHuman;
    void PlayerHuman_SetDefaultControls(void *player, s32 arg);
    PlayerHuman *human = player;

    Player_Init(player, 0);
    human->wheelChannel = 0;
    PlayerHuman_SetDefaultControls(player, 0);
    human->flags = 0;
    human->bindings = human->bindingData;
}

void Player_Init(void *player, s32 arg1) {
    s32 i;

    Control_ClearControl(player);
    M2C_FIELD(player, u8 *, 0x40) = 0;
    M2C_FIELD(player, u8 *, 0x31) = 0;
    M2C_FIELD(player, s32 *, 0x3C) = 0;
    M2C_FIELD(player, s32 *, 0x68) = -1;
    M2C_FIELD(player, s32 *, 0x6C) = 0;
    memset((u8 *)player + 0x70, 0, 0x38);
    M2C_FIELD(player, u8 *, 0x71) = 1;
    M2C_FIELD(player, s32 *, 0x34) = 0;
    for (i = 0; i < 8; i++) {
        ((s16 *)((u8 *)player + 0x94))[i] = 30;
    }
    M2C_FIELD(player, s16 *, 0xA2) = 0;
    M2C_FIELD(player, s32 *, 0x6C) = 0;
    M2C_FIELD(player, s16 *, 0xAC) = -1;
    M2C_FIELD(player, s16 *, 0xAE) = -1;
}

void Player_SetName(void *player, s32 port, const char *name) {
    char *strcpy(char *dst, const char *src);
    s32 Input_GetPlayerDevice(s32 port);

    strcpy((char *)player + 0x40, name);
    M2C_FIELD(player, s32 *, 0x60) = port;
    if (Player_IsHuman(player)) {
        M2C_FIELD(player, s32 *, 0xC8) = Input_GetPlayerDevice(M2C_FIELD(player, s32 *, 0x60));
    }
}

void Player_RefreshInterface(void *arg0) {
    s32 InterfaceManager_Create(u8 *script, void *player, s32 arg2);
    void InterfaceManager_ReleaseInterface(u32 index);
    void InterfaceManager_SetOffset(s32 index, f32 x, f32 y);
    typedef struct Thing {
        u8 pad0[0x8C];
        void (**vtable)();
    } Thing;
    typedef struct Player {
        u8 pad0[0x2C];
        Thing *thing;
        u8 pad30[0x68 - 0x30];
        u32 interfaceIndex;
        u32 flags;
        u8 pad70;
        u8 nameDirty;
    } Player;
    struct {
        u32 unk0;
        u8 *script;
        u32 x;
        u32 y;
        u8 unk10[0xC0];
    } info;
    Player *player = arg0;
    Thing *thing;

    if (player->thing != NULL && (player->flags & 0x10)) {
        memset(&info, 0, sizeof(info));
        player->flags |= 0x200;
        thing = player->thing;
        ((void (*)(Thing *, void *))thing->vtable[8])(thing, &info);
        if (player->interfaceIndex < 0xFFFFFFFF) {
            InterfaceManager_ReleaseInterface(player->interfaceIndex);
            player->interfaceIndex = 0xFFFFFFFF;
        }
        if (info.script != NULL) {
            player->interfaceIndex = InterfaceManager_Create(info.script, player, 0);
            InterfaceManager_SetOffset(player->interfaceIndex, info.x, info.y);
            player->nameDirty = 1;
        }
        player->flags &= ~0x200;
    }
}

void Player_ReleaseInterface(void *player) {
    void InterfaceManager_ReleaseInterface(u32 index);
    u32 index = M2C_FIELD(player, u32 *, 0x68);

    if (index != 0xFFFFFFFF) {
        InterfaceManager_ReleaseInterface(index);
    }
}

extern u8 g_carAVTable[];
extern u8 g_creatureVTable[];
void CarA_SetSlavePlayers(void *car);
void Thing_SetSlavePlayers(void *thing);
void AI_Car_Reset(void *player);
void Player_SetThing(void *player, void *thing) {
    s32 isCreature;

    *(void **)((u8 *)player + 0x2C) = thing;
    if (thing != NULL) {
        *(void **)(*(u8 **)((u8 *)player + 0x2C) + 0x94) = player;
        *(u32 *)((u8 *)player + 0x38) = *(u32 *)((u8 *)thing + 0xA8);
        if (*(void **)((u8 *)thing + 0x8C) == g_carAVTable) {
            *(u32 *)((u8 *)player + 0x3C) = *(u32 *)((u8 *)thing + 0x3BC);
            CarA_SetSlavePlayers(thing);
            if (!Player_IsHuman(player)) {
                AI_Car_Reset(player);
            }
        } else if (*(void **)((u8 *)thing + 0x8C) == g_creatureVTable) {
            *(u32 *)((u8 *)player + 0x3C) = *(u32 *)((u8 *)thing + 0xB8);
            Thing_SetSlavePlayers(thing);
        }
        if (*(void **)((u8 *)player + 0x2C) == thing) {
            Player_RefreshInterface(player);
        }
    }
}

s32 Player_InputHeartbeat(void *arg0) {
    return M2C_FIELD(arg0, s8 (**)(), 0x20)() != 0;
}

void Player_GetInterfaceInfo(void *arg0, void *arg1) {
    int sprintf(char *buf, const char *fmt, ...);
    typedef struct Thing {
        u8 pad0[0x8C];
        void (**vtable)();
    } Thing;
    typedef struct Player {
        u8 pad0[0x2C];
        Thing *thing;
        u8 attached;
        u8 pad31[0x71 - 0x31];
        u8 nameDirty;
        u8 pad72[0x7C - 0x72];
        s32 score;
    } Player;
    typedef struct InterfaceInfo {
        u8 pad0[0x74];
        char text[0xC];
        u8 state;
    } InterfaceInfo;
    Player *player = arg0;
    InterfaceInfo *info = arg1;
    Thing *thing;

    if (player->thing != NULL) {
        info->state = 0;
        thing = player->thing;
        ((void (*)(Thing *, InterfaceInfo *))thing->vtable[8])(thing, info);
    } else {
        info->state = 2;
        if (player->attached != 0) {
            info->state = 1;
            player->attached = 0;
        }
    }
    if (player->nameDirty != 0) {
        sprintf(info->text, "%d", player->score);
        player->nameDirty = 0;
    }
}

void Player_DetachThingDeath(void *arg0) {
    void *temp_r5;

    temp_r5 = M2C_FIELD(arg0, void **, 0x2C);
    M2C_FIELD(arg0, void **, 0x2C) = NULL;
    M2C_FIELD(arg0, s8 *, 0x30) = 1;
    if (temp_r5 != NULL) {
        M2C_FIELD(temp_r5, s32 *, 0x94) = 0;
    }
}

void PlayerHuman_Heartbeat(void *player) {
    void ForceFeedback_GetForce(void *ff, u8 *a, u8 *b, s32 *c);
    void Input_SetAlternativeForceFeedback(s32 input, u8 a, u8 b);
    void Player_Heartbeat(void *player);
    s32 c;
    u8 a;
    u8 b;
    void *car;

    car = M2C_FIELD(player, void **, 0x2C);
    if (car != NULL && !(M2C_FIELD(car, u32 *, 0x74) & 2)) {
        ForceFeedback_GetForce((u8 *)car + 0x40, &a, &b, &c);
        Input_SetAlternativeForceFeedback(M2C_FIELD(player, s32 *, 0xC8), a, b);
    }
    Player_Heartbeat(player);
}

void Player_Heartbeat()
{}

void PlayerHuman_SetDefaultControls(void *arg0, s32 index) {
    typedef struct { u8 unk0; u8 unk1; u8 keys[14]; } KeyBinding;
    typedef struct { u8 pad0[0xB8]; u8 keys[14]; u8 unkC6; u8 unkC7; } PlayerHuman;
    extern KeyBinding g_keyBinding[9];
    void *memset(void *dst, int c, u32 n);
    PlayerHuman *player = arg0;
    u16 i;

    memset(player->keys, 0, 0x10);
    for (i = 0; i < 14; i++) {
        player->keys[i] = 0xFF;
    }
    index++;
    for (i = 0; i < 14; i++) {
        player->keys[g_keyBinding[0].keys[i]] = g_keyBinding[index].keys[i];
    }
    player->unkC6 = g_keyBinding[index].unk0;
    player->unkC7 = g_keyBinding[index].unk1;
}

void PlayerHuman_SetControlBinding(void *arg0, void *arg1) {
    typedef struct { u8 unk0; u8 unk1; u8 keys[14]; } KeyBinding;
    typedef struct { u8 pad0[0xB8]; u8 keys[14]; u8 unkC6; u8 unkC7; } PlayerHuman;
    extern KeyBinding g_keyBinding[9];
    void *memset(void *dst, int c, u32 n);
    PlayerHuman *player = arg0;
    KeyBinding *binding = arg1;
    u16 i;

    memset(player->keys, 0, 0x10);
    for (i = 0; i < 14; i++) {
        player->keys[i] = 0xFF;
    }
    for (i = 0; i < 14; i++) {
        player->keys[g_keyBinding[0].keys[i]] = binding->keys[i];
    }
    player->unkC6 = binding->unk0;
    player->unkC7 = binding->unk1;
}

u8 Input_KeyDown(s32 device, u8 key);
s8 Input_KeyPress(s32 device, u8 key);

typedef struct PlayerHumanKeys {
    u8 pad0[0xB8];
    u8 keys[14];
    u8 stickMode;
    u8 substickMode;
    s32 device;
} PlayerHumanKeys;

static inline u8 PlayerHuman_KeyDown(PlayerHumanKeys *player, s32 key) {
    return Input_KeyDown(player->device, player->keys[key]);
}

static inline s8 PlayerHuman_KeyPress(PlayerHumanKeys *player, s32 key) {
    return Input_KeyPress(player->device, player->keys[key]);
}

s32 Player_NULL(void) {
    return 0;
}

void PlayerHuman_EvaluateCombo(u8 *player) {
    extern s8 g_bFreezeWorld;
    void TimeHandler_SetSlowMotion(void);
    void CameraManager_ReturnAllToPlayer(void);
    u32 *combo = (u32 *)(player + 0xD8);

    if (!(*(u32 *)(player + 0x6C) & 0x20)) {
        if (*(u32 *)(player + 0xCC) == 13 && (combo[0] & 0x800) && (combo[1] & 0x800) && (combo[2] & 0x800) &&
            (combo[3] & 4) && (combo[4] & 4) && (combo[5] & 8) && (combo[6] & 8) && (combo[7] & 0x20) &&
            (combo[8] & 0x20) && (combo[9] & 0x800) && (combo[10] & 0x800) && (combo[11] & 0x800)) {
            *(u32 *)(player + 0x6C) |= 0x20;
        }
    } else if (*(u32 *)(player + 0xCC) == 4 && (combo[0] & 0x800) && (combo[1] & 4) && (combo[2] & 4)) {
    } else if (*(u32 *)(player + 0xCC) == 4 && (combo[0] & 0x800) && (combo[1] & 4) && (combo[2] & 8)) {
        player[0x32] = 7;
    } else if (*(u32 *)(player + 0xCC) == 4 && (combo[0] & 0x800) && (combo[1] & 4) && (combo[2] & 0x10)) {
    } else if (*(u32 *)(player + 0xCC) == 4 && (combo[0] & 0x800) && (combo[1] & 0x800) && (combo[2] & 0x10)) {
        player[0x32] = 3;
    } else if (*(u32 *)(player + 0xCC) == 4 && (combo[0] & 0x800) && (combo[1] & 0x800) && (combo[2] & 0x20)) {
        TimeHandler_SetSlowMotion();
    } else if (*(u32 *)(player + 0xCC) == 4 && (combo[0] & 0x800) && (combo[1] & 0x10) && (combo[2] & 0x10)) {
    } else if (*(u32 *)(player + 0xCC) == 4 && (combo[0] & 0x800) && (combo[1] & 0x10) && (combo[2] & 0x20)) {
        CameraManager_ReturnAllToPlayer();
    } else if (*(u32 *)(player + 0xCC) == 4 && (combo[0] & 0x800) && (combo[1] & 8) && (combo[2] & 8)) {
    } else if (*(u32 *)(player + 0xCC) == 6 && (combo[0] & 0x800) && (combo[1] & 0x800) && (combo[2] & 0x800) &&
               (combo[3] & 0x20) && (combo[4] & 0x20)) {
    } else if (*(u32 *)(player + 0xCC) == 4 && (combo[0] & 0x800) && (combo[1] & 0x20) && (combo[2] & 0x20)) {
        g_bFreezeWorld = !g_bFreezeWorld;
    } else if (*(u32 *)(player + 0xCC) == 4 && (combo[0] & 4) && (combo[1] & 8) && (combo[2] & 8)) {
        player[0x32] = 8;
    }
}

s8 Input_KeyPress(s32 device, u8 key);
u8 Input_KeyDown(s32 device, u8 key);
s8 Wheel_IsConnected(s32 device);
s8 Input_GetAnalog(s32 chan, s16 *x, s16 *y, s16 *cx, s16 *cy);
s8 Timer_Interrupt300(void);
void Dprintf(const char *fmt, ...);
s32 Playback_GetElement(void);
void Playback_PutElement(s32 element);
s32 Player_DefaultControlReader(void *arg0) {
    typedef struct Thing {
        u8 pad0[0x24];
        f32 speed;
    } Thing;
    typedef struct PlayerHuman {
        s8 steer;
        s8 accel;
        s8 steerHi;
        s8 accelHi;
        u8 pad4[0xC - 0x4];
        u32 pressed;
        u8 down[14];
        u8 pad1E[0x2C - 0x1E];
        Thing *thing;
        u8 pad30[0x6C - 0x30];
        u32 flags;
        u8 pad70[0xB8 - 0x70];
        u8 keys[14];
        u8 stickMode;
        u8 substickMode;
        s32 device;
        u32 comboCount;
        u8 padD0[4];
        u32 combo[16];
    } PlayerHuman;
    extern u32 g_playbackStatus;
    PlayerHuman *player = arg0;
    s16 stickX;
    s16 substickX;
    s16 stickY;
    s16 substickY;
    u32 i;

    Control_ClearControl(player);
    if (player->flags & 1) {
        player->down[3] = 0xFF;
        player->down[2] = 0xFF;
        if (player->thing != NULL && player->thing->speed < 0.0f) {
            player->steer = -128;
        }
        return 1;
    }
    if (g_playbackStatus == 2) {
        u32 element = Playback_GetElement();
        player->steer = element;
        player->steerHi = element >> 16;
        element = Playback_GetElement();
        player->accel = element;
        player->accelHi = element >> 16;
    } else {
        Control_ClearControl(player);
        for (i = 0; i < 14; i++) {
            player->down[i] = PlayerHuman_KeyDown((PlayerHumanKeys *)player, i);
            if (PlayerHuman_KeyPress((PlayerHumanKeys *)player, i)) {
                player->pressed |= 1 << i;
            }
        }
        if (player->down[1]) {
            if (player->pressed) {
                player->combo[player->comboCount++] = player->pressed;
                if (player->comboCount >= 16) {
                    player->comboCount = 0;
                }
                PlayerHuman_EvaluateCombo((u8 *)player);
            }
        } else {
            player->comboCount = 0;
        }
        if (!Wheel_IsConnected(player->device) && player->down[2]) {
            player->down[2] = 0xFF;
        }
        if (Input_GetAnalog(player->device, &stickX, &stickY, &substickX, &substickY)) {
            u8 mode = player->stickMode;
            switch (mode) {
            case 0:
                break;
            case 1:
                player->steer = stickX;
                player->accel = stickY;
                break;
            case 2:
                player->steer = stickX;
                player->accel = stickY;
                if (player->accel > 0) {
                    player->down[2] = player->accel << 1;
                } else {
                    player->down[3] = ~player->accel << 1;
                }
                break;
            case 3:
                player->steer = stickX;
                player->accel = stickY;
                if (player->accel > 0) {
                    player->down[2] = player->accel << 1;
                } else {
                    player->down[3] = ~player->accel << 1;
                }
                break;
            case 4:
                player->steer = stickX;
                player->accel = stickY;
                if (player->accel < 0) {
                    player->down[3] = ~player->accel << 1;
                }
                break;
            case 5:
                if (Timer_Interrupt300() && stickY > 0) {
                    player->down[12] = 0xFF;
                }
                break;
            case 6:
                if (Timer_Interrupt300() && stickX > 0) {
                    player->down[1] = 0xFF;
                }
                break;
            default:
                Dprintf("%d\n", mode);
                break;
            }
            switch (player->substickMode) {
            case 0:
                break;
            case 1:
                player->steer = substickX;
                break;
            case 2:
                player->accel = substickY;
                if (player->accel >= 0) {
                    if (!player->down[2]) {
                        player->down[2] = player->accel << 1;
                    }
                } else {
                    if (!player->down[3]) {
                        player->down[3] = ~player->accel << 1;
                    }
                }
                break;
            case 3:
                player->steer = substickX;
                player->accel = substickY;
                if (player->accel > 0) {
                    if (!player->down[2]) {
                        player->down[2] = player->accel << 1;
                    }
                } else {
                    if (!player->down[3]) {
                        player->down[3] = ~player->accel << 1;
                    }
                }
                break;
            case 4:
                player->steer = substickX;
                if (substickY < 0 && !player->down[3]) {
                    player->down[3] = ~substickY << 1;
                }
                break;
            case 5:
                if (Timer_Interrupt300()) {
                    if (substickY > 40 || substickY < -40 || substickX > 40 || substickX < -40) {
                        player->down[12] = 0xFF;
                        player->pressed |= 0x1000;
                    }
                }
                break;
            case 6:
                if (Timer_Interrupt300()) {
                    if (substickY > 40 || substickY < -40 || substickX > 40 || substickX < -40) {
                        player->down[1] = 0xFF;
                        player->pressed |= 2;
                    }
                }
                break;
            }
        }
        if (Input_KeyDown(player->device, player->keys[6])) {
            player->accel = 0x7F;
        }
        if (Input_KeyDown(player->device, player->keys[7])) {
            player->accel = -0x7F;
        }
        if (Input_KeyDown(player->device, player->keys[9])) {
            player->steer = -0x7F;
        }
        if (Input_KeyDown(player->device, player->keys[8])) {
            player->steer = 0x7F;
        }
        if (g_playbackStatus == 1) {
            Playback_PutElement((u16)player->steer + (player->steerHi << 16));
            Playback_PutElement((u16)player->accel + (player->accelHi << 16));
        }
    }
    return 1;
}

s32 Player_AIControlReader(s8 *ctrl) {
    extern u32 g_playbackStatus;
    s32 element;

    if (M2C_FIELD(ctrl, u32 *, 0x6C) & 1) {
        return 0;
    }
    if (g_playbackStatus == 2) {
        element = Playback_GetElement();
        ctrl[0] = element;
        ctrl[2] = (s8)(element & 0xFFFF0000) >> 16;
        element = Playback_GetElement();
        ctrl[1] = element;
        ctrl[3] = (s8)(element & 0xFFFF0000) >> 16;
    } else if (g_playbackStatus == 1) {
        Playback_PutElement(ctrl[0] + (ctrl[2] << 16));
        Playback_PutElement(ctrl[1] + (ctrl[3] << 16));
    }
    return 1;
}

s32 ActionVehicleChangeControl(u32 *action) {
    extern void Player_DefaultControlReader(void);
    void Input_SetForceFeedbackActive(s32 input, u8 active);
    u8 *GameSettings_GetControllerSettings(s32 index);
    u8 i;
    u8 *vehicle;
    u8 *player;
    s32 isHuman;

    for (i = 0; i < action[2]; i++) {
        vehicle = *(u8 **)(action[i + 3] + 0x28);
        if (vehicle == NULL) {
            continue;
        }
        player = *(u8 **)(vehicle + 0x94);
        if (player == NULL) {
            continue;
        }
        if (action[0] == 0x22) {
            isHuman = 0;
            if (player != NULL && *(void **)(player + 0x20) == Player_DefaultControlReader) {
                isHuman = 1;
            }
            if (isHuman) {
                Input_SetForceFeedbackActive(*(s32 *)(player + 0xC8), 0);
            }
            *(u32 *)(player + 0x6C) |= 1;
        } else {
            isHuman = 0;
            if (player != NULL && *(void **)(player + 0x20) == Player_DefaultControlReader) {
                isHuman = 1;
            }
            if (isHuman) {
                Input_SetForceFeedbackActive(*(s32 *)(player + 0xC8), *GameSettings_GetControllerSettings(*(s32 *)(player + 0x60)));
            }
            *(u32 *)(player + 0x6C) &= ~1;
        }
    }
    return 1;
}

