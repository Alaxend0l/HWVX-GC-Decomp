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
