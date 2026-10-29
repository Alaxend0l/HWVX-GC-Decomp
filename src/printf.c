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
    char sign;
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
    char c;
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

