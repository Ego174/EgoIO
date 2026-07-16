/*
scanf.c - своя реализация scanf.

Хаиров Егор Вадимович
МК-101
*/

// Реализация scanf (без использования <stdio.h>)

#include "utils.h"
#include "EgoIO.h"
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

#define EOF (-1)

// Проверка, является ли символ пробельным
static int is_space(int ch) {
    return ch == ' ' || ch == '\t' || ch == '\n' ||
           ch == '\r' || ch == '\v' || ch == '\f';
}

// Структура для буферизированного чтения
typedef struct {
    char buf[128];
    size_t pos;
    size_t count;
    int eof;
} InputBuffer;

static void init_input(InputBuffer *ib) {
    ib->pos = 0;
    ib->count = 0;
    ib->eof = 0;
}

static int get_char(InputBuffer *ib) {
    if(ib->pos < ib->count)
        return ib->buf[ib->pos++];
    if(ib->eof)
        return EOF;
    int n = console_read(ib->buf, sizeof(ib->buf));
    if(n <= 0) {
        ib->eof = 1;
        return EOF;
    }
    ib->count = n;
    ib->pos = 0;
    return ib->buf[ib->pos++];
}

static void unget_char(InputBuffer *ib, int ch) {
    if(ch == EOF) return;
    if(ib->pos > 0) {
        ib->pos--;
        ib->buf[ib->pos] = (char)ch;
    }
}

static void skip_whitespace(InputBuffer *ib) {
    int ch;
    while((ch = get_char(ib)) != EOF && is_space(ch)) {}
    if(ch != EOF)
        unget_char(ib, ch);
}

// Читает токен до пробела или EOF
static int read_token(InputBuffer *ib, char *out, size_t max_len) {
    int ch;
    size_t len = 0;
    while((ch = get_char(ib)) != EOF && !is_space(ch)) {
        if(len < max_len - 1) {
            out[len++] = (char)ch;
        }
        else {
            unget_char(ib, ch);
            break;
        }
    }
    if(ch != EOF)
        unget_char(ib, ch);
    out[len] = '\0';
    return (len > 0) ? (int)len : -1;
}

int ego_scanf(const char *format, char **p, ...) {
    va_list args;
    va_start(args, p);

    InputBuffer ib;
    init_input(&ib);

    const char *f = format;
    int total_read = 0;
    int spec_index = 0;

    while(*f) {
        // Пропускаем пробелы в формате
        if(is_space(*f)) {
            skip_whitespace(&ib);
            while(is_space(*f)) f++;
            continue;
        }

        if(*f != '%') {
            int ch = get_char(&ib);
            if(ch == EOF || ch != *f) {
                if(p) *p = (char*)f;
                va_end(args);
                return -1;
            }
            total_read++;
            f++;
            continue;
        }

        f++;
        SpecInfo info;
        int err = parse_format_specifier(&f, &info, &args);
        if(err != 0) {
            if(p) *p = (char*)f;
            va_end(args);
            return -1;
        }

        spec_index++;

        if(info.specifier != 'c')
            skip_whitespace(&ib);

        char tmp[256];
        int bytes_read = 0;
        int ok = 1;

        switch(info.specifier) {
            case 'd': {
                int *ptr = va_arg(args, int*);
                int len = read_token(&ib, tmp, sizeof(tmp));
                if(len < 0) { ok = 0; break; }
                long long val;
                int base = 10;
                const char *alpha = NULL;
                int res = parse_signed_from_str(tmp, base, alpha, &val);
                if(res < 0) { ok = 0; break; }
                *ptr = (int)val;
                bytes_read = len;
                break;
            }
            case 'u':
            case 'x':
            case 'X': {
                unsigned int *ptr = va_arg(args, unsigned int*);
                int len = read_token(&ib, tmp, sizeof(tmp));
                if(len < 0) { ok = 0; break; }
                unsigned long long val;
                int base = (info.specifier == 'x' || info.specifier == 'X') ? 16 : 10;
                const char *alpha = NULL;
                int res = parse_unsigned_from_str(tmp, base, alpha, &val);
                if(res < 0) { ok = 0; break; }
                *ptr = (unsigned int)val;
                bytes_read = len;
                break;
            }
            case 's': {
                char *str = va_arg(args, char*);
                int len = read_token(&ib, str, 1024);
                if(len < 0) { ok = 0; break; }
                bytes_read = len;
                break;
            }
            case 'c': {
                char *ch_ptr = va_arg(args, char*);
                int ch = get_char(&ib);
                if(ch == EOF) { ok = 0; break; }
                *ch_ptr = (char)ch;
                bytes_read = 1;
                break;
            }
            case 'b': {
                long long *ptr = va_arg(args, long long*);
                int len = read_token(&ib, tmp, sizeof(tmp));
                if(len < 0) { ok = 0; break; }
                long long val;
                int base = info.custom_base ? info.custom_base : 10;
                const char *alpha = info.custom_alphabet[0] ? info.custom_alphabet : NULL;
                int res = parse_signed_from_str(tmp, base, alpha, &val);
                if(res < 0) { ok = 0; break; }
                *ptr = val;
                bytes_read = len;
                break;
            }
            case 'B': {
                unsigned long long *ptr = va_arg(args, unsigned long long*);
                int len = read_token(&ib, tmp, sizeof(tmp));
                if(len < 0) { ok = 0; break; }
                unsigned long long val;
                int base = info.custom_base ? info.custom_base : 10;
                const char *alpha = info.custom_alphabet[0] ? info.custom_alphabet : NULL;
                int res = parse_unsigned_from_str(tmp, base, alpha, &val);
                if(res < 0) { ok = 0; break; }
                *ptr = val;
                bytes_read = len;
                break;
            }
            default:
                if(p) *p = (char*)f;
                va_end(args);
                return -1;
        }

        if(!ok) {
            if(p) *p = (char*)f;
            va_end(args);
            return -(spec_index + 1);
        }

        total_read += bytes_read;
    }

    va_end(args);
    return total_read;
}