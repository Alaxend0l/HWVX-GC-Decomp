#include "common.h"


void * TRK_memcpy(void * dst, const void * src, u32 count)
{
    unsigned char * d;
    const unsigned char * s;
    u32 n;

    d = (unsigned char *)dst - 1;
    s = (unsigned char *)src - 1;
    n = count + 1;
    while (--n)
    {
        *++d = *++s;
    }
}
