#include "common.h"

#define __HI(x) *(int *)&x

double __kernel_sin(double x, double y, int iy);
double __kernel_cos(double x, double y);
int __ieee754_rem_pio2(double x, double *y);
