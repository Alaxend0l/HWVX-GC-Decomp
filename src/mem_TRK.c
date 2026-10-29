#include "common.h"


void TRK_fill_mem(void *dst, int val, u32 count);
void *TRK_memset(void *dst, int val, u32 count)
{
    TRK_fill_mem(dst, val, count);
    return dst;
}

void TRK_fill_mem(void * dst, int val, u32 count)
{
    u8 b;
    u32 w;
    u32 i;

    ((u8 *)dst) = ((u8 *)dst) - 1;
    b = (u8)val;
    w = b;
    if (count >= 0x20)
    {
        i = (~(u32)dst) & 3;
        if (i)
        {
            count -= i;
            do
            {
                i -= 1;
                *++((u8 *)dst) = b;
            } while (i);
        }
        if (b)
        {
            w |= (w << 24) | (w << 16) | (w << 8);
        }
        i = count >> 5;
        ((u32 *)dst) = ((u32 *)(((u8 *)dst) + 1)) - 1;
        if (i)
        {
            do
            {
                *++((u32 *)dst) = w;
                i -= 1;
                *++((u32 *)dst) = w;
                *++((u32 *)dst) = w;
                *++((u32 *)dst) = w;
                *++((u32 *)dst) = w;
                *++((u32 *)dst) = w;
                *++((u32 *)dst) = w;
                *++((u32 *)dst) = w;
            } while (i);
        }
        i = (count & 0x1f) >> 2;
        if (i)
        {
            do
            {
                i -= 1;
                *++((u32 *)dst) = w;
            } while (i);
        }
        ((u8 *)dst) = ((u8 *)(((u32 *)dst) + 1)) - 1;
        count %= 4;
    }
    if (count)
    {
        do
        {
            count -= 1;
            *++((u8 *)dst) = b;
        } while (count);
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
