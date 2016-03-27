#include <picox/filesystem/xvfs.h>
#include <picox/filesystem/xposixfs.h>
#include "testutils.h"
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>




TEST_GROUP(xvfs);


TEST_SETUP(xvfs)
{
    char cwd[255];
    getcwd(cwd, sizeof(cwd));
}


TEST_TEAR_DOWN(xvfs)
{
}


TEST(xvfs, open)
{
    XPosixFs fs;
    xposixfs_init(&fs);

    XVirtualFs vfs;
    xposixfs_init_vfs(&fs, &vfs);

    XFile* fp;
    xvfs_open(&vfs, "hoge.txt", X_OPEN_MODE_WRITE, &fp);

    xvfs_close(fp);

    xposixfs_deinit(&fs);
}


TEST_GROUP_RUNNER(xvfs)
{
    char cwd[255];
    char workdir[255];
    TEST_ASSERT_NOT_NULL(getcwd(cwd, sizeof(cwd)));

    strcpy(workdir, cwd);
    strcat(workdir, "/posixfstmp");
    TEST_ASSERT_EQUAL(0, mkdir(workdir, 0777));
    TEST_ASSERT_EQUAL(0, chdir(workdir));


    RUN_TEST_CASE(xvfs, open);

    TEST_ASSERT_EQUAL(0, chdir(cwd));
    system("rm -r ./posixfstmp");
}
