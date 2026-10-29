#include "common.h"

extern "C" int __register_fragment(void *info, char *TOC);
extern "C" void __unregister_fragment(int fragmentID);
extern "C" char _eti_init_info[];

static int fragmentID = -2;
