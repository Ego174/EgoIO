/*
test_lib.c - модуль проверки библиотеки. 

Хаиров Егор Вадимович
МК-101
*/

#include "unity.h"
#include "EgoIO.h"
#include <string.h>
#include <stddef.h>

extern int (*console_write_ptr)(const char*, size_t);
extern int (*console_read_ptr)(char*, size_t);

static char output_buf[4096];
static size_t output_len = 0;

static const char *input_data = NULL;
static size_t input_pos = 0;

static int test_write(const char *buf, size_t count) {
    if(output_len + count >= sizeof(output_buf)) return -1;
    memcpy(output_buf + output_len, buf, count);
    output_len += count;
    return (int)count;
}

static int test_read(char *buf, size_t count) {
    if(!input_data) return -1;
    size_t i = 0;
    while(i < count && input_data[input_pos] != '\0')
        buf[i++] = input_data[input_pos++];
    return (int)i;
}

static void setup_output(void) {
    output_len = 0;
    output_buf[0] = '\0';
}

static const char *get_output(void) {
    output_buf[output_len] = '\0';
    return output_buf;
}

static void setup_input(const char *str) {
    input_data = str;
    input_pos = 0;
}

void setUp(void) {
    console_write_ptr = test_write;
    console_read_ptr = test_read;
    setup_output();
    input_data = NULL;
    input_pos = 0;
}

void tearDown(void) {
    console_write_ptr = NULL;
    console_read_ptr = NULL;
}

void test_printf_simple(void) {
    char *p;
    ego_printf("Hello", &p);
    TEST_ASSERT_EQUAL_STRING("Hello", get_output());
}

void test_printf_int(void) {
    char *p;
    ego_printf("%d", &p, 123);
    TEST_ASSERT_EQUAL_STRING("123", get_output());
}

void test_printf_width(void) {
    char *p;
    ego_printf("%10d", &p, 123);
    TEST_ASSERT_EQUAL_STRING("       123", get_output());
}

void test_printf_left_align(void) {
    char *p;
    ego_printf("%-10d", &p, 123);
    TEST_ASSERT_EQUAL_STRING("123       ", get_output());
}

void test_printf_hex_upper(void) {
    char *p;
    ego_printf("%X", &p, 255);
    TEST_ASSERT_EQUAL_STRING("FF", get_output());
}

void test_printf_alternate_form(void) {
    char *p;
    ego_printf("%#x", &p, 255);
    TEST_ASSERT_EQUAL_STRING("0xff", get_output());
}

void test_printf_custom_base(void) {
    char *p;
    ego_printf("%[2]b", &p, 5);
    TEST_ASSERT_EQUAL_STRING("101", get_output());
}

void test_printf_custom_alphabet(void) {
    char *p;
    ego_printf("%[2]{ZA}b", &p, 10);
    TEST_ASSERT_EQUAL_STRING("AZAZ", get_output());
}

void test_printf_error_invalid_specifier(void) {
    char *p;
    int ret = ego_printf("%q", &p);
    TEST_ASSERT_EQUAL_INT(-1, ret);
    TEST_ASSERT_EQUAL_STRING("q", p);
}

void test_scanf_int(void) {
    char *p;
    int val;
    setup_input("123");
    int ret = ego_scanf("%d", &p, &val);
    TEST_ASSERT_EQUAL_INT(3, ret);
    TEST_ASSERT_EQUAL_INT(123, val);
}

void test_scanf_string(void) {
    char *p;
    char str[100];
    setup_input("hello");
    int ret = ego_scanf("%s", &p, str);
    TEST_ASSERT_EQUAL_INT(5, ret);
    TEST_ASSERT_EQUAL_STRING("hello", str);
}

void test_scanf_custom_base(void) {
    char *p;
    unsigned long long val;
    setup_input("1010");
    int ret = ego_scanf("%[2]B", &p, &val);
    TEST_ASSERT_EQUAL_INT(4, ret);
    TEST_ASSERT_EQUAL_UINT64(10, val);
}

void test_scanf_custom_alphabet(void) {
    char *p;
    unsigned long long val;
    setup_input("AZAZ");
    int ret = ego_scanf("%[2]{ZA}B", &p, &val);
    TEST_ASSERT_EQUAL_INT(4, ret);
    TEST_ASSERT_EQUAL_UINT64(10, val);
}

void test_scanf_error_conversion(void) {
    char *p;
    int val;
    setup_input("abc");
    int ret = ego_scanf("%d", &p, &val);
    TEST_ASSERT_EQUAL_INT(-2, ret);
}

void test_scanf_error_mismatch(void) {
    char *p;
    int val;
    setup_input("xyz");
    int ret = ego_scanf("%d", &p, &val);
    TEST_ASSERT_EQUAL_INT(-2, ret);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_printf_simple);
    RUN_TEST(test_printf_int);
    RUN_TEST(test_printf_width);
    RUN_TEST(test_printf_left_align);
    RUN_TEST(test_printf_hex_upper);
    RUN_TEST(test_printf_alternate_form);
    RUN_TEST(test_printf_custom_base);
    RUN_TEST(test_printf_custom_alphabet);
    RUN_TEST(test_printf_error_invalid_specifier);

    RUN_TEST(test_scanf_int);
    RUN_TEST(test_scanf_string);
    RUN_TEST(test_scanf_custom_base);
    RUN_TEST(test_scanf_custom_alphabet);
    RUN_TEST(test_scanf_error_conversion);
    RUN_TEST(test_scanf_error_mismatch);

    return UNITY_END();
}