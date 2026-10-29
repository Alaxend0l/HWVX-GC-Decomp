#include "common.h"

#define __HI(x) *(int *)&x
#define __LO(x) *(1 + (int *)&x)

double atan(double x);

static inline double fabs(double x) { return __fabs(x); }

static const double
tiny  = 1.0e-300,
zero  = 0.0,
pi_o_4  = 7.8539816339744827900E-01,
pi_o_2  = 1.5707963267948965580E+00,
pi      = 3.1415926535897931160E+00,
pi_lo   = 1.2246467991473531772E-16;
