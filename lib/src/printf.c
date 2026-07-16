/*
printf.c - своя реализация printf.

Хаиров Егор Вадимович
МК-101
*/

#include "utils.h"
#include "EgoIO.h"
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

// Вспомогательная функция для формирования строки с учётом ширины и выравнивания
static int format_output(char *out, size_t out_size, const char *str, int len,
                         const SpecInfo *info, bool is_signed) {
    int width = info->width;
    int precision = info->precision;

    if(info->specifier == 's' && precision > 0 && len > precision)
        len = precision;

    if(width < len)
        width = len;

    int extra = 0;
    char sign_char = 0;
    if(is_signed && info->show_plus && str[0] != '-') {
        sign_char = '+';
        extra = 1;
    }
    else if(is_signed && info->show_space && str[0] != '-') {
        sign_char = ' ';
        extra = 1;
    }

    int total_width = width + extra;
    if(total_width + 1 > (int)out_size)
        return -1;

    char *out_ptr = out;
    char pad_char = (info->zero_pad && !info->left_align) ? '0' : ' ';

    if(sign_char && pad_char == '0') {
        *out_ptr++ = sign_char;
        total_width--;
    }

    if(!info->left_align) {
        int pad_count = total_width - len;
        for(int i = 0; i < pad_count; i++)
            *out_ptr++ = pad_char;
    }

    if(sign_char && pad_char != '0') {
        *out_ptr++ = sign_char;
        total_width--;
    }

    for(int i = 0; i < len; i++)
        *out_ptr++ = str[i];

    if(info->left_align) {
        int pad_count = total_width - len;
        for(int i = 0; i < pad_count; i++)
            *out_ptr++ = ' ';
    }

    *out_ptr = '\0';
    return (int)(out_ptr - out);
}

int ego_printf(const char *format, char **p, ...) {
    va_list args;
    va_start(args, p);

    const char *f = format;
    int total_bytes = 0;

    while(*f) {
        if(*f != '%') {
            char ch = *f++;
            int written = console_write(&ch, 1);
            if(written < 0) {
                va_end(args);
                return -1;
            }
            total_bytes += written;
            continue;
        }

        f++;
        SpecInfo info;
        int err = parse_format_specifier(&f, &info, &args);  // передаём &args
        if(err != 0) {
            if(p) *p = (char*)f;
            va_end(args);
            return -1;
        }

        char tmp[256];
        int len = 0;
        bool is_signed = false;

        switch (info.specifier) {
            case 'd': {
                int val = va_arg(args, int);
                long long ll = val;
                int base = 10;
                const char *alpha = NULL;
                len = format_signed(ll, base, alpha, tmp, sizeof(tmp), &info);
                if(len < 0) {
                    if(p) *p = (char*)f;
                    va_end(args);
                    return -1;
                }
                is_signed = true;
                break;
            }
            case 'u': {
                unsigned int val = va_arg(args, unsigned int);
                unsigned long long ull = val;
                int base = 10;
                const char *alpha = NULL;
                len = format_unsigned(ull, base, alpha, tmp, sizeof(tmp), &info);
                if(len < 0) {
                    if(p) *p = (char*)f;
                    va_end(args);
                    return -1;
                }
                is_signed = false;
                break;
            }
            case 'x': {
                unsigned int val = va_arg(args, unsigned int);
                unsigned long long ull = val;
                len = format_unsigned(ull, 16, NULL, tmp, sizeof(tmp), &info);
                if(len < 0) {
                    if(p) *p = (char*)f;
                    va_end(args);
                    return -1;
                }
                if(info.alternate_form && ull != 0) {
                    memmove(tmp + 2, tmp, len + 1);
                    tmp[0] = '0';
                    tmp[1] = 'x';
                    len += 2;
                }
                is_signed = false;
                break;
            }
            case 'X': {
                unsigned int val = va_arg(args, unsigned int);
                unsigned long long ull = val;
                const char *upper_alpha = "0123456789ABCDEF";
                len = format_unsigned(ull, 16, upper_alpha, tmp, sizeof(tmp), &info);
                if(len < 0) {
                    if(p) *p = (char*)f;
                    va_end(args);
                    return -1;
                }
                if(info.alternate_form && ull != 0) {
                    memmove(tmp + 2, tmp, len + 1);
                    tmp[0] = '0';
                    tmp[1] = 'X';
                    len += 2;
                }
                is_signed = false;
                break;
            }
            case 's': {
                const char *str = va_arg(args, const char*);
                if(!str) str = "(null)";
                len = strlen(str);
                if(len >= (int)sizeof(tmp)) len = sizeof(tmp) - 1;
                memcpy(tmp, str, len);
                tmp[len] = '\0';
                is_signed = false;
                break;
            }
            case 'c': {
                char ch = (char)va_arg(args, int);
                tmp[0] = ch;
                tmp[1] = '\0';
                len = 1;
                is_signed = false;
                break;
            }
            case 'b': {
                long long val = va_arg(args, long long);
                int base = info.custom_base ? info.custom_base : 10;
                const char *alpha = info.custom_alphabet[0] ? info.custom_alphabet : NULL;
                len = format_signed(val, base, alpha, tmp, sizeof(tmp), &info);
                if(len < 0) {
                    if(p) *p = (char*)f;
                    va_end(args);
                    return -1;
                }
                is_signed = true;
                break;
            }
            case 'B': {
                unsigned long long val = va_arg(args, unsigned long long);
                int base = info.custom_base ? info.custom_base : 10;
                const char *alpha = info.custom_alphabet[0] ? info.custom_alphabet : NULL;
                len = format_unsigned(val, base, alpha, tmp, sizeof(tmp), &info);
                if(len < 0) {
                    if(p) *p = (char*)f;
                    va_end(args);
                    return -1;
                }
                is_signed = false;
                break;
            }
            default:
                if(p) *p = (char*)f;
                va_end(args);
                return -1;
        }

        char out_buf[512];
        int out_len = format_output(out_buf, sizeof(out_buf), tmp, len, &info, is_signed);
        if(out_len < 0) {
            va_end(args);
            return -1;
        }

        int written = console_write(out_buf, out_len);
        if(written < 0) {
            va_end(args);
            return -1;
        }
        total_bytes += written;
    }

    va_end(args);
    return total_bytes;
}