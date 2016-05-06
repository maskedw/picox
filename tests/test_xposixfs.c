#include <picox/filesystem/xposixfs.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "testutils.h"


TEST_GROUP(xposixfs);
#ifndef PATH_MAX
#define PATH_MAX 1024
#endif

static XPosixFs* fs;
static char*     cwd;
static char*     workdir;
static const char WRITE_DATA[] = "Hello world";
static const size_t WRITE_LEN = 11; // strlen WRITE_DATA


TEST_SETUP(xposixfs)
{
    TEST_ASSERT_EQUAL(0, chdir(cwd));

    strcpy(workdir, cwd);
    strcat(workdir, "/posixfstmp");

    struct stat statbuf;
    if (stat(workdir, &statbuf) == 0)
        system("rm -r ./posixfstmp");

#ifdef _WIN32
    TEST_ASSERT_EQUAL(0, mkdir(workdir));
#else
    TEST_ASSERT_EQUAL(0, mkdir(workdir, 0777));
#endif
    TEST_ASSERT_EQUAL(0, chdir(workdir));
    TEST_ASSERT_EQUAL(0, chdir(workdir));
}


TEST_TEAR_DOWN(xposixfs)
{
}


TEST(xposixfs, open_write)
{
    XFile* fp;
    XError err;

    err = xposixfs_open(fs, "foo.txt", X_OPEN_MODE_WRITE, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_NOT_NULL(fp);
    xposixfs_close(fp);
}


TEST(xposixfs, open_read)
{
    XFile* fp;
    XError err;
    const char name[] = "foo.txt";

    err = xposixfs_open(fs, name, X_OPEN_MODE_READ, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NO_ENTRY, err);
    TEST_ASSERT_NULL(fp);

    err = xposixfs_open(fs, name, X_OPEN_MODE_WRITE, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_NOT_NULL(fp);
    xposixfs_close(fp);

    err = xposixfs_open(fs, name, X_OPEN_MODE_READ, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_NOT_NULL(fp);
    xposixfs_close(fp);
}


TEST(xposixfs, write)
{
    XFile* fp;
    XError err;
    size_t nwritten;
    const char name[] = "foo.txt";


    err = xposixfs_open(fs, name, X_OPEN_MODE_WRITE, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_NOT_NULL(fp);

    err = xposixfs_write(fp, WRITE_DATA, WRITE_LEN, &nwritten);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nwritten);

    xposixfs_close(fp);
}


TEST(xposixfs, read)
{
    XFile* fp;
    XError err;
    char buf[256];
    size_t nread;
    const char name[] = "foo.txt";

    err = xposixfs_open(fs, name, X_OPEN_MODE_WRITE, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xposixfs_write(fp, WRITE_DATA, WRITE_LEN, NULL);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    xposixfs_close(fp);

    err = xposixfs_open(fs, name, X_OPEN_MODE_READ, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_NOT_NULL(fp);

    err = xposixfs_read(fp, buf, WRITE_LEN, &nread);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nread);
    xposixfs_close(fp);
}


TEST(xposixfs, stat)
{
    XFile* fp;
    XError err;
    XStat statbuf;
    const char name[] = "foo.txt";

    err = xposixfs_open(fs, name, X_OPEN_MODE_WRITE, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xposixfs_write(fp, WRITE_DATA, WRITE_LEN, NULL);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    xposixfs_close(fp);

    err = xposixfs_stat(fs, name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, statbuf.size);
}


TEST(xposixfs, tell)
{
    XFile* fp;
    XError err;
    XSize pos;
    const char name[] = "foo.txt";

    err = xposixfs_open(fs, name, X_OPEN_MODE_WRITE, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xposixfs_tell(fp, &pos);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(0, pos);

    err = xposixfs_write(fp, WRITE_DATA, WRITE_LEN, NULL);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xposixfs_tell(fp, &pos);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, pos);

    xposixfs_close(fp);
}


TEST(xposixfs, seek)
{
    XFile* fp;
    XError err;
    char name[] = "foo.txt";
    XSize pos;

    err = xposixfs_open(fs, name, X_OPEN_MODE_WRITE_PLUS, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xposixfs_seek(fp, 10, X_SEEK_SET);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(10, (xposixfs_tell(fp, &pos), pos));

    err = xposixfs_seek(fp, 10, X_SEEK_CUR);
    TEST_ASSERT_EQUAL(20, (xposixfs_tell(fp, &pos), pos));

    err = xposixfs_seek(fp, 10, X_SEEK_END);
    TEST_ASSERT_EQUAL(10, (xposixfs_tell(fp, &pos), pos));


    err = xposixfs_seek(fp, 9, X_SEEK_END);
    xposixfs_write(fp, "A", 1, NULL);
    TEST_ASSERT_EQUAL(10, (xposixfs_tell(fp, &pos), pos));

    err = xposixfs_seek(fp, 10, X_SEEK_CUR);
    TEST_ASSERT_EQUAL(20, (xposixfs_tell(fp, &pos), pos));

    err = xposixfs_seek(fp, -10, X_SEEK_CUR);
    TEST_ASSERT_EQUAL(10, (xposixfs_tell(fp, &pos), pos));

    err = xposixfs_seek(fp, -10, X_SEEK_CUR);
    TEST_ASSERT_EQUAL(0, (xposixfs_tell(fp, &pos), pos));

    xposixfs_close(fp);
}


TEST(xposixfs, flush)
{
    XFile* fp;
    XError err;
    char name[] = "foo.txt";

    err = xposixfs_open(fs, name, X_OPEN_MODE_WRITE, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    xposixfs_write(fp, "A", 1, NULL);
    err = xposixfs_flush(fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    xposixfs_close(fp);
}


TEST(xposixfs, mkdir)
{
    XError err;
    char name[] = "foo";
    XStat statbuf;

    err = xposixfs_mkdir(fs, name);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    err = xposixfs_stat(fs, name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
}


TEST(xposixfs, opendir)
{
    XDir* dir;
    XError err;
    char name[] = "foo";

    xposixfs_mkdir(fs, name);
    err = xposixfs_opendir(fs, name, &dir);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_NOT_NULL(dir);
    xposixfs_closedir(dir);
}


TEST(xposixfs, readdir)
{
    XFile* fp;
    XDir* dir;
    XError err;
    XDirEnt entbuf;
    XDirEnt* ent;
    char name[] = "foo";
    char name2[] = "bar.txt";

    xposixfs_mkdir(fs, name);
    xposixfs_open(fs, name2, X_OPEN_MODE_WRITE, &fp);
    xposixfs_close(fp);
    xposixfs_opendir(fs, name, &dir);

    while ((err = xposixfs_readdir(dir, &entbuf, &ent)), ent)
    {
        if (strcmp(".", ent->name) == 0)
            continue;

        if (strcmp("..", ent->name) == 0)
            continue;
        TEST_ASSERT_EQUAL_STRING(name2, ent->name);
    }
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    xposixfs_closedir(dir);
}


TEST(xposixfs, remove)
{
    XFile* fp;
    XError err;
    XStat statbuf;
    char name[] = "foo";

    err = xposixfs_stat(fs, name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NO_ENTRY, err);

    xposixfs_open(fs, name, X_OPEN_MODE_WRITE, &fp);

    xposixfs_close(fp);

    err = xposixfs_stat(fs, name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xposixfs_remove(fs, name);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xposixfs_stat(fs, name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NO_ENTRY, err);

    err = xposixfs_mkdir(fs, name);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xposixfs_stat(fs, name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xposixfs_remove(fs, name);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xposixfs_stat(fs, name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NO_ENTRY, err);
}


TEST(xposixfs, rename)
{
    XFile* fp;
    XError err;
    XStat statbuf;
    char name[] = "foo";
    char name2[] = "bar";

    xposixfs_open(fs, name, X_OPEN_MODE_WRITE, &fp);
    xposixfs_close(fp);

    err = xposixfs_stat(fs, name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xposixfs_stat(fs, name2, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NO_ENTRY, err);

    err = xposixfs_rename(fs, name, name2);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xposixfs_stat(fs, name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NO_ENTRY, err);

    err = xposixfs_stat(fs, name2, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

}


TEST(xposixfs, open_write_plus)
{
    XFile* fp;
    XError err;
    XStat statbuf;
    size_t nwritten;
    size_t nread;
    char buf[WRITE_LEN];
    const char name[] = "foo.txt";

    /* ファイルが存在しない場合は新規作成される */
    err = xposixfs_open(fs, name, X_OPEN_MODE_WRITE_PLUS, &fp);
    TEST_ASSERT_NOT_NULL(fp);
    err = xposixfs_write(fp, WRITE_DATA, WRITE_LEN, &nwritten);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nwritten);
    xposixfs_close(fp);

    err = xposixfs_stat(fs, name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, statbuf.size);

    /* ファイルが存在している場合はサイズは0に切り詰められる */
    err = xposixfs_open(fs, name, X_OPEN_MODE_WRITE_PLUS, &fp);
    TEST_ASSERT_NOT_NULL(fp);
    xposixfs_close(fp);
    err = xposixfs_stat(fs, name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(0, statbuf.size);

    /* 読み込みも可能 */
    err = xposixfs_open(fs, name, X_OPEN_MODE_WRITE_PLUS, &fp);
    TEST_ASSERT_NOT_NULL(fp);
    err = xposixfs_write(fp, WRITE_DATA, WRITE_LEN, &nwritten);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nwritten);

    err = xposixfs_seek(fp, 0, X_SEEK_SET);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xposixfs_read(fp, buf, WRITE_LEN, &nread);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nread);
    TEST_ASSERT_TRUE(memcmp(buf, WRITE_DATA, WRITE_LEN) == 0);

    xposixfs_close(fp);
}


TEST(xposixfs, open_read_plus)
{
    XFile* fp;
    XError err;
    XStat statbuf;
    size_t nwritten;
    size_t nread;
    char buf[WRITE_LEN * 2];
    const char name[] = "foo.txt";

    /* ファイルが存在しない場合はエラーになる。 */
    err = xposixfs_open(fs, name, X_OPEN_MODE_READ_PLUS, &fp);
    TEST_ASSERT_NULL(fp);

    err = xposixfs_open(fs, name, X_OPEN_MODE_WRITE, &fp);
    TEST_ASSERT_NOT_NULL(fp);

    err = xposixfs_write(fp, WRITE_DATA, WRITE_LEN, &nwritten);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nwritten);
    xposixfs_close(fp);

    err = xposixfs_stat(fs, name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, statbuf.size);

    /* ファイルが存在している場合、サイズ0にはならない */
    err = xposixfs_open(fs, name, X_OPEN_MODE_READ_PLUS, &fp);
    TEST_ASSERT_NOT_NULL(fp);
    xposixfs_close(fp);
    err = xposixfs_stat(fs, name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, statbuf.size);

    /* 読み書きできる */
    err = xposixfs_open(fs, name, X_OPEN_MODE_READ_PLUS, &fp);
    TEST_ASSERT_NOT_NULL(fp);

    err = xposixfs_read(fp, buf, WRITE_LEN, &nread);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nread);
    TEST_ASSERT_TRUE(memcmp(buf, WRITE_DATA, WRITE_LEN) == 0);

    err = xposixfs_seek(fp, 0, X_SEEK_END);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xposixfs_write(fp, WRITE_DATA, WRITE_LEN, &nwritten);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nwritten);

    err = xposixfs_seek(fp, 0, X_SEEK_SET);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xposixfs_read(fp, buf, WRITE_LEN * 2, &nread);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN * 2, nread);
    TEST_ASSERT_TRUE(memcmp(buf, WRITE_DATA, WRITE_LEN) == 0);
    TEST_ASSERT_TRUE(memcmp(buf + WRITE_LEN, WRITE_DATA, WRITE_LEN) == 0);

    xposixfs_close(fp);
}


TEST(xposixfs, open_append)
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
    err = xposixfs_open(fs, name, X_OPEN_MODE_APPEND, &fp);
    TEST_ASSERT_NOT_NULL(fp);

    err = xposixfs_write(fp, WRITE_DATA, WRITE_LEN, &nwritten);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nwritten);
    xposixfs_close(fp);

    err = xposixfs_stat(fs, name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, statbuf.size);

    /* ファイルが存在している場合はファイルポインタは末尾にセットされる */
    err = xposixfs_open(fs, name, X_OPEN_MODE_APPEND, &fp);
    TEST_ASSERT_NOT_NULL(fp);

    err = xposixfs_tell(fp, &pos);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, pos);

    /* 読み込みは不可 */
    err = xposixfs_read(fp, buf, WRITE_LEN, &nread);
    TEST_ASSERT_NOT_EQUAL(X_ERR_NONE, err);

    /* 書き込みは可 */
    err = xposixfs_write(fp, WRITE_DATA, WRITE_LEN, &nwritten);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nwritten);

    xposixfs_close(fp);
}


TEST(xposixfs, open_append_plus)
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
    err = xposixfs_open(fs, name, X_OPEN_MODE_APPEND_PLUS, &fp);
    TEST_ASSERT_NOT_NULL(fp);

    err = xposixfs_write(fp, WRITE_DATA, WRITE_LEN, &nwritten);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nwritten);
    xposixfs_close(fp);

    err = xposixfs_stat(fs, name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, statbuf.size);

    /* ファイルが存在している場合はファイルポインタは先頭にセットされる */
    err = xposixfs_open(fs, name, X_OPEN_MODE_APPEND_PLUS, &fp);
    TEST_ASSERT_NOT_NULL(fp);

    err = xposixfs_tell(fp, &pos);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(0, pos);

    /* 読み書きできる */
    err = xposixfs_write(fp, WRITE_DATA, WRITE_LEN, &nwritten);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nwritten);

    err = xposixfs_seek(fp, 0, X_SEEK_SET);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xposixfs_read(fp, buf, WRITE_LEN * 2, &nread);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN * 2, nread);
    TEST_ASSERT_TRUE(memcmp(buf, WRITE_DATA, WRITE_LEN) == 0);
    TEST_ASSERT_TRUE(memcmp(buf + WRITE_LEN, WRITE_DATA, WRITE_LEN) == 0);

    xposixfs_close(fp);
}


TEST_GROUP_RUNNER(xposixfs)
{
    fs = malloc(sizeof(XPosixFs));
    cwd = malloc(PATH_MAX);
    workdir = malloc(PATH_MAX);

    TEST_ASSERT_NOT_NULL(fs);
    TEST_ASSERT_NOT_NULL(cwd);
    TEST_ASSERT_NOT_NULL(workdir);
    TEST_ASSERT_NOT_NULL(getcwd(cwd, PATH_MAX));

    xposixfs_init(fs);

    RUN_TEST_CASE(xposixfs, open_write);
    RUN_TEST_CASE(xposixfs, open_read);
    RUN_TEST_CASE(xposixfs, write);
    RUN_TEST_CASE(xposixfs, read);
    RUN_TEST_CASE(xposixfs, stat);
    RUN_TEST_CASE(xposixfs, tell);
    RUN_TEST_CASE(xposixfs, seek);
    RUN_TEST_CASE(xposixfs, flush);
    RUN_TEST_CASE(xposixfs, mkdir);
    RUN_TEST_CASE(xposixfs, opendir);
    RUN_TEST_CASE(xposixfs, readdir);
    RUN_TEST_CASE(xposixfs, remove);
    RUN_TEST_CASE(xposixfs, rename);
    RUN_TEST_CASE(xposixfs, open_write_plus);
    RUN_TEST_CASE(xposixfs, open_read_plus);
    RUN_TEST_CASE(xposixfs, open_append);
    RUN_TEST_CASE(xposixfs, open_append_plus);

    xposixfs_deinit(fs);

    free(fs);
    free(cwd);
    free(workdir);
}
