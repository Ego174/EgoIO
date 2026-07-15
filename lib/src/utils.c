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