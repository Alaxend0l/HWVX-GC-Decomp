

int memcmp(const void *src1, const void *src2, unsigned long n) {
    const unsigned char *p1;
    const unsigned char *p2;

    for (p1 = (const unsigned char *)src1 - 1, p2 = (const unsigned char *)src2 - 1, n++; --n;) {
        if (*++p1 != *++p2) {
            return (*p1 < *p2) ? -1 : +1;
        }
    }

    return 0;
}

void *__memrchr(const void *src, int val, unsigned long n) {
    const unsigned char *p;
    unsigned long v = (val & 0xff);

    for (p = (unsigned char *)src + n, n++; --n;) {
        if (*--p == v) {
            return (void *)p;
        }
    }

    return 0;
}

