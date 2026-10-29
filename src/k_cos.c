#include "common.h"

#define __HI(x) *(int *)&x
#define __LO(x) *(1 + (int *)&x)

double __kernel_cos(double x, double y) {
    double a, hz, z, r, qx;
    int ix;
    ix = __HI(x) & 0x7fffffff;
    if (ix < 0x3e400000) {
        if (((int)x) == 0) {
            return 1.00000000000000000000e+00;
        }
    }
    z = x * x;
    r = z * (4.16666666666666019037e-02 + z * (-1.38888888888741095749e-03 + z * (2.48015872894767294178e-05 + z * (-2.75573143513906633035e-07 + z * (2.08757232129817482790e-09 + z * -1.13596475577881948265e-11)))));
    if (ix < 0x3FD33333) {
        return 1.00000000000000000000e+00 - (0.5 * z - (z * r - x * y));
    } else {
        if (ix > 0x3fe90000) {
            qx = 0.28125;
        } else {
            __HI(qx) = ix - 0x00200000;
            __LO(qx) = 0;
        }
        hz = 0.5 * z - qx;
        a = 1.00000000000000000000e+00 - qx;
        return a - (hz - (z * r - x * y));
    }
}
