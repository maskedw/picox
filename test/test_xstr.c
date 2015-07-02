#include <picox/string/xstr.h>
#include <unity.h>
#include <unity_fixture.h>
#include "testutils.h"


TEST_GROUP(xstr);




TEST_SETUP(xstr)
{
}


TEST_TEAR_DOWN(xstr)
{
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

#if 1
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
#endif
}


TEST(xstr, search_substring)
{
    X_TEST_ASSERTION_FAILED(xstr_search_substring(NULL, NULL));
    X_TEST_ASSERTION_FAILED(xstr_search_substring("A", NULL));
    X_TEST_ASSERTION_FAILED(xstr_search_substring(NULL, "A"));
    X_TEST_ASSERTION_SUCCESS(xstr_search_substring("", ""));
}


TEST(xstr, case_search_substring)
{
    X_TEST_ASSERTION_FAILED(xstr_case_search_substring(NULL, NULL));
    X_TEST_ASSERTION_FAILED(xstr_case_search_substring("A", NULL));
    X_TEST_ASSERTION_FAILED(xstr_case_search_substring(NULL, "A"));
    X_TEST_ASSERTION_SUCCESS(xstr_case_search_substring("", ""));
}


TEST(xstr, duplicate)
{
    X_TEST_ASSERTION_FAILED(xstr_duplicate(NULL));
}


TEST(xstr, duplicate2)
{
    X_TEST_ASSERTION_FAILED(xstr_duplicate2(NULL, NULL));
    X_TEST_ASSERTION_FAILED(xstr_duplicate2(NULL, X_MALLOC));
    X_TEST_ASSERTION_FAILED(xstr_duplicate2("", NULL));
}


TEST(xstr, nduplicate)
{
    X_TEST_ASSERTION_FAILED(xstr_nduplicate(NULL, 1));
}


TEST(xstr, nduplicate2)
{
    X_TEST_ASSERTION_FAILED(xstr_nduplicate2(NULL, 1, NULL));
    X_TEST_ASSERTION_FAILED(xstr_nduplicate2(NULL, 1, X_MALLOC));
    X_TEST_ASSERTION_FAILED(xstr_nduplicate2("", 1, NULL));
}


TEST(xstr, reverse)
{
    X_TEST_ASSERTION_FAILED(xstr_reverse(NULL));
}


TEST(xstr, strip)
{
    X_TEST_ASSERTION_FAILED(xstr_strip(NULL, NULL));
}


TEST(xstr, strip_left)
{
    X_TEST_ASSERTION_FAILED(xstr_strip_left(NULL, NULL));
}


TEST(xstr, strip_right)
{
    X_TEST_ASSERTION_FAILED(xstr_strip_right(NULL, NULL));
}


TEST(xstr, to_int)
{
    int x;
    X_TEST_ASSERTION_FAILED(xstr_to_int(NULL, NULL, 0));
    X_TEST_ASSERTION_FAILED(xstr_to_int(NULL, &x, 0));
    X_TEST_ASSERTION_FAILED(xstr_to_int("10", NULL, 0));
}


TEST_GROUP_RUNNER(xstr)
{
    printf("\n");
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
}
