#include "common.h"


void Player_Init(void *player) {
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

void Player_RefreshInterface(void *player) {
    u32 InterfaceManager_Create();
    void InterfaceManager_ReleaseInterface(u32 index);
    struct {
        u32 unk0;
        u8 *script;
        u32 x;
        u32 y;
        u8 unk10[0xC0];
    } info;
    void *thing;

    if (M2C_FIELD(player, void **, 0x2C) != NULL && (M2C_FIELD(player, u32 *, 0x6C) & 0x10)) {
        memset(&info, 0, sizeof(info));
        M2C_FIELD(player, u32 *, 0x6C) |= 0x200;
        thing = M2C_FIELD(player, void **, 0x2C);
        M2C_FIELD(M2C_FIELD(thing, void **, 0x8C), void (**)(void *, void *), 0x20)(thing, &info);
        if (M2C_FIELD(player, u32 *, 0x68) < 0xFFFFFFFF) {
            InterfaceManager_ReleaseInterface(M2C_FIELD(player, u32 *, 0x68));
            M2C_FIELD(player, u32 *, 0x68) = 0xFFFFFFFF;
        }
        if (info.script != NULL) {
            M2C_FIELD(player, u32 *, 0x68) = InterfaceManager_Create(info.script, player, 0);
            InterfaceManager_SetOffset(M2C_FIELD(player, u32 *, 0x68), info.x, info.y);
            M2C_FIELD(player, u8 *, 0x71) = 1;
        }
        M2C_FIELD(player, u32 *, 0x6C) &= ~0x200;
    }
}

void Player_ReleaseInterface(void *player) {
    void InterfaceManager_ReleaseInterface(u32 index);
    u32 index = M2C_FIELD(player, u32 *, 0x68);

    if (index != 0xFFFFFFFF) {
        InterfaceManager_ReleaseInterface(index);
    }
}

