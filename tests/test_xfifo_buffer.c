#include <picox/container/xfifo_buffer.h>
#include "testutils.h"


TEST_GROUP(xfifo);


static XFifoBuffer* fifo;
#define X__BUF_SIZE 128


TEST_SETUP(xfifo)
{
    void* buf = x_malloc(X__BUF_SIZE);
    fifo = x_malloc(sizeof(XFifoBuffer));

    memset(buf, 0x00, X__BUF_SIZE);
    xfifo_init(fifo, buf, X__BUF_SIZE, NULL);
}


TEST_TEAR_DOWN(xfifo)
{
    x_free(xfifo_data(fifo));
    x_free(fifo);
}


TEST(xfifo, init)
{
    void* data = xfifo_data(fifo);
    X_UNUSED(data);
    X_TEST_ASSERTION_FAILED(xfifo_init(NULL, data, X__BUF_SIZE, NULL));
    X_TEST_ASSERTION_FAILED(xfifo_init(fifo, NULL, X__BUF_SIZE, NULL));
    X_TEST_ASSERTION_FAILED(xfifo_init(fifo, data, 0, NULL));
    X_TEST_ASSERTION_FAILED(xfifo_init(fifo, data, 3, NULL));
}


TEST(xfifo, clear)
{
    X_TEST_ASSERTION_FAILED(xfifo_clear(NULL));
    xfifo_push_back(fifo, 10);
    TEST_ASSERT_EQUAL(1, xfifo_size(fifo));
    xfifo_clear(fifo);
    TEST_ASSERT_EQUAL(0, xfifo_size(fifo));
}


TEST(xfifo, empty)
{
    X_TEST_ASSERTION_FAILED(xfifo_empty(NULL));
    xfifo_push_back(fifo, 10);
    TEST_ASSERT_EQUAL(1, xfifo_size(fifo));
    TEST_ASSERT_FALSE(xfifo_empty(fifo));
}


TEST(xfifo, capacity)
{
    X_TEST_ASSERTION_FAILED(xfifo_capacity(NULL));
    TEST_ASSERT_EQUAL(X__BUF_SIZE - 1, xfifo_capacity(fifo));
}


TEST(xfifo, full)
{
    X_TEST_ASSERTION_FAILED(xfifo_full(NULL));
    size_t i;
    for (i = 0; i < xfifo_capacity(fifo); i++)
    {
        TEST_ASSERT_FALSE(xfifo_full(fifo));
        xfifo_push_back(fifo, 10);
    }
    TEST_ASSERT_TRUE(xfifo_full(fifo));
}


TEST(xfifo, reserve)
{
    X_TEST_ASSERTION_FAILED(xfifo_reserve(NULL));
    size_t i;
    for (i = 0; i < xfifo_capacity(fifo); i++)
    {
        TEST_ASSERT_EQUAL(xfifo_capacity(fifo) - i, xfifo_reserve(fifo));
        xfifo_push_back(fifo, 10);
    }
    TEST_ASSERT_EQUAL(0, xfifo_reserve(fifo));
}


TEST(xfifo, data)
{
    X_TEST_ASSERTION_FAILED(xfifo_data(NULL));
}


TEST(xfifo, size)
{
    X_TEST_ASSERTION_FAILED(xfifo_size(NULL));
    size_t i;
    for (i = 0; i < xfifo_capacity(fifo); i++)
    {
        TEST_ASSERT_EQUAL(i, xfifo_size(fifo));
        xfifo_push_back(fifo, 10);
    }
    TEST_ASSERT_EQUAL(xfifo_capacity(fifo), xfifo_size(fifo));
}


TEST(xfifo, push_pop)
{
    X_TEST_ASSERTION_FAILED(xfifo_push_back(NULL, 10));
    X_TEST_ASSERTION_FAILED(xfifo_pop_front(NULL));

    const uint8_t val = 0xAA;
    xfifo_push_back(fifo, val);
    TEST_ASSERT_EQUAL(1, xfifo_size(fifo));
    TEST_ASSERT_EQUAL(val, xfifo_pop_front(fifo));

    /* 10個入れたらサイズは10 */
    uint8_t i;
    for (i = 0; i < 10; i++)
        xfifo_push_back(fifo, i);
    TEST_ASSERT_EQUAL(10, xfifo_size(fifo));

    /* 10個取り出したらサイズは0 */
    for (i = 0; i < 10; i++)
        TEST_ASSERT_EQUAL(i, xfifo_pop_front(fifo));
    TEST_ASSERT_EQUAL(0, xfifo_size(fifo));
}


TEST_GROUP_RUNNER(xfifo)
{
    RUN_TEST_CASE(xfifo, init);
    RUN_TEST_CASE(xfifo, clear);
    RUN_TEST_CASE(xfifo, empty);
    RUN_TEST_CASE(xfifo, capacity);
    RUN_TEST_CASE(xfifo, size);
    RUN_TEST_CASE(xfifo, full);
    RUN_TEST_CASE(xfifo, reserve);
    RUN_TEST_CASE(xfifo, data);
    RUN_TEST_CASE(xfifo, push_pop);
}
