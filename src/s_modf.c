#include "common.h"

#define __HI(x) *(int *)&x
#define __LO(x) *(1 + (int *)&x)

double modf(double x, double *iptr)
{
    int i0, i1, j0;
    unsigned i;
    i0 = __HI(x);
    i1 = __LO(x);
    j0 = ((i0 >> 20) & 0x7ff) - 0x3ff;
    if (j0 < 20) {
        if (j0 < 0) {
            __HI(*iptr) = i0 & 0x80000000;
            __LO(*iptr) = 0;
            return x;
        } else {
            i = (0x000fffff) >> j0;
            if (((i0 & i) | i1) == 0) {
                *iptr = x;
                __HI(x) &= 0x80000000;
                __LO(x) = 0;
                return x;
            } else {
                __HI(*iptr) = i0 & (~i);
                __LO(*iptr) = 0;
                return x - *iptr;
            }
        }
    } else if (j0 > 51) {
        *iptr = x;
        __HI(x) &= 0x80000000;
        __LO(x) = 0;
        return x;
    } else {
        i = ((unsigned)(0xffffffff)) >> (j0 - 20);
        if ((i1 & i) == 0) {
            *iptr = x;
            __HI(x) &= 0x80000000;
            __LO(x) = 0;
            return x;
        } else {
            __HI(*iptr) = i0;
            __LO(*iptr) = i1 & (~i);
            return x - *iptr;
        }
    }
}
