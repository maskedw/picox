#include <picox/filesystem/xfatfs.h>
#include <ff.h>
#include "testutils.h"


TEST_GROUP(xfatfs);

static XFatFs* fs;
static FATFS* fatfs;
static const char WRITE_DATA[] = "Hello world";
static const size_t WRITE_LEN = 11; // strlen WRITE_DATA
void disk_deinit(BYTE pdrv);


TEST_SETUP(xfatfs)
{
    fs = malloc(sizeof(XFatFs));
    fatfs = malloc(sizeof(FATFS));
    TEST_ASSERT_NOT_NULL(fs);
    TEST_ASSERT_NOT_NULL(fatfs);

    FRESULT res;
    res = f_mount(fatfs, "0:", 0);
    TEST_ASSERT_EQUAL(FR_OK, res);
    res = f_mkfs("0:", 0, 0);
    TEST_ASSERT_EQUAL(FR_OK, res);
    xfatfs_init(fs);

}


TEST_TEAR_DOWN(xfatfs)
{
    xfatfs_deinit(fs);
    f_mount(NULL, "0:", 0);
    free(fs);
    free(fatfs);
    disk_deinit(0);
}


TEST(xfatfs, open_write)
{
    XFile* fp;
    XError err;

    err = xfatfs_open(fs, "foo.txt", X_OPEN_MODE_WRITE, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_NOT_NULL(fp);
    xfatfs_close(fp);
}


TEST(xfatfs, open_read)
{
    XFile* fp;
    XError err;
    const char name[] = "foo.txt";

    err = xfatfs_open(fs, name, X_OPEN_MODE_READ, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NO_ENTRY, err);
    TEST_ASSERT_NULL(fp);
    xfatfs_close(fp);

    err = xfatfs_open(fs, name, X_OPEN_MODE_WRITE, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_NOT_NULL(fp);
    xfatfs_close(fp);

    err = xfatfs_open(fs, name, X_OPEN_MODE_READ, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_NOT_NULL(fp);
    xfatfs_close(fp);
}


TEST(xfatfs, write)
{
    XFile* fp;
    XError err;
    size_t nwritten;
    const char name[] = "foo.txt";


    err = xfatfs_open(fs, name, X_OPEN_MODE_WRITE, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_NOT_NULL(fp);

    err = xfatfs_write(fp, WRITE_DATA, WRITE_LEN, &nwritten);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nwritten);

    xfatfs_close(fp);
}


TEST(xfatfs, read)
{
    XFile* fp;
    XError err;
    char buf[256];
    size_t nread;
    const char name[] = "foo.txt";

    err = xfatfs_open(fs, name, X_OPEN_MODE_WRITE, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xfatfs_write(fp, WRITE_DATA, WRITE_LEN, NULL);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    xfatfs_close(fp);

    err = xfatfs_open(fs, name, X_OPEN_MODE_READ, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_NOT_NULL(fp);

    err = xfatfs_read(fp, buf, WRITE_LEN, &nread);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nread);
    xfatfs_close(fp);
}


TEST(xfatfs, stat)
{
    XFile* fp;
    XError err;
    XStat statbuf;
    const char name[] = "foo.txt";

    err = xfatfs_open(fs, name, X_OPEN_MODE_WRITE, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xfatfs_write(fp, WRITE_DATA, WRITE_LEN, NULL);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    xfatfs_close(fp);

    err = xfatfs_stat(fs, name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, statbuf.size);
}


TEST(xfatfs, tell)
{
    XFile* fp;
    XError err;
    XSize pos;
    const char name[] = "foo.txt";

    err = xfatfs_open(fs, name, X_OPEN_MODE_WRITE, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xfatfs_tell(fp, &pos);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(0, pos);

    err = xfatfs_write(fp, WRITE_DATA, WRITE_LEN, NULL);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xfatfs_tell(fp, &pos);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, pos);

    xfatfs_close(fp);
}


TEST(xfatfs, seek)
{
    XFile* fp;
    XError err;
    char name[] = "foo.txt";
    XSize pos;

    err = xfatfs_open(fs, name, X_OPEN_MODE_WRITE_PLUS, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    // err = xfatfs_seek(fp, 10, X_SEEK_SET);
    // TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    // TEST_ASSERT_EQUAL(10, (xfatfs_tell(fp, &pos), pos));

    // err = xfatfs_seek(fp, 10, X_SEEK_CUR);
    // TEST_ASSERT_EQUAL(20, (xfatfs_tell(fp, &pos), pos));

    // err = xfatfs_seek(fp, 10, X_SEEK_END);
    // TEST_ASSERT_EQUAL(10, (xfatfs_tell(fp, &pos), pos));


    err = xfatfs_seek(fp, 9, X_SEEK_END);
    xfatfs_write(fp, "A", 1, NULL);
    TEST_ASSERT_EQUAL(10, (xfatfs_tell(fp, &pos), pos));

    // err = xfatfs_seek(fp, 10, X_SEEK_CUR);
    // TEST_ASSERT_EQUAL(20, (xfatfs_tell(fp, &pos), pos));

    // err = xfatfs_seek(fp, -10, X_SEEK_CUR);
    // TEST_ASSERT_EQUAL(10, (xfatfs_tell(fp, &pos), pos));

    // err = xfatfs_seek(fp, -10, X_SEEK_CUR);
    // TEST_ASSERT_EQUAL(0, (xfatfs_tell(fp, &pos), pos));

    xfatfs_close(fp);
}


TEST(xfatfs, flush)
{
    XFile* fp;
    XError err;
    char name[] = "foo.txt";

    err = xfatfs_open(fs, name, X_OPEN_MODE_WRITE, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    xfatfs_write(fp, "A", 1, NULL);
    err = xfatfs_flush(fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    xfatfs_close(fp);
}


TEST(xfatfs, mkdir)
{
    XError err;
    char name[] = "foo";
    XStat statbuf;

    err = xfatfs_mkdir(fs, name);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    err = xfatfs_stat(fs, name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
}


TEST(xfatfs, opendir)
{
    XDir* dir;
    XError err;
    char name[] = "foo";

    xfatfs_mkdir(fs, name);
    err = xfatfs_opendir(fs, name, &dir);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_NOT_NULL(dir);
    xfatfs_closedir(dir);

    err = xfatfs_opendir(fs, "/", &dir);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_NOT_NULL(dir);
    xfatfs_closedir(dir);
}


TEST(xfatfs, readdir)
{
    XFile* fp;
    XDir* dir;
    XError err;
    XDirEnt entbuf;
    XDirEnt* ent;
    char name[] = "foo";
    char name2[] = "bar.txt";

    xfatfs_mkdir(fs, name);
    xfatfs_open(fs, name2, X_OPEN_MODE_WRITE, &fp);
    xfatfs_close(fp);
    xfatfs_opendir(fs, name, &dir);

    while ((err = xfatfs_readdir(dir, &entbuf, &ent)), ent)
    {
        if (strcmp(".", ent->name) == 0)
            continue;

        if (strcmp("..", ent->name) == 0)
            continue;
        TEST_ASSERT_EQUAL_STRING(name2, ent->name);
    }
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    xfatfs_closedir(dir);
}


TEST(xfatfs, remove)
{
    XFile* fp;
    XError err;
    XStat statbuf;
    char name[] = "foo";

    err = xfatfs_stat(fs, name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NO_ENTRY, err);

    xfatfs_open(fs, name, X_OPEN_MODE_WRITE, &fp);

    xfatfs_close(fp);

    err = xfatfs_stat(fs, name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xfatfs_remove(fs, name);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xfatfs_stat(fs, name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NO_ENTRY, err);

    err = xfatfs_mkdir(fs, name);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xfatfs_stat(fs, name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xfatfs_remove(fs, name);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xfatfs_stat(fs, name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NO_ENTRY, err);
}


TEST(xfatfs, rename)
{
    XFile* fp;
    XError err;
    XStat statbuf;
    char name[] = "foo";
    char name2[] = "bar";

    xfatfs_open(fs, name, X_OPEN_MODE_WRITE, &fp);
    xfatfs_close(fp);

    err = xfatfs_stat(fs, name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xfatfs_stat(fs, name2, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NO_ENTRY, err);

    err = xfatfs_rename(fs, name, name2);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xfatfs_stat(fs, name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NO_ENTRY, err);

    err = xfatfs_stat(fs, name2, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

}


TEST(xfatfs, open_write_plus)
{
    XFile* fp;
    XError err;
    XStat statbuf;
    size_t nwritten;
    size_t nread;
    char buf[WRITE_LEN];
    const char name[] = "foo.txt";

    /* ファイルが存在しない場合は新規作成される */
    err = xfatfs_open(fs, name, X_OPEN_MODE_WRITE_PLUS, &fp);
    TEST_ASSERT_NOT_NULL(fp);
    err = xfatfs_write(fp, WRITE_DATA, WRITE_LEN, &nwritten);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nwritten);
    xfatfs_close(fp);

    err = xfatfs_stat(fs, name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, statbuf.size);

    /* ファイルが存在している場合はサイズは0に切り詰められる */
    err = xfatfs_open(fs, name, X_OPEN_MODE_WRITE_PLUS, &fp);
    TEST_ASSERT_NOT_NULL(fp);
    xfatfs_close(fp);
    err = xfatfs_stat(fs, name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(0, statbuf.size);

    /* 読み込みも可能 */
    err = xfatfs_open(fs, name, X_OPEN_MODE_WRITE_PLUS, &fp);
    TEST_ASSERT_NOT_NULL(fp);
    err = xfatfs_write(fp, WRITE_DATA, WRITE_LEN, &nwritten);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nwritten);

    err = xfatfs_seek(fp, 0, X_SEEK_SET);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xfatfs_read(fp, buf, WRITE_LEN, &nread);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nread);
    TEST_ASSERT_TRUE(memcmp(buf, WRITE_DATA, WRITE_LEN) == 0);

    xfatfs_close(fp);
}


TEST(xfatfs, open_read_plus)
{
    XFile* fp;
    XError err;
    XStat statbuf;
    size_t nwritten;
    size_t nread;
    char buf[WRITE_LEN * 2];
    const char name[] = "foo.txt";

    /* ファイルが存在しない場合はエラーになる。 */
    err = xfatfs_open(fs, name, X_OPEN_MODE_READ_PLUS, &fp);
    TEST_ASSERT_NULL(fp);

    err = xfatfs_open(fs, name, X_OPEN_MODE_WRITE, &fp);
    TEST_ASSERT_NOT_NULL(fp);

    err = xfatfs_write(fp, WRITE_DATA, WRITE_LEN, &nwritten);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nwritten);
    xfatfs_close(fp);

    err = xfatfs_stat(fs, name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, statbuf.size);

    /* ファイルが存在している場合、サイズ0にはならない */
    err = xfatfs_open(fs, name, X_OPEN_MODE_READ_PLUS, &fp);
    TEST_ASSERT_NOT_NULL(fp);
    xfatfs_close(fp);
    err = xfatfs_stat(fs, name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, statbuf.size);

    /* 読み書きできる */
    err = xfatfs_open(fs, name, X_OPEN_MODE_READ_PLUS, &fp);
    TEST_ASSERT_NOT_NULL(fp);

    err = xfatfs_read(fp, buf, WRITE_LEN, &nread);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nread);
    TEST_ASSERT_TRUE(memcmp(buf, WRITE_DATA, WRITE_LEN) == 0);

    err = xfatfs_seek(fp, 0, X_SEEK_END);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xfatfs_write(fp, WRITE_DATA, WRITE_LEN, &nwritten);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nwritten);

    err = xfatfs_seek(fp, 0, X_SEEK_SET);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xfatfs_read(fp, buf, WRITE_LEN * 2, &nread);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN * 2, nread);
    TEST_ASSERT_TRUE(memcmp(buf, WRITE_DATA, WRITE_LEN) == 0);
    TEST_ASSERT_TRUE(memcmp(buf + WRITE_LEN, WRITE_DATA, WRITE_LEN) == 0);

    xfatfs_close(fp);
}


TEST(xfatfs, open_append)
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
    err = xfatfs_open(fs, name, X_OPEN_MODE_APPEND, &fp);
    TEST_ASSERT_NOT_NULL(fp);

    err = xfatfs_write(fp, WRITE_DATA, WRITE_LEN, &nwritten);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nwritten);
    xfatfs_close(fp);

    err = xfatfs_stat(fs, name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, statbuf.size);

    /* ファイルが存在している場合はファイルポインタは末尾にセットされる */
    err = xfatfs_open(fs, name, X_OPEN_MODE_APPEND, &fp);
    TEST_ASSERT_NOT_NULL(fp);

    err = xfatfs_tell(fp, &pos);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, pos);

    /* 読み込みは不可 */
    err = xfatfs_read(fp, buf, WRITE_LEN, &nread);
    TEST_ASSERT_NOT_EQUAL(X_ERR_NONE, err);

    /* 書き込みは可 */
    err = xfatfs_write(fp, WRITE_DATA, WRITE_LEN, &nwritten);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nwritten);

    xfatfs_close(fp);
}


TEST(xfatfs, open_append_plus)
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
    err = xfatfs_open(fs, name, X_OPEN_MODE_APPEND_PLUS, &fp);
    TEST_ASSERT_NOT_NULL(fp);

    err = xfatfs_write(fp, WRITE_DATA, WRITE_LEN, &nwritten);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nwritten);
    xfatfs_close(fp);

    err = xfatfs_stat(fs, name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, statbuf.size);

    /* ファイルが存在している場合はファイルポインタは先頭にセットされる */
    err = xfatfs_open(fs, name, X_OPEN_MODE_APPEND_PLUS, &fp);
    TEST_ASSERT_NOT_NULL(fp);

    err = xfatfs_tell(fp, &pos);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(0, pos);

    /* 読み書きできる */
    err = xfatfs_write(fp, WRITE_DATA, WRITE_LEN, &nwritten);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nwritten);

    err = xfatfs_seek(fp, 0, X_SEEK_SET);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xfatfs_read(fp, buf, WRITE_LEN * 2, &nread);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN * 2, nread);
    TEST_ASSERT_TRUE(memcmp(buf, WRITE_DATA, WRITE_LEN) == 0);
    TEST_ASSERT_TRUE(memcmp(buf + WRITE_LEN, WRITE_DATA, WRITE_LEN) == 0);

    xfatfs_close(fp);
}


TEST(xfatfs, stream)
{
    XFile* fp;
    XError err;
    XStream st;
    const char name[] = "foo.txt";

    err = xfatfs_open(fs, name, X_OPEN_MODE_WRITE_PLUS, &fp);
    TEST_ASSERT_NOT_NULL(fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    xfatfs_init_stream(&st, fp);
    x_test_stream(&st);

    xfatfs_close(fp);
}


TEST_GROUP_RUNNER(xfatfs)
{
    RUN_TEST_CASE(xfatfs, open_write);
    RUN_TEST_CASE(xfatfs, open_read);
    RUN_TEST_CASE(xfatfs, write);
    RUN_TEST_CASE(xfatfs, read);
    RUN_TEST_CASE(xfatfs, stat);
    RUN_TEST_CASE(xfatfs, tell);
    RUN_TEST_CASE(xfatfs, seek);
    RUN_TEST_CASE(xfatfs, flush);
    RUN_TEST_CASE(xfatfs, mkdir);
    RUN_TEST_CASE(xfatfs, opendir);
    RUN_TEST_CASE(xfatfs, readdir);
    RUN_TEST_CASE(xfatfs, remove);
    RUN_TEST_CASE(xfatfs, rename);
    RUN_TEST_CASE(xfatfs, open_write_plus);
    RUN_TEST_CASE(xfatfs, open_read_plus);
    RUN_TEST_CASE(xfatfs, open_append);
    RUN_TEST_CASE(xfatfs, open_append_plus);
    RUN_TEST_CASE(xfatfs, stream);
}
