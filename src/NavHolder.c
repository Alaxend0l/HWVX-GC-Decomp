#include "common.h"


void NavHolder_FillPointerArrayPath(u8 *point, u8 **array, u32 max) {
    u8 **out = array;
    u32 i = 0;
    while (point != NULL && i < max) {
        *out++ = point;
        i++;
        point = *(u8 **)(point + 0x38);
    }
    if (i < max) {
        array[i] = NULL;
    }
}

void NavHolder_UseMainRouteOnly(void) {
    extern s8 gs_useMainRouteOnly_803C7000;
    gs_useMainRouteOnly_803C7000 = 1;
}
