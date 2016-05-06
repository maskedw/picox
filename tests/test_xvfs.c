#include <picox/filesystem/xvfs.h>
#include <picox/filesystem/xramfs.h>
#include "testutils.h"


static XRamFs* ramfs;
static XVirtualFs* vfs;
static const char WRITE_DATA[] = "Hello world";
static const size_t WRITE_LEN = 11; // strlen WRITE_DATA


TEST_GROUP(xvfs);


TEST_SETUP(xvfs)
{
    ramfs = x_malloc(sizeof(*ramfs));
    vfs = x_malloc(sizeof(*vfs));
    X_ASSERT(ramfs);
    X_ASSERT(vfs);

    X_ASSERT(xramfs_init(ramfs, NULL, 10 * 1024) == X_ERR_NONE);
    xramfs_init_vfs(ramfs, vfs);
}


TEST_TEAR_DOWN(xvfs)
{
    xramfs_deinit(ramfs);
    x_free(vfs);
    x_free(ramfs);
}


TEST(xvfs, open)
{
    XFile* fp;
    xvfs_open(vfs, "hoge.txt", X_OPEN_MODE_WRITE, &fp);
    xvfs_close(fp);
}


TEST(xvfs, copyfile)
{
    XFile* fp;
    XError err;
    char buf[256];
    size_t nread;
    size_t nwritten;
    const char name[] = "foo.txt";
    const char name2[] = "bar.txt";

    err = xvfs_open(vfs, name, X_OPEN_MODE_WRITE, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_NOT_NULL(fp);

    err = xvfs_write(fp, WRITE_DATA, WRITE_LEN, &nwritten);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nwritten);
    xvfs_close(fp);

    err = xvfs_copyfile(vfs, name, name2);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xvfs_open(vfs, name, X_OPEN_MODE_READ, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_NOT_NULL(fp);

    err = xvfs_read(fp, buf, WRITE_LEN, &nread);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nread);
    TEST_ASSERT_EQUAL(0, memcmp(buf, WRITE_DATA, WRITE_LEN));
    xvfs_close(fp);
}


static bool PrintTree(void* userptr, const char* path, const XStat* statbuf, const XDirEnt* dirent)
{
    X_UNUSED(userptr);
    X_UNUSED(statbuf);
    X_UNUSED(dirent);
    X_UNUSED(path);
    // printf("%s\n", path);

    return true;
}


TEST(xvfs, copytree)
{
    XError err;
    XFile* fp;

    err = xvfs_mkdir(vfs, "foo");
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xvfs_mkdir(vfs, "foo/bar");
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xvfs_mkdir(vfs, "foo/baz");
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xvfs_open(vfs, "foo/foo.txt", X_OPEN_MODE_WRITE, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_NOT_NULL(fp);

    err = xvfs_copytree(vfs, "foo", "hoge");
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xvfs_copytree(vfs, "foo", "/hoge/tree");
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xvfs_walktree(vfs, "/", PrintTree, NULL);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
}


TEST(xvfs, rmtree)
{
    XFile* fp;
    XError err;
    err = xvfs_mkdir(vfs, "foo");
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xvfs_open(vfs, "foo.txt", X_OPEN_MODE_WRITE, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_NOT_NULL(fp);

    err = xvfs_mkdir(vfs, "foo/bar");
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xvfs_mkdir(vfs, "foo/baz");
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);


    err = xvfs_rmtree(vfs, "foo");
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xvfs_walktree(vfs, "/", PrintTree, NULL);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
}



TEST_GROUP_RUNNER(xvfs)
{
    RUN_TEST_CASE(xvfs, open);
    RUN_TEST_CASE(xvfs, copyfile);
    RUN_TEST_CASE(xvfs, copytree);
    RUN_TEST_CASE(xvfs, rmtree);
}
