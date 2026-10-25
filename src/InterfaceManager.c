#include "common.h"


void InterfaceManager_SetMissionObjectiveText(u8 *obj, u8 **data) {
    u32 strlen(const char *s);
    char *strcpy(char *dst, const char *src);
    int sprintf(char *buf, const char *fmt, ...);
    f32 GetCodeVariable(s32 var, u8 *thing);
    char **text;
    char buf[16];
    char *str;
    s32 i;
    s32 j;
    s32 len;

    if (*(u8 **)(data[1] + 0xC) == NULL) {
        if (*(char ***)(data[1] + 0x14) != NULL) {
            text = *(char ***)(data[1] + 0x14);
        } else {
            text = *(char ***)(data[1] + 0x18);
        }
    } else if (*(f32 *)(*(u8 **)(data[1] + 0xC) + 4)) {
        text = *(char ***)(data[1] + 0x18);
    } else {
        text = *(char ***)(data[1] + 0x14);
    }
    if (text == NULL) {
        return;
    }
    if (data[1][0] == 0) {
        InterfaceManager_SetObjectText(obj, text);
        return;
    }
    if (data[1][0] == 1) {
        sprintf(buf, "%d", (u32)*(f32 *)(*(u8 **)(data[1] + 0x10) + 4));
    } else if (data[1][0] == 2) {
        u32 ms = *(f32 *)(*(u8 **)(data[1] + 0x10) + 8);
        u32 secs = ms / 1000;
        u32 mins = secs / 60;
        secs %= 60;
        sprintf(buf, "%02d:%02d", mins, secs);
    } else if (data[1][0] == 3) {
        u32 *var = *(u32 **)(data[1] + 0x10);
        u32 value;
        if (var[0] < 10) {
            value = GetCodeVariable(var[0], data[0]);
        } else {
            value = GetCodeVariable(var[0], (u8 *)var[1]);
        }
        sprintf(buf, "%d", value);
    }
    str = *text;
    len = strlen(str);
    j = strlen(buf);
    if (len != strlen((char *)(obj + 0x290))) {
        strcpy((char *)(obj + 0x290), str);
    } else {
        strcpy(str, (char *)(obj + 0x290));
    }
    j--;
    for (i = len - 1; i >= 0; i--) {
        if (str[i] == '#') {
            if (j >= 0) {
                str[i] = buf[j--];
            } else {
                str[i] = ' ';
            }
        }
    }
    InterfaceManager_SetObjectText(obj, text);
}

void InterfaceManager_SetPlayerMessage(void *message, u32 player) {
    extern void *g_playerMessage1;
    extern void *g_playerMessage2;
    extern void *g_playerMessage3;

    if (player == 0) {
        g_playerMessage1 = message;
    } else if (player == 1) {
        g_playerMessage2 = message;
    } else if (player == 2) {
        g_playerMessage3 = message;
    }
}

