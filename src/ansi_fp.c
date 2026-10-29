#include "common.h"

#define SIGDIGLEN 36

typedef struct decimal {
    char sign;
    char unk1;
    s16 exp;
    struct {
        u8 length;
        u8 text[SIGDIGLEN];
        u8 unk25;
    } sig;
} decimal;

static inline void __dorounddecup(decimal *d, int digits) {
    u8 *b = d->sig.text;
    u8 *i = b + digits - 1;

    while (1) {
        if (*i < 9) {
            (*i)++;
            break;
        }
        if (i == b) {
            *i = 1;
            d->exp++;
            break;
        }
        *i-- = 0;
    }
}

typedef struct decform {
    char style;
    char unk1;
    s16 digits;
} decform;

void __num2dec_internal(decimal *d, double x);
void __ull2dec(decimal *result, unsigned long long val);
s32 __count_trailing_zerol(u32 x);
double frexp(double x, int *eptr);
double ldexp(double x, int exp);
double modf(double x, double *iptr);
void __two_exp(decimal *result, short exp);
void __timesdec(decimal *result, const decimal *x, const decimal *y);

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

static inline s32 __count_trailing_zero(double x) {
    u32 *l = (u32 *)&x;

    if (l[1] != 0) {
        return __count_trailing_zerol(l[1]);
    }
    return 32 + __count_trailing_zerol(l[0] | 0x00100000);
}

static inline int __must_round(const decimal *d, int digits) {
    const u8 *i = d->sig.text + digits;

    if (*i > 5) {
        return 1;
    }
    if (*i < 5) {
        return -1;
    }
    {
        const u8 *e = d->sig.text + d->sig.length;
        for (i++; i < e; i++) {
            if (*i != 0) {
                return 1;
            }
        }
    }
    if (d->sig.text[digits - 1] & 1) {
        return 1;
    }
    return -1;
}

static inline void __rounddec(decimal *d, int digits) {
    if (digits > 0 && digits < d->sig.length) {
        int round = __must_round(d, digits);
        d->sig.length = digits;
        if (round >= 0) {
            __dorounddecup(d, digits);
        }
    }
}

void __num2dec(const decform *form, double x, decimal *d) {
    short digits = form->digits;
    int i;

    __num2dec_internal(d, x);
    if (d->sig.text[0] > 9) {
        return;
    }
    if (digits > SIGDIGLEN) {
        digits = SIGDIGLEN;
    }
    __rounddec(d, digits);
    while (d->sig.length < digits) {
        d->sig.text[d->sig.length++] = 0;
    }
    d->exp -= d->sig.length - 1;
    for (i = 0; i < d->sig.length; i++) {
        d->sig.text[i] += '0';
    }
}

void __num2dec_internal(decimal *d, double x) {
    signed char sign = (signed char)(*(u32 *)&x >> 31);

    if (x == 0.0) {
        d->sign = sign;
        d->exp = 0;
        d->sig.length = 1;
        d->sig.text[0] = 0;
        return;
    }

    if (!(__fpclassifyd(x) > 2)) {
        d->sign = sign;
        d->exp = 0;
        d->sig.length = 1;
        d->sig.text[0] = (__fpclassifyd(x) == 1) ? 'N' : 'I';
        return;
    }

    if (sign) {
        x = -x;
    }

    {
        int exp;
        double frac = frexp(x, &exp);
        short num_bits_extract = 53 - __count_trailing_zero(frac);
        decimal int_d;
        decimal pow2_d;
        double integer;

        __two_exp(&pow2_d, (short)(exp - num_bits_extract));
        modf(ldexp(frac, num_bits_extract), &integer);
        __ull2dec(&int_d, (unsigned long long)integer);
        __timesdec(d, &int_d, &pow2_d);
        d->sign = sign;
    }
}

static void __str2dec(decimal *d, const signed char *s, short exp) {
    int i;

    d->exp = exp;
    d->sign = 0;

    for (i = 0; i < SIGDIGLEN && *s;) {
        d->sig.text[i++] = *s++ - '0';
    }
    d->sig.length = i;

    if (*s != 0) {
        if (*s < 5) {
            return;
        }
        {
            const signed char *p = s + 1;
            for (; *p != 0; p++) {
                if (*p != '0') {
                    goto round_up;
                }
            }
            if ((d->sig.text[i - 1] & 1) == 0) {
                return;
            }
        }
    round_up:
        __dorounddecup(d, d->sig.length);
    }
}

void __ull2dec(decimal *result, unsigned long long val) {
    result->sign = 0;

    if (val == 0) {
        result->exp = 0;
        result->sig.length = 1;
        result->sig.text[0] = 0;
        return;
    }

    result->sig.length = 0;

    for (; val != 0; val /= 10) {
        result->sig.text[result->sig.length++] = val % 10;
    }

    {
        unsigned char *i = result->sig.text;
        unsigned char *j = result->sig.text + result->sig.length;

        for (; i < --j; ++i) {
            unsigned char t = *i;
            *i = *j;
            *j = t;
        }
    }

    result->exp = result->sig.length - 1;
}

s32 __count_trailing_zerol(u32 x) {
    s32 result = 0;
    s32 bits_not_checked = sizeof(u32) * 8;
    s32 n = bits_not_checked / 2;
    s32 mask_size = n;
    u32 mask = (~0UL) >> (bits_not_checked - n);

    while (bits_not_checked) {
        if (!(x & mask)) {
            result += mask_size;
            x >>= mask_size;
            bits_not_checked -= mask_size;
        } else if (mask == 1) {
            break;
        }
        if (n > 1) {
            n /= 2;
        }
        if (mask > 1) {
            mask >>= n;
            mask_size -= n;
        }
    }
    return result;
}