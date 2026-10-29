// Code of main that had no source file yet: decompdecomp gave it this one.
// split_file divides it into the game's real files (file_status shows its range).
#include "common.h"
#include "exception.h"

typedef struct FragmentInfo {
    void *eti_init_info;
    char *TOC;
    s32 active;
} FragmentInfo;

extern FragmentInfo fragmentinfo_8030DC08[1];

extern "C" s32 __register_fragment(void *info, char *TOC);
extern "C" void __unregister_fragment(s32 fragmentID);

s32 __register_fragment(void *info, char *TOC) {
    FragmentInfo *f;
    s32 i;

    for (i = 0, f = fragmentinfo_8030DC08; i < 1; ++i, ++f) {
        if (f->active == 0) {
            f->eti_init_info = info;
            f->TOC = TOC;
            f->active = 1;
            return i;
        }
    }
    return -1;
}

const char *std::bad_exception::what() const {
    return "bad_exception";
}
