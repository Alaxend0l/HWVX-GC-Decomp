#include "common.h"

int raise(int sig);
void _ExitProcess(void);

void (*__atexit_funcs[64])(void);
int __aborting;
int __atexit_curr_func;
void (*__stdio_exit)(void);
void (*__console_exit)(void);

extern void (*_dtors[])(void);
void __destroy_global_chain(void);
void exit(int status) {
    void (**dtor)(void);

    if (!__aborting) {
        __destroy_global_chain();
        dtor = _dtors;
        while (*dtor != NULL) {
            (*dtor)();
            dtor++;
        }
        if (__stdio_exit) {
            (*__stdio_exit)();
            __stdio_exit = 0;
        }
    }
    while (__atexit_curr_func > 0) {
        (*__atexit_funcs[--__atexit_curr_func])();
    }
    if (__console_exit) {
        (*__console_exit)();
        __console_exit = 0;
    }
    _ExitProcess();
}

void abort(void) {
    raise(1);
    __aborting = 1;
    while (__atexit_curr_func > 0) {
        (*__atexit_funcs[--__atexit_curr_func])();
    }
    if (__console_exit) {
        (*__console_exit)();
        __console_exit = 0;
    }
    _ExitProcess();
}
