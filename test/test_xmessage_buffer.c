#include <picox/container/xmessage_buffer.h>
#include "testutils.h"


TEST_GROUP(xmsgbuf);


#define X__BUF_SIZE     (256)
static XMessageBuffer* mbuf;



TEST_SETUP(xmsgbuf)
{
    void* buf = x_malloc(X__BUF_SIZE);
    mbuf = x_malloc(sizeof(XMessageBuffer));

    memset(buf, 0x00, X__BUF_SIZE);
    xmsgbuf_init(mbuf, buf, X__BUF_SIZE);
}


TEST_TEAR_DOWN(xmsgbuf)
{
    x_free(xmsgbuf_data(mbuf));
    x_free(mbuf);
}


TEST(xmsgbuf, init)
{
    void* data = xmsgbuf_data(mbuf);
    X_UNUSED(data);
    X_TEST_ASSERTION_FAILED(xmsgbuf_init(NULL, data, X__BUF_SIZE));
    X_TEST_ASSERTION_FAILED(xmsgbuf_init(mbuf, NULL, X__BUF_SIZE));
    X_TEST_ASSERTION_FAILED(xmsgbuf_init(mbuf, NULL, sizeof(XMessageBuffer) - 1));
}


TEST(xmsgbuf, data)
{
    X_TEST_ASSERTION_FAILED(xmsgbuf_data(NULL));
}


TEST(xmsgbuf, clear)
{
    X_TEST_ASSERTION_FAILED(xmsgbuf_clear(NULL));
    int data;
    xmsgbuf_push(mbuf, &data, sizeof(data));
    TEST_ASSERT_FALSE(xmsgbuf_empty(mbuf));
    xmsgbuf_clear(mbuf);
    TEST_ASSERT_TRUE(xmsgbuf_empty(mbuf));
}

TEST(xmsgbuf, size)
{
    X_TEST_ASSERTION_FAILED(xmsgbuf_size(NULL));

    int data;
    xmsgbuf_push(mbuf, &data, sizeof(data));
    TEST_ASSERT_EQUAL(sizeof(data) + sizeof(XMessageHeader), xmsgbuf_size(mbuf));
}


TEST(xmsgbuf, empty)
{
    X_TEST_ASSERTION_FAILED(xmsgbuf_empty(NULL));
    int data;
    TEST_ASSERT_TRUE(xmsgbuf_empty(mbuf));
    xmsgbuf_push(mbuf, &data, sizeof(data));
    TEST_ASSERT_FALSE(xmsgbuf_empty(mbuf));
}


TEST(xmsgbuf, capacity)
{
    X_TEST_ASSERTION_FAILED(xmsgbuf_capacity(NULL));
    TEST_ASSERT_EQUAL(X__BUF_SIZE, xmsgbuf_capacity(mbuf));
}


TEST(xmsgbuf, reserve)
{
    X_TEST_ASSERTION_FAILED(xmsgbuf_reserve(NULL));

    TEST_ASSERT_EQUAL(X__BUF_SIZE, xmsgbuf_reserve(mbuf));
    int data;
    xmsgbuf_push(mbuf, &data, sizeof(data));
    TEST_ASSERT_EQUAL(X__BUF_SIZE - sizeof(data) - sizeof(XMessageHeader),
                      xmsgbuf_reserve(mbuf));
}


TEST(xmsgbuf, full)
{
    X_TEST_ASSERTION_FAILED(xmsgbuf_full(NULL));

    int data;
    size_t i;
    const size_t n = X__BUF_SIZE / (sizeof(data) + sizeof(XMessageHeader));

    for (i = 0; i < n; i++)
    {
        TEST_ASSERT_FALSE(xmsgbuf_full(mbuf));
        xmsgbuf_push(mbuf, &data, sizeof(data));
    }
    TEST_ASSERT_TRUE(xmsgbuf_full(mbuf));
}


TEST(xmsgbuf, num)
{
    X_TEST_ASSERTION_FAILED(xmsgbuf_num(NULL));

    int data;
    size_t i;
    const size_t n = X__BUF_SIZE / (sizeof(data) + sizeof(XMessageHeader));

    for (i = 0; i < n; i++)
    {
        xmsgbuf_push(mbuf, &data, sizeof(data));
        TEST_ASSERT_EQUAL(i + 1, xmsgbuf_num(mbuf));
    }
}


TEST(xmsgbuf, skip)
{
    X_TEST_ASSERTION_FAILED(xmsgbuf_skip(NULL));

    int data;
    size_t i;
    const size_t n = X__BUF_SIZE / (sizeof(data) + sizeof(XMessageHeader));

    for (i = 0; i < n; i++)
    {
        xmsgbuf_push(mbuf, &data, sizeof(data));
    }

    for (i = 0; i < n; i++)
    {
        xmsgbuf_skip(mbuf);
        TEST_ASSERT_EQUAL(n - i - 1, xmsgbuf_num(mbuf));
    }
    TEST_ASSERT_TRUE(xmsgbuf_empty(mbuf));
}


TEST(xmsgbuf, msg_size)
{
    X_TEST_ASSERTION_FAILED(xmsgbuf_msg_size(NULL));

    uint8_t u8;
    uint16_t u16;
    uint32_t u32;

    xmsgbuf_push(mbuf, &u8, sizeof(u8));
    xmsgbuf_push(mbuf, &u16, sizeof(u16));
    xmsgbuf_push(mbuf, &u32, sizeof(u32));

    TEST_ASSERT_EQUAL(sizeof(u8), xmsgbuf_msg_size(mbuf));
    xmsgbuf_skip(mbuf);
    TEST_ASSERT_EQUAL(sizeof(u16), xmsgbuf_msg_size(mbuf));
    xmsgbuf_skip(mbuf);
    TEST_ASSERT_EQUAL(sizeof(u32), xmsgbuf_msg_size(mbuf));
}


TEST(xmsgbuf, push)
{
    uint8_t w[X__BUF_SIZE - sizeof(XMessageHeader)];
    uint8_t r[sizeof(w)] = {0};

    size_t i;
    for (i = 0; i < sizeof(w); i++)
        w[i] = i;

    /* データがNULL */
    X_TEST_ASSERTION_FAILED(xmsgbuf_push(mbuf, NULL, sizeof(w)));

    /* サイズ0は不正 */
    X_TEST_ASSERTION_FAILED(xmsgbuf_push(mbuf, w, 0));

    /* ヘッダサイズ分の領域が余分に必要 */
    X_TEST_ASSERTION_FAILED(xmsgbuf_push(mbuf, w, xmsgbuf_capacity(mbuf)));

    /* 境界チェック */
    X_TEST_ASSERTION_FAILED(xmsgbuf_push(mbuf, w, X__BUF_SIZE - sizeof(XMessageHeader) + 1));
    X_TEST_ASSERTION_SUCCESS(xmsgbuf_push(mbuf, w, X__BUF_SIZE - sizeof(XMessageHeader)));
    xmsgbuf_clear(mbuf);
    X_TEST_ASSERTION_SUCCESS(xmsgbuf_push(mbuf, w, X__BUF_SIZE - sizeof(XMessageHeader) - 1));
    xmsgbuf_clear(mbuf);

    /* ちゃんと書けてる？ */
    xmsgbuf_push(mbuf, w, sizeof(w));
    TEST_ASSERT_EQUAL(sizeof(w), xmsgbuf_msg_size(mbuf));
    xmsgbuf_pull(mbuf, r);
    TEST_ASSERT_EQUAL_MEMORY(w, r, sizeof(w));

    /* 空きがないと書けないよ */
    xmsgbuf_clear(mbuf);
    xmsgbuf_push(mbuf, w, X__BUF_SIZE - sizeof(XMessageHeader));
    TEST_ASSERT_TRUE(xmsgbuf_reserve(mbuf) == 0);
    X_TEST_ASSERTION_FAILED(xmsgbuf_push(mbuf, w, 1));

    /* 1バイトのデータの格納には sizeof(XMessageHeader) + 1の空きが必要 */
    xmsgbuf_clear(mbuf);
    xmsgbuf_push(mbuf, w, X__BUF_SIZE - sizeof(XMessageHeader) * 2 - 1);
    X_TEST_ASSERTION_SUCCESS(xmsgbuf_push(mbuf, w, 1));
    X_TEST_ASSERTION_FAILED(xmsgbuf_push(mbuf, w, 1));
}


TEST(xmsgbuf, pull)
{
    /* データがNULL */
    X_TEST_ASSERTION_FAILED(xmsgbuf_pull(mbuf, NULL));

    const char* msg[] = {
        "Sunday", "Monday", "Tuesyday", "Wednesday", "Thursday", "Friday", "Saturday",
        "Sunday comes again!"};
    const size_t nmsgs = sizeof(msg) / sizeof(msg[0]);

    size_t i;
    for (i = 0; i < nmsgs; i++)
        xmsgbuf_push(mbuf, msg[i], strlen(msg[i]));

    char buf[128];
    for (i = 0; i < nmsgs; i++) {
        xmsgbuf_pull(mbuf, buf);
        buf[strlen(msg[i])] = '\0';
        TEST_ASSERT_EQUAL_STRING(msg[i], buf);
    }
}


TEST(xmsgbuf, boundary)
{
    const uint32_t v = 0xDEADBEEF;
    char buf[X__BUF_SIZE];

    /* リングバッファの境界をまたがってデータが配置されるように調整 */
    xmsgbuf_push(mbuf, buf, 10);
    xmsgbuf_push(mbuf, buf, xmsgbuf_reserve(mbuf) - sizeof(XMessageHeader) - sizeof(XMessageHeader) / 2);
    xmsgbuf_pull(mbuf, buf);
    xmsgbuf_push(mbuf, &v, sizeof(v));

    /* OKすか？ */
    xmsgbuf_pull(mbuf, buf);
    TEST_ASSERT_EQUAL(sizeof(v), xmsgbuf_msg_size(mbuf));
    xmsgbuf_pull(mbuf, buf);
    TEST_ASSERT_EQUAL_MEMORY(&v, buf, sizeof(v));
}


TEST_GROUP_RUNNER(xmsgbuf)
{
    RUN_TEST_CASE(xmsgbuf, init);
    RUN_TEST_CASE(xmsgbuf, data);
    RUN_TEST_CASE(xmsgbuf, clear);
    RUN_TEST_CASE(xmsgbuf, size);
    RUN_TEST_CASE(xmsgbuf, empty);
    RUN_TEST_CASE(xmsgbuf, capacity);
    RUN_TEST_CASE(xmsgbuf, reserve);
    RUN_TEST_CASE(xmsgbuf, full);
    RUN_TEST_CASE(xmsgbuf, num);
    RUN_TEST_CASE(xmsgbuf, skip);
    RUN_TEST_CASE(xmsgbuf, msg_size);
    RUN_TEST_CASE(xmsgbuf, push);
    RUN_TEST_CASE(xmsgbuf, pull);
    RUN_TEST_CASE(xmsgbuf, boundary);
}
