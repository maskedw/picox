#include <picox/filesystem/xposixfs.h>
#include <unistd.h>
#include "testutils.h"


TEST_GROUP(xposixfs);


static const char* path = "tmp/foo.txt";


TEST_SETUP(xposixfs)
{
}


TEST_TEAR_DOWN(xposixfs)
{
}


TEST(xposixfs, open)
{
    XPosixFs fs;
    char cwd[255];
    getcwd(cwd, sizeof(cwd));
    xposixfs_init(&fs, cwd);

    XFile file;
    XFile* fp = &file;
    int err = xposixfs_open(&fs, &file, "hoge.txt", "w");

    xposixfs_close(fp);

    xposixfs_deinit(&fs);
}


TEST_GROUP_RUNNER(xposixfs)
{
    RUN_TEST_CASE(xposixfs, open);
}
