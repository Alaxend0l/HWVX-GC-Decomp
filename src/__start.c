#include "common.h"

typedef struct rom_copy_info {
    void * source;
    void * target;
    u32 size;
} rom_copy_info;

typedef struct bss_init_info {
    void * target;
    u32 size;
} bss_init_info;

extern rom_copy_info _rom_copy_info[];
extern bss_init_info _bss_init_info[];

void * memcpy(void *dest, void *source, u32 count);
void * memset(void *dest, s32 value, u32 count);
void __flush_cache(void *address, u32 size);
void OSResetSystem(s32 reset, u32 resetCode, s32 forceMenu);

void __check_pad3(void)
{
    if ((*((u16 *)0x800030e4) & 0xeef) == 0xeef)
    {
        OSResetSystem(0, 0, 0);
    }
}

void __init_registers(void)
{
}

void __init_data(void)
{
}
