#define __HI(x) *(int *)&x
#define __LO(x) *(1 + (int *)&x)

static const double two54 = 1.80143985094819840000e+16;
