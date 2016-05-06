#include <picox/libc/xstdio.h>
#include "testutils.h"


TEST_GROUP(xstdio);


TEST_SETUP(xstdio)
{
    xdev_out(putchar);
}


TEST_TEAR_DOWN(xstdio)
{
}


TEST(xstdio, sprintf)
{
    char buf[1024];
    xprintf("Hello xprintf!!\n");
    xsprintf(buf, "%s", "abc");
    TEST_ASSERT_EQUAL_STRING("abc", buf);
    xsprintf(buf, "%5s", "abc");
    TEST_ASSERT_EQUAL_STRING("  abc", buf);
    xsprintf(buf, "%-5s", "abc");
    TEST_ASSERT_EQUAL_STRING("abc  ", buf);

    xsprintf(buf, "%-5s", "abc");
    TEST_ASSERT_EQUAL_STRING("abc  ", buf);

    int c = 10;
    void*p =  (void*)c;
}



TEST_GROUP_RUNNER(xstdio)
{
    RUN_TEST_CASE(xstdio, sprintf);
}
