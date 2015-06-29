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


TEST(xutils, swap)
{
    int ia = 10;
    int ib = 20;
    X_SWAP(ia, ib, int);
    TEST_ASSERT_EQUAL(ia, 20);
    TEST_ASSERT_EQUAL(ib, 10);


    double da = 3.1415;
    double db = 5.1413;

    X_SWAP(da, db, double);
    TEST_ASSERT_EQUAL(da, 5.1413);
    TEST_ASSERT_EQUAL(db, 3.1415);
}


TEST(xutils, constrain)
{
    TEST_ASSERT_EQUAL(X_CONSTRAIN(10, 5, 15), 10);
    TEST_ASSERT_EQUAL(X_CONSTRAIN(4, 5, 15), 5);
    TEST_ASSERT_EQUAL(X_CONSTRAIN(16, 5, 15), 15);
}


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

    TEST_ASSERT_EQUAL(X_PP_CAT(A, B), AB);
    TEST_ASSERT_EQUAL(X_PP_CAT3(A, B, C), ABC);
    TEST_ASSERT_EQUAL(X_PP_CAT4(A, B, C, D), ABCD);
    TEST_ASSERT_EQUAL(X_PP_CAT5(A, B, C, D, E), ABCDE);
}


TEST(xutils, stringize)
{
    const char* hello = "Hello";
    const char* world = "World";

    TEST_ASSERT_EQUAL_STRING(X_PP_STRINGIZE(Hello), hello);
    TEST_ASSERT_EQUAL_STRING(X_PP_STRINGIZE(World), world);
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

    TEST_ASSERT_EQUAL_HEX32(4, X_ROUNDUP_ALIGNMENT(3, 2));
    TEST_ASSERT_EQUAL_HEX32(8, X_ROUNDUP_ALIGNMENT(6, 4));
    TEST_ASSERT_EQUAL_HEX32(24, X_ROUNDUP_ALIGNMENT(19, 8));
    TEST_ASSERT_EQUAL_HEX32(256, X_ROUNDUP_ALIGNMENT(211, 128));
    TEST_ASSERT_EQUAL_HEX32(1024, X_ROUNDUP_ALIGNMENT(1012, 128));

    TEST_ASSERT_EQUAL_HEX32(4, x_roundup_multiple(3, 2));
    TEST_ASSERT_EQUAL_HEX32(8, x_roundup_multiple(6, 4));
    TEST_ASSERT_EQUAL_HEX32(14, x_roundup_multiple(13, 7));
    TEST_ASSERT_EQUAL_HEX32(32, x_roundup_multiple(32, 8));
    TEST_ASSERT_EQUAL_HEX32(444, x_roundup_multiple(345, 111));

    TEST_ASSERT_EQUAL_HEX32(4, x_roundup_alignment(3, 2));
    TEST_ASSERT_EQUAL_HEX32(8, x_roundup_alignment(6, 4));
    TEST_ASSERT_EQUAL_HEX32(24, x_roundup_alignment(19, 8));
    TEST_ASSERT_EQUAL_HEX32(256, x_roundup_alignment(211, 128));
    TEST_ASSERT_EQUAL_HEX32(1024, x_roundup_alignment(1012, 128));
}


TEST(xutils, rounddown_multiple)
{
    TEST_ASSERT_EQUAL_HEX32(2, X_ROUNDDOWN_MULTIPLE(3, 2));
    TEST_ASSERT_EQUAL_HEX32(4, X_ROUNDDOWN_MULTIPLE(6, 4));
    TEST_ASSERT_EQUAL_HEX32(14, X_ROUNDDOWN_MULTIPLE(15, 7));
    TEST_ASSERT_EQUAL_HEX32(32, X_ROUNDDOWN_MULTIPLE(32, 8));
    TEST_ASSERT_EQUAL_HEX32(333, X_ROUNDDOWN_MULTIPLE(345, 111));

    TEST_ASSERT_EQUAL_HEX32(2, X_ROUNDDOWN_ALIGNMENT(3, 2));
    TEST_ASSERT_EQUAL_HEX32(4, X_ROUNDDOWN_ALIGNMENT(6, 4));
    TEST_ASSERT_EQUAL_HEX32(16, X_ROUNDDOWN_ALIGNMENT(19, 8));
    TEST_ASSERT_EQUAL_HEX32(128, X_ROUNDDOWN_ALIGNMENT(211, 128));
    TEST_ASSERT_EQUAL_HEX32(1024, X_ROUNDDOWN_ALIGNMENT(1055, 128));

    TEST_ASSERT_EQUAL_HEX32(2, x_rounddown_multiple(3, 2));
    TEST_ASSERT_EQUAL_HEX32(4, x_rounddown_multiple(6, 4));
    TEST_ASSERT_EQUAL_HEX32(14, x_rounddown_multiple(15, 7));
    TEST_ASSERT_EQUAL_HEX32(32, x_rounddown_multiple(32, 8));
    TEST_ASSERT_EQUAL_HEX32(333, x_rounddown_multiple(345, 111));

    TEST_ASSERT_EQUAL_HEX32(2, x_rounddown_alignment(3, 2));
    TEST_ASSERT_EQUAL_HEX32(4, x_rounddown_alignment(6, 4));
    TEST_ASSERT_EQUAL_HEX32(16, x_rounddown_alignment(19, 8));
    TEST_ASSERT_EQUAL_HEX32(128, x_rounddown_alignment(211, 128));
    TEST_ASSERT_EQUAL_HEX32(1024, x_rounddown_alignment(1055, 128));
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


TEST(xutils, rounddown_power_of_two) {
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

static const unsigned char bit_reverse_table256[] =
    {
      0x00, 0x80, 0x40, 0xC0, 0x20, 0xA0, 0x60, 0xE0, 0x10, 0x90, 0x50, 0xD0, 0x30, 0xB0, 0x70, 0xF0,
      0x08, 0x88, 0x48, 0xC8, 0x28, 0xA8, 0x68, 0xE8, 0x18, 0x98, 0x58, 0xD8, 0x38, 0xB8, 0x78, 0xF8,
      0x04, 0x84, 0x44, 0xC4, 0x24, 0xA4, 0x64, 0xE4, 0x14, 0x94, 0x54, 0xD4, 0x34, 0xB4, 0x74, 0xF4,
      0x0C, 0x8C, 0x4C, 0xCC, 0x2C, 0xAC, 0x6C, 0xEC, 0x1C, 0x9C, 0x5C, 0xDC, 0x3C, 0xBC, 0x7C, 0xFC,
      0x02, 0x82, 0x42, 0xC2, 0x22, 0xA2, 0x62, 0xE2, 0x12, 0x92, 0x52, 0xD2, 0x32, 0xB2, 0x72, 0xF2,
      0x0A, 0x8A, 0x4A, 0xCA, 0x2A, 0xAA, 0x6A, 0xEA, 0x1A, 0x9A, 0x5A, 0xDA, 0x3A, 0xBA, 0x7A, 0xFA,
      0x06, 0x86, 0x46, 0xC6, 0x26, 0xA6, 0x66, 0xE6, 0x16, 0x96, 0x56, 0xD6, 0x36, 0xB6, 0x76, 0xF6,
      0x0E, 0x8E, 0x4E, 0xCE, 0x2E, 0xAE, 0x6E, 0xEE, 0x1E, 0x9E, 0x5E, 0xDE, 0x3E, 0xBE, 0x7E, 0xFE,
      0x01, 0x81, 0x41, 0xC1, 0x21, 0xA1, 0x61, 0xE1, 0x11, 0x91, 0x51, 0xD1, 0x31, 0xB1, 0x71, 0xF1,
      0x09, 0x89, 0x49, 0xC9, 0x29, 0xA9, 0x69, 0xE9, 0x19, 0x99, 0x59, 0xD9, 0x39, 0xB9, 0x79, 0xF9,
      0x05, 0x85, 0x45, 0xC5, 0x25, 0xA5, 0x65, 0xE5, 0x15, 0x95, 0x55, 0xD5, 0x35, 0xB5, 0x75, 0xF5,
      0x0D, 0x8D, 0x4D, 0xCD, 0x2D, 0xAD, 0x6D, 0xED, 0x1D, 0x9D, 0x5D, 0xDD, 0x3D, 0xBD, 0x7D, 0xFD,
      0x03, 0x83, 0x43, 0xC3, 0x23, 0xA3, 0x63, 0xE3, 0x13, 0x93, 0x53, 0xD3, 0x33, 0xB3, 0x73, 0xF3,
      0x0B, 0x8B, 0x4B, 0xCB, 0x2B, 0xAB, 0x6B, 0xEB, 0x1B, 0x9B, 0x5B, 0xDB, 0x3B, 0xBB, 0x7B, 0xFB,
      0x07, 0x87, 0x47, 0xC7, 0x27, 0xA7, 0x67, 0xE7, 0x17, 0x97, 0x57, 0xD7, 0x37, 0xB7, 0x77, 0xF7,
      0x0F, 0x8F, 0x4F, 0xCF, 0x2F, 0xAF, 0x6F, 0xEF, 0x1F, 0x9F, 0x5F, 0xDF, 0x3F, 0xBF, 0x7F, 0xFF
    };


static uint8_t reverse_bits8(uint8_t x)
{
    return bit_reverse_table256[x];
}


static uint16_t reverse_bits16(uint16_t x)
{
    return (bit_reverse_table256[x & 0xFF] << 8) |
           (bit_reverse_table256[(x >> 8) & 0xFF]);
}


static uint32_t reverse_bits32(uint32_t x)
{
    return (bit_reverse_table256[x & 0xff] << 24)           |
           (bit_reverse_table256[(x >> 8) & 0xff]  << 16)   |
           (bit_reverse_table256[(x >> 16) & 0xff] << 8)    |
           (bit_reverse_table256[(x >> 24) & 0xff]);
}


TEST(xutils, reverse_bits)
{
    {
        uint8_t i = 0;
        do
        {
            TEST_ASSERT_EQUAL_HEX8(reverse_bits8(i), x_reverse_bits8(i));
        } while (i++ < 0xFF);
    }

#if 0
    {
        uint16_t i;
        do
        {
            TEST_ASSERT_EQUAL_HEX16(reverse_bits16(i), x_reverse_bits16(i));
        } while (i++ < 0xFFFF);
    }

    {
        uint32_t i;
        do
        {
            TEST_ASSERT_EQUAL_HEX32(reverse_bits32(i), x_reverse_bits32(i));
        } while (i++ < 0xFFFFFFFF);
    }
#endif
}


TEST(xutils, load)
{
    uint8_t big[] = {0x00, 0x00, 0x00, 0xDE, 0xAD, 0xBE, 0xEF};
    uint8_t lit[] = {0x00, 0x00, 0x00, 0xEF, 0xBE, 0xAD, 0xDE};
    uint8_t buf[sizeof(big)];
    uint8_t* p = big + 3;

    TEST_ASSERT_EQUAL_HEX8(0xDE, X_LOAD_U8(p));
    TEST_ASSERT_EQUAL_HEX16(0xDEAD, X_LOAD_U16_BIG(p));
    TEST_ASSERT_EQUAL_HEX32(0xDEADBEEF, X_LOAD_U32_BIG(p));

    TEST_ASSERT_EQUAL_HEX8(0xDE, X_LOAD_U8((uint16_t*)p));
    TEST_ASSERT_EQUAL_HEX16(0xDEAD, X_LOAD_U16_BIG((uint16_t*)p));
    TEST_ASSERT_EQUAL_HEX32(0xDEADBEEF, X_LOAD_U32_BIG((uint16_t*)p));

    TEST_ASSERT_EQUAL_HEX8(0xDE, X_LOAD_U8((uint32_t*)p));
    TEST_ASSERT_EQUAL_HEX16(0xDEAD, X_LOAD_U16_BIG((uint32_t*)p));
    TEST_ASSERT_EQUAL_HEX32(0xDEADBEEF, X_LOAD_U32_BIG((uint32_t*)p));

    p = lit + 3;
    TEST_ASSERT_EQUAL_HEX16(0xBEEF, X_LOAD_U16_LIT(p));
    TEST_ASSERT_EQUAL_HEX32(0xDEADBEEF, X_LOAD_U32_LIT(p));

    TEST_ASSERT_EQUAL_HEX16(0xBEEF, X_LOAD_U16_LIT((uint16_t*)p));
    TEST_ASSERT_EQUAL_HEX32(0xDEADBEEF, X_LOAD_U32_LIT((uint16_t*)p));

    TEST_ASSERT_EQUAL_HEX16(0xBEEF, X_LOAD_U32_LIT((uint32_t*)p));
    TEST_ASSERT_EQUAL_HEX32(0xDEADBEEF, X_LOAD_U32_LIT((uint32_t*)p));

    memset(buf, 0, sizeof(buf));
    p = buf + 3;
    X_STORE_U8(p, 0xDE);
    TEST_ASSERT_EQUAL_HEX8(0xDE, X_LOAD_U8(p));
    X_STORE_U16_BIG(p, 0xDEAD);
    TEST_ASSERT_EQUAL_HEX16(0xDEAD, X_LOAD_U16_BIG(p));
    X_STORE_U32_BIG(p, 0xDEADBEEF);
    TEST_ASSERT_EQUAL_HEX32(0xDEADBEEF, X_LOAD_U32_BIG(p));
    X_STORE_U16_BIG((uint16_t*)p, 0xDEAD);
    TEST_ASSERT_EQUAL_HEX16(0xDEAD, X_LOAD_U16_BIG((uint16_t*)p));
    X_STORE_U32_BIG((uint16_t*)p, 0xDEADBEEF);
    TEST_ASSERT_EQUAL_HEX32(0xDEADBEEF, X_LOAD_U32_BIG((uint16_t*)p));
    X_STORE_U32_BIG((uint32_t*)p, 0xDEAD);
    TEST_ASSERT_EQUAL_HEX32(0xDEAD, X_LOAD_U32_BIG((uint32_t*)p));
    X_STORE_U32_BIG((uint32_t*)p, 0xDEADBEEF);
    TEST_ASSERT_EQUAL_HEX32(0xDEADBEEF, X_LOAD_U32_BIG((uint32_t*)p));

    memset(buf, 0, sizeof(buf));
    p = buf + 3;
    X_STORE_U8(p, 0xDE);
    TEST_ASSERT_EQUAL_HEX8(0xDE, X_LOAD_U8(p));
    X_STORE_U16_LIT(p, 0xDEAD);
    TEST_ASSERT_EQUAL_HEX16(0xDEAD, X_LOAD_U16_LIT(p));
    X_STORE_U32_LIT(p, 0xDEADBEEF);
    TEST_ASSERT_EQUAL_HEX32(0xDEADBEEF, X_LOAD_U32_LIT(p));
    X_STORE_U16_LIT((uint16_t*)p, 0xDEAD);
    TEST_ASSERT_EQUAL_HEX16(0xDEAD, X_LOAD_U16_LIT((uint16_t*)p));
    X_STORE_U32_LIT((uint16_t*)p, 0xDEADBEEF);
    TEST_ASSERT_EQUAL_HEX32(0xDEADBEEF, X_LOAD_U32_LIT((uint16_t*)p));
    X_STORE_U32_LIT((uint32_t*)p, 0xDEAD);
    TEST_ASSERT_EQUAL_HEX32(0xDEAD, X_LOAD_U32_LIT((uint32_t*)p));
    X_STORE_U32_LIT((uint32_t*)p, 0xDEADBEEF);
    TEST_ASSERT_EQUAL_HEX32(0xDEADBEEF, X_LOAD_U32_LIT((uint32_t*)p));
}


TEST(xutils, store)
{
    uint8_t big[] = {0x00, 0x00, 0x00, 0xDE, 0xAD, 0xBE, 0xEF};
    uint8_t buf[sizeof(big)];
    uint8_t* p;

    p = buf + 3;
    memset(buf, 0, sizeof(buf));
    X_STORE_U8(p, 0xDE);
    TEST_ASSERT_EQUAL_HEX8(0xDE, X_LOAD_U8(p));
    X_STORE_U16_BIG(p, 0xDEAD);
    TEST_ASSERT_EQUAL_HEX16(0xDEAD, X_LOAD_U16_BIG(p));
    X_STORE_U32_BIG(p, 0xDEADBEEF);
    TEST_ASSERT_EQUAL_HEX32(0xDEADBEEF, X_LOAD_U32_BIG(p));

    p = buf + 3;
    memset(buf, 0, sizeof(buf));
    X_STORE_U8(p, 0xDE);
    TEST_ASSERT_EQUAL_HEX8(0xDE, X_LOAD_U8(p));
    X_STORE_U16_LIT(p, 0xDEAD);
    TEST_ASSERT_EQUAL_HEX16(0xDEAD, X_LOAD_U16_LIT(p));
    X_STORE_U32_LIT(p, 0xDEADBEEF);
    TEST_ASSERT_EQUAL_HEX32(0xDEADBEEF, X_LOAD_U32_LIT(p));
}


TEST(xutils, distance_ptr)
{
    TEST_ASSERT_EQUAL_HEX32(5, x_distance_ptr((void*)10, (void*)15));
    TEST_ASSERT_EQUAL_HEX32(0x0F, x_distance_ptr((void*)0xFFFFFFF0, (void*)0xFFFFFFFF));
}


TEST(xutils, is_alignment)
{
    int i;
    uint32_t x = 1;

    for (i = 0; i < 32; i++)
    {
        TEST_ASSERT_TRUE(x_is_alignment((void*)x));
        TEST_ASSERT_FALSE(x_is_alignment((void*)(x + 5)));
        x = x_roundup_power_of_two(x + 1);
    }
}


TEST(xutils, is_aligned)
{
    int i;
    uint32_t x = 4;

    for (i = 0; i < 100; i++)
    {
        TEST_ASSERT_TRUE(x_is_aligned((void*)x, 4));
        x = x_roundup_alignment(x + 1, 4);
    }
}

TEST(xutils, is_within)
{
    const int32_t b = -50;
    const int32_t e = 1000;
    TEST_ASSERT_TRUE(x_is_within(b, b, e));
    TEST_ASSERT_TRUE(x_is_within(b + 1, b, e));
    TEST_ASSERT_TRUE(x_is_within(b + 2, b, e));
    TEST_ASSERT_TRUE(x_is_within(e - 1, b, e));
    TEST_ASSERT_TRUE(x_is_within(e - 2, b, e));

    TEST_ASSERT_FALSE(x_is_within(b - 1, b, e));
    TEST_ASSERT_FALSE(x_is_within(e, b, e));
    TEST_ASSERT_FALSE(x_is_within(e + 1, b, e));
}

TEST(xutils, is_uwithin)
{
    const uint32_t b = 50;
    const uint32_t e = 1000;
    TEST_ASSERT_TRUE(x_is_uwithin(b, b, e));
    TEST_ASSERT_TRUE(x_is_uwithin(b + 1, b, e));
    TEST_ASSERT_TRUE(x_is_uwithin(b + 2, b, e));
    TEST_ASSERT_TRUE(x_is_uwithin(e - 1, b, e));
    TEST_ASSERT_TRUE(x_is_uwithin(e - 2, b, e));

    TEST_ASSERT_FALSE(x_is_uwithin(b - 1, b, e));
    TEST_ASSERT_FALSE(x_is_uwithin(e, b, e));
    TEST_ASSERT_FALSE(x_is_uwithin(e + 1, b, e));
}


TEST(xutils, is_within_ptr)
{
    const uint8_t* b = (void*)50;
    const uint8_t* e = (void*)1000;
    TEST_ASSERT_TRUE(x_is_within_ptr(b, b, e));
    TEST_ASSERT_TRUE(x_is_within_ptr(b + 1, b, e));
    TEST_ASSERT_TRUE(x_is_within_ptr(b + 2, b, e));
    TEST_ASSERT_TRUE(x_is_within_ptr(e - 1, b, e));
    TEST_ASSERT_TRUE(x_is_within_ptr(e - 2, b, e));

    TEST_ASSERT_FALSE(x_is_within_ptr(b - 1, b, e));
    TEST_ASSERT_FALSE(x_is_within_ptr(e, b, e));
    TEST_ASSERT_FALSE(x_is_within_ptr(e + 1, b, e));
}


TEST(xutils, find_lsb)
{
    int i;
    uint32_t x;

    x = 0xff;

    for (i = 0; i < 8; i++)
    {
        TEST_ASSERT_EQUAL(i, x_find_lsb_pos8(x));
        TEST_ASSERT_EQUAL(1UL << i, x_find_lsb8(x));
        x &= ~(1UL << i);
    }

    x = 0xffff;
    for (i = 0; i < 16; i++)
    {
        TEST_ASSERT_EQUAL(i, x_find_lsb_pos16(x));
        TEST_ASSERT_EQUAL(1UL << i, x_find_lsb16(x));
        x &= ~(1UL << i);
    }

    x = 0xffffffff;
    for (i = 0; i < 32; i++)
    {
        TEST_ASSERT_EQUAL(i, x_find_lsb_pos32(x));
        TEST_ASSERT_EQUAL(1UL << i, x_find_lsb32(x));
        x &= ~(1UL << i);
    }
}


TEST(xutils, find_msb)
{
    int i;
    uint32_t x;

    x = 0xff;

    for (i = 7; i >= 0; i--)
    {
        TEST_ASSERT_EQUAL(i, x_find_msb_pos8(x));
        TEST_ASSERT_EQUAL(1UL << i, x_find_msb8(x));
        x &= ~(1UL << i);
    }

    x = 0xffff;
    for (i = 15; i >= 0; i--)
    {
        TEST_ASSERT_EQUAL(i, x_find_msb_pos16(x));
        TEST_ASSERT_EQUAL(1UL << i, x_find_msb16(x));
        x &= ~(1UL << i);
    }

    x = 0xffffffff;
    for (i = 31; i >= 0; i--)
    {
        TEST_ASSERT_EQUAL(i, x_find_msb_pos32(x));
        TEST_ASSERT_EQUAL(1UL << i, x_find_msb32(x));
        x &= ~(1UL << i);
    }
}


TEST(xutils, count_bits)
{
    int i;
    uint32_t x;

    x = 0;
    for (i = 0; i < 8; i++)
    {
        TEST_ASSERT_EQUAL(i, x_count_bits8(x));
        x |= 1UL << i;
    }

    x = 0;
    for (i = 0; i < 16; i++)
    {
        TEST_ASSERT_EQUAL(i, x_count_bits16(x));
        x |= 1UL << i;
    }

    x = 0;
    for (i = 0; i < 32; i++)
    {
        TEST_ASSERT_EQUAL(i, x_count_bits32(x));
        x |= 1UL << i;
    }
}


TEST(xutils, reverse_bytes)
{
    {
        uint32_t x = 0xDEAD;

        x_reverse_2byte(&x);
        TEST_ASSERT_EQUAL_HEX16(0xADDE, x);
    }

    {
        uint32_t x = 0xDEADBEEF;
        x_reverse_4byte(&x);
        TEST_ASSERT_EQUAL_HEX32(0xEFBEADDE, x);
    }
}


TEST(xutils, byteorder)
{
    {
        uint16_t x = 0xDEAD;
        TEST_ASSERT_EQUAL_HEX16(0xADDE, X_REVERSE_ENDIAN16(x));
        TEST_ASSERT_EQUAL_HEX16(0xADDE, x_reverse_endian16(x));

        uint16_t xx = x_host_to_big16(x);
        TEST_ASSERT_EQUAL_HEX16(x, X_LOAD_U16_BIG(&xx));

        xx = x_host_to_little16(x);
        TEST_ASSERT_EQUAL_HEX16(x, X_LOAD_U16_LIT(&xx));
    }

    {
        uint32_t x = 0xDEADBEEF;
        TEST_ASSERT_EQUAL_HEX32(0xEFBEADDE, X_REVERSE_ENDIAN32(x));
        TEST_ASSERT_EQUAL_HEX32(0xEFBEADDE, x_reverse_endian32(x));

        uint32_t xx = x_host_to_big32(x);
        TEST_ASSERT_EQUAL_HEX32(x, X_LOAD_U32_BIG(&xx));

        xx = x_host_to_little32(x);
        TEST_ASSERT_EQUAL_HEX32(x, X_LOAD_U32_LIT(&xx));
    }
}


TEST(xutils, memswap)
{
    char hello[] = "Hello";
    char world[] = "World";

    x_memswap(hello, world, sizeof(hello));
    TEST_ASSERT_EQUAL_STRING(hello, "World");
    TEST_ASSERT_EQUAL_STRING(world, "Hello");
}


TEST(xutils, memreverse)
{
    int a[10];
    int b[10];

    int i;
    for (i = 0; i < X_COUNT_OF(a); i++)
    {
        a[i] = i;
        b[X_COUNT_OF(a) - 1 - i] = i;
    }

    x_memreverse(a, sizeof(a[0]), X_COUNT_OF(a));
    TEST_ASSERT_EQUAL_MEMORY_ARRAY(a, b, sizeof(a[0]), X_COUNT_OF(a));
}


TEST(xutils, memrotate_right)
{
    int a[5] = {0, 1, 2, 3, 4};
    int b[5] = {3, 4, 0, 1, 2};

    x_memrotate_right(a, 2, sizeof(a[0]), X_COUNT_OF(a));
    TEST_ASSERT_EQUAL_MEMORY_ARRAY(a, b, sizeof(a[0]), X_COUNT_OF(a));
}


TEST(xutils, memrotate_left)
{
    int a[5] = {0, 1, 2, 3, 4};
    int b[5] = {2, 3, 4, 0, 1};

    x_memrotate_left(a, 2, sizeof(a[0]), X_COUNT_OF(a));
    TEST_ASSERT_EQUAL_MEMORY_ARRAY(a, b, sizeof(a[0]), X_COUNT_OF(a));
}


TEST(xutils, memblt)
{
    int dst[5][4] = { {0} };
    int src[5][5] = {
        {1, 2, 3, 0, 0},
        {4, 5, 6, 0, 0},
        {7, 8, 9, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0}};

    int expected[5][4] = {
        { 0, 0, 0, 0},
        { 0, 1, 2, 3},
        { 0, 4, 5, 6},
        { 0, 7, 8, 9},
        { 0, 0, 0, 0}};

    x_memblt(((uint8_t*)dst) + (sizeof(int) * X_COUNT_OF_COL(dst) * 1 + sizeof(int)),
             src,
             sizeof(int) * 3,                       // linesize
             3,                                     // height
             sizeof(int) * X_COUNT_OF_COL(dst),     // dstride
             sizeof(int) * X_COUNT_OF_COL(src));    // sstride

    TEST_ASSERT_EQUAL_MEMORY_ARRAY(expected, dst, sizeof(int), X_COUNT_OF_2D(dst));
}


TEST_GROUP_RUNNER(xutils)
{
    RUN_TEST_CASE(xutils, sizeof_mem);

#ifdef __GNUC__
    RUN_TEST_CASE(xutils, align_of);
#endif

    RUN_TEST_CASE(xutils, swap);
    RUN_TEST_CASE(xutils, constrain);
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
    RUN_TEST_CASE(xutils, reverse_bits);
    RUN_TEST_CASE(xutils, load);
    RUN_TEST_CASE(xutils, store);
    RUN_TEST_CASE(xutils, distance_ptr);
    RUN_TEST_CASE(xutils, is_alignment);
    RUN_TEST_CASE(xutils, is_aligned);
    RUN_TEST_CASE(xutils, is_within);
    RUN_TEST_CASE(xutils, is_uwithin);
    RUN_TEST_CASE(xutils, is_within_ptr);
    RUN_TEST_CASE(xutils, find_lsb);
    RUN_TEST_CASE(xutils, find_msb);
    RUN_TEST_CASE(xutils, count_bits);
    RUN_TEST_CASE(xutils, reverse_bytes);
    RUN_TEST_CASE(xutils, byteorder);
    RUN_TEST_CASE(xutils, memswap);
    RUN_TEST_CASE(xutils, memreverse);
    RUN_TEST_CASE(xutils, memrotate_right);
    RUN_TEST_CASE(xutils, memrotate_left);
    RUN_TEST_CASE(xutils, memblt);
}
