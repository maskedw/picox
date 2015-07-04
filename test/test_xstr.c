#include <unity.h>
#include <unity_fixture.h>
#include <picox/string/xstr.h>
#include "testutils.h"

#ifdef UNITY_EXCLUDE_FLOAT
#error hgoe
#endif


TEST_GROUP(xstr);




TEST_SETUP(xstr)
{
}


TEST_TEAR_DOWN(xstr)
{
}


static void* X__GetMem(size_t size)
{
    return X_MALLOC(size);
}


static void X__FreeMem(void* ptr)
{
    X_FREE(ptr);
}


TEST(xstr, equal)
{
    X_TEST_ASSERTION_FAILED(xstr_equal(NULL, NULL));
    X_TEST_ASSERTION_FAILED(xstr_equal("A", NULL));
    X_TEST_ASSERTION_FAILED(xstr_equal(NULL, "A"));
    X_TEST_ASSERTION_SUCCESS(xstr_equal("", ""));

    const char* expected = "Hello";
    TEST_ASSERT_TRUE(xstr_equal("Hello", expected));
    TEST_ASSERT_FALSE(xstr_equal("hello", expected));
    TEST_ASSERT_FALSE(xstr_equal("Hell", expected));
    TEST_ASSERT_FALSE(xstr_equal("ello", expected));
}


TEST(xstr, case_equal)
{
    X_TEST_ASSERTION_FAILED(xstr_case_equal(NULL, NULL));
    X_TEST_ASSERTION_FAILED(xstr_case_equal("A", NULL));
    X_TEST_ASSERTION_FAILED(xstr_case_equal(NULL, "A"));
    X_TEST_ASSERTION_SUCCESS(xstr_case_equal("", ""));

    const char* expected = "Hello";
    TEST_ASSERT_TRUE(xstr_case_equal("Hello", expected));
    TEST_ASSERT_TRUE(xstr_case_equal("hello", expected));
    TEST_ASSERT_FALSE(xstr_case_equal("Hell", expected));
    TEST_ASSERT_FALSE(xstr_case_equal("ello", expected));
}


TEST(xstr, compare)
{
    X_TEST_ASSERTION_FAILED(xstr_compare(NULL, NULL));
    X_TEST_ASSERTION_FAILED(xstr_compare("A", NULL));
    X_TEST_ASSERTION_FAILED(xstr_compare(NULL, "A"));
    X_TEST_ASSERTION_SUCCESS(xstr_compare("", ""));

    TEST_ASSERT_TRUE(xstr_compare("ABC", "ABC") == 0);
    TEST_ASSERT_TRUE(xstr_compare("ABC", "BBC") < 0);
    TEST_ASSERT_TRUE(xstr_compare("ABC", "ABD") < 0);
    TEST_ASSERT_TRUE(xstr_compare("ABC", "ACC") < 0);
    TEST_ASSERT_TRUE(xstr_compare("BBC", "ABC") > 0);
    TEST_ASSERT_TRUE(xstr_compare("ABD", "ABC") > 0);
    TEST_ASSERT_TRUE(xstr_compare("ACC", "ABC") > 0);
    TEST_ASSERT_TRUE(xstr_compare("ABC", "ABCD") < 0);
    TEST_ASSERT_TRUE(xstr_compare("ABCD", "ABC") > 0);
}


TEST(xstr, case_compare)
{
    X_TEST_ASSERTION_FAILED(xstr_case_compare(NULL, NULL));
    X_TEST_ASSERTION_FAILED(xstr_case_compare("A", NULL));
    X_TEST_ASSERTION_FAILED(xstr_case_compare(NULL, "A"));
    X_TEST_ASSERTION_SUCCESS(xstr_case_compare("", ""));

    TEST_ASSERT_TRUE(xstr_case_compare("ABC", "ABC") == 0);
    TEST_ASSERT_TRUE(xstr_case_compare("ABC", "BBC") < 0);
    TEST_ASSERT_TRUE(xstr_case_compare("ABC", "ABD") < 0);
    TEST_ASSERT_TRUE(xstr_case_compare("ABC", "ACC") < 0);
    TEST_ASSERT_TRUE(xstr_case_compare("BBC", "ABC") > 0);
    TEST_ASSERT_TRUE(xstr_case_compare("ABD", "ABC") > 0);
    TEST_ASSERT_TRUE(xstr_case_compare("ACC", "ABC") > 0);
    TEST_ASSERT_TRUE(xstr_case_compare("ABC", "ABCD") < 0);
    TEST_ASSERT_TRUE(xstr_case_compare("ABCD", "ABC") > 0);

    TEST_ASSERT_TRUE(xstr_case_compare("abc", "ABC") == 0);
    TEST_ASSERT_TRUE(xstr_case_compare("abc", "BBC") < 0);
    TEST_ASSERT_TRUE(xstr_case_compare("abc", "ABD") < 0);
    TEST_ASSERT_TRUE(xstr_case_compare("abc", "ACC") < 0);
    TEST_ASSERT_TRUE(xstr_case_compare("BBC", "abc") > 0);
    TEST_ASSERT_TRUE(xstr_case_compare("ABD", "abc") > 0);
    TEST_ASSERT_TRUE(xstr_case_compare("ACC", "abc") > 0);
    TEST_ASSERT_TRUE(xstr_case_compare("abc", "ABCD") < 0);
    TEST_ASSERT_TRUE(xstr_case_compare("ABCD", "abc") > 0);
}


TEST(xstr, ncase_compare)
{
    X_TEST_ASSERTION_FAILED(xstr_ncase_compare(NULL, NULL, 1));
    X_TEST_ASSERTION_FAILED(xstr_ncase_compare("A", NULL, 1));
    X_TEST_ASSERTION_FAILED(xstr_ncase_compare(NULL, "A", 1));
    X_TEST_ASSERTION_SUCCESS(xstr_ncase_compare("", "", 1));
    X_TEST_ASSERTION_SUCCESS(xstr_ncase_compare("", "", 0));

    TEST_ASSERT_TRUE(xstr_ncase_compare("ABC", "ABC", 3) == 0);
    TEST_ASSERT_TRUE(xstr_ncase_compare("ABC", "ABC", 3) == 0);
    TEST_ASSERT_TRUE(xstr_ncase_compare("ABC", "BBC", 3) < 0);
    TEST_ASSERT_TRUE(xstr_ncase_compare("ABC", "ABD", 3) < 0);
    TEST_ASSERT_TRUE(xstr_ncase_compare("ABC", "ACC", 3) < 0);
    TEST_ASSERT_TRUE(xstr_ncase_compare("BBC", "ABC", 3) > 0);
    TEST_ASSERT_TRUE(xstr_ncase_compare("ABD", "ABC", 3) > 0);
    TEST_ASSERT_TRUE(xstr_ncase_compare("ACC", "ABC", 3) > 0);

    TEST_ASSERT_TRUE(xstr_ncase_compare("abc", "ABC", 3) == 0);
    TEST_ASSERT_TRUE(xstr_ncase_compare("abc", "BBC", 3) < 0);
    TEST_ASSERT_TRUE(xstr_ncase_compare("abc", "ABD", 3) < 0);
    TEST_ASSERT_TRUE(xstr_ncase_compare("abc", "ACC", 3) < 0);
    TEST_ASSERT_TRUE(xstr_ncase_compare("BBC", "abc", 3) > 0);
    TEST_ASSERT_TRUE(xstr_ncase_compare("ABD", "abc", 3) > 0);
    TEST_ASSERT_TRUE(xstr_ncase_compare("ACC", "abc", 3) > 0);

    TEST_ASSERT_TRUE(xstr_ncase_compare("ABC", "ABCD", 3) == 0);
    TEST_ASSERT_TRUE(xstr_ncase_compare("abc", "ABCD", 3) == 0);
}


TEST(xstr, search_substring)
{
    X_TEST_ASSERTION_FAILED(xstr_search_substring(NULL, NULL));
    X_TEST_ASSERTION_FAILED(xstr_search_substring("A", NULL));
    X_TEST_ASSERTION_FAILED(xstr_search_substring(NULL, "A"));
    X_TEST_ASSERTION_SUCCESS(xstr_search_substring("", ""));

    const char* str = "Hello World!";

    TEST_ASSERT_EQUAL_STRING("World!", xstr_search_substring(str, "World"));
    TEST_ASSERT_EQUAL_STRING("o World!", xstr_search_substring(str, "o W"));
    TEST_ASSERT_NULL(xstr_search_substring(str, "world"));
}


TEST(xstr, case_search_substring)
{
    X_TEST_ASSERTION_FAILED(xstr_case_search_substring(NULL, NULL));
    X_TEST_ASSERTION_FAILED(xstr_case_search_substring("A", NULL));
    X_TEST_ASSERTION_FAILED(xstr_case_search_substring(NULL, "A"));
    X_TEST_ASSERTION_SUCCESS(xstr_case_search_substring("", ""));

    const char* str = "Hello World!";

    TEST_ASSERT_EQUAL_STRING("World!", xstr_case_search_substring(str, "World"));
    TEST_ASSERT_EQUAL_STRING("o World!", xstr_case_search_substring(str, "o W"));
    TEST_ASSERT_EQUAL_STRING("World!", xstr_case_search_substring(str, "world"));
}


TEST(xstr, duplicate)
{
    X_TEST_ASSERTION_FAILED(xstr_duplicate(NULL));

    const char* str = "Hello World!";
    char* dup;

    dup = xstr_duplicate(str);
    TEST_ASSERT_EQUAL_STRING(str, dup);
    X_FREE(dup);
}


TEST(xstr, duplicate2)
{
    X_TEST_ASSERTION_FAILED(xstr_duplicate2(NULL, NULL));
    X_TEST_ASSERTION_FAILED(xstr_duplicate2(NULL, X__GetMem));
    X_TEST_ASSERTION_FAILED(xstr_duplicate2("", NULL));

    const char* str = "Hello World!";
    char* dup;

    dup = xstr_duplicate2(str, X__GetMem);
    TEST_ASSERT_EQUAL_STRING(str, dup);
    X__FreeMem(dup);
}


TEST(xstr, nduplicate)
{
    X_TEST_ASSERTION_FAILED(xstr_nduplicate(NULL, 0));

    const char* str = "Hello World!";
    char* dup;

    dup = xstr_nduplicate(str, 5);
    TEST_ASSERT_EQUAL_STRING("Hello", dup);
    X_FREE(dup);

    dup = xstr_nduplicate(str, 0);
    TEST_ASSERT_EQUAL_STRING("", dup);
    X_FREE(dup);
}


TEST(xstr, nduplicate2)
{
    X_TEST_ASSERTION_FAILED(xstr_nduplicate2(NULL, 0, NULL));
    X_TEST_ASSERTION_FAILED(xstr_nduplicate2(NULL, 0, X__GetMem));
    X_TEST_ASSERTION_FAILED(xstr_nduplicate2("", 0, NULL));

    const char* str = "Hello World!";
    char* dup;

    dup = xstr_nduplicate2(str, 5, X__GetMem);
    TEST_ASSERT_EQUAL_STRING("Hello", dup);
    X__FreeMem(dup);

    dup = xstr_nduplicate2(str, 0, X__GetMem);
    TEST_ASSERT_EQUAL_STRING("", dup);
    X__FreeMem(dup);
}


TEST(xstr, reverse)
{
    X_TEST_ASSERTION_FAILED(xstr_reverse(NULL));

    char str1[] = "abc";
    char str2[] = "abce";

    TEST_ASSERT_EQUAL_STRING("cba", xstr_reverse(str1));
    TEST_ASSERT_EQUAL_STRING("ecba", xstr_reverse(str2));
}


TEST(xstr, strip)
{
    X_TEST_ASSERTION_FAILED(xstr_strip(NULL, NULL));

    char str1[] = "  abc  ";
    char str2[] = "@|]abc ^^`";

    TEST_ASSERT_EQUAL_STRING("abc", xstr_strip(str1, NULL));
    TEST_ASSERT_EQUAL_STRING("abc", xstr_strip(str2, " @|]^^`"));
}


TEST(xstr, strip_left)
{
    X_TEST_ASSERTION_FAILED(xstr_strip_left(NULL, NULL));

    char str1[] = "  abc  ";
    char str2[] = "@|]abc ^^`";

    TEST_ASSERT_EQUAL_STRING("abc  ", xstr_strip_left(str1, NULL));
    TEST_ASSERT_EQUAL_STRING("abc ^^`", xstr_strip_left(str2, " @|]^^`"));
}


TEST(xstr, strip_right)
{
    X_TEST_ASSERTION_FAILED(xstr_strip_right(NULL, NULL));

    char str1[] = "  abc  ";
    char str2[] = "@|]abc ^^`";

    TEST_ASSERT_EQUAL_STRING("  abc", xstr_strip_right(str1, NULL));
    TEST_ASSERT_EQUAL_STRING("@|]abc", xstr_strip_right(str2, " @|]^^`"));
}


TEST(xstr, to_int)
{
    bool ok;

    X_TEST_ASSERTION_FAILED(xstr_to_int32(NULL, 0, &ok));
    X_TEST_ASSERTION_SUCCESS(xstr_to_int32("10", 0, NULL));

    TEST_ASSERT_EQUAL(0, xstr_to_int32("0", 100, &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL(1, xstr_to_int32("1", 100, &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL(1, xstr_to_int32("+1", 100, &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL(-1, xstr_to_int32("-1", 100, &ok));
    TEST_ASSERT_TRUE(ok);

    TEST_ASSERT_EQUAL(INT32_MAX, xstr_to_int32("2147483647", 0, &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL(0, xstr_to_int32("2147483648", 0, &ok));
    TEST_ASSERT_TRUE(! ok);
    TEST_ASSERT_EQUAL(0, xstr_to_int32("2147483650", 0, &ok));
    TEST_ASSERT_TRUE(! ok);
    TEST_ASSERT_EQUAL(INT32_MIN, xstr_to_int32("-2147483648", 0, &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL(INT32_MIN + 1, xstr_to_int32("-2147483647", 0, &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL(0, xstr_to_int32("-2147483649", 0, &ok));
    TEST_ASSERT_TRUE(! ok);
}


TEST(xstr, to_uint)
{
    bool ok;

    X_TEST_ASSERTION_FAILED(xstr_to_uint32(NULL, 0, &ok));
    X_TEST_ASSERTION_SUCCESS(xstr_to_uint32("10", 0, NULL));

    TEST_ASSERT_EQUAL(0, xstr_to_uint32("0", 100, &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL(1, xstr_to_uint32("1", 100, &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL(1, xstr_to_uint32("+1", 100, &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL(100, xstr_to_uint32("-1", 100, &ok));
    TEST_ASSERT_TRUE(! ok);

    TEST_ASSERT_EQUAL_HEX32(0, xstr_to_uint32("0x0", 100, &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_HEX32(1, xstr_to_uint32("0x1", 100, &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_HEX32(100, xstr_to_uint32("+0x1", 100, &ok));
    TEST_ASSERT_TRUE(! ok);
    TEST_ASSERT_EQUAL_HEX32(100, xstr_to_uint32("-0x1", 100, &ok));
    TEST_ASSERT_TRUE(! ok);
    TEST_ASSERT_EQUAL_HEX32(0xFFFFFFFF, xstr_to_uint32("0xFFFFFFFF", 100, &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_HEX32(100, xstr_to_uint32("0xZ", 100, &ok));
    TEST_ASSERT_TRUE(! ok);

    TEST_ASSERT_EQUAL_HEX32(0, xstr_to_uint32("0b0", 100, &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_HEX32(1, xstr_to_uint32("0b1", 100, &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_HEX32(100, xstr_to_uint32("+0b1", 100, &ok));
    TEST_ASSERT_TRUE(! ok);
    TEST_ASSERT_EQUAL_HEX32(100, xstr_to_uint32("-0b1", 100, &ok));
    TEST_ASSERT_TRUE(! ok);
    TEST_ASSERT_EQUAL_HEX32(0xFFFFFFFF, xstr_to_uint32("0b11111111111111111111111111111111", 100, &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_HEX32(100, xstr_to_uint32("0b2", 100, &ok));
    TEST_ASSERT_TRUE(! ok);

    TEST_ASSERT_EQUAL(UINT32_MAX, xstr_to_uint32("4294967295", 0, &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL(UINT32_MAX - 1, xstr_to_uint32("4294967294", 0, &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL(0, xstr_to_uint32("4294967394", 0, &ok));
    TEST_ASSERT_TRUE(! ok);
}


TEST(xstr, to_float)
{
    bool ok;

    X_TEST_ASSERTION_FAILED(xstr_to_float(NULL, 0.0f, &ok));
    X_TEST_ASSERTION_SUCCESS(xstr_to_float("0.0", 0.0f, NULL));

    TEST_ASSERT_EQUAL_FLOAT(0.1f, xstr_to_float("0.1", 0.0f, &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_FLOAT(1.0f, xstr_to_float("1.0", 0.0f, &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_FLOAT(1.0f, xstr_to_float("1", 0.0f, &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, xstr_to_float("0xDEADBEEF", 0.0f, &ok));
    TEST_ASSERT_TRUE(! ok);
}


TEST(xstr, to_double)
{
    bool ok;

    X_TEST_ASSERTION_FAILED(xstr_to_double(NULL, 0.0, &ok));
    X_TEST_ASSERTION_SUCCESS(xstr_to_double("0.0", 0.0, NULL));


    TEST_ASSERT_EQUAL_DOUBLE(0.1, xstr_to_double("0.1", 0.0, &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_DOUBLE(1.0, xstr_to_double("1.0", 0.0, &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_DOUBLE(1.0, xstr_to_double("1", 0.0, &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_DOUBLE(0.0, xstr_to_double("0xDEADBEEF", 0.0, &ok));
    TEST_ASSERT_TRUE(! ok);
}


TEST(xstr, to_bool)
{
    bool ok;

    X_TEST_ASSERTION_FAILED(xstr_to_bool(NULL, false, &ok));
    X_TEST_ASSERTION_SUCCESS(xstr_to_bool("true", false, NULL));

    TEST_ASSERT_TRUE(xstr_to_bool("Y", false, &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_TRUE(xstr_to_bool("y", false, &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_TRUE(xstr_to_bool("yes", false, &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_TRUE(xstr_to_bool("Yes", false, &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_TRUE(xstr_to_bool("true", false, &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_FALSE(xstr_to_bool("N", true, &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_FALSE(xstr_to_bool("n", true, &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_FALSE(xstr_to_bool("no", true, &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_FALSE(xstr_to_bool("No", true, &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_FALSE(xstr_to_bool("false", true, &ok));
    TEST_ASSERT_TRUE(ok);
}


TEST_GROUP_RUNNER(xstr)
{
    RUN_TEST_CASE(xstr, equal);
    RUN_TEST_CASE(xstr, case_equal);
    RUN_TEST_CASE(xstr, compare);
    RUN_TEST_CASE(xstr, case_compare);
    RUN_TEST_CASE(xstr, ncase_compare);
    RUN_TEST_CASE(xstr, search_substring);
    RUN_TEST_CASE(xstr, case_search_substring);
    RUN_TEST_CASE(xstr, duplicate);
    RUN_TEST_CASE(xstr, duplicate2);
    RUN_TEST_CASE(xstr, nduplicate);
    RUN_TEST_CASE(xstr, nduplicate2);
    RUN_TEST_CASE(xstr, reverse);
    RUN_TEST_CASE(xstr, strip);
    RUN_TEST_CASE(xstr, strip_left);
    RUN_TEST_CASE(xstr, strip_right);
    RUN_TEST_CASE(xstr, to_int);
    RUN_TEST_CASE(xstr, to_uint);
    RUN_TEST_CASE(xstr, to_float);
    RUN_TEST_CASE(xstr, to_double);
    RUN_TEST_CASE(xstr, to_bool);
}
