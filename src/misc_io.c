#include "common.h"

void __close_all(void);
extern void (*__stdio_exit)(void);

void __stdio_atexit(void) {
    __stdio_exit = __close_all;
}
