/*
EgoIO.h - публичный интерфейс библиотеки EgoIO.

Хаиров Егор Вадимович
МК-101
*/

#pragma once

#include <stdarg.h>

// Свой printf
int ego_printf(const char *format, char **p, ...);

// Свой scanf
int ego_scanf(const char *format, char **p, ...);