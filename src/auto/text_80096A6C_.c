// Code of main that had no source file yet: decompdecomp gave it this one.
// split_file divides it into the game's real files (file_status shows its range).
#include "common.h"

typedef struct CatchInfo {
    void *location;
    void *typeinfo;
    void *dtor;
    void *sublocation;
    s32 pointercopy;
    void *stacktop;
} CatchInfo;

void __end__catch(CatchInfo *catchinfo) {
    if (catchinfo->location && catchinfo->dtor) {
        ((void (*)(void *, s16))catchinfo->dtor)(catchinfo->location, -1);
    }
}
