/*
utils.h - заголовочный файл utils.c.

Хаиров Егор Вадимович
МК-101
*/

#pragma once

#include <stddef.h>
#include <stdarg.h>
#include <stdbool.h>

// Структура спецификатора
typedef struct {
    // Флаги форматирования
    bool left_align;      // '-' – выравнивание влево
    bool zero_pad;        // '0' – дополнение нулями
    bool show_plus;       // '+' – всегда выводить знак +
    bool show_space;      // ' ' – пробел перед положительным числом
    bool alternate_form;  // '#' – 0x

    int width;            // ширина поля
    int precision;        // точность

    // Поля для пользовательских спецификаторов b/B (произвольное основание)
    int custom_base;          // основание
    char custom_alphabet[64]; // пользовательский алфавит

    char specifier;
} SpecInfo;

// Парсер форматной строки
int parse_format_specifier(const char **format, SpecInfo *info, va_list *args);

// Преобразование чисел в строки
int format_unsigned(unsigned long long num, int base, const char *alphabet,
                    char *out, size_t out_size, const SpecInfo *info);
int format_signed(long long num, int base, const char *alphabet,
                  char *out, size_t out_size, const SpecInfo *info);

// Разбор строки в число
int parse_unsigned_from_str(const char *str, int base, const char *alphabet,
                            unsigned long long *out);
int parse_signed_from_str(const char *str, int base, const char *alphabet,
                          long long *out);

// Функции для ввода/вывода
int console_write(const char *buf, size_t count);
int console_read(char *buf, size_t count);