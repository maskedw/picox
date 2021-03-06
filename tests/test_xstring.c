#include <picox/core/xcore.h>
#include "testutils.h"


TEST_GROUP(xstring);


TEST_SETUP(xstring)
{
}


TEST_TEAR_DOWN(xstring)
{
}


static void* X__GetMem(size_t size)
{
    return x_malloc(size);
}


static void X__FreeMem(void* ptr)
{
    x_free(ptr);
}


TEST(xstring, equal)
{
    const char* expected = "Hello";
    TEST_ASSERT_TRUE(x_strequal("Hello", expected));
    TEST_ASSERT_FALSE(x_strequal("hello", expected));
    TEST_ASSERT_FALSE(x_strequal("Hell", expected));
    TEST_ASSERT_FALSE(x_strequal("ello", expected));
}


TEST(xstring, caseequal)
{
    const char* expected = "Hello";
    TEST_ASSERT_TRUE(x_strcaseequal("Hello", expected));
    TEST_ASSERT_TRUE(x_strcaseequal("hello", expected));
    TEST_ASSERT_FALSE(x_strcaseequal("Hell", expected));
    TEST_ASSERT_FALSE(x_strcaseequal("ello", expected));
}


TEST(xstring, casecmp)
{
    TEST_ASSERT_TRUE(x_strcasecmp("ABC", "ABC") == 0);
    TEST_ASSERT_TRUE(x_strcasecmp("ABC", "BBC") < 0);
    TEST_ASSERT_TRUE(x_strcasecmp("ABC", "ABD") < 0);
    TEST_ASSERT_TRUE(x_strcasecmp("ABC", "ACC") < 0);
    TEST_ASSERT_TRUE(x_strcasecmp("BBC", "ABC") > 0);
    TEST_ASSERT_TRUE(x_strcasecmp("ABD", "ABC") > 0);
    TEST_ASSERT_TRUE(x_strcasecmp("ACC", "ABC") > 0);
    TEST_ASSERT_TRUE(x_strcasecmp("ABC", "ABCD") < 0);
    TEST_ASSERT_TRUE(x_strcasecmp("ABCD", "ABC") > 0);

    TEST_ASSERT_TRUE(x_strcasecmp("abc", "ABC") == 0);
    TEST_ASSERT_TRUE(x_strcasecmp("abc", "BBC") < 0);
    TEST_ASSERT_TRUE(x_strcasecmp("abc", "ABD") < 0);
    TEST_ASSERT_TRUE(x_strcasecmp("abc", "ACC") < 0);
    TEST_ASSERT_TRUE(x_strcasecmp("BBC", "abc") > 0);
    TEST_ASSERT_TRUE(x_strcasecmp("ABD", "abc") > 0);
    TEST_ASSERT_TRUE(x_strcasecmp("ACC", "abc") > 0);
    TEST_ASSERT_TRUE(x_strcasecmp("abc", "ABCD") < 0);
    TEST_ASSERT_TRUE(x_strcasecmp("ABCD", "abc") > 0);
}


TEST(xstring, ncasecmp)
{
    TEST_ASSERT_TRUE(x_strncasecmp("ABC", "ABC", 3) == 0);
    TEST_ASSERT_TRUE(x_strncasecmp("ABC", "ABC", 3) == 0);
    TEST_ASSERT_TRUE(x_strncasecmp("ABC", "BBC", 3) < 0);
    TEST_ASSERT_TRUE(x_strncasecmp("ABC", "ABD", 3) < 0);
    TEST_ASSERT_TRUE(x_strncasecmp("ABC", "ACC", 3) < 0);
    TEST_ASSERT_TRUE(x_strncasecmp("BBC", "ABC", 3) > 0);
    TEST_ASSERT_TRUE(x_strncasecmp("ABD", "ABC", 3) > 0);
    TEST_ASSERT_TRUE(x_strncasecmp("ACC", "ABC", 3) > 0);

    TEST_ASSERT_TRUE(x_strncasecmp("abc", "ABC", 3) == 0);
    TEST_ASSERT_TRUE(x_strncasecmp("abc", "BBC", 3) < 0);
    TEST_ASSERT_TRUE(x_strncasecmp("abc", "ABD", 3) < 0);
    TEST_ASSERT_TRUE(x_strncasecmp("abc", "ACC", 3) < 0);
    TEST_ASSERT_TRUE(x_strncasecmp("BBC", "abc", 3) > 0);
    TEST_ASSERT_TRUE(x_strncasecmp("ABD", "abc", 3) > 0);
    TEST_ASSERT_TRUE(x_strncasecmp("ACC", "abc", 3) > 0);

    TEST_ASSERT_TRUE(x_strncasecmp("ABC", "ABCD", 3) == 0);
    TEST_ASSERT_TRUE(x_strncasecmp("abc", "ABCD", 3) == 0);
}


TEST(xstring, nstr)
{
    const char* str = "Hello World";

    TEST_ASSERT_EQUAL_STRING("World", x_strnstr(str, "World", 11));
    TEST_ASSERT_NULL(x_strnstr(str, "World", 10));
}


TEST(xstring, casestr)
{
    const char* str = "Hello World!";

    TEST_ASSERT_EQUAL_STRING("World!", x_strcasestr(str, "World"));
    TEST_ASSERT_EQUAL_STRING("o World!", x_strcasestr(str, "o W"));
    TEST_ASSERT_EQUAL_STRING("World!", x_strcasestr(str, "world"));
}


TEST(xstring, ncasestr)
{
    const char* str = "Hello World!";

    TEST_ASSERT_EQUAL_STRING("World!", x_strncasestr(str, "WoRlD", 12));
    TEST_ASSERT_EQUAL_STRING("o World!", x_strncasestr(str, "o W", 12));
    TEST_ASSERT_NULL(x_strncasestr(str, "WoRlD", 10));
}


TEST(xstring, dup)
{
    const char* str = "Hello World!";
    char* dup;

    dup = x_strdup(str);
    TEST_ASSERT_EQUAL_STRING(str, dup);
    x_free(dup);
}


TEST(xstring, dup2)
{
    const char* str = "Hello World!";
    char* dup;

    dup = x_strdup2(str, X__GetMem);
    TEST_ASSERT_EQUAL_STRING(str, dup);
    X__FreeMem(dup);
}


TEST(xstring, ndup)
{
    const char* str = "Hello World!";
    char* dup;

    dup = x_strndup(str, 5);
    TEST_ASSERT_EQUAL_STRING("Hello", dup);
    x_free(dup);

    dup = x_strndup(str, 0);
    TEST_ASSERT_EQUAL_STRING("", dup);
    x_free(dup);
}


TEST(xstring, ndup2)
{
    const char* str = "Hello World!";
    char* dup;

    dup = x_strndup2(str, 5, X__GetMem);
    TEST_ASSERT_EQUAL_STRING("Hello", dup);
    X__FreeMem(dup);

    dup = x_strndup2(str, 0, X__GetMem);
    TEST_ASSERT_EQUAL_STRING("", dup);
    X__FreeMem(dup);
}


TEST(xstring, reverse)
{
    char str1[] = "abc";
    char str2[] = "abce";

    TEST_ASSERT_EQUAL_STRING("cba", x_strreverse(str1));
    TEST_ASSERT_EQUAL_STRING("ecba", x_strreverse(str2));
}


TEST(xstring, strip)
{
    char str1[] = "  abc  ";
    char str2[] = "@|]abc ^^`";

    TEST_ASSERT_EQUAL_STRING("abc", x_strstrip(str1, NULL));
    TEST_ASSERT_EQUAL_STRING("abc", x_strstrip(str2, " @|]^^`"));
}


TEST(xstring, lstrip)
{
    char str1[] = "  abc  ";
    char str2[] = "@|]abc ^^`";

    TEST_ASSERT_EQUAL_STRING("abc  ", x_strlstrip(str1, NULL));
    TEST_ASSERT_EQUAL_STRING("abc ^^`", x_strlstrip(str2, " @|]^^`"));
}


TEST(xstring, rstrip)
{
    char str1[] = "  abc  ";
    char str2[] = "@|]abc ^^`";

    TEST_ASSERT_EQUAL_STRING("  abc", x_strrstrip(str1, NULL));
    TEST_ASSERT_EQUAL_STRING("@|]abc", x_strrstrip(str2, " @|]^^`"));
}


TEST(xstring, toint32)
{
    bool ok;

    TEST_ASSERT_EQUAL(0, x_strtoint32("0", 100, &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL(1, x_strtoint32("1", 100, &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL(1, x_strtoint32("+1", 100, &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL(-1, x_strtoint32("-1", 100, &ok));
    TEST_ASSERT_TRUE(ok);

    TEST_ASSERT_EQUAL(INT32_MAX, x_strtoint32("2147483647", 0, &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL(0, x_strtoint32("2147483648", 0, &ok));
    TEST_ASSERT_TRUE(! ok);
    TEST_ASSERT_EQUAL(0, x_strtoint32("2147483650", 0, &ok));
    TEST_ASSERT_TRUE(! ok);
    TEST_ASSERT_EQUAL(INT32_MIN, x_strtoint32("-2147483648", 0, &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL(INT32_MIN + 1, x_strtoint32("-2147483647", 0, &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL(0, x_strtoint32("-2147483649", 0, &ok));
    TEST_ASSERT_TRUE(! ok);
}


TEST(xstring, touint32)
{
    bool ok;

    TEST_ASSERT_EQUAL(0, x_strtouint32("0", 100, &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL(1, x_strtouint32("1", 100, &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL(1, x_strtouint32("+1", 100, &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL(100, x_strtouint32("-1", 100, &ok));
    TEST_ASSERT_TRUE(! ok);

    TEST_ASSERT_EQUAL_HEX32(0, x_strtouint32("0x0", 100, &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_HEX32(1, x_strtouint32("0x1", 100, &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_HEX32(100, x_strtouint32("+0x1", 100, &ok));
    TEST_ASSERT_TRUE(! ok);
    TEST_ASSERT_EQUAL_HEX32(100, x_strtouint32("-0x1", 100, &ok));
    TEST_ASSERT_TRUE(! ok);
    TEST_ASSERT_EQUAL_HEX32(0xFFFFFFFF, x_strtouint32("0xFFFFFFFF", 100, &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_HEX32(100, x_strtouint32("0xZ", 100, &ok));
    TEST_ASSERT_TRUE(! ok);

    TEST_ASSERT_EQUAL_HEX32(0, x_strtouint32("0b0", 100, &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_HEX32(1, x_strtouint32("0b1", 100, &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_HEX32(100, x_strtouint32("+0b1", 100, &ok));
    TEST_ASSERT_TRUE(! ok);
    TEST_ASSERT_EQUAL_HEX32(100, x_strtouint32("-0b1", 100, &ok));
    TEST_ASSERT_TRUE(! ok);
    TEST_ASSERT_EQUAL_HEX32(0xFFFFFFFF, x_strtouint32("0b11111111111111111111111111111111", 100, &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_HEX32(100, x_strtouint32("0b2", 100, &ok));
    TEST_ASSERT_TRUE(! ok);

    TEST_ASSERT_EQUAL(UINT32_MAX, x_strtouint32("4294967295", 0, &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL(UINT32_MAX - 1, x_strtouint32("4294967294", 0, &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL(0, x_strtouint32("4294967394", 0, &ok));
    TEST_ASSERT_TRUE(! ok);
}


TEST(xstring, tofloat)
{
    bool ok;

    TEST_ASSERT_EQUAL_FLOAT(0.1f, x_strtofloat("0.1", 0.0f, &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_FLOAT(1.0f, x_strtofloat("1.0", 0.0f, &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_FLOAT(1.0f, x_strtofloat("1", 0.0f, &ok));
    TEST_ASSERT_TRUE(ok);
}


TEST(xstring, todouble)
{
    bool ok;

    TEST_ASSERT_EQUAL_DOUBLE(0.1, x_strtodouble("0.1", 0.0, &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_DOUBLE(1.0, x_strtodouble("1.0", 0.0, &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_DOUBLE(1.0, x_strtodouble("1", 0.0, &ok));
    TEST_ASSERT_TRUE(ok);
}


TEST(xstring, tobool)
{
    bool ok;
    TEST_ASSERT_TRUE(x_strtobool("Y", false, &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_TRUE(x_strtobool("y", false, &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_TRUE(x_strtobool("yes", false, &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_TRUE(x_strtobool("Yes", false, &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_TRUE(x_strtobool("true", false, &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_FALSE(x_strtobool("N", true, &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_FALSE(x_strtobool("n", true, &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_FALSE(x_strtobool("no", true, &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_FALSE(x_strtobool("No", true, &ok));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_FALSE(x_strtobool("false", true, &ok));
    TEST_ASSERT_TRUE(ok);
}


TEST(xstring, rpbrk)
{
    TEST_ASSERT_NULL(x_strrpbrk("ABC", "DE"));
    TEST_ASSERT_EQUAL_STRING("BC", x_strrpbrk("ABCBC", "B"));
    TEST_ASSERT_EQUAL_STRING("C", x_strrpbrk("ABCBC",  "C"));
    TEST_ASSERT_EQUAL_STRING("C", x_strrpbrk("ABCBC",  "CB"));
    TEST_ASSERT_EQUAL_STRING("ABCBC", x_strrpbrk("AAABCBC",  "A"));
}


TEST(xstring, casepbrk)
{
    TEST_ASSERT_NULL(x_strcasepbrk("ABC", "DE"));
    TEST_ASSERT_EQUAL_STRING("bcBc", x_strcasepbrk("AbcBc", "B"));
    TEST_ASSERT_EQUAL_STRING("CBC", x_strcasepbrk("ABCBC",  "c"));
    TEST_ASSERT_EQUAL_STRING("BCBC", x_strcasepbrk("ABCBC",  "cB"));
    TEST_ASSERT_EQUAL_STRING("D", x_strcasepbrk("ABCD",  "d"));
}


TEST(xstring, caserpbrk)
{
    TEST_ASSERT_NULL(x_strcaserpbrk("ABC", "DE"));
    TEST_ASSERT_EQUAL_STRING("BC", x_strcaserpbrk("ABCBC", "b"));
    TEST_ASSERT_EQUAL_STRING("c", x_strcaserpbrk("ABCBc",  "C"));
    TEST_ASSERT_EQUAL_STRING("c", x_strcaserpbrk("ABCBc",  "cb"));
    TEST_ASSERT_EQUAL_STRING("aBCBC", x_strcaserpbrk("AAaBCBC",  "a"));
}


TEST(xstring, tolower)
{
    char str[] = "Hello world";
    TEST_ASSERT_EQUAL_STRING("hello world", x_strtolower(str));
}


TEST(xstring, toupper)
{
    char str[] = "Hello world";
    TEST_ASSERT_EQUAL_STRING("HELLO WORLD", x_strtoupper(str));
}


TEST(xstring, lcpy)
{
    char buf[10];
    TEST_ASSERT_EQUAL(5, x_strlcpy(buf, "Hello", sizeof(buf)));
    TEST_ASSERT_EQUAL_STRING("Hello", buf);

    TEST_ASSERT_EQUAL(11, x_strlcpy(buf, "Hello World", sizeof(buf)));
    TEST_ASSERT_EQUAL_STRING("Hello Wor", buf);
}

TEST(xstring, lcat)
{
    char buf[10];

    buf[0] = '\0';
    TEST_ASSERT_EQUAL(5, x_strlcat(buf, "Hello", sizeof(buf)));
    TEST_ASSERT_EQUAL_STRING("Hello", buf);

    TEST_ASSERT_EQUAL(10, x_strlcat(buf, "Hello", sizeof(buf)));
    TEST_ASSERT_EQUAL_STRING("HelloHell", buf);

    TEST_ASSERT_EQUAL(6, x_strlcat(buf, "Hello", 1));
    TEST_ASSERT_EQUAL_STRING("HelloHell", buf);
}


TEST(xstring, countequal)
{
    TEST_ASSERT_EQUAL(11, x_strcountequal("Hello World", "Hello World"));
    TEST_ASSERT_EQUAL(6, x_strcountequal("Hello Picox", "Hello World"));
    TEST_ASSERT_EQUAL(6, x_strcountequal("Hello World", "Hello world"));
}


TEST(xstring, countcaseequal)
{
    TEST_ASSERT_EQUAL(11, x_strcountequal("Hello World", "Hello World"));
    TEST_ASSERT_EQUAL(11, x_strcountcaseequal("Hello Picox", "HELlo PiCox"));
}


TEST(xstring, pcpy)
{
    char buf[10];
    TEST_ASSERT_EQUAL(buf + 2, x_stpcpy(buf, "AB"));
    TEST_ASSERT_EQUAL_STRING(buf, "AB");
    TEST_ASSERT_EQUAL(buf + 3, x_stpcpy(buf, "ABC"));
    TEST_ASSERT_EQUAL_STRING(buf, "ABC");
}

TEST(xstring, prcpy)
{
    char buf[10];
    TEST_ASSERT_EQUAL(buf + 2, x_stprcpy(buf, "AB"));
    TEST_ASSERT_EQUAL_STRING(buf, "BA");
    TEST_ASSERT_EQUAL(buf + 3, x_stprcpy(buf, "ABC"));
    TEST_ASSERT_EQUAL_STRING(buf, "CBA");
}


TEST(xstring, pncpy)
{
    char buf[10];

    memset(buf, 0xFF, sizeof(buf));
    TEST_ASSERT_EQUAL(buf + 2, x_stpncpy(buf, "ABC", 2));
    TEST_ASSERT_EQUAL_STRING(buf, "AB");

    TEST_ASSERT_EQUAL(buf + 3, x_stpncpy(buf, "ABC", 3));
    TEST_ASSERT_EQUAL_STRING(buf, "ABC");

    TEST_ASSERT_EQUAL(buf + 3, x_stpncpy(buf, "ABC", 10));
    TEST_ASSERT_EQUAL_STRING(buf, "ABC");

    const char zero[7] = {0};
    TEST_ASSERT_EQUAL_MEMORY(zero, buf + 3, sizeof(zero));
}


TEST(xstring, pncpy2)
{
    char buf[10];

    memset(buf, 0xFF, sizeof(buf));
    TEST_ASSERT_EQUAL(buf + 2, x_stpncpy2(buf, "ABC", 2));
    TEST_ASSERT_EQUAL_STRING(buf, "AB");

    TEST_ASSERT_EQUAL(buf + 3, x_stpncpy2(buf, "ABC", 3));
    TEST_ASSERT_EQUAL_STRING(buf, "ABC");

    TEST_ASSERT_EQUAL(buf + 3, x_stpncpy2(buf, "ABC", 10));
    TEST_ASSERT_EQUAL_STRING(buf, "ABC");

    char ff[6];
    memset(ff, 0xFF, sizeof(ff));
    TEST_ASSERT_EQUAL_MEMORY(ff, buf + 4, sizeof(ff));
}


TEST(xstring, ncpy2)
{
    char buf[10];

    memset(buf, 0xFF, sizeof(buf));
    TEST_ASSERT_EQUAL(buf, x_strncpy2(buf, "ABC", 2));
    TEST_ASSERT_EQUAL_STRING(buf, "AB");

    TEST_ASSERT_EQUAL(buf, x_strncpy2(buf, "ABC", 3));
    TEST_ASSERT_EQUAL_STRING(buf, "ABC");

    TEST_ASSERT_EQUAL(buf, x_strncpy2(buf, "ABC", 10));
    TEST_ASSERT_EQUAL_STRING(buf, "ABC");

    char ff[6];
    memset(ff, 0xFF, sizeof(ff));
    TEST_ASSERT_EQUAL_MEMORY(ff, buf + 4, sizeof(ff));
}


TEST(xstring, chrnul)
{
    char buf[10];

    strcpy(buf, "AB");
    TEST_ASSERT_EQUAL(buf + 0, x_strchrnul(buf, 'A'));
    TEST_ASSERT_EQUAL(buf + 1, x_strchrnul(buf, 'B'));
    TEST_ASSERT_EQUAL(buf + 2, x_strchrnul(buf, 'C'));
}


TEST(xstring, nlen)
{
    char buf[10];

    strcpy(buf, "ABC");
    TEST_ASSERT_EQUAL(3, x_strnlen(buf, 0xFF));
    TEST_ASSERT_EQUAL(2, x_strnlen(buf, 2));
    TEST_ASSERT_EQUAL(0, x_strnlen(buf, 0));
}


TEST(xstring, tomode)
{
    TEST_ASSERT_EQUAL(X_OPEN_MODE_READ, x_strtomode("r"));
    TEST_ASSERT_EQUAL(X_OPEN_MODE_READ, x_strtomode("rb"));
    TEST_ASSERT_EQUAL(X_OPEN_MODE_READ_PLUS, x_strtomode("r+"));
    TEST_ASSERT_EQUAL(X_OPEN_MODE_READ_PLUS, x_strtomode("r+b"));
    TEST_ASSERT_EQUAL(X_OPEN_MODE_WRITE, x_strtomode("w"));
    TEST_ASSERT_EQUAL(X_OPEN_MODE_WRITE, x_strtomode("wb"));
    TEST_ASSERT_EQUAL(X_OPEN_MODE_WRITE_PLUS, x_strtomode("w+"));
    TEST_ASSERT_EQUAL(X_OPEN_MODE_WRITE_PLUS, x_strtomode("w+b"));
    TEST_ASSERT_EQUAL(X_OPEN_MODE_APPEND, x_strtomode("a"));
    TEST_ASSERT_EQUAL(X_OPEN_MODE_APPEND, x_strtomode("ab"));
    TEST_ASSERT_EQUAL(X_OPEN_MODE_APPEND_PLUS, x_strtomode("a+"));
    TEST_ASSERT_EQUAL(X_OPEN_MODE_APPEND_PLUS, x_strtomode("a+b"));
    TEST_ASSERT_EQUAL(X_OPEN_MODE_UNKNOWN, x_strtomode("s"));
}

TEST(xstring, memmem)
{
    char buf[10];
    memcpy(buf, "ABC\0DEF\0GH", 10);
    TEST_ASSERT_EQUAL(buf + 4, x_memmem(buf, sizeof(buf), "DEF", 3));
    TEST_ASSERT_EQUAL(buf + 4, x_memmem(buf, sizeof(buf), "DEF\0GH", 6));
    TEST_ASSERT_EQUAL(NULL, x_memmem(buf, sizeof(buf), "DEFG", 4));
}


TEST(xstring, memrchr)
{
    char buf[10];
    memcpy(buf, "ABC\0DEF\0GH", 10);
    TEST_ASSERT_EQUAL(buf + 4, x_memrchr(buf, 'D', sizeof(buf)));
    TEST_ASSERT_EQUAL(buf + 0, x_memrchr(buf, 'A', sizeof(buf)));
    TEST_ASSERT_EQUAL(buf + 7, x_memrchr(buf, '\0', sizeof(buf)));
}


TEST(xstring, memswap)
{
    char a[] = "FooBar";
    char b[] = "AbcDef";

    x_memswap(a, b, 3);
    TEST_ASSERT_EQUAL_STRING("AbcBar", a);
    TEST_ASSERT_EQUAL_STRING("FooDef", b);
}


TEST(xstring, memreverse)
{
    int i;

    int iarray[5] = {4, 3, 2, 1, 0};
    x_memreverse(iarray, sizeof(int), X_COUNT_OF(iarray));
    for (i = 0; i < (int)X_COUNT_OF(iarray); ++i)
        TEST_ASSERT_EQUAL(i, iarray[i]);

    long larray[5] = {4, 3, 2, 1, 0};
    x_memreverse(larray, sizeof(long), X_COUNT_OF(larray));
    for (i = 0; i < (int)X_COUNT_OF(larray); ++i)
        TEST_ASSERT_EQUAL(i, larray[i]);

    short sarray[5] = {4, 3, 2, 1, 0};
    x_memreverse(sarray, sizeof(short), X_COUNT_OF(sarray));
    for (i = 0; i < (int)X_COUNT_OF(sarray); ++i)
        TEST_ASSERT_EQUAL(i, sarray[i]);

    char carray[5] = {4, 3, 2, 1, 0};
    x_memreverse(carray, sizeof(char), X_COUNT_OF(carray));
    for (i = 0; i < (int)X_COUNT_OF(carray); ++i)
        TEST_ASSERT_EQUAL(i, carray[i]);
}


TEST(xstring, memrrot)
{
    int iarray[5] = {4, 3, 2, 1, 0};
    x_memrrot(iarray, 3, sizeof(int), X_COUNT_OF(iarray));

    const int expected[5] = {2, 1, 0, 4, 3};
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, iarray, X_COUNT_OF(iarray));
}


TEST(xstring, memlrot)
{
    int iarray[5] = {4, 3, 2, 1, 0};
    x_memlrot(iarray, 3, sizeof(int), X_COUNT_OF(iarray));

    const int expected[5] = {1, 0, 4, 3, 2};
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, iarray, X_COUNT_OF(iarray));
}


TEST(xstring, memblt)
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


TEST(xstring, memequal)
{
    char buf[5];
    memset(buf, 0xFF, sizeof(buf));
    TEST_ASSERT_FALSE(x_memequal(buf, "Hello", 5));

    memcpy(buf, "Hello", 5);
    TEST_ASSERT_TRUE(x_memequal(buf, "Hello", 5));
}


TEST(xstring, memrandom)
{
    char buf[10];
    x_memrandom(buf, sizeof(buf));
}


TEST(xstring, memrandom_alpha)
{
    char buf[100];
    x_memrandom_alpha(buf, sizeof(buf));
    int i;

    for (i = 0; i < (int)X_COUNT_OF(buf); ++i)
        TEST_ASSERT_TRUE(isalpha((int)buf[i]));
}


TEST(xstring, btos)
{
    TEST_ASSERT_EQUAL_STRING("true", x_btos(1 > 0));
    TEST_ASSERT_EQUAL_STRING("false", x_btos(1 + 1 == 3));
}


TEST(xstring, replace)
{
    char buf[128];
    strcpy(buf, "ABCDEF");
    x_strreplace(buf, sizeof(buf), strlen(buf), 3, "GHI", 3);
    TEST_ASSERT_EQUAL_STRING("GHIDEF", buf);

    strcpy(buf, "ABCDEF");
    x_strreplace(buf, sizeof(buf), strlen(buf), 3, "GHI", 2);
    TEST_ASSERT_EQUAL_STRING("GHDEF", buf);

    strcpy(buf, "ABCDEF");
    x_strreplace(buf, sizeof(buf), strlen(buf), 3, "GHI", 1);
    TEST_ASSERT_EQUAL_STRING("GDEF", buf);

    strcpy(buf, "ABCDEF");
    x_strreplace(buf, sizeof(buf), strlen(buf), 2, "GHI", 3);
    TEST_ASSERT_EQUAL_STRING("GHICDEF", buf);

    strcpy(buf, "ABCDEF");
    x_strreplace(buf, sizeof(buf), strlen(buf), 1, "GHI", 3);
    TEST_ASSERT_EQUAL_STRING("GHIBCDEF", buf);

    strcpy(buf, "/home/foo/bar.txt");
    x_strreplace(buf, sizeof(buf), strlen(buf),
                 strlen("/home"),
                 "/mnt",
                 strlen("/mnt"));
    TEST_ASSERT_EQUAL_STRING("/mnt/foo/bar.txt", buf);

    strcpy(buf, "/home/foo/bar.txt");
    x_strreplace(buf, sizeof(buf), strlen(buf),
                 strlen("/home"),
                 "/media",
                 strlen("/media"));
    TEST_ASSERT_EQUAL_STRING("/media/foo/bar.txt", buf);
}


TEST_GROUP_RUNNER(xstring)
{
    RUN_TEST_CASE(xstring, equal);
    RUN_TEST_CASE(xstring, caseequal);
    RUN_TEST_CASE(xstring, casecmp);
    RUN_TEST_CASE(xstring, ncasecmp);
    RUN_TEST_CASE(xstring, nstr);
    RUN_TEST_CASE(xstring, casestr);
    RUN_TEST_CASE(xstring, ncasestr);
    RUN_TEST_CASE(xstring, dup);
    RUN_TEST_CASE(xstring, dup2);
    RUN_TEST_CASE(xstring, ndup);
    RUN_TEST_CASE(xstring, ndup2);
    RUN_TEST_CASE(xstring, reverse);
    RUN_TEST_CASE(xstring, strip);
    RUN_TEST_CASE(xstring, lstrip);
    RUN_TEST_CASE(xstring, rstrip);
    RUN_TEST_CASE(xstring, toint32);
    RUN_TEST_CASE(xstring, touint32);
    RUN_TEST_CASE(xstring, tofloat);
    RUN_TEST_CASE(xstring, todouble);
    RUN_TEST_CASE(xstring, tobool);
    RUN_TEST_CASE(xstring, rpbrk);
    RUN_TEST_CASE(xstring, casepbrk);
    RUN_TEST_CASE(xstring, caserpbrk);
    RUN_TEST_CASE(xstring, tolower);
    RUN_TEST_CASE(xstring, toupper);
    RUN_TEST_CASE(xstring, lcpy);
    RUN_TEST_CASE(xstring, lcat);
    RUN_TEST_CASE(xstring, countequal);
    RUN_TEST_CASE(xstring, countcaseequal);
    RUN_TEST_CASE(xstring, pcpy);
    RUN_TEST_CASE(xstring, prcpy);
    RUN_TEST_CASE(xstring, pncpy);
    RUN_TEST_CASE(xstring, pncpy2);
    RUN_TEST_CASE(xstring, ncpy2);
    RUN_TEST_CASE(xstring, chrnul);
    RUN_TEST_CASE(xstring, nlen);
    RUN_TEST_CASE(xstring, replace);
    RUN_TEST_CASE(xstring, tomode);
    RUN_TEST_CASE(xstring, memmem);
    RUN_TEST_CASE(xstring, memrchr);
    RUN_TEST_CASE(xstring, memswap);
    RUN_TEST_CASE(xstring, memreverse);
    RUN_TEST_CASE(xstring, memrrot);
    RUN_TEST_CASE(xstring, memlrot);
    RUN_TEST_CASE(xstring, memblt);
    RUN_TEST_CASE(xstring, memequal);
    RUN_TEST_CASE(xstring, memrandom);
    RUN_TEST_CASE(xstring, memrandom_alpha);
    RUN_TEST_CASE(xstring, btos);
}
