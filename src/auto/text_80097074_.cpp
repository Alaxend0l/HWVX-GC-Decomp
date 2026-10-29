// Code of main that had no source file yet: decompdecomp gave it this one.
// split_file divides it into the game's real files (file_status shows its range).
#include "common.h"

namespace std {

class exception {
public:
    exception() {}
    virtual ~exception() {}
    virtual const char *what() const;
};

class bad_exception : public exception {
public:
    bad_exception() {}
    virtual ~bad_exception();
    virtual const char *what() const;
};

}

std::bad_exception::~bad_exception() {}
