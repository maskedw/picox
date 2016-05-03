#include <picox/filesystem/xvfs.h>
#include <picox/filesystem/xramfs.h>
#include <picox/filesystem/xsinglefs.h>
#include "testutils.h"


static XRamFs* ramfs;
static XVirtualFs* vfs;
static const char WRITE_DATA[] = "Hello world";
static const size_t WRITE_LEN = 11; // strlen WRITE_DATA


TEST_GROUP(xsinglefs);


TEST_SETUP(xsinglefs)
{
    ramfs = x_malloc(sizeof(*ramfs));
    vfs = x_malloc(sizeof(*vfs));
    X_ASSERT(ramfs);
    X_ASSERT(vfs);

    X_ASSERT(xramfs_init(ramfs, NULL, 10 * 1024) == X_ERR_NONE);
    xramfs_init_vfs(ramfs, vfs);
    xsinglefs_init(vfs);
}


TEST_TEAR_DOWN(xsinglefs)
{
    xramfs_deinit(ramfs);
    x_free(vfs);
    x_free(ramfs);
    xsinglefs_deinit();
}


TEST(xsinglefs, open)
{
    XFile* fp;
    xsinglefs_open("hoge.txt", X_OPEN_MODE_WRITE, &fp);
    xsinglefs_close(fp);
}


TEST(xsinglefs, copyfile)
{
    XFile* fp;
    XError err;
    char buf[256];
    size_t nread;
    size_t nwritten;
    const char name[] = "foo.txt";
    const char name2[] = "bar.txt";

    err = xsinglefs_open(name, X_OPEN_MODE_WRITE, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_NOT_NULL(fp);

    err = xsinglefs_write(fp, WRITE_DATA, WRITE_LEN, &nwritten);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nwritten);
    xsinglefs_close(fp);

    err = xsinglefs_copyfile(name, name2);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xsinglefs_open(name, X_OPEN_MODE_READ, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_NOT_NULL(fp);

    err = xsinglefs_read(fp, buf, WRITE_LEN, &nread);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nread);
    TEST_ASSERT_EQUAL(0, memcmp(buf, WRITE_DATA, WRITE_LEN));
    xsinglefs_close(fp);
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


TEST(xsinglefs, copytree)
{
    XError err;
    XFile* fp;

    err = xsinglefs_mkdir("foo");
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xsinglefs_mkdir("foo/bar");
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xsinglefs_mkdir("foo/baz");
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xsinglefs_open("foo/foo.txt", X_OPEN_MODE_WRITE, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_NOT_NULL(fp);

    err = xsinglefs_copytree("foo", "hoge");
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xsinglefs_copytree("foo", "/hoge/tree");
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xsinglefs_walktree("/", PrintTree, NULL);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
}


TEST(xsinglefs, rmtree)
{
    XFile* fp;
    XError err;
    err = xsinglefs_mkdir("foo");
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xsinglefs_open("foo.txt", X_OPEN_MODE_WRITE, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_NOT_NULL(fp);

    err = xsinglefs_mkdir("foo/bar");
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xsinglefs_mkdir("foo/baz");
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);


    err = xsinglefs_rmtree("foo");
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xsinglefs_walktree("/", PrintTree, NULL);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
}



TEST_GROUP_RUNNER(xsinglefs)
{
    RUN_TEST_CASE(xsinglefs, open);
    RUN_TEST_CASE(xsinglefs, copyfile);
    RUN_TEST_CASE(xsinglefs, copytree);
    RUN_TEST_CASE(xsinglefs, rmtree);
}
