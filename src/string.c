#include "common.h"


char *strstr(const char *str, const char *pat) {
    const unsigned char *s1 = (const unsigned char *)str - 1;
    const unsigned char *p1 = (const unsigned char *)pat - 1;
    unsigned long firstc, c1, c2;

    if ((pat == 0) || (!(firstc = *++p1))) {
        return (char *)str;
    }

    while (c1 = *++s1) {
        if (c1 == firstc) {
            const unsigned char *s2 = s1 - 1;
            const unsigned char *p2 = p1 - 1;

            while ((c1 = *++s2) == (c2 = *++p2) && c1)
                ;

            if (!c2) {
                return (char *)s1;
            }
        }
    }

    return 0;
}

char *strrchr(const char *str, int chr) {
    const unsigned char *p = (unsigned char *)str - 1;
    const unsigned char *q = 0;
    unsigned long c = (chr & 0xff);
    unsigned long ch;

    while (ch = *++p) {
        if (ch == c) {
            q = p;
        }
    }

    if (q) {
        return (char *)q;
    }

    return c ? 0 : (char *)p;
}

char *strchr(const char *str, int chr) {
    const unsigned char *p = (unsigned char *)str - 1;
    unsigned long c = (chr & 0xff);
    unsigned long ch;

    while (ch = *++p) {
        if (ch == c) {
            return (char *)p;
        }
    }

    return c ? 0 : (char *)p;
}

int strcmp(const char *str1, const char *str2)
{
    register u8 *left = (u8 *)str1;
    register u8 *right = (u8 *)str2;
    u32 align, l1, r1, x;

    l1 = *left;
    r1 = *right;
    if (l1 - r1) {
        return l1 - r1;
    }

    if ((align = ((u32)left & 3)) != ((u32)right & 3)) {
        goto bytecopy;
    }

    if (align) {
        if (l1 == 0) {
            return 0;
        }
        for (align = 3 - align; align; align--) {
            l1 = *(++left);
            r1 = *(++right);
            if (l1 - r1) {
                return l1 - r1;
            }
            if (l1 == 0) {
                return 0;
            }
        }
        left++;
        right++;
    }

    l1 = *(u32 *)left;
    r1 = *(u32 *)right;
    x = l1 + 0xFEFEFEFF;
    if (x & 0x80808080) {
        goto adjust;
    }

    while (l1 == r1) {
        l1 = *(++((u32 *)left));
        r1 = *(++((u32 *)right));
        x = l1 + 0xFEFEFEFF;
        if (x & 0x80808080) {
            goto adjust;
        }
    }

    if (l1 > r1) {
        return 1;
    }
    return -1;

adjust:
    l1 = *left;
    r1 = *right;
    if (l1 - r1) {
        return l1 - r1;
    }

bytecopy:
    if (l1 == 0) {
        return 0;
    }

    do {
        l1 = *(++left);
        r1 = *(++right);
        if (l1 - r1) {
            return l1 - r1;
        }
        if (l1 == 0) {
            return 0;
        }
    } while (1);
}

char *strcat(char *dst, const char *src) {
    const unsigned char *p = (unsigned char *)src - 1;
    unsigned char *q = (unsigned char *)dst - 1;

    while (*++q)
        ;

    q--;

    while (*++q = *++p)
        ;

    return dst;
}

char *strncpy(char *dst, const char *src, u32 n) {
    const u8 *p = (const u8 *)src - 1;
    u8 *q = (u8 *)dst - 1;

    n++;
    while (--n) {
        if (!(*++q = *++p)) {
            while (--n) {
                *++q = 0;
            }
            break;
        }
    }

    return dst;
}

char *strcpy(char *dst, const char *src) {
    register u8 *destb, *fromb;
    register u32 w, t, align;

    fromb = (u8 *)src;
    destb = (u8 *)dst;

    if ((align = ((s32)fromb & 3)) != ((s32)destb & 3)) {
        goto bytecopy;
    }

    if (align) {
        if ((*destb = *fromb) == 0) {
            return dst;
        }
        for (align = 3 - align; align; align--) {
            if ((*(++destb) = *(++fromb)) == 0) {
                return dst;
            }
        }
        ++destb;
        ++fromb;
    }

    w = *((u32 *)fromb);
    t = w + 0xFEFEFEFF;
    t &= 0x80808080;
    if (t) {
        goto bytecopy;
    }
    --((u32 *)destb);

    do {
        *(++((u32 *)destb)) = w;
        w = *(++((u32 *)fromb));
        t = w + 0xFEFEFEFF;
        t &= 0x80808080;
        if (t) {
            goto adjust;
        }
    } while (1);

adjust:
    ++((u32 *)destb);

bytecopy:
    if ((*destb = *fromb) == 0) {
        return dst;
    }

    do {
        if ((*(++destb) = *(++fromb)) == 0) {
            return dst;
        }
    } while (1);

    return dst;
}

u32 strlen(const char *str) {
    u32 len = -1;
    u8 *p = (u8 *)str - 1;

    do {
        len++;
    } while (*++p);

    return len;
}
