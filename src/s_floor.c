#include "common.h"

#define __HI(x) *(int *)&x
#define __LO(x) *(1 + (int *)&x)

static const double huge = 1.0e300;
