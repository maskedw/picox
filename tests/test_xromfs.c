#include <picox/filesystem/xromfs.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "testutils.h"
#include "romfsimg.h"


TEST_GROUP(xromfs);

static XRomFs* fs;


TEST_SETUP(xromfs)
{
    XError err = xromfs_init(fs, &bin2c_romfs_data);
    X_ASSERT(err == X_ERR_NONE);
}


TEST_TEAR_DOWN(xromfs)
{
    xromfs_deinit(fs);
}

TEST(xromfs, open_read)
{
    XFile* fp;
    XError err;
    const char name[] = "foo.txt";

    err = xromfs_open(fs, name, X_OPEN_MODE_READ, &fp);
    TEST_ASSERT_NOT_NULL(fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    xromfs_close(fp);
}


TEST(xromfs, read)
{
    XFile* fp;
    XError err;
    char buf[256];
    size_t nread;
    const char name[] = "foo.txt";

    err = xromfs_open(fs, name, X_OPEN_MODE_READ, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_NOT_NULL(fp);

    err = xromfs_read(fp, buf, sizeof(buf), &nread);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_TRUE(nread < sizeof(buf));
    buf[nread] = '\0';

    TEST_ASSERT_EQUAL_STRING("Hello Foo", x_strstrip(buf, NULL));
    xromfs_close(fp);
}


TEST(xromfs, stat)
{
    XError err;
    XStat statbuf;
    const char* names[] = {
        "/",
        "foo",
        "/foo.txt",
        "foo.txt",
        "/foo/bar",
        "/foo/bar.txt",
        "/foo/bar/baz.txt",
    };

    int i;
    for (i = 0; i < (int)X_COUNT_OF(names); i++)
    {
        const char* name = names[1];
        err = xromfs_stat(fs, name, &statbuf);
        TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    }

    err = xromfs_stat(fs, "hogehogefoobar", &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NO_ENTRY, err);
}


TEST(xromfs, tell)
{
    XFile* fp;
    XError err;
    XSize pos;
    char buf[256];
    size_t nread;
    const char name[] = "foo.txt";

    err = xromfs_open(fs, name, X_OPEN_MODE_READ, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xromfs_tell(fp, &pos);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(0, pos);

    err = xromfs_read(fp, buf, sizeof(buf), &nread);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_TRUE(nread < sizeof(buf));
    buf[nread] = '\0';

    err = xromfs_tell(fp, &pos);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(strlen(buf), pos);

    xromfs_close(fp);
}


TEST(xromfs, seek)
{
    XFile* fp;
    XError err;
    char buf[256];
    size_t nread;
    char name[] = "foo.txt";
    XSize pos;

    err = xromfs_open(fs, name, X_OPEN_MODE_READ, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xromfs_seek(fp, 6, X_SEEK_SET);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(6, (xromfs_tell(fp, &pos), pos));

    err = xromfs_seek(fp, 10, X_SEEK_CUR);
    TEST_ASSERT_EQUAL(16, (xromfs_tell(fp, &pos), pos));

    err = xromfs_seek(fp, 6, X_SEEK_SET);
    err = xromfs_read(fp, buf, sizeof(buf), &nread);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_TRUE(nread < sizeof(buf));
    buf[nread] = '\0';
    TEST_ASSERT_EQUAL_STRING("Foo", x_strstrip(buf, NULL));

    err = xromfs_seek(fp, -6, X_SEEK_END);
    err = xromfs_read(fp, buf, sizeof(buf), &nread);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_TRUE(nread < sizeof(buf));
    buf[nread] = '\0';
    TEST_ASSERT_EQUAL_STRING("Foo", x_strstrip(buf, NULL));

    xromfs_close(fp);
}


TEST(xromfs, opendir)
{
    XDir* dir;
    XError err;
    int i;

    {
        const char* names[] = {
            "/",
            "/foo",
            "/foo/bar",
            "foo",
            "foo/bar",
            "foo//",
            "foo/bar////",
        };

        for (i = 0; i < (int)(X_COUNT_OF(names)); ++i)
        {
            err = xromfs_opendir(fs, names[i], &dir);
            TEST_ASSERT_EQUAL(X_ERR_NONE, err);
            TEST_ASSERT_NOT_NULL(dir);
            xromfs_closedir(dir);
        }
    }


    {
        const char* names[] = {
            "foo.txt",
            "foo/bar.txt",
            "foo/bar/baz.txt",
        };

        for (i = 0; i < (int)(X_COUNT_OF(names)); ++i)
        {
            err = xromfs_opendir(fs, names[i], &dir);
            TEST_ASSERT_EQUAL(X_ERR_NOT_DIRECTORY, err);
            TEST_ASSERT_NULL(dir);
        }
    }

    {
        const char* names[] = {
            "hoge.txt",
            "hoge/bar.txt",
            "hoge/bar/baz.txt",
        };

        for (i = 0; i < (int)(X_COUNT_OF(names)); ++i)
        {
            err = xromfs_opendir(fs, names[i], &dir);
            TEST_ASSERT_EQUAL(X_ERR_NO_ENTRY, err);
            TEST_ASSERT_NULL(dir);
        }
    }
}


TEST(xromfs, readdir)
{
    XDir* dir;
    XError err;
    XDirEnt entbuf;
    XDirEnt* ent;

    xromfs_opendir(fs, "/foo", &dir);
    while ((err = xromfs_readdir(dir, &entbuf, &ent)), ent)
    {
        if (strcmp(".", ent->name) == 0)
            continue;

        if (strcmp("..", ent->name) == 0)
            continue;

        if (strcmp("bar", ent->name) == 0)
            continue;

        if (strcmp("bar.txt", ent->name) == 0)
            continue;

        TEST_ASSERT_EQUAL_STRING("error", ent->name);
    }
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    xromfs_closedir(dir);
}


TEST(xromfs, stream)
{
#if 0
    XFile* fp;
    XError err;
    XStream st;
    const char name[] = "foo.txt";

    // err = xromfs_open(fs, name, X_OPEN_MODE_WRITE_PLUS, &fp);
    // TEST_ASSERT_NOT_NULL(fp);
    // TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    // xromfs_init_stream(&st, fp);
    // // x_test_stream(&st);

    // xromfs_close(fp);
#endif
}


TEST_GROUP_RUNNER(xromfs)
{
    fs = x_malloc(sizeof(XRomFs));

    TEST_ASSERT_NOT_NULL(fs);

    RUN_TEST_CASE(xromfs, open_read);
    RUN_TEST_CASE(xromfs, read);
    RUN_TEST_CASE(xromfs, stat);
    RUN_TEST_CASE(xromfs, tell);
    RUN_TEST_CASE(xromfs, seek);
    RUN_TEST_CASE(xromfs, opendir);
    RUN_TEST_CASE(xromfs, readdir);
    RUN_TEST_CASE(xromfs, stream);

    x_free(fs);
}
