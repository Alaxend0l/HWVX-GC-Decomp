#include "common.h"


u8 GetUseSerialIO(void);
s32 GetTRKConnected(void);
u8 fn_80094FC0(s32 command, s32 handle);
u8 fn_80094FB0(s32 command, s32 handle, u32 *length, u8 *buffer);
s32 __close_console(s32 handle) {
    if (GetTRKConnected() == 0) {
        return 1;
    }
    switch (fn_80094FC0(0xD3, handle)) {
    case 0:
        return 0;
    case 2:
        return 2;
    case 1:
    default:
        return 1;
    }
}

s32 __TRK_write_console(s32 handle, u8 *buffer, u32 *count, void *idle) {
    u32 length;
    u8 result;

    if (GetUseSerialIO() == 0) {
        return 1;
    }
    if (GetTRKConnected() == 0) {
        return 1;
    }
    length = *count;
    result = fn_80094FB0(0xD0, 1, &length, buffer);
    *count = length;
    switch (result) {
    case 0:
        return 0;
    case 2:
        return 2;
    case 1:
    default:
        return 1;
    }
}

s32 __read_console(s32 handle, u8 *buffer, u32 *count, void *idle) {
    u32 length;
    u8 result;

    if (GetUseSerialIO() == 0) {
        return 1;
    }
    if (GetTRKConnected() == 0) {
        return 1;
    }
    length = *count;
    result = fn_80094FB0(0xD1, 0, &length, buffer);
    *count = length;
    switch (result) {
    case 0:
        return 0;
    case 2:
        return 2;
    case 1:
    default:
        return 1;
    }
}

