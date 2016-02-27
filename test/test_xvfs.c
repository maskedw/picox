#include <picox/filesystem/xvfs.h>
#include <picox/filesystem/xposixfs.h>
#include "testutils.h"
#include <unistd.h>




TEST_GROUP(xvfs);


static const char* path = "tmp/foo.txt";


TEST_SETUP(xvfs)
{
}


TEST_TEAR_DOWN(xvfs)
{
}


TEST(xvfs, open)
{
    XPosixFs fs;
    char cwd[255];
    getcwd(cwd, sizeof(cwd));
    xposixfs_init(&fs, cwd);

    XVirtualFs vfs;
    xposixfs_init_vfs(&fs, &vfs);

    XFile file;
    XFile* fp = &file;
    int err = xvfs_open(&vfs, fp, "hoge.txt", "w");

    xvfs_close(fp);

    xposixfs_deinit(&fs);
}


TEST_GROUP_RUNNER(xvfs)
{
    RUN_TEST_CASE(xvfs, open);
}
