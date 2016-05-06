#include "testutils.h"
#include <picox/filesystem/xfs.h>


static const char WRITE_DATA[] = "Hello world";
static const size_t WRITE_LEN = 11; // strlen WRITE_DATA


TEST_GROUP(xfs);


TEST_SETUP(xfs)
{
    x_test_init_fs();
}


TEST_TEAR_DOWN(xfs)
{
    x_test_deinit_fs();
}


TEST(xfs, open)
{
    XFile* fp;
    xfs_open("hoge.txt", X_OPEN_MODE_WRITE, &fp);
    xfs_close(fp);
}


TEST(xfs, copyfile)
{
    XFile* fp;
    XError err;
    char buf[256];
    size_t nread;
    size_t nwritten;
    const char name[] = "foo.txt";
    const char name2[] = "bar.txt";

    err = xfs_open(name, X_OPEN_MODE_WRITE, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_NOT_NULL(fp);

    err = xfs_write(fp, WRITE_DATA, WRITE_LEN, &nwritten);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nwritten);
    xfs_close(fp);

    err = xfs_copyfile(name, name2);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xfs_open(name, X_OPEN_MODE_READ, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_NOT_NULL(fp);

    err = xfs_read(fp, buf, WRITE_LEN, &nread);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nread);
    TEST_ASSERT_EQUAL(0, memcmp(buf, WRITE_DATA, WRITE_LEN));
    xfs_close(fp);
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


TEST(xfs, copytree)
{
    XError err;
    XFile* fp;

    err = xfs_mkdir("foo");
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xfs_mkdir("foo/bar");
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xfs_mkdir("foo/baz");
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xfs_open("foo/foo.txt", X_OPEN_MODE_WRITE, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_NOT_NULL(fp);

    err = xfs_copytree("foo", "hoge");
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xfs_copytree("foo", "/hoge/tree");
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xfs_walktree("/", PrintTree, NULL);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
}


TEST(xfs, rmtree)
{
    XFile* fp;
    XError err;
    err = xfs_mkdir("foo");
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xfs_open("foo.txt", X_OPEN_MODE_WRITE, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_NOT_NULL(fp);

    err = xfs_mkdir("foo/bar");
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xfs_mkdir("foo/baz");
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);


    err = xfs_rmtree("foo");
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xfs_walktree("/", PrintTree, NULL);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
}


TEST_GROUP_RUNNER(xfs)
{
    RUN_TEST_CASE(xfs, open);
    RUN_TEST_CASE(xfs, copyfile);
    RUN_TEST_CASE(xfs, copytree);
    RUN_TEST_CASE(xfs, rmtree);
}
