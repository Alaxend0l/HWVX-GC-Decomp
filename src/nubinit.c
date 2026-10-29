#include "common.h"


void TRKNubWelcome(void)
{
    void TRK_board_display(char *str);

    static const char name[] = "MetroTRK for GAMECUBE v0.10";
    TRK_board_display((char *)name);
}

s32 TRKTerminateNub(void)
{
    s32 TRKTerminateSerialHandler(void);

    TRKTerminateSerialHandler();
    return 0;
}

