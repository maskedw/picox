#include <picox/core/xcore.h>
#include <ff.h>
#include "testutils.h"




TEST_GROUP(xfatfs);



TEST_SETUP(xfatfs)
{
}


TEST_TEAR_DOWN(xfatfs)
{
}


TEST(xfatfs, mkfs)
{
    FATFS fs;
    f_mount(&fs, "", 0);
    FRESULT res = f_mkfs("0", 0, 0);
    TEST_ASSERT_EQUAL(FR_OK, res);
}


TEST(xfatfs, open)
{
}


TEST_GROUP_RUNNER(xfatfs)
{
    RUN_TEST_CASE(xfatfs, mkfs);
    RUN_TEST_CASE(xfatfs, open);
}
