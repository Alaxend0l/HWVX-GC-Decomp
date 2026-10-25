#include "common.h"

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
void *__va_arg(char *ap, s32 type);
char *strchr(const char *str, s32 chr);
u32 strlen(const char *str);
u32 wcstombs(char *s, const u16 *pwcs, u32 n);
void *memchr(const void *src, s32 val, u32 n);
