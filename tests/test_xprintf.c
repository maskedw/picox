#include <unity.h>
#include <unity_fixture.h>
#include <stdio.h>
#include <string.h>
#include <picox/core/xcore.h>


TEST_GROUP(xprintf);


TEST_SETUP(xprintf)
{
}


TEST_TEAR_DOWN(xprintf)
{
}


TEST(xprintf, print_string)
{
    char buf[128];
    int ret;
    ret = x_snprintf(buf, sizeof(buf), "hello world");
    TEST_ASSERT_EQUAL_STRING("hello world", buf);
    TEST_ASSERT_EQUAL(strlen("hello world"), ret);

    ret = x_snprintf(buf, sizeof(buf), "%s", "hello world");
    TEST_ASSERT_EQUAL_STRING("hello world", buf);
    TEST_ASSERT_EQUAL(strlen("hello world"), ret);

    ret = x_snprintf(buf, sizeof(buf), "%7s", "hello");
    TEST_ASSERT_EQUAL_STRING("  hello", buf);
    TEST_ASSERT_EQUAL(strlen("  hello"), ret);

    ret = x_snprintf(buf, sizeof(buf), "%-7s", "hello");
    TEST_ASSERT_EQUAL_STRING("hello  ", buf);
    TEST_ASSERT_EQUAL(strlen("hello  "), ret);

    ret = x_snprintf(buf, sizeof(buf), "%s", NULL);
    TEST_ASSERT_EQUAL_STRING("(null)", buf);
    TEST_ASSERT_EQUAL(strlen("(null)"), ret);
}


TEST(xprintf, print_signed)
{
    char buf[128];
    int ret;
    ret = x_snprintf(buf, sizeof(buf), "%d", 1234);
    TEST_ASSERT_EQUAL_STRING("1234", buf);
    TEST_ASSERT_EQUAL(strlen("1234"), ret);

    ret = x_snprintf(buf, sizeof(buf), "%d", -1234);
    TEST_ASSERT_EQUAL_STRING("-1234", buf);
    TEST_ASSERT_EQUAL(strlen("-1234"), ret);

    ret = x_snprintf(buf, sizeof(buf), "%04d", 10);
    TEST_ASSERT_EQUAL_STRING("0010", buf);
    TEST_ASSERT_EQUAL(strlen("0010"), ret);

    ret = x_snprintf(buf, sizeof(buf), "%04d", -10);
    TEST_ASSERT_EQUAL_STRING("-010", buf);
    TEST_ASSERT_EQUAL(strlen("-010"), ret);

    ret = x_snprintf(buf, sizeof(buf), "%-4d", 10);
    TEST_ASSERT_EQUAL_STRING("10  ", buf);
    TEST_ASSERT_EQUAL(strlen("10  "), ret);

    ret = x_snprintf(buf, sizeof(buf), "%-4d", -10);
    TEST_ASSERT_EQUAL_STRING("-10 ", buf);
    TEST_ASSERT_EQUAL(strlen("-10 "), ret);

    ret = x_snprintf(buf, sizeof(buf), "%ld", LONG_MAX);
    TEST_ASSERT_EQUAL(atol(buf), LONG_MAX);

    ret = x_snprintf(buf, sizeof(buf), "%zd", SSIZE_MAX);
    TEST_ASSERT_EQUAL(atol(buf), SSIZE_MAX);

    ret = x_snprintf(buf, sizeof(buf), "%td", PTRDIFF_MAX);
    TEST_ASSERT_EQUAL(atol(buf), PTRDIFF_MAX);

    ret = x_snprintf(buf, sizeof(buf), "%hhd", 0xFFF);
    TEST_ASSERT_EQUAL_STRING("-1", buf);

    ret = x_snprintf(buf, sizeof(buf), "%hhd", CHAR_MAX);
    TEST_ASSERT_EQUAL(atoi(buf), CHAR_MAX);

    ret = x_snprintf(buf, sizeof(buf), "%hd", 0xFFFFF);
    TEST_ASSERT_EQUAL_STRING("-1", buf);
    ret = x_snprintf(buf, sizeof(buf), "%hd", SHRT_MAX);
    TEST_ASSERT_EQUAL(atoi(buf), SHRT_MAX);
}


TEST(xprintf, print_unsigned)
{
    char buf[128];
    int ret;

    ret = x_snprintf(buf, sizeof(buf), "%u", 1234);
    TEST_ASSERT_EQUAL_STRING("1234", buf);

    ret = x_snprintf(buf, sizeof(buf), "%04u", 10);
    TEST_ASSERT_EQUAL_STRING("0010", buf);
    TEST_ASSERT_EQUAL(strlen("0010"), ret);

    ret = x_snprintf(buf, sizeof(buf), "%-4u", 10);
    TEST_ASSERT_EQUAL_STRING("10  ", buf);
    TEST_ASSERT_EQUAL(strlen("10  "), ret);

    ret = x_snprintf(buf, sizeof(buf), "%lu", ULONG_MAX);
    TEST_ASSERT_EQUAL(strtoul(buf, NULL, 10), ULONG_MAX);

    ret = x_snprintf(buf, sizeof(buf), "%zu", SIZE_MAX);
    TEST_ASSERT_EQUAL(strtoul(buf, NULL, 10), SIZE_MAX);

    ret = x_snprintf(buf, sizeof(buf), "%tu", PTRDIFF_MAX);
    TEST_ASSERT_EQUAL(strtoul(buf, NULL, 10), PTRDIFF_MAX);

    ret = x_snprintf(buf, sizeof(buf), "%hhu", 0xFFF);
    TEST_ASSERT_EQUAL(strtoul(buf, NULL, 10), UCHAR_MAX);

    ret = x_snprintf(buf, sizeof(buf), "%hhu", UCHAR_MAX);
    TEST_ASSERT_EQUAL(atoi(buf), UCHAR_MAX);

    ret = x_snprintf(buf, sizeof(buf), "%hu", 0xFFFFF);
    TEST_ASSERT_EQUAL(atoi(buf), USHRT_MAX);

    ret = x_snprintf(buf, sizeof(buf), "%hu", USHRT_MAX);
    TEST_ASSERT_EQUAL(atoi(buf), USHRT_MAX);
}



TEST(xprintf, print_binary)
{
    char buf[128];
    int ret;

    ret = x_snprintf(buf, sizeof(buf), "%b", 0xAB);
    TEST_ASSERT_EQUAL_STRING("10101011", buf);
    TEST_ASSERT_EQUAL(strlen("10101011"), ret);
}


TEST(xprintf, print_hex)
{
    char buf[128];
    int ret;

    ret = x_snprintf(buf, sizeof(buf), "%x", 0xDEADBEEF);
    TEST_ASSERT_EQUAL_STRING("deadbeef", buf);
    TEST_ASSERT_EQUAL(strlen("deadbeef"), ret);

    ret = x_snprintf(buf, sizeof(buf), "%X", 0xDEADBEEF);
    TEST_ASSERT_EQUAL_STRING("DEADBEEF", buf);
    TEST_ASSERT_EQUAL(strlen("DEADBEEF"), ret);
}


TEST(xprintf, print_oct)
{
    char buf[128];
    int ret;

    ret = x_snprintf(buf, sizeof(buf), "%o", 0xDEADBEEF);
    TEST_ASSERT_EQUAL_STRING("33653337357", buf);
    TEST_ASSERT_EQUAL(strlen("33653337357"), ret);
}


TEST(xprintf, print_pointer)
{
    char buf[128];
    int ret;

    ret = x_snprintf(buf, sizeof(buf), "%p", (void*)0xDEADBEEF);
    TEST_ASSERT_EQUAL_STRING("0xdeadbeef", buf);
    TEST_ASSERT_EQUAL(strlen("0xdeadbeef"), ret);

    ret = x_snprintf(buf, sizeof(buf), "%p", NULL);
    TEST_ASSERT_EQUAL_STRING("(nil)", buf);
    TEST_ASSERT_EQUAL(strlen("(nil)"), ret);
}


TEST(xprintf, print_floating_point)
{
    char buf[128];
    int ret;

    ret = x_snprintf(buf, sizeof(buf), "%f", 123.456789);
    TEST_ASSERT_EQUAL_STRING("123.456789", buf);
    TEST_ASSERT_EQUAL(strlen("123.456789"), ret);

    ret = x_snprintf(buf, sizeof(buf), "%f", -123.456789);
    TEST_ASSERT_EQUAL_STRING("-123.456789", buf);
    TEST_ASSERT_EQUAL(strlen("-123.456789"), ret);

    ret = x_snprintf(buf, sizeof(buf), "%.3f", 123.456789);
    TEST_ASSERT_EQUAL_STRING("123.456", buf);
    TEST_ASSERT_EQUAL(strlen("123.456"), ret);

    ret = x_snprintf(buf, sizeof(buf), "%.3f", -123.456789);
    TEST_ASSERT_EQUAL_STRING("-123.456", buf);
    TEST_ASSERT_EQUAL(strlen("-123.456"), ret);

    ret = x_snprintf(buf, sizeof(buf), "%9.3f", 123.456789);
    TEST_ASSERT_EQUAL_STRING("  123.456", buf);
    TEST_ASSERT_EQUAL(strlen("  123.456"), ret);

    ret = x_snprintf(buf, sizeof(buf), "%9.3f", -123.456789);
    TEST_ASSERT_EQUAL_STRING(" -123.456", buf);
    TEST_ASSERT_EQUAL(strlen(" -123.456"), ret);
}


TEST(xprintf, overflow)
{
    char buf[4] = {0};
    int ret;

    /* sizeを超える場合は、途中で書き込みを止めるが、返り値はすべての文字列が書
     * き込めた時の文字数を返す
     */
    ret = x_snprintf(buf, sizeof(buf), "%d", 1234);
    TEST_ASSERT_EQUAL_STRING("123", buf);
    TEST_ASSERT_EQUAL(strlen("1234"), ret);
}


TEST_GROUP_RUNNER(xprintf)
{
    RUN_TEST_CASE(xprintf, print_string);
    RUN_TEST_CASE(xprintf, print_signed);
    RUN_TEST_CASE(xprintf, print_unsigned);
    RUN_TEST_CASE(xprintf, print_binary);
    RUN_TEST_CASE(xprintf, print_hex);
    RUN_TEST_CASE(xprintf, print_oct);
    RUN_TEST_CASE(xprintf, print_pointer);
    RUN_TEST_CASE(xprintf, print_floating_point);
    RUN_TEST_CASE(xprintf, overflow);
}
