// Code of main that had no source file yet: decompdecomp gave it this one.
// split_file divides it into the game's real files (file_status shows its range).
#include "common.h"
#pragma cplusplus on
#pragma exceptions on
#pragma readonly_strings on

namespace std {
void unexpected();
void terminate();

class exception {
public:
    exception() {}
    virtual ~exception();
    virtual const char *what() const;
};

class bad_exception : public exception {
public:
    bad_exception() {}
    virtual ~bad_exception();
    virtual const char *what() const;
};
}

typedef struct CatchInfo {
    void *location;
    void *typeinfo;
    void *dtor;
    void *sublocation;
    long pointercopy;
    void *stacktop;
} CatchInfo;

typedef struct ExSpec {
    unsigned short flags;
    unsigned short count;
    long unk4;
    long unk8;
    const char *specs[1];
} ExSpec;

extern "C" char __throw_catch_compare(const char *throwtype, const char *catchtype, long *offset);

static inline int __is_exception_spec(ExSpec *spec, const char *type)
{
    long offset;
    int i;

    for (i = 0; i < spec->count; i++) {
        if (__throw_catch_compare(type, spec->specs[i], &offset)) {
            return 1;
        }
    }
    return 0;
}

#pragma use_lmw_stmw on
extern "C" void __unexpected(CatchInfo *catchinfo)
{
    ExSpec *unexp = (ExSpec *)catchinfo->stacktop;

#pragma exception_magic
    try {
        std::unexpected();
    } catch (...) {
        if (__is_exception_spec(unexp, (const char *)((CatchInfo *)&__exception_magic)->typeinfo)) {
            throw;
        }
        if (__is_exception_spec(unexp, "!bad_exception!!")) {
            throw std::bad_exception();
        }
        if (__is_exception_spec(unexp, "!std::bad_exception!!")) {
            throw std::bad_exception();
        }
    }
    std::terminate();
}
