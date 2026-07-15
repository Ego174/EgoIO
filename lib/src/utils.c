/*
utils.c - внутренние заголовки для EgoIO.

Хаиров Егор Вадимович
МК-101
*/

// Вспомогательные функции для ввода/вывода и преобразований.
// Содержит заглушки для будущих реализаций.

#include "utils.h"
#include <string.h>

#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

static const char *default_alphabet = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

// Вывод
int console_write(const char *buf, size_t count) {
    if (!buf || count == 0)
        return 0;

#ifdef _WIN32
    ssize_t res = _write(1, buf, count);
    if (res < 0)
        return -1;
    return (int)res;
#else
    ssize_t res = write(1, buf, count);
    if (res < 0)
        return -1;
    return (int)res;
#endif
}

// Ввод
int console_read(char *buf, size_t count) {
    if (!buf || count == 0)
        return 0;

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
    char *f = *format;
    memset(info, 0, sizeof(SpecInfo));

    // Флаги
    bool done = false;
    while(!done) {
        switch(*f) {
            case '-': info->left_align = true; f++; break;
            case '0': info->zero_pad = true; f++; break;
            case '+': info->show_plus = true; f++; break;
            case ' ': info->show_space = true; f++; break;
            case '#': info->alternate_form = true; f++; break;
            default: done = true; break;
        }
    }

    // 2. Ширина
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

    // 3. Точность
    if(*f == '.') {
        f++;
        if(*f == '*') {
            f++;
            int prec = va_arg(*args, int);
            if(prec < 0) prec = 0;
            info->precision = prec;
        }
        else if(*f >= '0' && *f <= '9') {
            int val = 0;
            while(*f >= '0' && *f <= '9') {
                val = val * 10 + (*f - '0');
                f++;
            }
            info->precision = val;
        }
        else info->precision = 0;
    }

    // 4. [base]{alphabet}
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
            while(*f && *f != '}') f++;
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

    // 5. Спецификатор
    char spec = *f;
    if(spec == 'd' || spec == 'u' || spec == 'x' || spec == 'X' ||
       spec == 's' || spec == 'c' || spec == 'b' || spec == 'B') {
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