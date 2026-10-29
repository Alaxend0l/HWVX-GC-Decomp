#include "common.h"

extern "C" int __register_fragment(void *info, char *TOC);
extern "C" void __unregister_fragment(int fragmentID);
extern "C" char _eti_init_info[];

static int fragmentID = -2;

extern "C" void __fini_cpp_exceptions(void) {
    if (fragmentID != -2) {
        __unregister_fragment(fragmentID);
        fragmentID = -2;
    }
}

char *GetR2(void);
extern "C" void __init_cpp_exceptions(void) {
    if (fragmentID == -2) {
        fragmentID = __register_fragment(_eti_init_info, GetR2());
    }
}

