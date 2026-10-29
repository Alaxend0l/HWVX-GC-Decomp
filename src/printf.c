#include "common.h"
#include "msl_file.h"

typedef struct __OutStrCtrl {
    char *CharStr;
    u32 MaxCharCount;
    u32 CharsWritten;
} __OutStrCtrl;

extern FILE __files[];

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
