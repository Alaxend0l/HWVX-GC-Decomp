#include "common.h"


s32 InitializeUART(u32 baudRate);
s32 WriteUARTN(const void *buf, u32 len);
void __TRK_write_console(s32 handle, u8 *buffer, u32 *count, void *idle_proc);
int __write_console(s32 handle, u8 *buffer, u32 *count, void *idle_proc) {
    static int initialized;
    s32 status = 0;

    if (initialized == 0) {
        status = InitializeUART(0xE100);
        if (status == 0) {
            initialized = 1;
        }
    }
    if (status != 0) {
        return 1;
    }
    if (WriteUARTN(buffer, *count) != 0) {
        *count = 0;
        return 1;
    }
    __TRK_write_console(handle, buffer, count, idle_proc);
    return 0;
}
