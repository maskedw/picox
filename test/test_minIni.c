#include <minIni.h>
#include "testutils.h"


TEST_GROUP(minIni);

#define sizearray(a)  (sizeof(a) / sizeof((a)[0]))
static const char const testdata1[] =
{
    "[First]\n"
    "String=noot # trailing commment\n"
    "Val=1\n"
    "[Second]\n"
    "Val = 2\n"
    "#comment=3\n"
    "String = mies\n"
};

static const char const testdata2[] =
{
    "String=noot # trailing commment\n"
    "#comment=3\n"
    "Val=1\n"
};

const char inifile[] = "test.ini";
const char inifile2[] = "plain.ini";

TEST_SETUP(minIni)
{
}


TEST_TEAR_DOWN(minIni)
{
}


TEST(minIni, minIni)
{
    char str[100];
    long n;

    XFile* fp;
    TEST_ASSERT_EQUAL(X_ERR_NONE, xunionfs_open(inifile, X_OPEN_MODE_WRITE, &fp));
    TEST_ASSERT_TRUE(xunionfs_puts(fp, testdata1) >= 0);
    TEST_ASSERT_EQUAL(X_ERR_NONE, xunionfs_close(fp));

    TEST_ASSERT_EQUAL(X_ERR_NONE, xunionfs_open(inifile2, X_OPEN_MODE_WRITE, &fp));
    TEST_ASSERT_TRUE(xunionfs_puts(fp, testdata2) >= 0);
    TEST_ASSERT_EQUAL(X_ERR_NONE, xunionfs_close(fp));

    /* string reading */
    n = ini_gets("first", "string", "dummy", str, sizearray(str), inifile);
    TEST_ASSERT_TRUE(n==4 && strcmp(str,"noot")==0);
    n = ini_gets("second", "string", "dummy", str, sizearray(str), inifile);
    TEST_ASSERT_TRUE(n==4 && strcmp(str,"mies")==0);
    n = ini_gets("first", "undefined", "dummy", str, sizearray(str), inifile);
    TEST_ASSERT_TRUE(n==5 && strcmp(str,"dummy")==0);
    /* ----- */
    n = ini_gets("", "string", "dummy", str, sizearray(str), inifile2);
    TEST_ASSERT_TRUE(n==4 && strcmp(str,"noot")==0);
    n = ini_gets(NULL, "string", "dummy", str, sizearray(str), inifile2);
    TEST_ASSERT_TRUE(n==4 && strcmp(str,"noot")==0);
    /* ----- */

    /* value reading */
    n = ini_getl("first", "val", -1, inifile);
    TEST_ASSERT_TRUE(n==1);
    n = ini_getl("second", "val", -1, inifile);
    TEST_ASSERT_TRUE(n==2);
    n = ini_getl("first", "undefined", -1, inifile);
    TEST_ASSERT_TRUE(n==-1);
    /* ----- */
    n = ini_getl(NULL, "val", -1, inifile2);
    TEST_ASSERT_TRUE(n==1);
    /* ----- */

    /* string writing */
    n = ini_puts("first", "alt", "flagged as \"correct\"", inifile);
    TEST_ASSERT_TRUE(n==1);
    n = ini_gets("first", "alt", "dummy", str, sizearray(str), inifile);
    TEST_ASSERT_TRUE(n==20 && strcmp(str,"flagged as \"correct\"")==0);
    /* ----- */
    n = ini_puts("second", "alt", "correct", inifile);
    TEST_ASSERT_TRUE(n==1);
    n = ini_gets("second", "alt", "dummy", str, sizearray(str), inifile);
    TEST_ASSERT_TRUE(n==7 && strcmp(str,"correct")==0);
    /* ----- */
    n = ini_puts("third", "test", "correct", inifile);
    TEST_ASSERT_TRUE(n==1);
    n = ini_gets("third", "test", "dummy", str, sizearray(str), inifile);
    TEST_ASSERT_TRUE(n==7 && strcmp(str,"correct")==0);
    /* ----- */
    n = ini_puts("second", "alt", "overwrite", inifile);
    TEST_ASSERT_TRUE(n==1);
    n = ini_gets("second", "alt", "dummy", str, sizearray(str), inifile);
    TEST_ASSERT_TRUE(n==9 && strcmp(str,"overwrite")==0);
    /* ----- */
    n = ini_puts("second", "alt", "123456789", inifile);
    TEST_ASSERT_TRUE(n==1);
    n = ini_gets("second", "alt", "dummy", str, sizearray(str), inifile);
    TEST_ASSERT_TRUE(n==9 && strcmp(str,"123456789")==0);
    /* ----- */
    n = ini_puts(NULL, "alt", "correct", inifile2);
    TEST_ASSERT_TRUE(n==1);
    n = ini_gets(NULL, "alt", "dummy", str, sizearray(str), inifile2);
    TEST_ASSERT_TRUE(n==7 && strcmp(str,"correct")==0);
    /* ----- */

    /* string deletion */
    n = ini_puts("first", "alt", NULL, inifile);
    TEST_ASSERT_TRUE(n==1);
    n = ini_puts("second", "alt", NULL, inifile);
    n = ini_puts("third", NULL, NULL, inifile);
    TEST_ASSERT_TRUE(n==1);
    /* ----- */
    n = ini_puts(NULL, "alt", NULL, inifile2);
    TEST_ASSERT_TRUE(n==1);
}


TEST_GROUP_RUNNER(minIni)
{
    x_test_init_fs();

    RUN_TEST_CASE(minIni, minIni);

    x_test_deinit_fs();
}
