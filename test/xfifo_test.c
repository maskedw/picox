#include <unity.h>
#include <unity_fixture.h>
#include <picox/container/xfifo.h>
#include <stdio.h>


TEST_GROUP(xfifo);


static XFifo fifo;
static XFifo* const p = &fifo;
static uint8_t buf[128];


TEST_SETUP(xfifo)
{
    xfifo_init(p, buf, sizeof(buf), NULL);
    memset(buf, 0x00, sizeof(buf));
}


TEST_TEAR_DOWN(xfifo)
{
}


TEST(xfifo, push_pop)
{
    const uint8_t val = 0xAA;
    xfifo_push(p, val);
    TEST_ASSERT_EQUAL(1, xfifo_size(p));
    TEST_ASSERT_EQUAL(val, xfifo_pop(p));

    /* 10���ꂽ��T�C�Y��10 */
    uint8_t i;
    for (i = 0; i < 10; i++)
        xfifo_push(p, i);
    TEST_ASSERT_EQUAL(10, xfifo_size(p));

    /* 10���o������T�C�Y��0 */
    for (i = 0; i < 10; i++)
        TEST_ASSERT_EQUAL(i, xfifo_pop(p));
    TEST_ASSERT_EQUAL(0, xfifo_size(p));
}


TEST(xfifo, size)
{
    /* �����l */
    TEST_ASSERT_EQUAL(0, xfifo_size(p));
    TEST_ASSERT_TRUE(xfifo_empty(p));
    TEST_ASSERT_EQUAL(sizeof(buf) - 1, xfifo_capacity(p));
    TEST_ASSERT_EQUAL(sizeof(buf) - 1, xfifo_reserve(p));
    TEST_ASSERT_FALSE(xfifo_full(p));

    /* 1�v�f����ꂽ */
    xfifo_push(p, 0xAA);
    TEST_ASSERT_EQUAL(1, xfifo_size(p));
    TEST_ASSERT_FALSE(xfifo_empty(p));
    TEST_ASSERT_EQUAL(sizeof(buf) - 1, xfifo_capacity(p));
    TEST_ASSERT_EQUAL(sizeof(buf) - 1 - 1, xfifo_reserve(p));
    TEST_ASSERT_FALSE(xfifo_full(p));

    /* 10�v�f����ꂽ */
    xfifo_pop(p);
    int i;
    for (i = 0; i < 10; i++)
        xfifo_push(p, i);
    TEST_ASSERT_EQUAL(10, xfifo_size(p));
    TEST_ASSERT_FALSE(xfifo_empty(p));
    TEST_ASSERT_EQUAL(sizeof(buf) - 1, xfifo_capacity(p));
    TEST_ASSERT_EQUAL(sizeof(buf) - 1 - 10, xfifo_reserve(p));
    TEST_ASSERT_FALSE(xfifo_full(p));


    /* ��ɂ��� */
    xfifo_clear(p);
    TEST_ASSERT_EQUAL(0, xfifo_size(p));
    TEST_ASSERT_TRUE(xfifo_empty(p));
    TEST_ASSERT_EQUAL(sizeof(buf) - 1, xfifo_capacity(p));
    TEST_ASSERT_EQUAL(sizeof(buf) - 1, xfifo_reserve(p));
    TEST_ASSERT_FALSE(xfifo_full(p));


    /* ���^���ɂ��� */
    for (i = 0; i < sizeof(buf) - 1; i++) {
        TEST_ASSERT_FALSE(xfifo_full(p));
        xfifo_push(p, i);
    }
    TEST_ASSERT_EQUAL(sizeof(buf) - 1, xfifo_size(p));
    TEST_ASSERT_FALSE(xfifo_empty(p));
    TEST_ASSERT_EQUAL(sizeof(buf) - 1, xfifo_capacity(p));
    TEST_ASSERT_EQUAL(0, xfifo_reserve(p));
    TEST_ASSERT_TRUE(xfifo_full(p));
}


TEST(xfifo, write)
{
    uint8_t data[10];
    int i;

    /* �������񂾗v�f����Ԃ� */
    for (i = 0; i < sizeof(data); i++)
        data[i] = i;
    TEST_ASSERT_EQUAL(sizeof(data), xfifo_write(p, data, sizeof(data)));

    /* �����Ə������߂Ă�H */
    i = 0;
    while (! xfifo_empty(p)) {
        uint8_t popped = xfifo_pop(p);
        TEST_ASSERT_TRUE((popped < sizeof(data)));
        TEST_ASSERT_TRUE(popped == i);
        i++;
    }

    /* ���^�����Ə������߂Ȃ���� */
    while (! xfifo_full(p))
         xfifo_push(p, 0xFF);
    TEST_ASSERT_EQUAL(0, xfifo_write(p, data, sizeof(data)));

    /* �󂫗v�f��������Ȃ��Ă������镪�������� */
    while (xfifo_reserve(p) < (sizeof(data) / 2))
        xfifo_pop(p);
    TEST_ASSERT_EQUAL(sizeof(data) / 2, xfifo_write(p, data, sizeof(data)));
}


TEST(xfifo, read)
{
    uint8_t w[10];
    uint8_t r[10];
    int i;

    for (i = 0; i < sizeof(w); i++)
        w[i] = i;
    xfifo_write(p, w, sizeof(w));

    /* �ǂݍ��񂾗v�f����Ԃ� */
    TEST_ASSERT_EQUAL(sizeof(w), xfifo_read(p, r, sizeof(r)));

    /* �����Ɠǂ߂Ă�H */
    TEST_ASSERT_TRUE(memcmp(w, r, sizeof(w)) == 0);

    /* �ǂ߂镪�����ǂ� */
    memset(r, 0xFF, sizeof(r));
    memset(w, 0xAA, sizeof(w));
    xfifo_write(p, w, sizeof(w) / 2);
    TEST_ASSERT_EQUAL(sizeof(w) / 2, xfifo_read(p, r, sizeof(r)));
    TEST_ASSERT_TRUE(memcmp(w, r, sizeof(w) / 2) == 0);
    TEST_ASSERT_TRUE(memcmp(w + sizeof(w) / 2, r + sizeof(w) / 2,
                            sizeof(w) / 2) != 0);

    /* �v�f���Ȃ�������ǂ߂�� */
    TEST_ASSERT_TRUE(xfifo_empty(p));
    TEST_ASSERT_EQUAL(0, xfifo_read(p, r, sizeof(r)));
}


TEST_GROUP_RUNNER(xfifo)
{
    RUN_TEST_CASE(xfifo, push_pop);
    RUN_TEST_CASE(xfifo, size);
    RUN_TEST_CASE(xfifo, write);
    RUN_TEST_CASE(xfifo, read);
}
