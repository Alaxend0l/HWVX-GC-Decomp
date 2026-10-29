#define __HI(x) *(int *)&x

double __kernel_sin(double x, double y, int iy) {
    double z, r, v;
    int ix;
    ix = __HI(x) & 0x7fffffff;
    if (ix < 0x3e400000) {
        if ((int)x == 0) {
            return x;
        }
    }
    z = x * x;
    v = z * x;
    r = 8.33333333332248946124e-03 + z * (-1.98412698298579493134e-04 + z * (2.75573137070700676789e-06 + z * (-2.50507602534068634195e-08 + z * 1.58969099521155010221e-10)));
    if (iy == 0) {
        return x + v * (-1.66666666666666324348e-01 + z * r);
    } else {
        return x - ((z * (5.0e-1 * y - v * r) - y) - v * -1.66666666666666324348e-01);
    }
}
