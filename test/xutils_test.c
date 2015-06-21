#include <picox/core/xcore.h>
#include <unity.h>
#include <unity_fixture.h>
#include <stdio.h>


TEST_GROUP(xutils);


TEST_SETUP(xutils)
{
}


TEST_TEAR_DOWN(xutils)
{
}


TEST(xutils, sizeof_mem)
{
    typedef struct Foo
    {
        uint8_t  u8;
        uint16_t u16;
        uint32_t u32;
    } Foo;

    Foo foo;
    TEST_ASSERT_EQUAL(sizeof(foo.u8), X_SIZEOF_MEM(Foo, u8));
    TEST_ASSERT_EQUAL(sizeof(foo.u16), X_SIZEOF_MEM(Foo, u16));
    TEST_ASSERT_EQUAL(sizeof(foo.u32), X_SIZEOF_MEM(Foo, u32));
}


#ifdef __GNUC__
TEST(xutils, align_of)
{
    TEST_ASSERT_EQUAL(__alignof__(char), X_ALIGN_OF(char));
    TEST_ASSERT_EQUAL(__alignof__(short), X_ALIGN_OF(short));
    TEST_ASSERT_EQUAL(__alignof__(int), X_ALIGN_OF(int));
    TEST_ASSERT_EQUAL(__alignof__(long), X_ALIGN_OF(long));
    TEST_ASSERT_EQUAL(__alignof__(long long), X_ALIGN_OF(long long));
    TEST_ASSERT_EQUAL(__alignof__(float), X_ALIGN_OF(float));
    TEST_ASSERT_EQUAL(__alignof__(double), X_ALIGN_OF(double));

}
#endif

TEST(xutils, container_of)
{
    typedef struct Foo
    {
        int          a;
        int          b;
    } Foo;

    Foo foo;
    TEST_ASSERT_EQUAL_PTR(&foo, X_CONTAINER_OF(&foo.b, Foo, b));
}


TEST(xutils, cat)
{
    const int A = 5;
    const int B = 4;
    const int C = 3;
    const int D = 2;
    const int E = 1;
    const int AB = A + B;
    const int ABC = AB + C;
    const int ABCD = ABC + D;
    const int ABCDE = ABCD + E;

    TEST_ASSERT_EQUAL(X_CAT(A, B), AB);
    TEST_ASSERT_EQUAL(X_CAT3(A, B, C), ABC);
    TEST_ASSERT_EQUAL(X_CAT4(A, B, C, D), ABCD);
    TEST_ASSERT_EQUAL(X_CAT5(A, B, C, D, E), ABCDE);
}


TEST(xutils, stringize)
{
    const char* hello = "Hello";
    const char* world = "World";

    TEST_ASSERT_EQUAL_STRING(X_STRINGIZE(Hello), hello);
    TEST_ASSERT_EQUAL_STRING(X_STRINGIZE(World), world);
}


TEST(xutils, bit)
{
    uint32_t bit = 1;
    int i;

    for (i = 0; i < 32; i++)
    {
        TEST_ASSERT_EQUAL_UINT32(bit, X_BIT(i));
        bit <<= 1;
    }
}


TEST(xutils, min)
{
    TEST_ASSERT_EQUAL(0, X_MIN(0, 1));
    TEST_ASSERT_EQUAL(-1, X_MIN(-1, 0));
    TEST_ASSERT_EQUAL(999, X_MIN(999, 1000));
}


TEST(xutils, max)
{
    TEST_ASSERT_EQUAL(1, X_MAX(0, 1));
    TEST_ASSERT_EQUAL(0, X_MAX(-1, 0));
    TEST_ASSERT_EQUAL(1000, X_MAX(999, 1000));
}


TEST(xutils, count_of)
{
    int a[10];
    int ab[10][20];

    TEST_ASSERT_EQUAL(10, X_COUNT_OF(a));
    TEST_ASSERT_EQUAL(10, X_COUNT_OF_ROW(ab));
    TEST_ASSERT_EQUAL(20, X_COUNT_OF_COL(ab));
    TEST_ASSERT_EQUAL(200, X_COUNT_OF_2D(ab));
}


TEST(xutils, roundup_multiple)
{
    TEST_ASSERT_EQUAL_HEX32(4, X_ROUNDUP_MULTIPLE(3, 2));
    TEST_ASSERT_EQUAL_HEX32(8, X_ROUNDUP_MULTIPLE(6, 4));
    TEST_ASSERT_EQUAL_HEX32(14, X_ROUNDUP_MULTIPLE(13, 7));
    TEST_ASSERT_EQUAL_HEX32(32, X_ROUNDUP_MULTIPLE(32, 8));
    TEST_ASSERT_EQUAL_HEX32(444, X_ROUNDUP_MULTIPLE(345, 111));

    TEST_ASSERT_EQUAL_HEX32(4, X_ROUNDUP_MULTIPLE_WHEN_POWER_OF_TWO(3, 2));
    TEST_ASSERT_EQUAL_HEX32(8, X_ROUNDUP_MULTIPLE_WHEN_POWER_OF_TWO(6, 4));
    TEST_ASSERT_EQUAL_HEX32(24, X_ROUNDUP_MULTIPLE_WHEN_POWER_OF_TWO(19, 8));
    TEST_ASSERT_EQUAL_HEX32(256, X_ROUNDUP_MULTIPLE_WHEN_POWER_OF_TWO(211, 128));
    TEST_ASSERT_EQUAL_HEX32(1024, X_ROUNDUP_MULTIPLE_WHEN_POWER_OF_TWO(1012, 128));

    TEST_ASSERT_EQUAL_HEX32(4, x_roundup_multiple(3, 2));
    TEST_ASSERT_EQUAL_HEX32(8, x_roundup_multiple(6, 4));
    TEST_ASSERT_EQUAL_HEX32(14, x_roundup_multiple(13, 7));
    TEST_ASSERT_EQUAL_HEX32(32, x_roundup_multiple(32, 8));
    TEST_ASSERT_EQUAL_HEX32(444, x_roundup_multiple(345, 111));

    TEST_ASSERT_EQUAL_HEX32(4, x_roundup_multiple_when_power_of_two(3, 2));
    TEST_ASSERT_EQUAL_HEX32(8, x_roundup_multiple_when_power_of_two(6, 4));
    TEST_ASSERT_EQUAL_HEX32(24, x_roundup_multiple_when_power_of_two(19, 8));
    TEST_ASSERT_EQUAL_HEX32(256, x_roundup_multiple_when_power_of_two(211, 128));
    TEST_ASSERT_EQUAL_HEX32(1024, x_roundup_multiple_when_power_of_two(1012, 128));
}


TEST(xutils, rounddown_multiple)
{
    TEST_ASSERT_EQUAL_HEX32(2, X_ROUNDDOWN_MULTIPLE(3, 2));
    TEST_ASSERT_EQUAL_HEX32(4, X_ROUNDDOWN_MULTIPLE(6, 4));
    TEST_ASSERT_EQUAL_HEX32(14, X_ROUNDDOWN_MULTIPLE(15, 7));
    TEST_ASSERT_EQUAL_HEX32(32, X_ROUNDDOWN_MULTIPLE(32, 8));
    TEST_ASSERT_EQUAL_HEX32(333, X_ROUNDDOWN_MULTIPLE(345, 111));

    TEST_ASSERT_EQUAL_HEX32(2, X_ROUNDDOWN_MULTIPLE_WHEN_POWER_OF_TWO(3, 2));
    TEST_ASSERT_EQUAL_HEX32(4, X_ROUNDDOWN_MULTIPLE_WHEN_POWER_OF_TWO(6, 4));
    TEST_ASSERT_EQUAL_HEX32(16, X_ROUNDDOWN_MULTIPLE_WHEN_POWER_OF_TWO(19, 8));
    TEST_ASSERT_EQUAL_HEX32(128, X_ROUNDDOWN_MULTIPLE_WHEN_POWER_OF_TWO(211, 128));
    TEST_ASSERT_EQUAL_HEX32(1024, X_ROUNDDOWN_MULTIPLE_WHEN_POWER_OF_TWO(1055, 128));

    TEST_ASSERT_EQUAL_HEX32(2, x_rounddown_multiple(3, 2));
    TEST_ASSERT_EQUAL_HEX32(4, x_rounddown_multiple(6, 4));
    TEST_ASSERT_EQUAL_HEX32(14, x_rounddown_multiple(15, 7));
    TEST_ASSERT_EQUAL_HEX32(32, x_rounddown_multiple(32, 8));
    TEST_ASSERT_EQUAL_HEX32(333, x_rounddown_multiple(345, 111));

    TEST_ASSERT_EQUAL_HEX32(2, x_rounddown_multiple_when_power_of_two(3, 2));
    TEST_ASSERT_EQUAL_HEX32(4, x_rounddown_multiple_when_power_of_two(6, 4));
    TEST_ASSERT_EQUAL_HEX32(16, x_rounddown_multiple_when_power_of_two(19, 8));
    TEST_ASSERT_EQUAL_HEX32(128, x_rounddown_multiple_when_power_of_two(211, 128));
    TEST_ASSERT_EQUAL_HEX32(1024, x_rounddown_multiple_when_power_of_two(1055, 128));
}


TEST(xutils, roundup_power_of_two)
{
    TEST_ASSERT_EQUAL_HEX32(4, X_ROUNDUP_POWER_OF_TWO(3));
    TEST_ASSERT_EQUAL_HEX32(4, X_ROUNDUP_POWER_OF_TWO(4));
    TEST_ASSERT_EQUAL_HEX32(8, X_ROUNDUP_POWER_OF_TWO(5));
    TEST_ASSERT_EQUAL_HEX32(16, X_ROUNDUP_POWER_OF_TWO(9));
    TEST_ASSERT_EQUAL_HEX32(32, X_ROUNDUP_POWER_OF_TWO(17));
    TEST_ASSERT_EQUAL_HEX32(64, X_ROUNDUP_POWER_OF_TWO(33));
    TEST_ASSERT_EQUAL_HEX32(128, X_ROUNDUP_POWER_OF_TWO(127));
    TEST_ASSERT_EQUAL_HEX32(256, X_ROUNDUP_POWER_OF_TWO(129));

    TEST_ASSERT_EQUAL_HEX32(4, x_roundup_power_of_two(3));
    TEST_ASSERT_EQUAL_HEX32(4, x_roundup_power_of_two(4));
    TEST_ASSERT_EQUAL_HEX32(8, x_roundup_power_of_two(5));
    TEST_ASSERT_EQUAL_HEX32(16, x_roundup_power_of_two(9));
    TEST_ASSERT_EQUAL_HEX32(32, x_roundup_power_of_two(17));
    TEST_ASSERT_EQUAL_HEX32(64, x_roundup_power_of_two(33));
    TEST_ASSERT_EQUAL_HEX32(128, x_roundup_power_of_two(127));
    TEST_ASSERT_EQUAL_HEX32(256, x_roundup_power_of_two(129));
}


TEST(xutils, rounddown_power_of_two)
{
    TEST_ASSERT_EQUAL_HEX32(2, X_ROUNDDOWN_POWER_OF_TWO(3));
    TEST_ASSERT_EQUAL_HEX32(4, X_ROUNDDOWN_POWER_OF_TWO(4));
    TEST_ASSERT_EQUAL_HEX32(4, X_ROUNDDOWN_POWER_OF_TWO(5));
    TEST_ASSERT_EQUAL_HEX32(8, X_ROUNDDOWN_POWER_OF_TWO(9));
    TEST_ASSERT_EQUAL_HEX32(16, X_ROUNDDOWN_POWER_OF_TWO(17));
    TEST_ASSERT_EQUAL_HEX32(32, X_ROUNDDOWN_POWER_OF_TWO(33));
    TEST_ASSERT_EQUAL_HEX32(64, X_ROUNDDOWN_POWER_OF_TWO(127));
    TEST_ASSERT_EQUAL_HEX32(128, X_ROUNDDOWN_POWER_OF_TWO(129));
    TEST_ASSERT_EQUAL_HEX32(256, X_ROUNDDOWN_POWER_OF_TWO(299));

    TEST_ASSERT_EQUAL_HEX32(2, x_rounddown_power_of_two(3));
    TEST_ASSERT_EQUAL_HEX32(4, x_rounddown_power_of_two(4));
    TEST_ASSERT_EQUAL_HEX32(4, x_rounddown_power_of_two(5));
    TEST_ASSERT_EQUAL_HEX32(8, x_rounddown_power_of_two(9));
    TEST_ASSERT_EQUAL_HEX32(16, x_rounddown_power_of_two(17));
    TEST_ASSERT_EQUAL_HEX32(32, x_rounddown_power_of_two(33));
    TEST_ASSERT_EQUAL_HEX32(64, x_rounddown_power_of_two(127));
    TEST_ASSERT_EQUAL_HEX32(128, x_rounddown_power_of_two(129));
    TEST_ASSERT_EQUAL_HEX32(256, x_rounddown_power_of_two(299));
}


TEST(xutils, is_multiple)
{
    TEST_ASSERT_TRUE(X_IS_MULTIPLE(2, 2));
    TEST_ASSERT_FALSE(X_IS_MULTIPLE(3, 2));
    TEST_ASSERT_TRUE(X_IS_MULTIPLE(4, 2));
    TEST_ASSERT_TRUE(X_IS_MULTIPLE(1024, 2));
    TEST_ASSERT_FALSE(X_IS_MULTIPLE(1025, 2));

    TEST_ASSERT_TRUE(x_is_multiple(2, 2));
    TEST_ASSERT_FALSE(x_is_multiple(3, 2));
    TEST_ASSERT_TRUE(x_is_multiple(4, 2));
    TEST_ASSERT_TRUE(x_is_multiple(1024, 2));
    TEST_ASSERT_FALSE(x_is_multiple(1025, 2));
}


TEST(xutils, is_power_of_two)
{
    TEST_ASSERT_TRUE(X_IS_POWER_OF_TWO(2));
    TEST_ASSERT_FALSE(X_IS_POWER_OF_TWO(7));
    TEST_ASSERT_TRUE(X_IS_POWER_OF_TWO(4));
    TEST_ASSERT_TRUE(X_IS_POWER_OF_TWO(1024));
    TEST_ASSERT_FALSE(X_IS_POWER_OF_TWO(1025));

    TEST_ASSERT_TRUE(x_is_power_of_two(2));
    TEST_ASSERT_FALSE(x_is_power_of_two(7));
    TEST_ASSERT_TRUE(x_is_power_of_two(4));
    TEST_ASSERT_TRUE(x_is_power_of_two(1024));
    TEST_ASSERT_FALSE(x_is_power_of_two(1025));
}


TEST(xutils, high_low)
{
    TEST_ASSERT_EQUAL_HEX32(0xDEAD, X_HIGH_WORD(0xDEADBEEF));
    TEST_ASSERT_EQUAL_HEX32(0xBEEF, X_LOW_WORD(0xDEADBEEF));
    TEST_ASSERT_EQUAL_HEX32(0xBE, X_HIGH_BYTE(0xDEADBEEF));
    TEST_ASSERT_EQUAL_HEX32(0xEF, X_LOW_BYTE(0xDEADBEEF));
    TEST_ASSERT_EQUAL_HEX32(0xE, X_HIGH_NIBBLE(0xDEADBEEF));
    TEST_ASSERT_EQUAL_HEX32(0xF, X_LOW_NIBBLE(0xDEADBEEF));
}


TEST_GROUP_RUNNER(xutils)
{
    RUN_TEST_CASE(xutils, sizeof_mem);

#ifdef __GNUC__
    RUN_TEST_CASE(xutils, align_of);
#endif

    RUN_TEST_CASE(xutils, container_of);
    RUN_TEST_CASE(xutils, cat);
    RUN_TEST_CASE(xutils, stringize);
    RUN_TEST_CASE(xutils, bit);
    RUN_TEST_CASE(xutils, min);
    RUN_TEST_CASE(xutils, max);
    RUN_TEST_CASE(xutils, count_of);
    RUN_TEST_CASE(xutils, roundup_multiple);
    RUN_TEST_CASE(xutils, rounddown_multiple);
    RUN_TEST_CASE(xutils, roundup_power_of_two);
    RUN_TEST_CASE(xutils, rounddown_power_of_two);
    RUN_TEST_CASE(xutils, is_multiple);
    RUN_TEST_CASE(xutils, is_power_of_two);
    RUN_TEST_CASE(xutils, high_low);
}



