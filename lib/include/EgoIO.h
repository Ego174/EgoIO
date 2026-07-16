/*
EgoIO.h - публичный интерфейс библиотеки EgoIO.

Хаиров Егор Вадимович
МК-101
*/

#pragma once

#include <stdarg.h>

int ego_printf(const char *format, char **p, ...);
int ego_scanf(const char *format, char **p, ...);