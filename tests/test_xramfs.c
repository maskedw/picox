#include <picox/filesystem/xramfs.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "testutils.h"


TEST_GROUP(xramfs);

static XRamFs* fs;
static const char WRITE_DATA[] = "Hello world";
static const size_t WRITE_LEN = 11; // strlen WRITE_DATA


TEST_SETUP(xramfs)
{
    xramfs_init(fs, NULL, 2048);
}


TEST_TEAR_DOWN(xramfs)
{
    xramfs_deinit(fs);
}


TEST(xramfs, open_write)
{
    XFile* fp;
    XError err;

    err = xramfs_open(fs, "foo.txt", X_OPEN_MODE_WRITE, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_NOT_NULL(fp);
    xramfs_close(fp);
}


TEST(xramfs, open_read)
{
    XFile* fp;
    XError err;
    const char name[] = "foo.txt";

    err = xramfs_open(fs, name, X_OPEN_MODE_READ, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NO_ENTRY, err);
    TEST_ASSERT_NULL(fp);

    err = xramfs_open(fs, name, X_OPEN_MODE_WRITE, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_NOT_NULL(fp);
    xramfs_close(fp);

    err = xramfs_open(fs, name, X_OPEN_MODE_READ, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_NOT_NULL(fp);
    xramfs_close(fp);
}


TEST(xramfs, write)
{
    XFile* fp;
    XError err;
    size_t nwritten;
    const char name[] = "foo.txt";


    err = xramfs_open(fs, name, X_OPEN_MODE_WRITE, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_NOT_NULL(fp);

    err = xramfs_write(fp, WRITE_DATA, WRITE_LEN, &nwritten);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nwritten);
    err = xramfs_write(fp, WRITE_DATA, WRITE_LEN, &nwritten);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nwritten);
    err = xramfs_write(fp, WRITE_DATA, WRITE_LEN, &nwritten);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nwritten);
    err = xramfs_write(fp, WRITE_DATA, WRITE_LEN, &nwritten);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nwritten);
    err = xramfs_write(fp, WRITE_DATA, WRITE_LEN, &nwritten);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nwritten);
    err = xramfs_write(fp, WRITE_DATA, WRITE_LEN, &nwritten);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nwritten);

    xramfs_close(fp);
}


TEST(xramfs, read)
{
    XFile* fp;
    XError err;
    char buf[256];
    size_t nread;
    const char name[] = "foo.txt";

    err = xramfs_open(fs, name, X_OPEN_MODE_WRITE, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xramfs_write(fp, WRITE_DATA, WRITE_LEN, NULL);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    xramfs_close(fp);

    err = xramfs_open(fs, name, X_OPEN_MODE_READ, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_NOT_NULL(fp);

    err = xramfs_read(fp, buf, WRITE_LEN, &nread);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nread);
    xramfs_close(fp);
}


TEST(xramfs, stat)
{
    XFile* fp;
    XError err;
    XStat statbuf;
    const char name[] = "foo.txt";

    err = xramfs_open(fs, name, X_OPEN_MODE_WRITE, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xramfs_write(fp, WRITE_DATA, WRITE_LEN, NULL);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    xramfs_close(fp);

    err = xramfs_stat(fs, name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, statbuf.size);
}


TEST(xramfs, tell)
{
    XFile* fp;
    XError err;
    XSize pos;
    const char name[] = "foo.txt";

    err = xramfs_open(fs, name, X_OPEN_MODE_WRITE, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xramfs_tell(fp, &pos);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(0, pos);

    err = xramfs_write(fp, WRITE_DATA, WRITE_LEN, NULL);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xramfs_tell(fp, &pos);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, pos);

    xramfs_close(fp);
}


TEST(xramfs, seek)
{
    XFile* fp;
    XError err;
    char name[] = "foo.txt";
    XSize pos;

    err = xramfs_open(fs, name, X_OPEN_MODE_WRITE_PLUS, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xramfs_seek(fp, 10, X_SEEK_SET);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(10, (xramfs_tell(fp, &pos), pos));

    err = xramfs_seek(fp, 10, X_SEEK_CUR);
    TEST_ASSERT_EQUAL(20, (xramfs_tell(fp, &pos), pos));

    err = xramfs_seek(fp, 10, X_SEEK_END);
    TEST_ASSERT_EQUAL(10, (xramfs_tell(fp, &pos), pos));


    err = xramfs_seek(fp, 9, X_SEEK_END);
    xramfs_write(fp, "A", 1, NULL);
    TEST_ASSERT_EQUAL(10, (xramfs_tell(fp, &pos), pos));

    err = xramfs_seek(fp, 10, X_SEEK_CUR);
    TEST_ASSERT_EQUAL(20, (xramfs_tell(fp, &pos), pos));

    err = xramfs_seek(fp, -10, X_SEEK_CUR);
    TEST_ASSERT_EQUAL(10, (xramfs_tell(fp, &pos), pos));

    err = xramfs_seek(fp, -10, X_SEEK_CUR);
    TEST_ASSERT_EQUAL(0, (xramfs_tell(fp, &pos), pos));

    xramfs_close(fp);
}


TEST(xramfs, flush)
{
    XFile* fp;
    XError err;
    char name[] = "foo.txt";

    err = xramfs_open(fs, name, X_OPEN_MODE_WRITE, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    xramfs_write(fp, "A", 1, NULL);
    err = xramfs_flush(fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    xramfs_close(fp);
}


TEST(xramfs, mkdir)
{
    XError err;
    char name[] = "foo";
    char name2[] = "foo/bar";
    XStat statbuf;

    err = xramfs_mkdir(fs, name);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    err = xramfs_stat(fs, name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xramfs_mkdir(fs, name2);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    err = xramfs_stat(fs, name2, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
}


TEST(xramfs, opendir)
{
    XDir* dir;
    XError err;
    char name[] = "foo";

    xramfs_mkdir(fs, name);
    err = xramfs_opendir(fs, name, &dir);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_NOT_NULL(dir);
    xramfs_closedir(dir);
}


TEST(xramfs, readdir)
{
    XFile* fp;
    XDir* dir;
    XError err;
    XDirEnt entbuf;
    XDirEnt* ent;
    char name[] = "foo";
    char name2[] = "bar.txt";

    xramfs_mkdir(fs, name);
    xramfs_open(fs, name2, X_OPEN_MODE_WRITE, &fp);
    xramfs_close(fp);
    xramfs_opendir(fs, name, &dir);

    while ((err = xramfs_readdir(dir, &entbuf, &ent)), ent)
    {
        if (strcmp(".", ent->name) == 0)
            continue;

        if (strcmp("..", ent->name) == 0)
            continue;
        TEST_ASSERT_EQUAL_STRING(name2, ent->name);
    }
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    xramfs_closedir(dir);
}


TEST(xramfs, remove)
{
    XFile* fp;
    XError err;
    XStat statbuf;
    char name[] = "foo";

    err = xramfs_stat(fs, name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NO_ENTRY, err);

    xramfs_open(fs, name, X_OPEN_MODE_WRITE, &fp);

    xramfs_close(fp);

    err = xramfs_stat(fs, name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xramfs_remove(fs, name);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xramfs_stat(fs, name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NO_ENTRY, err);

    err = xramfs_mkdir(fs, name);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xramfs_stat(fs, name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xramfs_remove(fs, name);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xramfs_stat(fs, name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NO_ENTRY, err);
}


TEST(xramfs, rename)
{
    XFile* fp;
    XError err;
    XStat statbuf;
    char name[] = "foo";
    char name2[] = "bar";

    xramfs_open(fs, name, X_OPEN_MODE_WRITE, &fp);
    err = xramfs_write(fp, WRITE_DATA, WRITE_LEN, NULL);
    err = xramfs_write(fp, WRITE_DATA, WRITE_LEN, NULL);
    err = xramfs_write(fp, WRITE_DATA, WRITE_LEN, NULL);
    err = xramfs_write(fp, WRITE_DATA, WRITE_LEN, NULL);
    err = xramfs_write(fp, WRITE_DATA, WRITE_LEN, NULL);

    xramfs_close(fp);

    err = xramfs_stat(fs, name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xramfs_stat(fs, name2, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NO_ENTRY, err);

    err = xramfs_rename(fs, name, name2);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xramfs_stat(fs, name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NO_ENTRY, err);

    err = xramfs_stat(fs, name2, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

}


TEST(xramfs, open_write_plus)
{
    XFile* fp;
    XError err;
    XStat statbuf;
    size_t nwritten;
    size_t nread;
    char buf[WRITE_LEN];
    const char name[] = "foo.txt";

    /* ファイルが存在しない場合は新規作成される */
    err = xramfs_open(fs, name, X_OPEN_MODE_WRITE_PLUS, &fp);
    TEST_ASSERT_NOT_NULL(fp);
    err = xramfs_write(fp, WRITE_DATA, WRITE_LEN, &nwritten);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nwritten);
    xramfs_close(fp);

    err = xramfs_stat(fs, name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, statbuf.size);

    /* ファイルが存在している場合はサイズは0に切り詰められる */
    err = xramfs_open(fs, name, X_OPEN_MODE_WRITE_PLUS, &fp);
    TEST_ASSERT_NOT_NULL(fp);
    xramfs_close(fp);
    err = xramfs_stat(fs, name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(0, statbuf.size);

    /* 読み込みも可能 */
    err = xramfs_open(fs, name, X_OPEN_MODE_WRITE_PLUS, &fp);
    TEST_ASSERT_NOT_NULL(fp);
    err = xramfs_write(fp, WRITE_DATA, WRITE_LEN, &nwritten);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nwritten);

    err = xramfs_seek(fp, 0, X_SEEK_SET);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xramfs_read(fp, buf, WRITE_LEN, &nread);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nread);
    TEST_ASSERT_TRUE(memcmp(buf, WRITE_DATA, WRITE_LEN) == 0);

    xramfs_close(fp);
}


TEST(xramfs, open_read_plus)
{
    XFile* fp;
    XError err;
    XStat statbuf;
    size_t nwritten;
    size_t nread;
    char buf[WRITE_LEN * 2];
    const char name[] = "foo.txt";

    /* ファイルが存在しない場合はエラーになる。 */
    err = xramfs_open(fs, name, X_OPEN_MODE_READ_PLUS, &fp);
    TEST_ASSERT_NULL(fp);

    err = xramfs_open(fs, name, X_OPEN_MODE_WRITE, &fp);
    TEST_ASSERT_NOT_NULL(fp);

    err = xramfs_write(fp, WRITE_DATA, WRITE_LEN, &nwritten);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nwritten);
    xramfs_close(fp);

    err = xramfs_stat(fs, name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, statbuf.size);

    /* ファイルが存在している場合、サイズ0にはならない */
    err = xramfs_open(fs, name, X_OPEN_MODE_READ_PLUS, &fp);
    TEST_ASSERT_NOT_NULL(fp);
    xramfs_close(fp);
    err = xramfs_stat(fs, name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, statbuf.size);

    /* 読み書きできる */
    err = xramfs_open(fs, name, X_OPEN_MODE_READ_PLUS, &fp);
    TEST_ASSERT_NOT_NULL(fp);

    err = xramfs_read(fp, buf, WRITE_LEN, &nread);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nread);
    TEST_ASSERT_TRUE(memcmp(buf, WRITE_DATA, WRITE_LEN) == 0);

    err = xramfs_seek(fp, 0, X_SEEK_END);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xramfs_write(fp, WRITE_DATA, WRITE_LEN, &nwritten);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nwritten);

    err = xramfs_seek(fp, 0, X_SEEK_SET);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xramfs_read(fp, buf, WRITE_LEN * 2, &nread);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN * 2, nread);
    TEST_ASSERT_TRUE(memcmp(buf, WRITE_DATA, WRITE_LEN) == 0);
    TEST_ASSERT_TRUE(memcmp(buf + WRITE_LEN, WRITE_DATA, WRITE_LEN) == 0);

    xramfs_close(fp);
}


TEST(xramfs, open_append)
{
    XFile* fp;
    XError err;
    XStat statbuf;
    XSize pos;
    size_t nwritten;
    size_t nread;
    char buf[WRITE_LEN * 2];
    const char name[] = "foo.txt";

    /* ファイルが存在しない場合は新規作成される */
    err = xramfs_open(fs, name, X_OPEN_MODE_APPEND, &fp);
    TEST_ASSERT_NOT_NULL(fp);

    err = xramfs_write(fp, WRITE_DATA, WRITE_LEN, &nwritten);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nwritten);
    xramfs_close(fp);

    err = xramfs_stat(fs, name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, statbuf.size);

    /* ファイルが存在している場合はファイルポインタは末尾にセットされる */
    err = xramfs_open(fs, name, X_OPEN_MODE_APPEND, &fp);
    TEST_ASSERT_NOT_NULL(fp);

    err = xramfs_tell(fp, &pos);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, pos);

    /* 読み込みは不可 */
    err = xramfs_read(fp, buf, WRITE_LEN, &nread);
    TEST_ASSERT_NOT_EQUAL(X_ERR_NONE, err);

    /* 書き込みは可 */
    err = xramfs_write(fp, WRITE_DATA, WRITE_LEN, &nwritten);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nwritten);

    xramfs_close(fp);
}


TEST(xramfs, open_append_plus)
{
    XFile* fp;
    XError err;
    XStat statbuf;
    XSize pos;
    size_t nwritten;
    size_t nread;
    char buf[WRITE_LEN * 2];
    const char name[] = "foo.txt";

    /* ファイルが存在しない場合は新規作成される */
    err = xramfs_open(fs, name, X_OPEN_MODE_APPEND_PLUS, &fp);
    TEST_ASSERT_NOT_NULL(fp);

    err = xramfs_write(fp, WRITE_DATA, WRITE_LEN, &nwritten);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nwritten);
    xramfs_close(fp);

    err = xramfs_stat(fs, name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, statbuf.size);

    /* ファイルが存在している場合はファイルポインタは先頭にセットされる */
    err = xramfs_open(fs, name, X_OPEN_MODE_APPEND_PLUS, &fp);
    TEST_ASSERT_NOT_NULL(fp);

    err = xramfs_tell(fp, &pos);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(0, pos);

    /* 読み書きできる */
    err = xramfs_write(fp, WRITE_DATA, WRITE_LEN, &nwritten);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nwritten);

    err = xramfs_seek(fp, 0, X_SEEK_SET);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xramfs_read(fp, buf, WRITE_LEN * 2, &nread);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN * 2, nread);
    TEST_ASSERT_TRUE(memcmp(buf, WRITE_DATA, WRITE_LEN) == 0);
    TEST_ASSERT_TRUE(memcmp(buf + WRITE_LEN, WRITE_DATA, WRITE_LEN) == 0);

    xramfs_close(fp);
}


TEST(xramfs, stream)
{
    XFile* fp;
    XError err;
    XStream st;
    const char name[] = "foo.txt";

    err = xramfs_open(fs, name, X_OPEN_MODE_WRITE_PLUS, &fp);
    TEST_ASSERT_NOT_NULL(fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    xramfs_init_stream(&st, fp);
    x_test_stream(&st);

    xramfs_close(fp);
}


TEST_GROUP_RUNNER(xramfs)
{
    fs = x_malloc(sizeof(XRamFs));

    TEST_ASSERT_NOT_NULL(fs);

    RUN_TEST_CASE(xramfs, open_write);
    RUN_TEST_CASE(xramfs, open_read);
    RUN_TEST_CASE(xramfs, write);
    RUN_TEST_CASE(xramfs, read);
    RUN_TEST_CASE(xramfs, stat);
    RUN_TEST_CASE(xramfs, tell);
    RUN_TEST_CASE(xramfs, seek);
    RUN_TEST_CASE(xramfs, flush);
    RUN_TEST_CASE(xramfs, mkdir);
    RUN_TEST_CASE(xramfs, opendir);
    RUN_TEST_CASE(xramfs, readdir);
    RUN_TEST_CASE(xramfs, remove);
    RUN_TEST_CASE(xramfs, rename);
    RUN_TEST_CASE(xramfs, open_write_plus);
    RUN_TEST_CASE(xramfs, open_read_plus);
    RUN_TEST_CASE(xramfs, open_append);
    RUN_TEST_CASE(xramfs, open_append_plus);
    RUN_TEST_CASE(xramfs, stream);

    x_free(fs);
}
