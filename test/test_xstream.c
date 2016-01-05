#include <picox/core/xcore.h>
#include <unity.h>
#include <unity_fixture.h>
#include "testutils.h"


static XStream* CreateStream(void)
{
    XMemStream* stream = x_malloc(sizeof(XMemStream));
    X_ASSERT(stream);
    void* buf = x_calloc(1, 1024);
    X_ASSERT(buf);
    xmemstream_init(stream, buf, 512, 1024);

    return (XStream*)stream;
}


static void DestoryStream(XStream* stream)
{
    x_free(((XMemStream*)stream)->mem);
    x_free(stream);
}



TEST_GROUP(xstream);


TEST_SETUP(xstream)
{
}


TEST_TEAR_DOWN(xstream)
{
}


TEST(xstream, main)
{
    XStream* stream = CreateStream();
    char buf[128];
    size_t n;
    memset(buf, '@', sizeof(buf));

    TEST_ASSERT_TRUE(xstream_write(stream, "Hello World", strlen("Hello World"), &n) == 0);
    TEST_ASSERT_EQUAL(n, strlen("Hello World"));
    TEST_ASSERT_TRUE(xstream_seek(stream, 0, X_SEEK_SET) == 0);
    TEST_ASSERT_TRUE(xstream_read(stream, buf, n, &n) == 0);
    TEST_ASSERT_EQUAL(n, strlen("Hello World"));
    buf[n] = '\0';
    TEST_ASSERT_EQUAL_STRING(buf, "Hello World");
    TEST_ASSERT_TRUE(xstream_seek(stream, 0, X_SEEK_SET) == 0);

    const int ret = xstream_printf(stream, "%s %s\n", "hello", "world");
    TEST_ASSERT_EQUAL(ret, (int)strlen("hello world\n"));
    TEST_ASSERT_TRUE(xstream_seek(stream, 0, X_SEEK_SET) == 0);
    TEST_ASSERT_TRUE(xstream_read_line(stream, buf, sizeof(buf), NULL, NULL) == 0);
    TEST_ASSERT_EQUAL_STRING(buf, "hello world");

    DestoryStream(stream);;
}

TEST_GROUP_RUNNER(xstream)
{
    RUN_TEST_CASE(xstream, main);
}
