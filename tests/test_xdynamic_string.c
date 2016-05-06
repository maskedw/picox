#include <picox/string/xdynamic_string.h>
#include <unity.h>
#include <unity_fixture.h>
#include "testutils.h"


TEST_GROUP(xdstr);


TEST_SETUP(xdstr)
{
}


TEST_TEAR_DOWN(xdstr)
{
}


TEST(xdstr, create)
{
    XDynamicString* dstr = xdstr_create("Hello");
    TEST_ASSERT_EQUAL_STRING("Hello", xdstr_c_str(dstr));
    xdstr_destroy(dstr);

    dstr = xdstr_create_length("Hello World", 5);
    TEST_ASSERT_EQUAL_STRING("Hello", xdstr_c_str(dstr));
    xdstr_destroy(dstr);

    dstr = xdstr_create_empty();
    TEST_ASSERT_EQUAL_STRING("", xdstr_c_str(dstr));
    xdstr_destroy(dstr);

    dstr = xdstr_create("Hello");
    XDynamicString* cloned = xdstr_clone(dstr);
    TEST_ASSERT_EQUAL_STRING("Hello", xdstr_c_str(cloned));
    xdstr_destroy(dstr);
    xdstr_destroy(cloned);
}


TEST(xdstr, cat)
{
    XDynamicString* dstr = xdstr_create("Hello");
    dstr = xdstr_cat(dstr, " World");
    TEST_ASSERT_EQUAL_STRING("Hello World", xdstr_c_str(dstr));
    xdstr_destroy(dstr);

    dstr = xdstr_create("Hello");
    dstr = xdstr_cat_n(dstr, " World", 3);
    TEST_ASSERT_EQUAL_STRING("Hello Wo", xdstr_c_str(dstr));
    xdstr_destroy(dstr);

    dstr = xdstr_create("Hello");
    dstr = xdstr_cat_printf(dstr, "%s %d", " World", 100);
    TEST_ASSERT_EQUAL_STRING("Hello World 100", xdstr_c_str(dstr));
    xdstr_destroy(dstr);
}


TEST(xdstr, copy)
{
    XDynamicString* dstr = xdstr_create("FooBar");
    dstr = xdstr_copy(dstr, "Hello");
    TEST_ASSERT_EQUAL_STRING("Hello", xdstr_c_str(dstr));
    xdstr_destroy(dstr);

    dstr = xdstr_create("FooBar");
    dstr = xdstr_copy_n(dstr, "Hello", 3);
    TEST_ASSERT_EQUAL_STRING("Hel", xdstr_c_str(dstr));
    xdstr_destroy(dstr);
}


TEST(xdstr, trim)
{
    XDynamicString* dstr = xdstr_create("@[+:^() Hello World$ @;:[]");
    xdstr_trim(dstr, "@[]+:^(); ");
    dstr = xdstr_copy(dstr, "HelloWorld$");
    TEST_ASSERT_EQUAL_STRING("HelloWorld$", xdstr_c_str(dstr));
    xdstr_destroy(dstr);
}


TEST(xdstr, range)
{
    XDynamicString* dstr = xdstr_create("Hello World");
    xdstr_range(dstr, 2, 5);
    TEST_ASSERT_EQUAL_STRING("llo ", xdstr_c_str(dstr));
    xdstr_destroy(dstr);

    dstr = xdstr_create("Hello World");
    xdstr_range(dstr, 1, -1);
    TEST_ASSERT_EQUAL_STRING("ello World", xdstr_c_str(dstr));
    xdstr_destroy(dstr);

    dstr = xdstr_create("Hello World");
    xdstr_range(dstr, -5, -1);
    TEST_ASSERT_EQUAL_STRING("World", xdstr_c_str(dstr));
    xdstr_destroy(dstr);
}


TEST(xdstr, to_upper)
{
    XDynamicString* dstr = xdstr_create("Hello World");
    xdstr_to_upper(dstr);
    TEST_ASSERT_EQUAL_STRING("HELLO WORLD", xdstr_c_str(dstr));
    xdstr_destroy(dstr);
}


TEST(xdstr, to_lower)
{
    XDynamicString* dstr = xdstr_create("Hello World");
    xdstr_to_lower(dstr);
    TEST_ASSERT_EQUAL_STRING("hello world", xdstr_c_str(dstr));
    xdstr_destroy(dstr);
}


TEST(xdstr, storage)
{
    XDynamicString* dstr = xdstr_create("Hello World");
    dstr = xdstr_reserve(dstr, 1000);
    TEST_ASSERT_EQUAL(1000, xdstr_capacity(dstr));
    TEST_ASSERT_EQUAL(strlen("Hello World"), xdstr_length(dstr));
    dstr = xdstr_shrink_to_fit(dstr);;
    TEST_ASSERT_EQUAL(strlen("Hello World"), xdstr_capacity(dstr));
    xdstr_destroy(dstr);
}


TEST_GROUP_RUNNER(xdstr)
{
    RUN_TEST_CASE(xdstr, create);
    RUN_TEST_CASE(xdstr, cat);
    RUN_TEST_CASE(xdstr, copy);
    RUN_TEST_CASE(xdstr, trim);
    RUN_TEST_CASE(xdstr, range);
    RUN_TEST_CASE(xdstr, to_upper);
    RUN_TEST_CASE(xdstr, to_lower);
    RUN_TEST_CASE(xdstr, storage);
}
