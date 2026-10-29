#include "common.h"
#include "msl_file.h"

typedef struct __OutStrCtrl {
    char *CharStr;
    u32 MaxCharCount;
    u32 CharsWritten;
} __OutStrCtrl;

extern FILE __files[];

typedef struct __va_list_struct {
    char gpr;
    char fpr;
    char reserved[2];
    char *input_arg_area;
    char *reg_save_area;
} va_list[1];

void *memcpy(void *dst, const void *src, u32 n);
u32 fwrite(const void *ptr, u32 memb_size, u32 num_memb, FILE *file);
int fwide(FILE *stream, int mode);

typedef struct print_format {
    u8 justification_options;
    u8 sign_options;
    u8 precision_specified;
    u8 alternate_form;
    u8 argument_options;
    u8 conversion_char;
    s32 field_width;
    s32 precision;
} print_format;

#define SIGDIGLEN 36

typedef struct decimal {
    s8 sign;
    char unk1;
    s16 exp;
    struct {
        u8 length;
        u8 text[SIGDIGLEN];
        u8 unk25;
    } sig;
} decimal;

#pragma readonly_strings on
#pragma pool_strings on

typedef struct decform {
    char style;
    char unk1;
    s16 digits;
} decform;

void __num2dec(const decform *form, double x, decimal *d);
char *strcpy(char *dst, const char *src);

const char *parse_format(const char *format_string, char *arg, print_format *format);
char *longlong2str(s64 num, char *buff, print_format format);
char *long2str(s32 num, char *buff, print_format format);
char *float2str(double num, char *buff, print_format format);
char *double2hex(double num, char *buff, print_format format);
void *__va_arg(void *ap, s32 type);
char *strchr(const char *str, s32 chr);
u32 strlen(const char *str);
u32 wcstombs(char *s, const u16 *pwcs, u32 n);
void *memchr(const void *src, s32 val, u32 n);
extern unsigned char __ctype_map[256];
static void round_decimal(decimal *dec, int new_length);

void *__FileWrite(void *File, const char *Buffer, u32 NumChars);
void *__StringWrite(void *osc, const char *Buffer, u32 NumChars);
int __pformatter(void *(*WriteProc)(void *, const char *, u32), void *WriteProcArg, const char *format_str, char *arg);
#pragma push
#pragma use_lmw_stmw on
int sprintf(char *s, const char *format, ...) {
    va_list args;
    int end;
    __OutStrCtrl osc;

    __builtin_va_info(&args);
    osc.CharStr = s;
    osc.MaxCharCount = 0xFFFFFFFF;
    osc.CharsWritten = 0;
    end = __pformatter(&__StringWrite, &osc, format, (char *)args);
    if (s) {
        s[(u32)end < 0xFFFFFFFF ? end : 0xFFFFFFFF - 1] = '\0';
    }
    return end;
}
#pragma pop
int vprintf(const char *format, char *arg) {
    int retval;
    FILE *stream = &__files[1];

    if (fwide(stream, -1) >= 0) {
        return -1;
    }
    retval = __pformatter(&__FileWrite, (void *)stream, format, arg);
    return retval;
}

int printf(const char *format, ...) {
    int result;
    FILE *stream = &__files[1];
    va_list args;

    if (fwide(stream, -1) >= 0) {
        return -1;
    }
    __builtin_va_info(&args);
    result = __pformatter(&__FileWrite, (void *)stream, format, (char *)args);
    return result;
}

void *__StringWrite(void *osc, const char *Buffer, u32 NumChars) {
    u32 CharsToBeWritten;
    __OutStrCtrl *Oscp = (__OutStrCtrl *)osc;

    CharsToBeWritten = ((Oscp->CharsWritten + NumChars) <= Oscp->MaxCharCount) ? NumChars : Oscp->MaxCharCount - Oscp->CharsWritten;
    memcpy(Oscp->CharStr + Oscp->CharsWritten, Buffer, CharsToBeWritten);
    Oscp->CharsWritten += CharsToBeWritten;
    return (void *)1;
}

void *__FileWrite(void *File, const char *Buffer, u32 NumChars) {
    return (fwrite(Buffer, 1, NumChars, File) == NumChars ? File : 0);
}

int __pformatter(void *(*WriteProc)(void *, const char *, u32), void *WriteProcArg, const char *format_str, char *arg) {
    char buff[512];
    print_format format;
    char fill_char;
    s32 long_num;
    s64 long_long_num;
    double long_double_num;
    char *buff_ptr;
    char *string_end;
    u16 *wcs_ptr;
    const char *format_ptr;
    const char *curr_format;
    s32 num_chars;
    s32 field_width;
    s32 chars_written;

    fill_char = ' ';
    format_ptr = format_str;
    chars_written = 0;

    while (*format_ptr) {
        if (!(curr_format = strchr(format_ptr, '%'))) {
            num_chars = strlen(format_ptr);
            chars_written += num_chars;
            if (num_chars && !(*WriteProc)(WriteProcArg, format_ptr, num_chars)) {
                return -1;
            }
            break;
        }

        num_chars = curr_format - format_ptr;
        chars_written += num_chars;
        if (num_chars && !(*WriteProc)(WriteProcArg, format_ptr, num_chars)) {
            return -1;
        }

        format_ptr = parse_format(curr_format, arg, &format);

        switch (format.conversion_char) {
        case 'd':
        case 'i':
            if (format.argument_options == 3) {
                long_num = *(s32 *)__va_arg(arg, 1);
            } else if (format.argument_options == 4) {
                long_long_num = *(s64 *)__va_arg(arg, 2);
            } else {
                long_num = *(int *)__va_arg(arg, 1);
            }

            if (format.argument_options == 2) {
                long_num = (s16)long_num;
            }
            if (format.argument_options == 1) {
                long_num = (s8)long_num;
            }

            if (format.argument_options == 4) {
                if (!(buff_ptr = longlong2str(long_long_num, buff + 511, format))) {
                    goto conversion_error;
                }
            } else {
                if (!(buff_ptr = long2str(long_num, buff + 511, format))) {
                    goto conversion_error;
                }
            }
            num_chars = buff + 511 - buff_ptr;
            break;

        case 'o':
        case 'u':
        case 'x':
        case 'X':
            if (format.argument_options == 3) {
                long_num = *(u32 *)__va_arg(arg, 1);
            } else if (format.argument_options == 4) {
                long_long_num = *(u64 *)__va_arg(arg, 2);
            } else {
                long_num = *(u32 *)__va_arg(arg, 1);
            }

            if (format.argument_options == 2) {
                long_num = (u16)long_num;
            }
            if (format.argument_options == 1) {
                long_num = (u8)long_num;
            }

            if (format.argument_options == 4) {
                if (!(buff_ptr = longlong2str(long_long_num, buff + 511, format))) {
                    goto conversion_error;
                }
            } else {
                if (!(buff_ptr = long2str(long_num, buff + 511, format))) {
                    goto conversion_error;
                }
            }
            num_chars = buff + 511 - buff_ptr;
            break;

        case 'f':
        case 'F':
        case 'e':
        case 'E':
        case 'g':
        case 'G':
            if (format.argument_options == 5) {
                long_double_num = *(double *)__va_arg(arg, 3);
            } else {
                long_double_num = *(double *)__va_arg(arg, 3);
            }

            if (!(buff_ptr = float2str(long_double_num, buff + 511, format))) {
                goto conversion_error;
            }
            num_chars = buff + 511 - buff_ptr;
            break;

        case 'a':
        case 'A':
            if (format.argument_options == 5) {
                long_double_num = *(double *)__va_arg(arg, 3);
            } else {
                long_double_num = *(double *)__va_arg(arg, 3);
            }

            if (!(buff_ptr = double2hex(long_double_num, buff + 511, format))) {
                goto conversion_error;
            }
            num_chars = buff + 511 - buff_ptr;
            break;

        case 's':
            if (format.argument_options == 6) {
                wcs_ptr = *(u16 **)__va_arg(arg, 1);
                if (wcs_ptr == NULL) {
                    wcs_ptr = L"";
                }
                if ((num_chars = wcstombs(buff, wcs_ptr, sizeof(buff))) < 0) {
                    goto conversion_error;
                }
                buff_ptr = &buff[0];
            } else {
                buff_ptr = *(char **)__va_arg(arg, 1);
            }

            if (buff_ptr == NULL) {
                buff_ptr = "";
            }

            if (format.alternate_form) {
                num_chars = (u8)*buff_ptr++;
                if (format.precision_specified && num_chars > format.precision) {
                    num_chars = format.precision;
                }
            } else if (format.precision_specified) {
                num_chars = format.precision;
                if ((string_end = (char *)memchr(buff_ptr, 0, num_chars)) != 0) {
                    num_chars = string_end - buff_ptr;
                }
            } else {
                num_chars = strlen(buff_ptr);
            }
            break;

        case 'n':
            buff_ptr = *(char **)__va_arg(arg, 1);
            switch (format.argument_options) {
            case 0:
                *(int *)buff_ptr = chars_written;
                break;
            case 2:
                *(s16 *)buff_ptr = chars_written;
                break;
            case 3:
                *(s32 *)buff_ptr = chars_written;
                break;
            case 4:
                *(s64 *)buff_ptr = chars_written;
                break;
            }
            continue;

        case 'c':
            buff_ptr = buff;
            *buff_ptr = *(int *)__va_arg(arg, 1);
            num_chars = 1;
            break;

        case '%':
            buff_ptr = buff;
            *buff_ptr = '%';
            num_chars = 1;
            break;

        case 0xFF:
        default:
        conversion_error:
            num_chars = strlen(curr_format);
            chars_written += num_chars;
            if (num_chars && !(*WriteProc)(WriteProcArg, curr_format, num_chars)) {
                return -1;
            }
            return chars_written;
        }

        field_width = num_chars;

        if (format.justification_options != 0) {
            fill_char = (format.justification_options == 2) ? '0' : ' ';
            if (((*buff_ptr == '+') || (*buff_ptr == '-') || (*buff_ptr == ' ')) && (fill_char == '0')) {
                if ((*WriteProc)(WriteProcArg, buff_ptr, 1) == 0) {
                    return -1;
                }
                ++buff_ptr;
                num_chars--;
            }

            while (field_width < format.field_width) {
                if ((*WriteProc)(WriteProcArg, &fill_char, 1) == 0) {
                    return -1;
                }
                ++field_width;
            }
        }

        if (num_chars && !(*WriteProc)(WriteProcArg, buff_ptr, num_chars)) {
            return -1;
        }

        if (format.justification_options == 0) {
            while (field_width < format.field_width) {
                char blank = ' ';
                if ((*WriteProc)(WriteProcArg, &blank, 1) == 0) {
                    return -1;
                }
                ++field_width;
            }
        }

        chars_written += field_width;
    }

    return chars_written;
}

char *float2str(double num, char *buff, print_format format) {
    decimal dec;
    decform form;
    char *p;
    char *q;
    int n;
    int digits;
    int sign;
    int int_digits;
    int frac_digits;

    if (format.precision > 509) {
        return 0;
    }

    form.style = 0;
    form.digits = 0x20;
    __num2dec(&form, num, &dec);

    p = (char *)dec.sig.text + dec.sig.length;
    while (dec.sig.length > 1 && (s8)*--p == '0') {
        --dec.sig.length;
        ++dec.exp;
    }

    switch (*dec.sig.text) {
    case '0':
        dec.exp = 0;
        break;
    case 'I':
        if (num < 0.0) {
            p = buff - 5;
            if (__ctype_map[format.conversion_char] & 0x80) {
                strcpy(p, "-INF");
            } else {
                strcpy(p, "-inf");
            }
        } else {
            p = buff - 4;
            if (__ctype_map[format.conversion_char] & 0x80) {
                strcpy(p, "INF");
            } else {
                strcpy(p, "inf");
            }
        }
        return p;
    case 'N':
        if (dec.sign) {
            p = buff - 5;
            if (__ctype_map[format.conversion_char] & 0x80) {
                strcpy(p, "-NAN");
            } else {
                strcpy(p, "-nan");
            }
        } else {
            p = buff - 4;
            if (__ctype_map[format.conversion_char] & 0x80) {
                strcpy(p, "NAN");
            } else {
                strcpy(p, "nan");
            }
        }
        return p;
    }

    dec.exp += dec.sig.length - 1;
    p = buff;
    *--p = 0;

    switch (format.conversion_char) {
    case 'g':
    case 'G':
        if (dec.sig.length > format.precision) {
            round_decimal(&dec, format.precision);
        }
        if (dec.exp < -4 || dec.exp >= format.precision) {
            if (format.alternate_form) {
                --format.precision;
            } else {
                format.precision = dec.sig.length - 1;
            }
            if (format.conversion_char == 'g') {
                format.conversion_char = 'e';
            } else {
                format.conversion_char = 'E';
            }
            goto e_format;
        }
        if (format.alternate_form) {
            format.precision -= dec.exp + 1;
        } else {
            if ((format.precision = dec.sig.length - (dec.exp + 1)) < 0) {
                format.precision = 0;
            }
        }
        goto f_format;

    case 'e':
    case 'E':
    e_format:
        if (dec.sig.length > format.precision + 1) {
            round_decimal(&dec, format.precision + 1);
        }
        n = dec.exp;
        sign = '+';
        if (n < 0) {
            n = -n;
            sign = '-';
        }
        for (digits = 0; n || digits < 2; ++digits) {
            *--p = n % 10 + '0';
            n /= 10;
        }
        *--p = sign;
        *--p = format.conversion_char;
        if (buff - p + format.precision > 509) {
            return 0;
        }
        if (dec.sig.length < format.precision + 1) {
            for (n = format.precision + 1 - dec.sig.length + 1; --n;) {
                *--p = '0';
            }
        }
        for (n = dec.sig.length, q = (char *)dec.sig.text + dec.sig.length; --n;) {
            *--p = *--q;
        }
        if (format.precision || format.alternate_form) {
            *--p = '.';
        }
        *--p = *dec.sig.text;
        if (dec.sign) {
            *--p = '-';
        } else if (format.sign_options == 1) {
            *--p = '+';
        } else if (format.sign_options == 2) {
            *--p = ' ';
        }
        break;

    case 'f':
    case 'F':
    f_format:
        if ((frac_digits = -dec.exp + dec.sig.length - 1) < 0) {
            frac_digits = 0;
        }
        if (frac_digits > format.precision) {
            round_decimal(&dec, dec.sig.length - (frac_digits - format.precision));
            if ((frac_digits = -dec.exp + dec.sig.length - 1) < 0) {
                frac_digits = 0;
            }
        }
        if ((int_digits = dec.exp + 1) < 0) {
            int_digits = 0;
        }
        if (int_digits + frac_digits > 509) {
            return 0;
        }
        q = (char *)dec.sig.text + dec.sig.length;
        for (digits = 0; digits < (format.precision - frac_digits); ++digits) {
            *--p = '0';
        }
        for (digits = 0; digits < frac_digits && digits < dec.sig.length; ++digits) {
            *--p = *--q;
        }
        for (; digits < frac_digits; ++digits) {
            *--p = '0';
        }
        if (format.precision || format.alternate_form) {
            *--p = '.';
        }
        if (int_digits) {
            for (digits = 0; digits < int_digits - dec.sig.length; ++digits) {
                *--p = '0';
            }
            for (; digits < int_digits; ++digits) {
                *--p = *--q;
            }
        } else {
            *--p = '0';
        }
        if (dec.sign) {
            *--p = '-';
        } else if (format.sign_options == 1) {
            *--p = '+';
        } else if (format.sign_options == 2) {
            *--p = ' ';
        }
        break;
    }

    return p;
}

static void round_decimal(decimal *dec, int new_length) {
    signed char c;
    signed char *p;
    int carry;

    if (new_length < 0) {
    return_zero:
        dec->exp = 0;
        dec->sig.length = 1;
        *dec->sig.text = '0';
        return;
    }

    if (new_length >= dec->sig.length) {
        return;
    }

    p = (signed char *)dec->sig.text + new_length + 1;
    c = *--p - '0';

    if (c == 5) {
        signed char *q = (signed char *)dec->sig.text + dec->sig.length;

        while (--q > p && *q == '0') {
        }

        carry = (q == p) ? p[-1] & 1 : 1;
    } else {
        carry = (c > 5);
    }

    while (new_length != 0) {
        c = *--p - '0' + carry;

        if ((carry = (c > 9)) != 0 || c == 0) {
            --new_length;
        } else {
            *p = c + '0';
            break;
        }
    }

    if (carry != 0) {
        dec->exp += 1;
        dec->sig.length = 1;
        *dec->sig.text = '1';
        return;
    } else if (new_length == 0) {
        goto return_zero;
    }

    dec->sig.length = new_length;
}

#pragma push
#pragma readonly_strings on
#pragma pool_strings on
char *double2hex(double num, char *buff, print_format format) {
    char *p;
    u8 *q;
    u8 working_byte;
    double ld;
    s32 exp;
    print_format exp_format;
    s32 hex_precision;
    s8 c;
    decform form;
    decimal dec;

    p = buff;
    ld = num;

    if (format.precision > 509) {
        return 0;
    }

    form.style = 0;
    form.digits = 0x20;
    __num2dec(&form, num, &dec);

    if (dec.sig.text[0] == 'I') {
        if (*(s16 *)&ld & 0x8000) {
            p = buff - 5;
            if (format.conversion_char == 'A') {
                strcpy(p, "-INF");
            } else {
                strcpy(p, "-inf");
            }
        } else {
            p = buff - 4;
            if (format.conversion_char == 'A') {
                strcpy(p, "INF");
            } else {
                strcpy(p, "inf");
            }
        }
        return p;
    } else if (dec.sig.text[0] == 'N') {
        if (*(u8 *)&num & 0x80) {
            p = buff - 5;
            if (format.conversion_char == 'A') {
                strcpy(p, "-NAN");
            } else {
                strcpy(p, "-nan");
            }
        } else {
            p = buff - 4;
            if (format.conversion_char == 'A') {
                strcpy(p, "NAN");
            } else {
                strcpy(p, "nan");
            }
        }
        return p;
    }

    exp_format.justification_options = 1;
    exp_format.sign_options = 1;
    exp_format.precision_specified = 0;
    exp_format.alternate_form = 0;
    exp_format.argument_options = 0;
    exp_format.conversion_char = 'd';
    exp_format.field_width = 0;
    exp_format.precision = 1;

    exp = (s16)((*(s16 *)&ld & 0x7FF0) >> 4) - 1023;
    p = long2str(exp, buff, exp_format);

    if (format.conversion_char == 'a') {
        *--p = 'p';
    } else {
        *--p = 'P';
    }

    q = (u8 *)&num;
    for (hex_precision = format.precision; hex_precision >= 1; hex_precision--) {
        working_byte = q[hex_precision / 2 + 1];
        if (hex_precision % 2) {
            c = working_byte & 0xF;
        } else {
            c = (working_byte >> 4) & 0xF;
        }
        if (c < 10) {
            c += '0';
        } else if (format.conversion_char == 'a') {
            c += 'a' - 10;
        } else {
            c += 'A' - 10;
        }
        *--p = c;
    }

    if (format.precision || format.alternate_form) {
        *--p = '.';
    }
    *--p = '1';
    if (format.conversion_char == 'a') {
        *--p = 'x';
    } else {
        *--p = 'X';
    }
    *--p = '0';

    if (*(s16 *)&ld & 0x8000) {
        *--p = '-';
    } else if (format.sign_options == 1) {
        *--p = '+';
    } else if (format.sign_options == 2) {
        *--p = ' ';
    }
    return p;
}
#pragma pop

#pragma push
#pragma use_lmw_stmw on
char *longlong2str(s64 num, char *buff, print_format format) {
    u64 unsigned_num;
    u64 base;
    char *p;
    s32 n;
    s32 digits;
    s32 minus = 0;

    unsigned_num = num;
    minus = 0;
    p = buff;
    *--p = 0;
    digits = 0;
    if (!num && !format.precision && !(format.alternate_form && format.conversion_char == 'o')) {
        return p;
    }
    switch (format.conversion_char) {
    case 'd':
    case 'i':
        base = 10;
        if (num < 0) {
            unsigned_num = -unsigned_num;
            minus = 1;
        }
        break;
    case 'o':
        base = 8;
        format.sign_options = 0;
        break;
    case 'u':
        base = 10;
        format.sign_options = 0;
        break;
    case 'x':
    case 'X':
        base = 16;
        format.sign_options = 0;
        break;
    }
    do {
        n = unsigned_num % base;
        unsigned_num /= base;
        if (n < 10) {
            n += '0';
        } else {
            n -= 10;
            if (format.conversion_char == 'x') {
                n += 'a';
            } else {
                n += 'A';
            }
        }
        *--p = n;
        ++digits;
    } while (unsigned_num != 0);
    if (base == 8 && format.alternate_form && *(s8 *)p != '0') {
        *--p = '0';
        ++digits;
    }
    if (format.justification_options == 2) {
        format.precision = format.field_width;
        if (minus || format.sign_options != 0) {
            --format.precision;
        }
        if (base == 16 && format.alternate_form) {
            format.precision -= 2;
        }
    }
    if (buff - p + format.precision > 509) {
        return NULL;
    }
    while (digits < format.precision) {
        *--p = '0';
        ++digits;
    }
    if (base == 16 && format.alternate_form) {
        *--p = format.conversion_char;
        *--p = '0';
    }
    if (minus) {
        *--p = '-';
    } else if (format.sign_options == 1) {
        *--p = '+';
    } else if (format.sign_options == 2) {
        *--p = ' ';
    }
    return p;
}
#pragma pop
char *long2str(s32 num, char *buff, print_format format) {
    u32 unsigned_num;
    u32 base;
    char *p;
    s32 n;
    s32 digits;
    s32 minus = 0;

    unsigned_num = num;
    minus = 0;
    p = buff;
    *--p = 0;
    digits = 0;
    if (!num && !format.precision && !(format.alternate_form && format.conversion_char == 'o')) {
        return p;
    }
    switch (format.conversion_char) {
    case 'd':
    case 'i':
        base = 10;
        if (num < 0) {
            unsigned_num = -unsigned_num;
            minus = 1;
        }
        break;
    case 'o':
        base = 8;
        format.sign_options = 0;
        break;
    case 'u':
        base = 10;
        format.sign_options = 0;
        break;
    case 'x':
    case 'X':
        base = 16;
        format.sign_options = 0;
        break;
    }
    do {
        n = unsigned_num % base;
        unsigned_num /= base;
        if (n < 10) {
            n += '0';
        } else {
            n -= 10;
            if (format.conversion_char == 'x') {
                n += 'a';
            } else {
                n += 'A';
            }
        }
        *--p = n;
        ++digits;
    } while (unsigned_num != 0);
    if (base == 8 && format.alternate_form && *(s8 *)p != '0') {
        *--p = '0';
        ++digits;
    }
    if (format.justification_options == 2) {
        format.precision = format.field_width;
        if (minus || format.sign_options != 0) {
            --format.precision;
        }
        if (base == 16 && format.alternate_form) {
            format.precision -= 2;
        }
    }
    if (buff - p + format.precision > 509) {
        return NULL;
    }
    while (digits < format.precision) {
        *--p = '0';
        ++digits;
    }
    if (base == 16 && format.alternate_form) {
        *--p = format.conversion_char;
        *--p = '0';
    }
    if (minus) {
        *--p = '-';
    } else if (format.sign_options == 1) {
        *--p = '+';
    } else if (format.sign_options == 2) {
        *--p = ' ';
    }
    return p;
}

extern unsigned char __ctype_map[256];
const char *parse_format(const char *format_string, char *arg, print_format *format) {
    print_format f;
    const signed char *s = (const signed char *)format_string;
    int c;
    int flag_found;

    f.justification_options = 1;
    f.sign_options = 0;
    f.precision_specified = 0;
    f.alternate_form = 0;
    f.argument_options = 0;
    f.field_width = 0;
    f.precision = 0;

    if ((c = *++s) == '%') {
        f.conversion_char = c;
        *format = f;
        return (const char *)s + 1;
    }

    for (;;) {
        flag_found = 1;
        switch (c) {
        case '-':
            f.justification_options = 0;
            break;
        case '+':
            f.sign_options = 1;
            break;
        case ' ':
            if (f.sign_options != 1) {
                f.sign_options = 2;
            }
            break;
        case '#':
            f.alternate_form = 1;
            break;
        case '0':
            if (f.justification_options != 0) {
                f.justification_options = 2;
            }
            break;
        default:
            flag_found = 0;
            break;
        }
        if (flag_found) {
            c = *++s;
        } else {
            break;
        }
    }

    if (c == '*') {
        if ((f.field_width = *(int *)__va_arg(arg, 1)) < 0) {
            f.justification_options = 0;
            f.field_width = -f.field_width;
        }
        c = *++s;
    } else {
        while (__ctype_map[(u8)c] & 0x10) {
            f.field_width = (f.field_width * 10) + (c - '0');
            c = *++s;
        }
    }

    if (f.field_width > 509) {
        f.conversion_char = 0xFF;
        *format = f;
        return (const char *)s + 1;
    }

    if (c == '.') {
        f.precision_specified = 1;
        if ((c = *++s) == '*') {
            if ((f.precision = *(int *)__va_arg(arg, 1)) < 0) {
                f.precision_specified = 0;
            }
            c = *++s;
        } else {
            while (__ctype_map[(u8)c] & 0x10) {
                f.precision = (f.precision * 10) + (c - '0');
                c = *++s;
            }
        }
    }

    flag_found = 1;
    switch (c) {
    case 'h':
        f.argument_options = 2;
        if (s[1] == 'h') {
            f.argument_options = 1;
            c = *++s;
        }
        break;
    case 'l':
        f.argument_options = 3;
        if (s[1] == 'l') {
            f.argument_options = 4;
            c = *++s;
        }
        break;
    case 'L':
        f.argument_options = 5;
        break;
    default:
        flag_found = 0;
    }

    if (flag_found) {
        c = *++s;
    }

    f.conversion_char = c;

    switch (c) {
    case 'd':
    case 'i':
    case 'u':
    case 'o':
    case 'x':
    case 'X':
        if (f.argument_options == 5) {
            f.conversion_char = 0xFF;
            break;
        }
        if (!f.precision_specified) {
            f.precision = 1;
        } else if (f.justification_options == 2) {
            f.justification_options = 1;
        }
        break;
    case 'f':
    case 'F':
        if (f.argument_options == 2 || f.argument_options == 4) {
            f.conversion_char = 0xFF;
            break;
        }
        if (!f.precision_specified) {
            f.precision = 6;
        }
        break;
    case 'a':
    case 'A':
        if (!f.precision_specified) {
            f.precision = 13;
        }
        if (f.argument_options == 2 || f.argument_options == 4 || f.argument_options == 1) {
            f.conversion_char = 0xFF;
        }
        break;
    case 'g':
    case 'G':
        if (!f.precision) {
            f.precision = 1;
        }
    case 'e':
    case 'E':
        if (f.argument_options == 2 || f.argument_options == 4 || f.argument_options == 1) {
            f.conversion_char = 0xFF;
            break;
        }
        if (!f.precision_specified) {
            f.precision = 6;
        }
        break;
    case 'p':
        f.conversion_char = 'x';
        f.alternate_form = 1;
        f.argument_options = 3;
        f.precision = 8;
        break;
    case 'c':
        if (f.argument_options == 3) {
            f.argument_options = 6;
        } else if (f.precision_specified || f.argument_options != 0) {
            f.conversion_char = 0xFF;
        }
        break;
    case 's':
        if (f.argument_options == 3) {
            f.argument_options = 6;
        } else if (f.argument_options != 0) {
            f.conversion_char = 0xFF;
        }
        break;
    case 'n':
        if (f.argument_options == 5) {
            f.conversion_char = 0xFF;
        }
        break;
    default:
        f.conversion_char = 0xFF;
        break;
    }

    *format = f;
    return (const char *)s + 1;
}

