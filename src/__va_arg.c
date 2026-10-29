#include "common.h"


void *__va_arg(void *ap, s32 type) {
    typedef struct {
        char gpr;
        char fpr;
        char reserved[2];
        char *input_arg_area;
        char *reg_save_area;
    } _va_list_struct;
    char *addr;
    char *reg = &((_va_list_struct *)ap)->gpr;
    s32 g_reg = ((_va_list_struct *)ap)->gpr;
    s32 maxsize = 8;
    s32 size = 4;
    s32 increment = 1;
    s32 even = 0;
    s32 fpr_offset = 0;
    s32 regsize = 4;
    _va_list_struct *list = ap;

    if (type == 3) {
        reg = &list->fpr;
        g_reg = list->fpr;
        size = 8;
        fpr_offset = 32;
        regsize = 8;
    }
    if (type == 2) {
        size = 8;
        maxsize--;
        if (g_reg & 1) {
            even = 1;
        }
        increment = 2;
    }
    if (g_reg < maxsize) {
        g_reg += even;
        addr = list->reg_save_area + fpr_offset + g_reg * regsize;
        *reg = g_reg + increment;
    } else {
        *reg = 8;
        addr = list->input_arg_area;
        addr = (char *)(((u32)addr + (size - 1)) & ~(size - 1));
        list->input_arg_area = addr + size;
    }
    if (type == 0) {
        addr = *(char **)addr;
    }
    return addr;
}
