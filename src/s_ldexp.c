#define __HI(x) *(int *)&x
#define __LO(x) *(1 + (int *)&x)

double copysign(double x, double y);

static inline int __fpclassifyd(double x) {
    switch ((*(int *)&x) & 0x7FF00000) {
    case 0x7FF00000:
        if (((*(int *)&x) & 0x000FFFFF) || (*((int *)&x + 1) & 0xFFFFFFFF)) {
            return 1;
        } else {
            return 2;
        }
    case 0:
        if (((*(int *)&x) & 0x000FFFFF) || (*((int *)&x + 1) & 0xFFFFFFFF)) {
            return 5;
        } else {
            return 3;
        }
    }
    return 4;
}

static const double two54 = 1.80143985094819840000e+16;
static const double twom54 = 5.55111512312578270212e-17;
static const double huge = 1.0e+300;
static const double tiny = 1.0e-300;
