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
extern u32 _stack_addr[];
extern u32 _SDA2_BASE_[];
extern u32 _SDA_BASE_[];

void * memcpy(void *dest, void *source, u32 count);
void * memset(void *dest, s32 value, u32 count);
void __flush_cache(void *address, u32 size);
void OSResetSystem(s32 reset, u32 resetCode, s32 forceMenu);

static void __copy_rom_section(void *target, void *source, u32 size)
{
    if (size && (target != source)) {
        memcpy(target, source, size);
        __flush_cache(target, size);
    }
}

static void __init_bss_section(void *target, u32 size)
{
    if (size) {
        memset(target, 0, size);
    }
}

void __check_pad3(void)
{
    if ((*((u16 *)0x800030e4) & 0xeef) == 0xeef) {
        OSResetSystem(0, 0, 0);
    }
}

void __check_pad3(void);
void __init_registers(void);
static void __init_data(void);
void __init_hardware(void);
void __init_user(void);
void DBInit(void);
void OSInit(void);
void InitMetroTRK(void);
int main(void *, void *);
void exit(int);
void __start(void)
{
    void * argc;
    void ** argv;
    u32 * table;
    u32 mode;
    u32 flag;
    char * p;
    char * q;
    u32 count;
    u32 n;
    void (*handler)(void);

    argc = (void *)-1;
    argv = (void **)-1;

    handler = InitMetroTRK;

    __init_registers();
    __init_hardware();
    __init_data();

    *(u32 *)0x80000044 = 0;

    table = *(u32 **)0x800000f4;
    if (table != NULL) {
        mode = table[3];
        flag = 0;
        if (mode == 2) {
            handler();
        }
        if (mode == 3) {
            flag = 1;
            handler();
        }
    }
    else if (*(u32 *)0x80000034 != 0) {
        mode = *(u32 *)0x800030e8;
        flag = 0;
        if (mode == 2) {
            handler();
        }
        if (mode == 3) {
            flag = 1;
            handler();
        }
    }

    p = (char *)*(void **)0x800000f4;
    if (p != NULL) {
        n = ((u32 *)p)[2];
        if (n != 0) {
            q = p + n;
            count = *(u32 *)q;
            if (count != 0) {
                p = q + 4;
                while (count-- != 0) {
                    *(u32 *)p += (u32)(char *)*(void **)0x800000f4;
                    p = p + 4;
                }
                *(u32 *)0x80000034 = (u32)(q + 4) & ~31;
            }
        }
    }
    if (argc == (void *)-1) {
        argc = NULL;
        argv = NULL;
    }

    DBInit();
    OSInit();

    if (((*(u16 *)0x800030e6 & 0x8000) != 0) &&
        ((*(u16 *)0x800030e6 & 0x7fff) == 1))
    {
        __check_pad3();
    }

    __init_user();
    exit(main(argc, argv));
}

void __init_registers(void)
{
}

#pragma scheduling off
#pragma peephole off
__declspec(section ".init") static void __init_data(void)
{
    rom_copy_info * info;
    bss_init_info * bss;

    info = _rom_copy_info;
    while (1) {
        if (info->size == 0) {
            break;
        }
        __copy_rom_section(info->target, info->source, info->size);
        info++;
    }

    bss = _bss_init_info;
    while (1) {
        if (bss->size == 0) {
            break;
        }
        __init_bss_section(bss->target, bss->size);
        bss++;
    }
}
