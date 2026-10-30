#define __HI(x) *(int *)&x

double copysign(double x, double y) {
    int hx, hy;
    hx = __HI(x);
    hy = __HI(y);
    __HI(x) = (hx & 0x7fffffff) | (hy & 0x80000000);
    return x;
}
