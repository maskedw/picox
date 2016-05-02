#include <picox/core/xcore.h>
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
    x_test_stream(stream);
    DestoryStream(stream);
}

TEST_GROUP_RUNNER(xstream)
{
    RUN_TEST_CASE(xstream, main);
}
