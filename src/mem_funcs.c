

void __copy_longs_unaligned(void *dst, const void *src, unsigned long n) {
    unsigned long i, v1, v2;
    unsigned long src_offset, left_shift, right_shift;

#define cps ((unsigned char *)src)
#define cpd ((unsigned char *)dst)
#define lps ((unsigned long *)src)
#define lpd ((unsigned long *)dst)
#define deref_auto_inc(p) *++(p)

    i = (-(unsigned long)dst) & 3;

    cps = ((unsigned char *)src) - 1;
    cpd = ((unsigned char *)dst) - 1;

    if (i) {
        n -= i;
        do {
            deref_auto_inc(cpd) = deref_auto_inc(cps);
        } while (--i);
    }

    src_offset = ((unsigned long)(cps + 1)) & 3;

    left_shift = src_offset << 3;
    right_shift = 32 - left_shift;

    cps -= src_offset;

    lps = ((unsigned long *)(cps + 1)) - 1;
    lpd = ((unsigned long *)(cpd + 1)) - 1;

    i = n >> 3;

    v1 = deref_auto_inc(lps);

    do {
        v2 = deref_auto_inc(lps);
        deref_auto_inc(lpd) = (v1 << left_shift) | (v2 >> right_shift);
        v1 = deref_auto_inc(lps);
        deref_auto_inc(lpd) = (v2 << left_shift) | (v1 >> right_shift);
    } while (--i);

    if (n & 4) {
        v2 = deref_auto_inc(lps);
        deref_auto_inc(lpd) = (v1 << left_shift) | (v2 >> right_shift);
    }

    cps = ((unsigned char *)(lps + 1)) - 1;
    cpd = ((unsigned char *)(lpd + 1)) - 1;

    n &= 3;

    if (n) {
        cps -= 4 - src_offset;
        do {
            deref_auto_inc(cpd) = deref_auto_inc(cps);
        } while (--n);
    }

#undef cps
#undef cpd
#undef lps
#undef lpd
#undef deref_auto_inc
}
