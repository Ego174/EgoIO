/*
utils.c - внутренние заголовки для EgoIO.

Хаиров Егор Вадимович
МК-101
*/

// Вспомогательные функции для ввода/вывода и преобразований.

#include "utils.h"
#include <string.h>
#include <limits.h>

#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

int (*console_write_ptr)(const char*, size_t) = NULL;
int (*console_read_ptr)(char*, size_t) = NULL;

static const char *default_alphabet = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

// Вывод
int console_write(const char *buf, size_t count) {
    if (!buf || count == 0)
        return 0;

    if (console_write_ptr)
        return console_write_ptr(buf, count);

#ifdef _WIN32
    ssize_t res = _write(1, buf, count);
    if(res < 0)
        return -1;
    return (int)res;
#else
    ssize_t res = write(1, buf, count);
    if(res < 0)
        return -1;
    return (int)res;
#endif
}

// Ввод
int console_read(char *buf, size_t count) {
    if (!buf || count == 0)
        return 0;

    if (console_read_ptr)
        return console_read_ptr(buf, count);

#ifdef _WIN32
    ssize_t res = _read(0, buf, count);
    if (res < 0)
        return -1;
    return (int)res;
#else
    ssize_t res = read(0, buf, count);
    if (res < 0)
        return -1;
    return (int)res;
#endif
}

// Парсер спецификатора
int parse_format_specifier(const char **format, SpecInfo *info, va_list *args) {
    const char *f = *format;
    memset(info, 0, sizeof(SpecInfo));

    // Флаги
    int done = 0;
    while(!done) {
        switch(*f) {
            case '-': info->left_align = true; f++; break;
            case '0': info->zero_pad = true; f++; break;
            case '+': info->show_plus = true; f++; break;
            case ' ': info->show_space = true; f++; break;
            case '#': info->alternate_form = true; f++; break;
            default: done = 1; break;
        }
    }

    // Ширина
    if(*f == '*') {
        f++;
        int w = va_arg(*args, int);
        if(w < 0) {
            info->left_align = true;
            info->width = -w;
        }
        else info->width = w;
    }
    else if(*f >= '0' && *f <= '9') {
        int val = 0;
        while(*f >= '0' && *f <= '9') {
            val = val * 10 + (*f - '0');
            f++;
        }
        info->width = val;
    }

    // Точность
    if(*f == '.') {
        f++;
        if(*f == '*') {
            f++;
            int prec = va_arg(*args, int);
            if(prec < 0) prec = 0;
            info->precision = prec;
        }
        else if (*f >= '0' && *f <= '9') {
            int val = 0;
            while(*f >= '0' && *f <= '9') {
                val = val * 10 + (*f - '0');
                f++;
            }
            info->precision = val;
        }
        else info->precision = 0;
    }

    // [base]{alphabet}
    if(*f == '[') {
        f++;
        int base = 0;
        if(!(*f >= '0' && *f <= '9')) {
            *format = f;
            return -1;
        }
        while(*f >= '0' && *f <= '9') {
            base = base * 10 + (*f - '0');
            f++;
        }
        if(*f != ']') {
            *format = f;
            return -1;
        }
        f++;
        info->custom_base = base;

        if(*f == '{') {
            f++;
            const char *start = f;
            while (*f && *f != '}') f++;
            if(*f != '}') {
                *format = f;
                return -1;
            }
            size_t len = f - start;
            if(len >= sizeof(info->custom_alphabet)) {
                *format = f;
                return -1;
            }
            memcpy(info->custom_alphabet, start, len);
            info->custom_alphabet[len] = '\0';
            f++;
        }
        else info->custom_alphabet[0] = '\0';
    }

    // Спецификатор
    char spec = *f;
    if(spec == 'd' || spec == 'u' || spec == 'x' || spec == 'X' ||
        spec == 's' || spec == 'c' || spec == 'b' || spec == 'B') {
        if(info->custom_base != 0 && spec != 'b' && spec != 'B') {
            *format = f;
            return -1;
        }
        info->specifier = spec;
        f++;
    }
    else {
        *format = f;
        return -1;
    }

    *format = f;
    return 0;
}

// Число в строку
int format_unsigned(unsigned long long num, int base, const char *alphabet, char *out, size_t out_size, const SpecInfo *info) {
    if(!alphabet || !*alphabet) alphabet = default_alphabet;
    if(base < 2 || base > 62) return -1;
    if(strlen(alphabet) < (size_t)base) return -1;

    char tmp[128];
    int i = 0;
    if(num == 0) tmp[i++] = alphabet[0];
    else {
        while(num > 0) {
            tmp[i++] = alphabet[num % base];
            num /= base;
        }
    }
    if((size_t)i + 1 > out_size) return -1;
    for(int j = 0; j < i; j++) out[j] = tmp[i - 1 - j];
    out[i] = '\0';
    return i;
}

// Число в строку
int format_signed(long long num, int base, const char *alphabet, char *out, size_t out_size, const SpecInfo *info) {
    unsigned long long unum;
    int sign = 0;
    if(num < 0) {
        sign = 1;
        unum = (unsigned long long)(-num);
    }
    else unum = (unsigned long long)num;
    int res = format_unsigned(unum, base, alphabet, out + sign, out_size - sign, info);
    if(res < 0) return -1;
    if(sign) {
        out[0] = '-';
        res++;
    }
    return res;
}

// Строка в число
int parse_unsigned_from_str(const char *str, int base, const char *alphabet, unsigned long long *out) {
    if(!str || !*str) return -1;
    if(!alphabet || !*alphabet) alphabet = default_alphabet;
    if(base < 2 || base > 62) return -1;
    if(strlen(alphabet) < (size_t)base) return -1;

    const char *s = str;
    unsigned long long val = 0;
    int count = 0;
    while(*s) {
        const char *pos = strchr(alphabet, *s);
        if(!pos) break;
        int digit = (int)(pos - alphabet);
        if(digit >= base) break;
        if(val > (unsigned long long)(-1) / base) return -1;
        val = val * base + digit;
        s++;
        count++;
    }
    if(count == 0) return -1;
    *out = val;
    return count;
}

// Строка в число
int parse_signed_from_str(const char *str, int base, const char *alphabet, long long *out) {
    const char *s = str;
    int sign = 1;
    if(*s == '-') {
        sign = -1;
        s++;
    }
    else if (*s == '+') s++;
    unsigned long long uval;
    int res = parse_unsigned_from_str(s, base, alphabet, &uval);
    if(res < 0) return -1;
    if(sign == 1) {
        if(uval > (unsigned long long)LLONG_MAX) return -1;
        *out = (long long)uval;
    }
    else {
        if(uval > (unsigned long long)LLONG_MAX + 1ULL) return -1;
        *out = -(long long)uval;
    }
    return res + (s - str);
}