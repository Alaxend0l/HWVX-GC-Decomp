#include "common.h"


extern GXColor g_textcolor;
void Gfx_SetTextColor(u8 r, u8 g, u8 b, u8 a) {
    g_textcolor.r = r;
    g_textcolor.g = g;
    g_textcolor.b = b;
    g_textcolor.a = a;
}
