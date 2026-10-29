#include "common.h"

typedef void (*__signal_func_ptr)(int);

#define SIG_DFL ((__signal_func_ptr)0)
#define SIG_IGN ((__signal_func_ptr)1)
#define SIGABRT 1
#define __signal_max 6

void exit(int status);

__signal_func_ptr signal_funcs[__signal_max];
