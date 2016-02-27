#include <picox/filesystem/xfs.h>
#include <picox/filesystem/xposixfs.h>
#include "testutils.h"
#include <unistd.h>




TEST_GROUP(xfs);


static const char* path = "tmp/foo.txt";


TEST_SETUP(xfs)
{
}


TEST_TEAR_DOWN(xfs)
{
}


TEST(xfs, open)
{
    XPosixFs fs;
    char cwd[255];
    getcwd(cwd, sizeof(cwd));
    xposixfs_init(&fs, cwd);

    XVirtualFs vfs;
    xposixfs_init_vfs(&fs, &vfs);

    xfs_mount(&vfs, "/");
    XFile file;
    XFile* fp = &file;
    int err = xfs_open(fp, "hoge.txt", "w");
    xfs_close(fp);

    xposixfs_deinit(&fs);
    xfs_umount("/");
}


TEST_GROUP_RUNNER(xfs)
{
    RUN_TEST_CASE(xfs, open);
}
