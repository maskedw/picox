#include <unity.h>
#include <unity_fixture.h>
#include <stdio.h>
#include <string.h>
#include <sds.h>


TEST_GROUP(sds);


TEST_SETUP(sds)
{
}


TEST_TEAR_DOWN(sds)
{
}


TEST(sds, sds)
{
    struct sdshdr *sh;
    sds x = sdsnew("foo"), y;

    TEST_ASSERT_TRUE(
        sdslen(x) == 3 && memcmp(x,"foo\0",4) == 0)

    sdsfree(x);
    x = sdsnewlen("foo",2);
    TEST_ASSERT_TRUE(
        sdslen(x) == 2 && memcmp(x,"fo\0",3) == 0)

    x = sdscat(x,"bar");
    TEST_ASSERT_TRUE(
        sdslen(x) == 5 && memcmp(x,"fobar\0",6) == 0);

    x = sdscpy(x,"a");
    TEST_ASSERT_TRUE(
        sdslen(x) == 1 && memcmp(x,"a\0",2) == 0)

    x = sdscpy(x,"xyzxxxxxxxxxxyyyyyyyyyykkkkkkkkkk");
    TEST_ASSERT_TRUE(
        sdslen(x) == 33 &&
        memcmp(x,"xyzxxxxxxxxxxyyyyyyyyyykkkkkkkkkk\0",33) == 0)

    sdsfree(x);
    x = sdscatprintf(sdsempty(),"%d",123);
    TEST_ASSERT_TRUE(
        sdslen(x) == 3 && memcmp(x,"123\0",4) ==0)

    sdsfree(x);
    x = sdsnew("xxciaoyyy");
    sdstrim(x,"xy");
    TEST_ASSERT_TRUE(
        sdslen(x) == 4 && memcmp(x,"ciao\0",5) == 0)

    y = sdsdup(x);
    sdsrange(y,1,1);
    TEST_ASSERT_TRUE(
        sdslen(y) == 1 && memcmp(y,"i\0",2) == 0)

    sdsfree(y);
    y = sdsdup(x);
    sdsrange(y,1,-1);
    TEST_ASSERT_TRUE(
        sdslen(y) == 3 && memcmp(y,"iao\0",4) == 0)

    sdsfree(y);
    y = sdsdup(x);
    sdsrange(y,-2,-1);
    TEST_ASSERT_TRUE(
        sdslen(y) == 2 && memcmp(y,"ao\0",3) == 0)

    sdsfree(y);
    y = sdsdup(x);
    sdsrange(y,2,1);
    TEST_ASSERT_TRUE(
        sdslen(y) == 0 && memcmp(y,"\0",1) == 0)

    sdsfree(y);
    y = sdsdup(x);
    sdsrange(y,1,100);
    TEST_ASSERT_TRUE(
        sdslen(y) == 3 && memcmp(y,"iao\0",4) == 0)

    sdsfree(y);
    y = sdsdup(x);
    sdsrange(y,100,100);
    TEST_ASSERT_TRUE(
        sdslen(y) == 0 && memcmp(y,"\0",1) == 0)

    sdsfree(y);
    sdsfree(x);
    x = sdsnew("foo");
    y = sdsnew("foa");
    TEST_ASSERT_TRUE( sdscmp(x,y) > 0)

    sdsfree(y);
    sdsfree(x);
    x = sdsnew("bar");
    y = sdsnew("bar");
    TEST_ASSERT_TRUE( sdscmp(x,y) == 0)

    sdsfree(y);
    sdsfree(x);
    x = sdsnew("aar");
    y = sdsnew("bar");
    TEST_ASSERT_TRUE( sdscmp(x,y) < 0)

    sdsfree(y);
    sdsfree(x);
    x = sdsnewlen("\a\n\0foo\r",7);
    y = sdscatrepr(sdsempty(),x,sdslen(x));
    TEST_ASSERT_TRUE(
        memcmp(y,"\"\\a\\n\\x00foo\\r\"",15) == 0)

    {
        int oldfree;

        sdsfree(x);
        x = sdsnew("0");
        sh = (void*) (x-(sizeof(struct sdshdr)));
        TEST_ASSERT_TRUE( sh->len == 1 && sh->free == 0);
        x = sdsMakeRoomFor(x,1);
        sh = (void*) (x-(sizeof(struct sdshdr)));
        TEST_ASSERT_TRUE( sh->len == 1 && sh->free > 0);
        oldfree = sh->free;
        x[1] = '1';
        sdsIncrLen(x,1);
        TEST_ASSERT_TRUE( x[0] == '0' && x[1] == '1');
        TEST_ASSERT_TRUE( sh->len == 2);
        TEST_ASSERT_TRUE( sh->free == oldfree-1);
    }
}


TEST_GROUP_RUNNER(sds)
{
    RUN_TEST_CASE(sds, sds);
}
