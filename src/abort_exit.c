#include "common.h"

int raise(int sig);
void _ExitProcess(void);

void (*__atexit_funcs[64])(void);
int __aborting;
int __atexit_curr_func;
void (*__stdio_exit)(void);
void (*__console_exit)(void);
