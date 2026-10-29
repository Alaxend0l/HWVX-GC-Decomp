#include "common.h"


void TRK_fill_mem(void *dst, int val, u32 count);
void *TRK_memset(void *dst, int val, u32 count)
{
    TRK_fill_mem(dst, val, count);
    return dst;
}

void TRK_fill_mem(void *dst, int val, u32 count)
{
    u32 v = (u8)val;
    u32 i;
    union {
        u8 *cpd;
        u32 *lpd;
    } dstu;

    dstu.cpd = ((u8 *)dst) - 1;

    if (count >= 32)
    {
        i = (~(u32)dstu.cpd) & 3;
        if (i)
        {
            count -= i;
            do
                *++dstu.cpd = v;
            while (--i);
        }

        if (v)
            v |= v << 24 | v << 16 | v << 8;

        dstu.lpd = ((u32 *)(dstu.cpd + 1)) - 1;

        i = count >> 5;
        if (i)
        {
            do
            {
                *++dstu.lpd = v;
                *++dstu.lpd = v;
                *++dstu.lpd = v;
                *++dstu.lpd = v;
                *++dstu.lpd = v;
                *++dstu.lpd = v;
                *++dstu.lpd = v;
                *++dstu.lpd = v;
            } while (--i);
        }

        i = (count & 31) >> 2;
        if (i)
        {
            do
                *++dstu.lpd = v;
            while (--i);
        }

        dstu.cpd = ((u8 *)(dstu.lpd + 1)) - 1;
        count &= 3;
    }

    if (count)
    {
        do
            *++dstu.cpd = v;
        while (--count);
    }
}

void *TRK_memcpy(void *dst, const void *src, u32 count)
{
    unsigned char *d;
    const unsigned char *s;
    u32 n;

    d = (unsigned char *)dst - 1;
    s = (unsigned char *)src - 1;
    n = count + 1;
    while (--n) {
        *++d = *++s;
    }
    return dst;
}
