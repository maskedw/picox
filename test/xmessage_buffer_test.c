#include <unity.h>
#include <unity_fixture.h>
#include <setjmp.h>
#include <string.h>

static jmp_buf jmp;
#define XMBUF_ASSERT(expr)          \
    do {                            \
        if (! (expr))               \
            longjmp(jmp, 1);        \
    } while (0);

#define IS_ASSERTION(x)             \
    ({                              \
        bool ret = false;           \
        if (setjmp(jmp) == 0)       \
            x;                      \
        else                        \
            ret = true;             \
        ret;                        \
    })

#include <picox/container/xmessage_buffer.h>
#include <stdio.h>


TEST_GROUP(xmbuf);


static XMBuf mbuf;
static XMBuf* const p = &mbuf;
static uint8_t mbuf_data[1024];



TEST_SETUP(xmbuf)
{
    xmbuf_init(p, mbuf_data, sizeof(mbuf_data));
    memset(mbuf_data, 0x00, sizeof(mbuf_data));
}


TEST_TEAR_DOWN(xmbuf)
{
}


TEST(xmbuf, init)
{
    TEST_ASSERT_TRUE(IS_ASSERTION(xmbuf_init(p, NULL, sizeof(mbuf_data))));
    TEST_ASSERT_TRUE(IS_ASSERTION(xmbuf_init(p, mbuf_data, 0)));
    TEST_ASSERT_TRUE(IS_ASSERTION(xmbuf_init(p, mbuf_data, sizeof(XMBufHeader) - 1)));
    TEST_ASSERT_FALSE(IS_ASSERTION(xmbuf_init(p, mbuf_data, sizeof(mbuf_data))));
}


TEST(xmbuf, push)
{
    uint8_t w[sizeof(mbuf_data) / 2];
    uint8_t r[sizeof(w)] = {0};

    int i;
    for (i = 0; i < sizeof(w); i++)
        w[i] = i;

    /* データがNULL */
    TEST_ASSERT_TRUE(IS_ASSERTION(xmbuf_push(p, NULL, sizeof(w))));

    /* サイズ0は不正 */
    TEST_ASSERT_TRUE(IS_ASSERTION(xmbuf_push(p, w, 0)));

    /* ヘッダサイズ分の領域が余分に必要 */
    TEST_ASSERT_TRUE(IS_ASSERTION(xmbuf_push(p, w, xmbuf_capacity(p))));

    /* 境界チェック */
    TEST_ASSERT_TRUE(IS_ASSERTION(xmbuf_push(p, w, sizeof(mbuf_data) - sizeof(XMBufHeader) + 1)));
    TEST_ASSERT_FALSE(IS_ASSERTION(xmbuf_push(p, w, sizeof(mbuf_data) - sizeof(XMBufHeader))));
    xmbuf_clear(p);
    TEST_ASSERT_FALSE(IS_ASSERTION(xmbuf_push(p, w, sizeof(mbuf_data) - sizeof(XMBufHeader) - 1)));
    xmbuf_clear(p);

    /* ちゃんと書けてる？ */
    xmbuf_push(p, w, sizeof(w));
    TEST_ASSERT_EQUAL(sizeof(w), xmbuf_msg_size(p));
    xmbuf_pull(p, r);
    TEST_ASSERT_EQUAL_MEMORY(w, r, sizeof(w));

    /* 空きがないと書けないよ */
    xmbuf_clear(p);
    xmbuf_push(p, mbuf_data, sizeof(mbuf_data) - sizeof(XMBufHeader));
    TEST_ASSERT_TRUE(xmbuf_reserve(p) == 0);
    TEST_ASSERT_TRUE(IS_ASSERTION(xmbuf_push(p, mbuf_data, 1)));

    /* 1バイトのデータの格納には sizeof(XMBufHeader) + 1の空きが必要 */
    xmbuf_clear(p);
    xmbuf_push(p, mbuf_data, sizeof(mbuf_data) - sizeof(XMBufHeader) * 2 - 1);
    TEST_ASSERT_FALSE(IS_ASSERTION(xmbuf_push(p, mbuf_data, 1)));
    TEST_ASSERT_TRUE(xmbuf_reserve(p) == 0);
}


TEST(xmbuf, pull)
{
    /* データがNULL */
    TEST_ASSERT_TRUE(IS_ASSERTION(xmbuf_pull(p, NULL)));

    const char* msg[] = {
        "Sunday", "Monday", "Tuesyday", "Wednesday", "Thursday", "Friday", "Saturday",
        "Sunday comes again!"};
    const size_t nmsgs = sizeof(msg) / sizeof(msg[0]);

    int i;
    for (i = 0; i < nmsgs; i++)
        xmbuf_push(p, msg[i], strlen(msg[i]));

    char buf[128];
    for (i = 0; i < nmsgs; i++) {
        xmbuf_pull(p, buf);
        buf[strlen(msg[i])] = '\0';
        TEST_ASSERT_EQUAL_STRING(msg[i], buf);
    }
}


TEST(xmbuf, boundary)
{
    const uint32_t v = 0xDEADBEEF;
    char buf[sizeof(mbuf_data)];

    /* リングバッファの境界をまたがってデータが配置されるように調整 */
    xmbuf_push(p, mbuf_data, 10);
    xmbuf_push(p, mbuf_data, xmbuf_reserve(p) - sizeof(XMBufHeader) - sizeof(XMBufHeader) / 2);
    xmbuf_pull(p, buf);
    xmbuf_push(p, &v, sizeof(v));

    /* OKすか？ */
    xmbuf_pull(p, buf);
    TEST_ASSERT_EQUAL(sizeof(v), xmbuf_msg_size(p));
    xmbuf_pull(p, buf);
    TEST_ASSERT_EQUAL_MEMORY(&v, buf, sizeof(v));
}


TEST(xmbuf, size)
{
    /* 初期値 */
    TEST_ASSERT_EQUAL(0, xmbuf_size(p));
    TEST_ASSERT_TRUE(xmbuf_empty(p));
    TEST_ASSERT_EQUAL(sizeof(mbuf_data), xmbuf_capacity(p));
    TEST_ASSERT_EQUAL(sizeof(mbuf_data), xmbuf_reserve(p));
    TEST_ASSERT_FALSE(xmbuf_full(p));

    /* 適当なデータを入れる */
    const uint32_t v = 0xDEADBEEF;
    xmbuf_push(p, &v, sizeof(v));
    TEST_ASSERT_FALSE(xmbuf_empty(p));
    TEST_ASSERT_EQUAL(sizeof(mbuf_data), xmbuf_capacity(p));
    TEST_ASSERT_EQUAL(sizeof(mbuf_data) - sizeof(v) - sizeof(XMBufHeader), xmbuf_reserve(p));
    TEST_ASSERT_FALSE(xmbuf_full(p));

    /* クリアしたら初期値に戻る */
    xmbuf_clear(p);
    TEST_ASSERT_EQUAL(0, xmbuf_size(p));
    TEST_ASSERT_TRUE(xmbuf_empty(p));
    TEST_ASSERT_EQUAL(sizeof(mbuf_data), xmbuf_capacity(p));
    TEST_ASSERT_EQUAL(sizeof(mbuf_data), xmbuf_reserve(p));
    TEST_ASSERT_FALSE(xmbuf_full(p));

    /* 満タンにする */
    xmbuf_push(p, mbuf_data, xmbuf_reserve(p) - sizeof(XMBufHeader));
    TEST_ASSERT_FALSE(xmbuf_empty(p));
    TEST_ASSERT_EQUAL(sizeof(mbuf_data), xmbuf_capacity(p));
    TEST_ASSERT_EQUAL(0, xmbuf_reserve(p));
    TEST_ASSERT_TRUE(xmbuf_full(p));
}


TEST_GROUP_RUNNER(xmbuf)
{
    RUN_TEST_CASE(xmbuf, init);
    RUN_TEST_CASE(xmbuf, push);
    RUN_TEST_CASE(xmbuf, pull);
    RUN_TEST_CASE(xmbuf, boundary);
    RUN_TEST_CASE(xmbuf, size);
}
