/*
EgoIO.h - публичный интерфейс библиотеки EgoIO.

Хаиров Егор Вадимович
МК-101
*/

#pragma once

#include <stdarg.h>

int printf(const char *format, char **p, ...);
int scanf(const char *format, char **p, ...);