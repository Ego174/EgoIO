/*
main.c - программа для демонстрации функций.

Хаиров Егор Вадимович
МК-101
*/

#include "EgoIO.h"

int main() {

    char *err_ptr;
    int ret;

    ego_printf("Hello, %s!\n", &err_ptr, "world");
    ego_printf("Number: %d\n", &err_ptr, 42);
    ego_printf("Unsigned: %u\n", &err_ptr, 12345);
    ego_printf("Hex lowercase: %x\n", &err_ptr, 255);
    ego_printf("Hex uppercase: %X\n", &err_ptr, 255);

    ego_printf("|%10d|\n", &err_ptr, 123);
    ego_printf("|%-10d|\n", &err_ptr, 123);
    ego_printf("|%010d|\n", &err_ptr, 123);
    ego_printf("|%10s|\n", &err_ptr, "abc");
    ego_printf("|%-10s|\n", &err_ptr, "abc");

    ego_printf("%+d\n", &err_ptr, 42);
    ego_printf("% d\n", &err_ptr, 42);

    ego_printf("%#x\n", &err_ptr, 255);
    ego_printf("%#X\n", &err_ptr, 255);

    ego_printf("Char: %c\n", &err_ptr, 'A');

    ego_printf("Binary (base 2): %[2]b\n", &err_ptr, 42);
    ego_printf("Octal (base 8): %[8]b\n", &err_ptr, 42);
    ego_printf("Base 16: %[16]b\n", &err_ptr, 255);
    ego_printf("Custom alphabet (base 2, {ZA}): %[2]{ZA}b\n", &err_ptr, 42);
    ego_printf("Unsigned binary: %[2]B\n", &err_ptr, 42u);

    // Ошибка в формате
    ret = ego_printf("Bad specifier: %q\n", &err_ptr);
    if(ret < 0) {
        ego_printf("Error: invalid format specifier at '%s'\n", &err_ptr, err_ptr);
    }

    int num;
    char str[100];
    unsigned long long ulnum;

    ego_printf("\nEnter an integer: ", &err_ptr);
    ret = ego_scanf("%d", &err_ptr, &num);
    if(ret > 0) {
        ego_printf("You entered: %d\n", &err_ptr, num);
    }
    else {
        ego_printf("Scanf error, return: %d\n", &err_ptr, ret);
    }

    ego_printf("Enter a string: ", &err_ptr);
    ret = ego_scanf("%s", &err_ptr, str);
    if(ret > 0) {
        ego_printf("You entered: '%s'\n", &err_ptr, str);
    }

    ego_printf("Enter a binary number (e.g., 1010): ", &err_ptr);
    ret = ego_scanf("%[2]B", &err_ptr, &ulnum);
    if(ret > 0) {
        ego_printf("Decimal value: %[10]B\n", &err_ptr, ulnum);
    }
    else {
        ego_printf("Conversion error, return: %d\n", &err_ptr, ret);
    }

    ego_printf("Enter a number in base 2 with alphabet {ZA} (e.g., ZAZA): ", &err_ptr);
    ret = ego_scanf("%[2]{ZA}B", &err_ptr, &ulnum);
    if(ret > 0) {
        ego_printf("Decimal value: %[10]B\n", &err_ptr, ulnum);
    }

    return 0;
}