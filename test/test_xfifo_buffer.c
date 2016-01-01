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
    X_TEST_ASSERTION_FAILED(xfifo_init(NULL, data, X__BUF_SIZE, NULL));
    X_TEST_ASSERTION_FAILED(xfifo_init(fifo, NULL, X__BUF_SIZE, NULL));
    X_TEST_ASSERTION_FAILED(xfifo_init(fifo, data, 0, NULL));
    X_TEST_ASSERTION_FAILED(xfifo_init(fifo, data, 3, NULL));
}


TEST(xfifo, clear)
{
    X_TEST_ASSERTION_FAILED(xfifo_clear(NULL));
    xfifo_push(fifo, 10);
    TEST_ASSERT_EQUAL(1, xfifo_size(fifo));
    xfifo_clear(fifo);
    TEST_ASSERT_EQUAL(0, xfifo_size(fifo));
}


TEST(xfifo, empty)
{
    X_TEST_ASSERTION_FAILED(xfifo_empty(NULL));
    xfifo_push(fifo, 10);
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
        xfifo_push(fifo, 10);
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
        xfifo_push(fifo, 10);
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
        xfifo_push(fifo, 10);
    }
    TEST_ASSERT_EQUAL(xfifo_capacity(fifo), xfifo_size(fifo));
}


TEST(xfifo, push_pop)
{
    X_TEST_ASSERTION_FAILED(xfifo_push(NULL, 10));
    X_TEST_ASSERTION_FAILED(xfifo_pop(NULL));

    const uint8_t val = 0xAA;
    xfifo_push(fifo, val);
    TEST_ASSERT_EQUAL(1, xfifo_size(fifo));
    TEST_ASSERT_EQUAL(val, xfifo_pop(fifo));

    /* 10個入れたらサイズは10 */
    uint8_t i;
    for (i = 0; i < 10; i++)
        xfifo_push(fifo, i);
    TEST_ASSERT_EQUAL(10, xfifo_size(fifo));

    /* 10個取り出したらサイズは0 */
    for (i = 0; i < 10; i++)
        TEST_ASSERT_EQUAL(i, xfifo_pop(fifo));
    TEST_ASSERT_EQUAL(0, xfifo_size(fifo));
}


TEST(xfifo, write)
{
    uint8_t data[10];
    size_t i;

    X_TEST_ASSERTION_FAILED(xfifo_write(NULL, data, sizeof(data)));
    X_TEST_ASSERTION_FAILED(xfifo_write(fifo, NULL, sizeof(data)));

    /* 書き込んだ要素数を返す */
    for (i = 0; i < sizeof(data); i++)
        data[i] = i;
    TEST_ASSERT_EQUAL(sizeof(data), xfifo_write(fifo, data, sizeof(data)));

    /* ちゃんと書き込めてる？ */
    i = 0;
    while (! xfifo_empty(fifo)) {
        uint8_t popped = xfifo_pop(fifo);
        TEST_ASSERT_TRUE((popped < sizeof(data)));
        TEST_ASSERT_TRUE(popped == i);
        i++;
    }

    /* 満タンだと書き込めないよね */
    while (! xfifo_full(fifo))
         xfifo_push(fifo, 0xFF);
    TEST_ASSERT_EQUAL(0, xfifo_write(fifo, data, sizeof(data)));

    /* 空き要素数が足りなくても書ける分だけ書く */
    while (xfifo_reserve(fifo) < (sizeof(data) / 2))
        xfifo_pop(fifo);
    TEST_ASSERT_EQUAL(sizeof(data) / 2, xfifo_write(fifo, data, sizeof(data)));
}


TEST(xfifo, read)
{
    uint8_t w[10];
    uint8_t r[10];
    size_t i;

    X_TEST_ASSERTION_FAILED(xfifo_read(NULL, r, sizeof(r)));
    X_TEST_ASSERTION_FAILED(xfifo_read(fifo, NULL, sizeof(r)));

    for (i = 0; i < sizeof(w); i++)
        w[i] = i;
    xfifo_write(fifo, w, sizeof(w));

    /* 読み込んだ要素数を返す */
    TEST_ASSERT_EQUAL(sizeof(w), xfifo_read(fifo, r, sizeof(r)));

    /* ちゃんと読めてる？ */
    TEST_ASSERT_TRUE(memcmp(w, r, sizeof(w)) == 0);

    /* 読める分だけ読む */
    memset(r, 0xFF, sizeof(r));
    memset(w, 0xAA, sizeof(w));
    xfifo_write(fifo, w, sizeof(w) / 2);
    TEST_ASSERT_EQUAL(sizeof(w) / 2, xfifo_read(fifo, r, sizeof(r)));
    TEST_ASSERT_TRUE(memcmp(w, r, sizeof(w) / 2) == 0);
    TEST_ASSERT_TRUE(memcmp(w + sizeof(w) / 2, r + sizeof(w) / 2,
                            sizeof(w) / 2) != 0);

    /* 要素がなかったら読めんよ */
    TEST_ASSERT_TRUE(xfifo_empty(fifo));
    TEST_ASSERT_EQUAL(0, xfifo_read(fifo, r, sizeof(r)));
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
    RUN_TEST_CASE(xfifo, write);
    RUN_TEST_CASE(xfifo, read);
}
