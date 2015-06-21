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

#include <picox/container/xmsgbuf.h>
#include <stdio.h>


TEST_GROUP(xmsgbuf);


static XMsgBuf mbuf;
static XMsgBuf* const p = &mbuf;
static uint8_t mbuf_data[1024];



TEST_SETUP(xmsgbuf)
{
    xmsgbuf_init(p, mbuf_data, sizeof(mbuf_data));
    memset(mbuf_data, 0x00, sizeof(mbuf_data));
}


TEST_TEAR_DOWN(xmsgbuf)
{
}


TEST(xmsgbuf, init)
{
    TEST_ASSERT_TRUE(IS_ASSERTION(xmsgbuf_init(p, NULL, sizeof(mbuf_data))));
    TEST_ASSERT_TRUE(IS_ASSERTION(xmsgbuf_init(p, mbuf_data, 0)));
    TEST_ASSERT_TRUE(IS_ASSERTION(xmsgbuf_init(p, mbuf_data, sizeof(XMsgBufHeader) - 1)));
    TEST_ASSERT_FALSE(IS_ASSERTION(xmsgbuf_init(p, mbuf_data, sizeof(mbuf_data))));
}


TEST(xmsgbuf, push)
{
    uint8_t w[sizeof(mbuf_data) / 2];
    uint8_t r[sizeof(w)] = {0};

    int i;
    for (i = 0; i < sizeof(w); i++)
        w[i] = i;

    /* データがNULL */
    TEST_ASSERT_TRUE(IS_ASSERTION(xmsgbuf_push(p, NULL, sizeof(w))));

    /* サイズ0は不正 */
    TEST_ASSERT_TRUE(IS_ASSERTION(xmsgbuf_push(p, w, 0)));

    /* ヘッダサイズ分の領域が余分に必要 */
    TEST_ASSERT_TRUE(IS_ASSERTION(xmsgbuf_push(p, w, xmsgbuf_capacity(p))));

    /* 境界チェック */
    TEST_ASSERT_TRUE(IS_ASSERTION(xmsgbuf_push(p, w, sizeof(mbuf_data) - sizeof(XMsgBufHeader) + 1)));
    TEST_ASSERT_FALSE(IS_ASSERTION(xmsgbuf_push(p, w, sizeof(mbuf_data) - sizeof(XMsgBufHeader))));
    xmsgbuf_clear(p);
    TEST_ASSERT_FALSE(IS_ASSERTION(xmsgbuf_push(p, w, sizeof(mbuf_data) - sizeof(XMsgBufHeader) - 1)));
    xmsgbuf_clear(p);

    /* ちゃんと書けてる？ */
    xmsgbuf_push(p, w, sizeof(w));
    TEST_ASSERT_EQUAL(sizeof(w), xmsgbuf_msg_size(p));
    xmsgbuf_pull(p, r);
    TEST_ASSERT_EQUAL_MEMORY(w, r, sizeof(w));

    /* 空きがないと書けないよ */
    xmsgbuf_clear(p);
    xmsgbuf_push(p, mbuf_data, sizeof(mbuf_data) - sizeof(XMsgBufHeader));
    TEST_ASSERT_TRUE(xmsgbuf_reserve(p) == 0);
    TEST_ASSERT_TRUE(IS_ASSERTION(xmsgbuf_push(p, mbuf_data, 1)));

    /* 1バイトのデータの格納には sizeof(XMsgBufHeader) + 1の空きが必要 */
    xmsgbuf_clear(p);
    xmsgbuf_push(p, mbuf_data, sizeof(mbuf_data) - sizeof(XMsgBufHeader) * 2 - 1);
    TEST_ASSERT_FALSE(IS_ASSERTION(xmsgbuf_push(p, mbuf_data, 1)));
    TEST_ASSERT_TRUE(xmsgbuf_reserve(p) == 0);
}


TEST(xmsgbuf, pull)
{
    /* データがNULL */
    TEST_ASSERT_TRUE(IS_ASSERTION(xmsgbuf_pull(p, NULL)));

    const char* msg[] = {
        "Sunday", "Monday", "Tuesyday", "Wednesday", "Thursday", "Friday", "Saturday",
        "Sunday comes again!"};
    const size_t nmsgs = sizeof(msg) / sizeof(msg[0]);

    int i;
    for (i = 0; i < nmsgs; i++)
        xmsgbuf_push(p, msg[i], strlen(msg[i]));

    char buf[128];
    for (i = 0; i < nmsgs; i++) {
        xmsgbuf_pull(p, buf);
        buf[strlen(msg[i])] = '\0';
        TEST_ASSERT_EQUAL_STRING(msg[i], buf);
    }
}


TEST(xmsgbuf, boundary)
{
    const uint32_t v = 0xDEADBEEF;
    char buf[sizeof(mbuf_data)];

    /* リングバッファの境界をまたがってデータが配置されるように調整 */
    xmsgbuf_push(p, mbuf_data, 10);
    xmsgbuf_push(p, mbuf_data, xmsgbuf_reserve(p) - sizeof(XMsgBufHeader) - sizeof(XMsgBufHeader) / 2);
    xmsgbuf_pull(p, buf);
    xmsgbuf_push(p, &v, sizeof(v));

    /* OKすか？ */
    xmsgbuf_pull(p, buf);
    TEST_ASSERT_EQUAL(sizeof(v), xmsgbuf_msg_size(p));
    xmsgbuf_pull(p, buf);
    TEST_ASSERT_EQUAL_MEMORY(&v, buf, sizeof(v));
}


TEST(xmsgbuf, size)
{
    /* 初期値 */
    TEST_ASSERT_EQUAL(0, xmsgbuf_size(p));
    TEST_ASSERT_TRUE(xmsgbuf_empty(p));
    TEST_ASSERT_EQUAL(sizeof(mbuf_data), xmsgbuf_capacity(p));
    TEST_ASSERT_EQUAL(sizeof(mbuf_data), xmsgbuf_reserve(p));
    TEST_ASSERT_FALSE(xmsgbuf_full(p));

    /* 適当なデータを入れる */
    const uint32_t v = 0xDEADBEEF;
    xmsgbuf_push(p, &v, sizeof(v));
    TEST_ASSERT_FALSE(xmsgbuf_empty(p));
    TEST_ASSERT_EQUAL(sizeof(mbuf_data), xmsgbuf_capacity(p));
    TEST_ASSERT_EQUAL(sizeof(mbuf_data) - sizeof(v) - sizeof(XMsgBufHeader), xmsgbuf_reserve(p));
    TEST_ASSERT_FALSE(xmsgbuf_full(p));

    /* クリアしたら初期値に戻る */
    xmsgbuf_clear(p);
    TEST_ASSERT_EQUAL(0, xmsgbuf_size(p));
    TEST_ASSERT_TRUE(xmsgbuf_empty(p));
    TEST_ASSERT_EQUAL(sizeof(mbuf_data), xmsgbuf_capacity(p));
    TEST_ASSERT_EQUAL(sizeof(mbuf_data), xmsgbuf_reserve(p));
    TEST_ASSERT_FALSE(xmsgbuf_full(p));

    /* 満タンにする */
    xmsgbuf_push(p, mbuf_data, xmsgbuf_reserve(p) - sizeof(XMsgBufHeader));
    TEST_ASSERT_FALSE(xmsgbuf_empty(p));
    TEST_ASSERT_EQUAL(sizeof(mbuf_data), xmsgbuf_capacity(p));
    TEST_ASSERT_EQUAL(0, xmsgbuf_reserve(p));
    TEST_ASSERT_TRUE(xmsgbuf_full(p));
}


TEST_GROUP_RUNNER(xmsgbuf)
{
    RUN_TEST_CASE(xmsgbuf, init);
    RUN_TEST_CASE(xmsgbuf, push);
    RUN_TEST_CASE(xmsgbuf, pull);
    RUN_TEST_CASE(xmsgbuf, boundary);
    RUN_TEST_CASE(xmsgbuf, size);
}
