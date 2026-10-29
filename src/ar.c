#include "common.h"

volatile u16 __DSPRegs[32] : 0xCC005000;

static inline void __ARClearInterrupt(void) {
    u16 tmp;
    tmp = __DSPRegs[5];
    tmp = (u16)((tmp & ~0x88) | 0x20);
    __DSPRegs[5] = tmp;
}

static inline void __ARWaitForDMA(void) {
    while (__DSPRegs[5] & 0x200) {
    }
}

static inline void __ARWriteDMA(u32 mmem_addr, u32 aram_addr, u32 length) {
    __DSPRegs[16] = (u16)((__DSPRegs[16] & ~0x3ff) | (u16)(mmem_addr >> 16));
    __DSPRegs[17] = (u16)((__DSPRegs[17] & ~0xffe0) | (u16)(mmem_addr & 0xffff));
    __DSPRegs[18] = (u16)((__DSPRegs[18] & ~0x3ff) | (u16)(aram_addr >> 16));
    __DSPRegs[19] = (u16)((__DSPRegs[19] & ~0xffe0) | (u16)(aram_addr & 0xffff));
    __DSPRegs[20] = (u16)(__DSPRegs[20] & ~0x8000);
    __DSPRegs[20] = (u16)((__DSPRegs[20] & ~0x3ff) | (u16)(length >> 16));
    __DSPRegs[21] = (u16)((__DSPRegs[21] & ~0xffe0) | (u16)(length & 0xffff));
    __ARWaitForDMA();
    __ARClearInterrupt();
}

static inline void __ARReadDMA(u32 mmem_addr, u32 aram_addr, u32 length) {
    __DSPRegs[16] = (u16)((__DSPRegs[16] & ~0x3ff) | (u16)(mmem_addr >> 16));
    __DSPRegs[17] = (u16)((__DSPRegs[17] & ~0xffe0) | (u16)(mmem_addr & 0xffff));
    __DSPRegs[18] = (u16)((__DSPRegs[18] & ~0x3ff) | (u16)(aram_addr >> 16));
    __DSPRegs[19] = (u16)((__DSPRegs[19] & ~0xffe0) | (u16)(aram_addr & 0xffff));
    __DSPRegs[20] = (u16)(__DSPRegs[20] | 0x8000);
    __DSPRegs[20] = (u16)((__DSPRegs[20] & ~0x3ff) | (u16)(length >> 16));
    __DSPRegs[21] = (u16)((__DSPRegs[21] & ~0xffe0) | (u16)(length & 0xffff));
    __ARWaitForDMA();
    __ARClearInterrupt();
}


void (*ARRegisterDMACallback(void (*cb)(void)))(void) {
    extern void (*__AR_Callback)(void);
    s32 OSDisableInterrupts(void);
    void OSRestoreInterrupts(void);
    void (*old)(void);

    old = __AR_Callback;
    OSDisableInterrupts();
    __AR_Callback = cb;
    OSRestoreInterrupts();
    return old;
}

void OSRestoreInterrupts(s32 level);
void ARStartDMA(u32 type, u32 mainmem_addr, u32 aram_addr, u32 length) {
    s32 enabled = OSDisableInterrupts();
    __DSPRegs[16] = (u16)((__DSPRegs[16] & ~0x3ff) | (mainmem_addr >> 16));
    __DSPRegs[17] = (u16)((__DSPRegs[17] & ~0xffe0) | (0xffff & mainmem_addr));
    __DSPRegs[18] = (u16)((__DSPRegs[18] & ~0x3ff) | (aram_addr >> 16));
    __DSPRegs[19] = (u16)((__DSPRegs[19] & ~0xffe0) | (0xffff & aram_addr));
    __DSPRegs[20] = (u16)((__DSPRegs[20] & ~0x8000) | (type << 15));
    __DSPRegs[20] = (u16)((__DSPRegs[20] & ~0x3ff) | (length >> 16));
    __DSPRegs[21] = (u16)((__DSPRegs[21] & ~0xffe0) | (0xffff & length));
    OSRestoreInterrupts(enabled);
}

void *__OSSetInterruptHandler(s16 interrupt, void *handler);
void __OSUnmaskInterrupts(u32 mask);
void __ARChecksize(void);
static void __ARHandler(s16 interrupt, void *context);
u32 ARInit(u32 *stack_index_addr, u32 num_entries) {
    extern s32 __AR_init_flag;
    extern void (*__AR_Callback)(void);
    extern u32 __AR_FreeBlocks;
    extern u32 __AR_StackPointer;
    extern u32 *__AR_BlockLength;
    s32 old;

    if (__AR_init_flag == 1) {
        return 0x4000;
    }
    old = OSDisableInterrupts();
    __AR_Callback = NULL;
    __OSSetInterruptHandler(6, __ARHandler);
    __OSUnmaskInterrupts(0x02000000);
    __AR_FreeBlocks = num_entries;
    __AR_StackPointer = 0x4000;
    __AR_BlockLength = stack_index_addr;
    __DSPRegs[13] = (u16)((__DSPRegs[13] & 0xFF) | (__DSPRegs[13] & ~0xFF));
    __ARChecksize();
    __AR_init_flag = 1;
    OSRestoreInterrupts(old);
    return __AR_StackPointer;
}

void ARReset(void) {
    extern s32 __AR_init_flag;
    __AR_init_flag = 0;
}

s32 ARGetBaseAddress(void)
{
    return 0x4000;
}

s32 ARGetSize(void) {
    extern s32 __AR_Size;
    return __AR_Size;
}

void OSClearContext(void *context);
void OSSetCurrentContext(void *context);
static void __ARHandler(s16 interrupt, void *context) {
    extern void (*__AR_Callback)(void);
    u8 exceptionContext[0x2C8];
    u16 tmp;

    tmp = __DSPRegs[5];
    tmp = (u16)((tmp & ~0x88) | 0x20);
    __DSPRegs[5] = tmp;
    OSClearContext(exceptionContext);
    OSSetCurrentContext(exceptionContext);
    if (__AR_Callback) {
        __AR_Callback();
    }
    OSClearContext(exceptionContext);
    OSSetCurrentContext(context);
}

void DCFlushRange(void *addr, u32 len);
void DCInvalidateRange(void *addr, u32 len);
void PPCSync(void);
void *memset(void *dst, s32 c, u32 n);
void __ARChecksize(void) {
    extern u32 __AR_InternalSize;
    extern u32 __AR_ExpansionSize;
    extern u32 __AR_Size;
    u8 test_data_pad[0x40];
    u8 dummy_data_pad[0x40];
    u8 buffer_pad[0x40];
    u32 *test_data;
    u32 *dummy_data;
    u32 *buffer;
    u16 ARAM_mode;
    u32 ARAM_size;
    u32 i;

    while (!(__DSPRegs[11] & 1)) {
    }
    ARAM_mode = 3;
    ARAM_size = __AR_InternalSize = 0x1000000;
    __DSPRegs[9] = (u16)((__DSPRegs[9] & ~0x3f) | 0x23);
    test_data = (u32 *)(((u32)test_data_pad + 0x1F) & ~0x1F);
    dummy_data = (u32 *)(((u32)dummy_data_pad + 0x1F) & ~0x1F);
    buffer = (u32 *)(((u32)buffer_pad + 0x1F) & ~0x1F);
    for (i = 0; i < 8; i++) {
        test_data[i] = 0xDEADBEEF;
        dummy_data[i] = 0xBAD0BAD0;
    }
    DCFlushRange(test_data, 0x20);
    DCFlushRange(dummy_data, 0x20);
    __AR_ExpansionSize = 0;
    __ARWriteDMA((u32)dummy_data, ARAM_size + 0, 0x20);
    __ARWriteDMA((u32)dummy_data, ARAM_size + 0x200000, 0x20);
    __ARWriteDMA((u32)dummy_data, ARAM_size + 0x1000000, 0x20);
    __ARWriteDMA((u32)dummy_data, ARAM_size + 0x200, 0x20);
    __ARWriteDMA((u32)dummy_data, ARAM_size + 0x400000, 0x20);
    memset(buffer, 0, 0x20);
    DCFlushRange(buffer, 0x20);
    __ARWriteDMA((u32)test_data, ARAM_size + 0, 0x20);
    DCInvalidateRange(buffer, 0x20);
    __ARReadDMA((u32)buffer, ARAM_size + 0, 0x20);
    PPCSync();
    if (buffer[0] == test_data[0]) {
        memset(buffer, 0, 0x20);
        DCFlushRange(buffer, 0x20);
        __ARReadDMA((u32)buffer, ARAM_size + 0x200000, 0x20);
        PPCSync();
        if (buffer[0] == test_data[0]) {
            __AR_ExpansionSize = 0x200000;
            ARAM_size += 0x200000;
            goto end;
        }
        memset(buffer, 0, 0x20);
        DCFlushRange(buffer, 0x20);
        __ARReadDMA((u32)buffer, ARAM_size + 0x1000000, 0x20);
        PPCSync();
        if (buffer[0] == test_data[0]) {
            __AR_ExpansionSize = 0x400000;
            ARAM_mode |= 8;
            ARAM_size += 0x400000;
            goto end;
        }
        memset(buffer, 0, 0x20);
        DCFlushRange(buffer, 0x20);
        __ARReadDMA((u32)buffer, ARAM_size + 0x200, 0x20);
        PPCSync();
        if (buffer[0] == test_data[0]) {
            __AR_ExpansionSize = 0x800000;
            ARAM_mode |= 0x10;
            ARAM_size += 0x800000;
            goto end;
        }
        memset(buffer, 0, 0x20);
        DCFlushRange(buffer, 0x20);
        __ARReadDMA((u32)buffer, ARAM_size + 0x400000, 0x20);
        PPCSync();
        if (buffer[0] == test_data[0]) {
            __AR_ExpansionSize = 0x1000000;
            ARAM_mode |= 0x18;
            ARAM_size += 0x1000000;
        } else {
            __AR_ExpansionSize = 0x2000000;
            ARAM_mode |= 0x20;
            ARAM_size += 0x2000000;
        }
    end:
        __DSPRegs[9] = (u16)((__DSPRegs[9] & ~0x3f) | ARAM_mode);
    }
    *(u32 *)0xC00000D0 = ARAM_size;
    __AR_Size = ARAM_size;
}

