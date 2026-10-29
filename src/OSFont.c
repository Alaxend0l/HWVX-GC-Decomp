#include "common.h"


u16 OSGetFontEncode(void) {
    static u16 fontEncode = 0xFFFF;

    if (fontEncode <= 1) {
        return fontEncode;
    }
    switch (*(u32 *)0x800000CC) {
    case 0:
        fontEncode = (*(volatile u16 *)0xCC00206E & 2) ? 1 : 0;
        break;
    case 1:
    case 2:
    default:
        fontEncode = 0;
        break;
    }
    return fontEncode;
}
