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

void __check_pad3(void)
{
    if ((*((u16 *)0x800030e4) & 0xeef) == 0xeef)
    {
        OSResetSystem(0, 0, 0);
    }
}

void __check_pad3(void);
void __init_registers(void);
void __init_data(void);
void __init_hardware(void);
void __init_user(void);
void DBInit(void);
void OSInit(void);
void InitMetroTRK(void);
int main(void *, void *);
void exit(int);
void __start(void)
{
    void * exceptionTable;
    void * exceptionTable2;
    void ** table;
    u32 mode;
    u32 flag;
    void * p;
    void * q;
    u32 n;
    u32 count;
    void * end;
    void * base;
    u16 hwflags;
    void (*handler)(void);

    exceptionTable = (void *)-1;
    exceptionTable2 = (void *)-1;

    __init_registers();
    __init_hardware();
    __init_data();

    *(u32 *)0x80000044 = 0;

    table = *(void **)0x800000f4;
    if (table != NULL) {
        mode = ((u32 *)table)[3];
    }
    else if (*(u32 *)0x80000034 != 0) {
        mode = *(u32 *)0x800030e8;
    }
    else {
        mode = 0;
    }

    flag = 0;
    if (mode == 2) {
        handler = InitMetroTRK;
        handler();
    }
    if (mode == 3) {
        flag = 1;
        handler = InitMetroTRK;
        handler();
    }

    base = *(void **)0x800000f4;
    if (base != NULL) {
        n = ((u32 *)base)[2];
        if (n != 0) {
            q = (char *)base + n;
            count = *(u32 *)q;
            if (count != 0) {
                end = (char *)q + 4;
                p = end;
                while (count != 0) {
                    *(u32 *)p = *(u32 *)p + (u32)base;
                    p = (char *)p + 4;
                    count = count - 1;
                }
                *(u32 *)0x80000034 = (u32)end & ~31;
                exceptionTable = (void *)count;
                exceptionTable2 = end;
            }
        }
    }
    if (exceptionTable2 == (void *)-1) {
        exceptionTable = NULL;
        exceptionTable2 = NULL;
    }

    DBInit();
    OSInit();

    hwflags = *(u16 *)0x800030e6;
    if ((hwflags & 0x8000) != 0) {
        if ((hwflags & 0x7fff) == 1) {
            __check_pad3();
        }
    }

    __init_user();
    exit(main(exceptionTable, exceptionTable2));
}

void __init_registers(void)
{
}

void __init_data(void)
{
    rom_copy_info * iter;
    bss_init_info * iter2;

    for (iter = _rom_copy_info; iter->size; iter++)
    {
        if (iter->source && iter->target != iter->source)
        {
            memcpy(iter->target, iter->source, iter->size);
            __flush_cache(iter->target, iter->size);
        }
    }
    for (iter2 = _bss_init_info; iter2->size; iter2++)
    {
        if (iter2->target)
        {
            memset(iter2->target, 0, iter2->size);
        }
    }
}
