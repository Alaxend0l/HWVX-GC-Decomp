#include "common.h"


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
