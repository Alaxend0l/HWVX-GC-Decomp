#include "common.h"


void NavHolder_UseMainRouteOnly(void) {
    extern s8 gs_useMainRouteOnly_803C7000;
    gs_useMainRouteOnly_803C7000 = 1;
}
