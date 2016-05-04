#include <picox/filesystem/xunionfs.h>
#include <picox/filesystem/xramfs.h>
#include <picox/filesystem/xfatfs.h>
#include <ff.h>
#include "testutils.h"

TEST_GROUP(xunionfs);

static XRamFs* ramfs;
static XVirtualFs* vramfs;
static XRamFs* ramfs2;
static XVirtualFs* vramfs2;
static XFatFs* fatfs;
static XVirtualFs* vfatfs;
static XFatFs* fatfs2;
static XVirtualFs* vfatfs2;
static FATFS* fatfsbody;
static FATFS* fatfsbody2;
static const char WRITE_DATA[] = "Hello world";
static const size_t WRITE_LEN = 11; // strlen WRITE_DATA
void disk_deinit(BYTE pdrv);


TEST_SETUP(xunionfs)
{
    XFile* fp;
    XError err;
    ramfs = x_malloc(sizeof(XRamFs));
    vramfs = x_malloc(sizeof(XVirtualFs));
    ramfs2 = x_malloc(sizeof(XRamFs));
    vramfs2 = x_malloc(sizeof(XVirtualFs));
    fatfs = x_malloc(sizeof(XFatFs));
    vfatfs = x_malloc(sizeof(XVirtualFs));
    fatfs2 = x_malloc(sizeof(XFatFs));
    vfatfs2 = x_malloc(sizeof(XVirtualFs));
    fatfsbody = x_malloc(sizeof(FATFS));
    fatfsbody2 = x_malloc(sizeof(FATFS));

    X_ASSERT(ramfs);
    X_ASSERT(vramfs);
    X_ASSERT(ramfs2);
    X_ASSERT(vramfs2);
    X_ASSERT(fatfs);
    X_ASSERT(vfatfs);
    X_ASSERT(fatfs2);
    X_ASSERT(vfatfs2);
    X_ASSERT(fatfsbody);
    X_ASSERT(fatfsbody2);

    FRESULT res;

    res = f_mount(fatfsbody, "0:", 0);
    X_ASSERT(res == FR_OK);
    res = f_mount(fatfsbody2, "1:", 0);
    X_ASSERT(res == FR_OK);

    res = f_mkfs("0:", 0, 0);
    X_ASSERT(res == FR_OK);
    res = f_mkfs("1:", 0, 0);
    X_ASSERT(res == FR_OK);

    xfatfs_init(fatfs);
    xfatfs_init_vfs(fatfs, vfatfs);
    err = xfatfs_mkdir(fatfs, "0:/otherfs");
    X_ASSERT(err == X_ERR_NONE);
    err = xfatfs_mkdir(fatfs, "0:/otherfs2");
    X_ASSERT(err == X_ERR_NONE);
    err = xfatfs_mkdir(fatfs, "0:/otherfs3");
    X_ASSERT(err == X_ERR_NONE);
    err = xfatfs_open(fatfs, "0:/fatfs.txt", X_OPEN_MODE_WRITE, &fp);
    X_ASSERT(err == X_ERR_NONE);
    xfatfs_close(fp);

    xfatfs_init(fatfs2);
    xfatfs_init_vfs(fatfs2, vfatfs2);
    err = xfatfs_mkdir(fatfs2, "1:/hoge");
    X_ASSERT(err == X_ERR_NONE);
    err = xfatfs_mkdir(fatfs2, "1:/hoge/fuga");
    X_ASSERT(err == X_ERR_NONE);
    err = xfatfs_open(fatfs2, "1:/hoge/fuga.txt", X_OPEN_MODE_WRITE, &fp);
    X_ASSERT(err == X_ERR_NONE);
    xfatfs_close(fp);
    err = xfatfs_open(fatfs2, "1:/hoge/fuga/fuge.txt", X_OPEN_MODE_WRITE, &fp);
    X_ASSERT(err == X_ERR_NONE);
    xfatfs_close(fp);

    xramfs_init(ramfs, NULL, 2048);
    xramfs_init_vfs(ramfs, vramfs);
    xramfs_init(ramfs2, NULL, 2048);
    xramfs_init_vfs(ramfs2, vramfs2);

    err = xramfs_open(ramfs, "/foo.txt", X_OPEN_MODE_WRITE, &fp);
    X_ASSERT(err == X_ERR_NONE);
    xramfs_close(fp);

    err = xramfs_mkdir(ramfs2, "/foo");
    X_ASSERT(err == X_ERR_NONE);
    err = xramfs_open(ramfs2, "/foo/bar.txt", X_OPEN_MODE_WRITE, &fp);
    X_ASSERT(err == X_ERR_NONE);
    xramfs_close(fp);
    err = xramfs_mkdir(ramfs2, "/foo/bar");
    X_ASSERT(err == X_ERR_NONE);
    err = xramfs_open(ramfs2, "/foo/bar/baz.txt", X_OPEN_MODE_WRITE, &fp);
    X_ASSERT(err == X_ERR_NONE);
    xramfs_close(fp);

    xunionfs_init();
    err = xunionfs_mount(vfatfs, "/", "0:/");
    X_ASSERT(err == X_ERR_NONE);
    err = xunionfs_mount(vramfs, "/otherfs", "/");
    X_ASSERT(err == X_ERR_NONE);
    err = xunionfs_mount(vramfs2, "/otherfs2", "/foo");
    X_ASSERT(err == X_ERR_NONE);
    err = xunionfs_mount(vfatfs2, "/otherfs3", "1:/hoge");
    X_ASSERT(err == X_ERR_NONE);
}

TEST_TEAR_DOWN(xunionfs)
{
    xunionfs_deinit();
    if (ramfs)
        xramfs_deinit(ramfs);
    if (ramfs2)
        xramfs_deinit(ramfs2);
    if (fatfs)
    {
        f_mount(NULL, "0:", 0);
        xfatfs_deinit(fatfs);
    }
    if (fatfs2)
    {
        f_mount(NULL, "1:", 0);
        xfatfs_deinit(fatfs2);
    }
    X_SAFE_FREE(ramfs);
    X_SAFE_FREE(vramfs);
    X_SAFE_FREE(ramfs2);
    X_SAFE_FREE(vramfs2);
    X_SAFE_FREE(fatfs);
    X_SAFE_FREE(vfatfs);
    X_SAFE_FREE(fatfs2);
    X_SAFE_FREE(vfatfs2);
    X_SAFE_FREE(fatfsbody);
    X_SAFE_FREE(fatfsbody2);
    disk_deinit(0);
    disk_deinit(1);
}


TEST(xunionfs, open_write)
{
    XFile* fp;
    XError err;

    err = xunionfs_open("foo.txt", X_OPEN_MODE_WRITE, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_NOT_NULL(fp);
    xunionfs_close(fp);
}


TEST(xunionfs, open_read)
{
    XFile* fp;
    XError err;
    const char name[] = "foo.txt";

    err = xunionfs_open(name, X_OPEN_MODE_READ, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NO_ENTRY, err);
    TEST_ASSERT_NULL(fp);
    xunionfs_close(fp);

    err = xunionfs_open(name, X_OPEN_MODE_WRITE, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_NOT_NULL(fp);
    xunionfs_close(fp);

    err = xunionfs_open(name, X_OPEN_MODE_READ, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_NOT_NULL(fp);
    xunionfs_close(fp);
}


TEST(xunionfs, write)
{
    XFile* fp;
    XError err;
    size_t nwritten;
    const char name[] = "foo.txt";


    err = xunionfs_open(name, X_OPEN_MODE_WRITE, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_NOT_NULL(fp);

    err = xunionfs_write(fp, WRITE_DATA, WRITE_LEN, &nwritten);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nwritten);

    xunionfs_close(fp);
}


TEST(xunionfs, read)
{
    XFile* fp;
    XError err;
    char buf[256];
    size_t nread;
    const char name[] = "foo.txt";

    err = xunionfs_open(name, X_OPEN_MODE_WRITE, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xunionfs_write(fp, WRITE_DATA, WRITE_LEN, NULL);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    xunionfs_close(fp);

    err = xunionfs_open(name, X_OPEN_MODE_READ, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_NOT_NULL(fp);

    err = xunionfs_read(fp, buf, WRITE_LEN, &nread);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nread);
    xunionfs_close(fp);
}


TEST(xunionfs, stat)
{
    XFile* fp;
    XError err;
    XStat statbuf;
    const char name[] = "foo.txt";

    err = xunionfs_open(name, X_OPEN_MODE_WRITE, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xunionfs_write(fp, WRITE_DATA, WRITE_LEN, NULL);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    xunionfs_close(fp);

    err = xunionfs_stat(name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, statbuf.size);
}


TEST(xunionfs, tell)
{
    XFile* fp;
    XError err;
    XSize pos;
    const char name[] = "foo.txt";

    err = xunionfs_open(name, X_OPEN_MODE_WRITE, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xunionfs_tell(fp, &pos);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(0, pos);

    err = xunionfs_write(fp, WRITE_DATA, WRITE_LEN, NULL);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xunionfs_tell(fp, &pos);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, pos);

    xunionfs_close(fp);
}


TEST(xunionfs, seek)
{
    XFile* fp;
    XError err;
    char name[] = "foo.txt";
    XSize pos;

    err = xunionfs_open(name, X_OPEN_MODE_WRITE_PLUS, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xunionfs_seek(fp, 10, X_SEEK_SET);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(10, (xunionfs_tell(fp, &pos), pos));

    err = xunionfs_seek(fp, 10, X_SEEK_CUR);
    TEST_ASSERT_EQUAL(20, (xunionfs_tell(fp, &pos), pos));

    err = xunionfs_seek(fp, 10, X_SEEK_END);
    TEST_ASSERT_EQUAL(10, (xunionfs_tell(fp, &pos), pos));


    err = xunionfs_seek(fp, 9, X_SEEK_END);
    xunionfs_write(fp, "A", 1, NULL);
    TEST_ASSERT_EQUAL(10, (xunionfs_tell(fp, &pos), pos));

    err = xunionfs_seek(fp, 10, X_SEEK_CUR);
    TEST_ASSERT_EQUAL(20, (xunionfs_tell(fp, &pos), pos));

    err = xunionfs_seek(fp, -10, X_SEEK_CUR);
    TEST_ASSERT_EQUAL(10, (xunionfs_tell(fp, &pos), pos));

    err = xunionfs_seek(fp, -10, X_SEEK_CUR);
    TEST_ASSERT_EQUAL(0, (xunionfs_tell(fp, &pos), pos));

    xunionfs_close(fp);
}


TEST(xunionfs, flush)
{
    XFile* fp;
    XError err;
    char name[] = "foo.txt";

    err = xunionfs_open(name, X_OPEN_MODE_WRITE, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    xunionfs_write(fp, "A", 1, NULL);
    err = xunionfs_flush(fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    xunionfs_close(fp);
}


TEST(xunionfs, mkdir)
{
    XError err;
    char name[] = "foo";
    XStat statbuf;

    err = xunionfs_mkdir(name);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    err = xunionfs_stat(name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
}


TEST(xunionfs, opendir)
{
    XDir* dir;
    XError err;
    char name[] = "foo";

    xunionfs_mkdir(name);
    err = xunionfs_opendir(name, &dir);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_NOT_NULL(dir);
    xunionfs_closedir(dir);
}


TEST(xunionfs, readdir)
{
    XFile* fp;
    XDir* dir;
    XError err;
    XDirEnt entbuf;
    XDirEnt* ent;
    char name[] = "foo";
    char name2[] = "bar.txt";

    xunionfs_mkdir(name);
    xunionfs_open(name2, X_OPEN_MODE_WRITE, &fp);
    xunionfs_close(fp);
    xunionfs_opendir(name, &dir);

    while ((err = xunionfs_readdir(dir, &entbuf, &ent)), ent)
    {
        if (strcmp(".", ent->name) == 0)
            continue;

        if (strcmp("..", ent->name) == 0)
            continue;
        TEST_ASSERT_EQUAL_STRING(name2, ent->name);
    }
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    xunionfs_closedir(dir);
}


TEST(xunionfs, remove)
{
    XFile* fp;
    XError err;
    XStat statbuf;
    char name[] = "foo";

    err = xunionfs_stat(name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NO_ENTRY, err);

    xunionfs_open(name, X_OPEN_MODE_WRITE, &fp);

    xunionfs_close(fp);

    err = xunionfs_stat(name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xunionfs_remove(name);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xunionfs_stat(name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NO_ENTRY, err);

    err = xunionfs_mkdir(name);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xunionfs_stat(name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xunionfs_remove(name);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xunionfs_stat(name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NO_ENTRY, err);
}


TEST(xunionfs, rename)
{
    XFile* fp;
    XError err;
    XStat statbuf;
    char name[] = "foo";
    char name2[] = "bar";

    xunionfs_open(name, X_OPEN_MODE_WRITE, &fp);
    xunionfs_close(fp);

    err = xunionfs_stat(name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xunionfs_stat(name2, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NO_ENTRY, err);

    err = xunionfs_rename(name, name2);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xunionfs_stat(name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NO_ENTRY, err);

    err = xunionfs_stat(name2, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

}


TEST(xunionfs, open_write_plus)
{
    XFile* fp;
    XError err;
    XStat statbuf;
    size_t nwritten;
    size_t nread;
    char buf[WRITE_LEN];
    const char name[] = "foo.txt";

    /* ファイルが存在しない場合は新規作成される */
    err = xunionfs_open(name, X_OPEN_MODE_WRITE_PLUS, &fp);
    TEST_ASSERT_NOT_NULL(fp);
    err = xunionfs_write(fp, WRITE_DATA, WRITE_LEN, &nwritten);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nwritten);
    xunionfs_close(fp);

    err = xunionfs_stat(name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, statbuf.size);

    /* ファイルが存在している場合はサイズは0に切り詰められる */
    err = xunionfs_open(name, X_OPEN_MODE_WRITE_PLUS, &fp);
    TEST_ASSERT_NOT_NULL(fp);
    xunionfs_close(fp);
    err = xunionfs_stat(name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(0, statbuf.size);

    /* 読み込みも可能 */
    err = xunionfs_open(name, X_OPEN_MODE_WRITE_PLUS, &fp);
    TEST_ASSERT_NOT_NULL(fp);
    err = xunionfs_write(fp, WRITE_DATA, WRITE_LEN, &nwritten);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nwritten);

    err = xunionfs_seek(fp, 0, X_SEEK_SET);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xunionfs_read(fp, buf, WRITE_LEN, &nread);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nread);
    TEST_ASSERT_TRUE(memcmp(buf, WRITE_DATA, WRITE_LEN) == 0);

    xunionfs_close(fp);
}


TEST(xunionfs, open_read_plus)
{
    XFile* fp;
    XError err;
    XStat statbuf;
    size_t nwritten;
    size_t nread;
    char buf[WRITE_LEN * 2];
    const char name[] = "foo.txt";

    /* ファイルが存在しない場合はエラーになる。 */
    err = xunionfs_open(name, X_OPEN_MODE_READ_PLUS, &fp);
    TEST_ASSERT_NULL(fp);

    err = xunionfs_open(name, X_OPEN_MODE_WRITE, &fp);
    TEST_ASSERT_NOT_NULL(fp);

    err = xunionfs_write(fp, WRITE_DATA, WRITE_LEN, &nwritten);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nwritten);
    xunionfs_close(fp);

    err = xunionfs_stat(name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, statbuf.size);

    /* ファイルが存在している場合、サイズ0にはならない */
    err = xunionfs_open(name, X_OPEN_MODE_READ_PLUS, &fp);
    TEST_ASSERT_NOT_NULL(fp);
    xunionfs_close(fp);
    err = xunionfs_stat(name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, statbuf.size);

    /* 読み書きできる */
    err = xunionfs_open(name, X_OPEN_MODE_READ_PLUS, &fp);
    TEST_ASSERT_NOT_NULL(fp);

    err = xunionfs_read(fp, buf, WRITE_LEN, &nread);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nread);
    TEST_ASSERT_TRUE(memcmp(buf, WRITE_DATA, WRITE_LEN) == 0);

    err = xunionfs_seek(fp, 0, X_SEEK_END);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xunionfs_write(fp, WRITE_DATA, WRITE_LEN, &nwritten);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nwritten);

    err = xunionfs_seek(fp, 0, X_SEEK_SET);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xunionfs_read(fp, buf, WRITE_LEN * 2, &nread);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN * 2, nread);
    TEST_ASSERT_TRUE(memcmp(buf, WRITE_DATA, WRITE_LEN) == 0);
    TEST_ASSERT_TRUE(memcmp(buf + WRITE_LEN, WRITE_DATA, WRITE_LEN) == 0);

    xunionfs_close(fp);
}


TEST(xunionfs, open_append)
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
    err = xunionfs_open(name, X_OPEN_MODE_APPEND, &fp);
    TEST_ASSERT_NOT_NULL(fp);

    err = xunionfs_write(fp, WRITE_DATA, WRITE_LEN, &nwritten);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nwritten);
    xunionfs_close(fp);

    err = xunionfs_stat(name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, statbuf.size);

    /* ファイルが存在している場合はファイルポインタは末尾にセットされる */
    err = xunionfs_open(name, X_OPEN_MODE_APPEND, &fp);
    TEST_ASSERT_NOT_NULL(fp);

    err = xunionfs_tell(fp, &pos);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, pos);

    /* 読み込みは不可 */
    err = xunionfs_read(fp, buf, WRITE_LEN, &nread);
    TEST_ASSERT_NOT_EQUAL(X_ERR_NONE, err);

    /* 書き込みは可 */
    err = xunionfs_write(fp, WRITE_DATA, WRITE_LEN, &nwritten);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nwritten);

    xunionfs_close(fp);
}


TEST(xunionfs, open_append_plus)
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
    err = xunionfs_open(name, X_OPEN_MODE_APPEND_PLUS, &fp);
    TEST_ASSERT_NOT_NULL(fp);

    err = xunionfs_write(fp, WRITE_DATA, WRITE_LEN, &nwritten);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nwritten);
    xunionfs_close(fp);

    err = xunionfs_stat(name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, statbuf.size);

    /* ファイルが存在している場合はファイルポインタは先頭にセットされる */
    err = xunionfs_open(name, X_OPEN_MODE_APPEND_PLUS, &fp);
    TEST_ASSERT_NOT_NULL(fp);

    err = xunionfs_tell(fp, &pos);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(0, pos);

    /* 読み書きできる */
    err = xunionfs_write(fp, WRITE_DATA, WRITE_LEN, &nwritten);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nwritten);

    err = xunionfs_seek(fp, 0, X_SEEK_SET);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xunionfs_read(fp, buf, WRITE_LEN * 2, &nread);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN * 2, nread);
    TEST_ASSERT_TRUE(memcmp(buf, WRITE_DATA, WRITE_LEN) == 0);
    TEST_ASSERT_TRUE(memcmp(buf + WRITE_LEN, WRITE_DATA, WRITE_LEN) == 0);

    xunionfs_close(fp);
}


TEST(xunionfs, copyfile)
{
    XFile* fp;
    XError err;
    char buf[256];
    size_t nread;
    size_t nwritten;
    const char name[] = "foo.txt";
    const char name2[] = "bar.txt";

    err = xunionfs_open(name, X_OPEN_MODE_WRITE, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_NOT_NULL(fp);

    err = xunionfs_write(fp, WRITE_DATA, WRITE_LEN, &nwritten);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nwritten);
    xunionfs_close(fp);

    err = xunionfs_copyfile(name, name2);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xunionfs_open(name, X_OPEN_MODE_READ, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_NOT_NULL(fp);

    err = xunionfs_read(fp, buf, WRITE_LEN, &nread);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nread);
    TEST_ASSERT_EQUAL(0, memcmp(buf, WRITE_DATA, WRITE_LEN));
    xunionfs_close(fp);
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


TEST(xunionfs, copytree)
{
    XError err;

    err = xunionfs_mkdir("foo");
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xunionfs_mkdir("foo/bar");
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xunionfs_mkdir("foo/baz");
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xunionfs_copytree("foo", "hoge");
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xunionfs_copytree("foo", "/otherfs/tree");
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xunionfs_walktree("/", PrintTree, NULL);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xunionfs_walktree("/otherfs", PrintTree, NULL);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
}


TEST(xunionfs, mount)
{
    XError err;
    XFile* fp;
    XDir*  dir;

    xunionfs_deinit();

    XRamFs rootfs;
    XVirtualFs vramfs1;
    XVirtualFs vramfs2;

    xramfs_init(&rootfs, NULL, 2048);
    xramfs_init_vfs(&rootfs, &vramfs1);
    xramfs_init_vfs(&rootfs, &vramfs2);

    err = xramfs_mkdir(&rootfs, "/mnt");
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xramfs_mkdir(&rootfs, "/mnt/ramfs");
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xramfs_mkdir(&rootfs, "/home");
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xramfs_open(&rootfs, "/home/test.txt", X_OPEN_MODE_WRITE, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    xramfs_close(fp);

    err = xunionfs_mount(&vramfs1, "/", "/");
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xunionfs_mount(&vramfs2, "/mnt", "/home");
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xunionfs_open("/mnt/test.txt", X_OPEN_MODE_READ, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    xunionfs_close(fp);

    xunionfs_deinit();

    err = xunionfs_mount(&vramfs1, "/", "/home");
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xunionfs_open("/test.txt", X_OPEN_MODE_READ, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    xunionfs_close(fp);

    err = xunionfs_opendir("/", &dir);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    xunionfs_closedir(dir);
    xunionfs_deinit();


    err = xunionfs_mount(&vramfs1, "/", "/");
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xunionfs_mount(vfatfs, "/mnt", "0:/");
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xunionfs_opendir("/mnt", &dir);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    xunionfs_closedir(dir);

    xunionfs_deinit();


    /* "/", "/"
     * "/tmp", "/"
     * "/", "/tmp"
     */
}


TEST(xunionfs, rmtree)
{
    XError err;

    err = xunionfs_rmtree("/otherfs2/bar");
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xunionfs_walktree("/", PrintTree, NULL);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
}


TEST(xunionfs, stream)
{
    XFile* fp;
    XError err;
    XStream st;
    const char name[] = "foo.txt";

    err = xunionfs_open(name, X_OPEN_MODE_WRITE_PLUS, &fp);
    TEST_ASSERT_NOT_NULL(fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    xunionfs_init_stream(&st, fp);
    x_test_stream(&st);

    xunionfs_close(fp);
}


TEST(xunionfs, putc)
{
    XFile* fp;
    XError err;
    const char name[] = "foo.txt";

    err = xunionfs_open(name, X_OPEN_MODE_WRITE_PLUS, &fp);
    TEST_ASSERT_NOT_NULL(fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    int i;
    const char data[] = "Hello World";

    for (i = 0; i < (int)sizeof(data); i++)
    {
        TEST_ASSERT_EQUAL(data[i], xunionfs_putc(fp, data[i]));
    }

    xunionfs_close(fp);

    err = xunionfs_open(name, X_OPEN_MODE_READ, &fp);
    TEST_ASSERT_NOT_NULL(fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    char buf[X_LINE_MAX];
    char* result;

    xunionfs_readline(fp, buf, sizeof(buf), &result, NULL);
    TEST_ASSERT_EQUAL_STRING(data, buf);

    xunionfs_close(fp);
}


TEST(xunionfs, puts)
{
    XFile* fp;
    XError err;
    const char name[] = "foo.txt";

    err = xunionfs_open(name, X_OPEN_MODE_WRITE_PLUS, &fp);
    TEST_ASSERT_NOT_NULL(fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    int i;
    const char* data[] = {
        "Hello World",
        "FooBar World",
        "HogeHoge World premium"
    };

    for (i = 0; i < (int)X_COUNT_OF(data); i++)
    {
        TEST_ASSERT_EQUAL(0, xunionfs_puts(fp, data[i]));
        xunionfs_putc(fp, '\n');
    }

    xunionfs_close(fp);

    err = xunionfs_open(name, X_OPEN_MODE_READ, &fp);
    TEST_ASSERT_NOT_NULL(fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    char buf[X_LINE_MAX];
    char* result;

    for (i = 0; i < (int)X_COUNT_OF(data); i++)
    {
        xunionfs_readline(fp, buf, sizeof(buf), &result, NULL);
        TEST_ASSERT_EQUAL_STRING(data[i], buf);
    }

    xunionfs_close(fp);
}


TEST(xunionfs, printf)
{
    XFile* fp;
    XError err;
    const char name[] = "foo.txt";

    err = xunionfs_open(name, X_OPEN_MODE_WRITE_PLUS, &fp);
    TEST_ASSERT_NOT_NULL(fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    int i;
    for (i = 0; i < 3; i++)
    {
        TEST_ASSERT_TRUE(xunionfs_printf(fp, "Hello world %d\n", i) >= 0);
    }

    xunionfs_close(fp);

    err = xunionfs_open(name, X_OPEN_MODE_READ, &fp);
    TEST_ASSERT_NOT_NULL(fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    char buf[X_LINE_MAX];
    char* result;

    xunionfs_readline(fp, buf, sizeof(buf), &result, NULL);
    TEST_ASSERT_EQUAL_STRING("Hello world 0", buf);

    xunionfs_readline(fp, buf, sizeof(buf), &result, NULL);
    TEST_ASSERT_EQUAL_STRING("Hello world 1", buf);

    xunionfs_readline(fp, buf, sizeof(buf), &result, NULL);
    TEST_ASSERT_EQUAL_STRING("Hello world 2", buf);

    xunionfs_close(fp);
}


TEST(xunionfs, exists)
{
    XFile* fp;
    const char name[] = "foo.txt";
    bool exists;

    xunionfs_open(name, X_OPEN_MODE_WRITE_PLUS, &fp);
    xunionfs_close(fp);
    xunionfs_exists(name, &exists);
    TEST_ASSERT_TRUE(exists);

    xunionfs_remove(name);
    xunionfs_exists(name, &exists);
    TEST_ASSERT_FALSE(exists);

    xunionfs_mkdir(name);
    xunionfs_exists(name, &exists);
    TEST_ASSERT_TRUE(exists);
}


TEST(xunionfs, is_regular)
{
    XFile* fp;
    const char name[] = "foo.txt";
    bool is_regular;

    xunionfs_open(name, X_OPEN_MODE_WRITE_PLUS, &fp);
    xunionfs_close(fp);
    xunionfs_is_regular(name, &is_regular);
    TEST_ASSERT_TRUE(is_regular);

    xunionfs_remove(name);
    xunionfs_is_regular(name, &is_regular);
    TEST_ASSERT_FALSE(is_regular);

    xunionfs_mkdir(name);
    xunionfs_is_regular(name, &is_regular);
    TEST_ASSERT_FALSE(is_regular);
}


TEST(xunionfs, is_directory)
{
    XFile* fp;
    const char name[] = "foo.txt";
    bool is_directory;

    xunionfs_open(name, X_OPEN_MODE_WRITE_PLUS, &fp);
    xunionfs_close(fp);
    xunionfs_is_directory(name, &is_directory);
    TEST_ASSERT_FALSE(is_directory);

    xunionfs_remove(name);
    xunionfs_is_directory(name, &is_directory);
    TEST_ASSERT_FALSE(is_directory);

    xunionfs_mkdir(name);
    xunionfs_is_directory(name, &is_directory);
    TEST_ASSERT_TRUE(is_directory);
}


TEST(xunionfs, makedirs)
{
    const char name[] = "makedirs/bar/baz";
    XError err;
    bool is_directory;

    err = xunionfs_makedirs(name, false);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    xunionfs_is_directory("makedirs", &is_directory);
    TEST_ASSERT_TRUE(is_directory);
    xunionfs_is_directory("makedirs/bar", &is_directory);
    TEST_ASSERT_TRUE(is_directory);
    xunionfs_is_directory("makedirs/bar/baz", &is_directory);
    TEST_ASSERT_TRUE(is_directory);

    err = xunionfs_makedirs(name, false);
    TEST_ASSERT_EQUAL(X_ERR_EXIST, err);

    err = xunionfs_makedirs(name, true);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
}


TEST(xunionfs, makedirs2)
{
    const char name[] = "/makedirs/bar/baz";
    XError err;
    bool is_directory;

    err = xunionfs_makedirs(name, false);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    xunionfs_is_directory("makedirs", &is_directory);
    TEST_ASSERT_TRUE(is_directory);
    xunionfs_is_directory("makedirs/bar", &is_directory);
    TEST_ASSERT_TRUE(is_directory);
    xunionfs_is_directory("makedirs/bar/baz", &is_directory);
    TEST_ASSERT_TRUE(is_directory);

    err = xunionfs_makedirs(name, false);
    TEST_ASSERT_EQUAL(X_ERR_EXIST, err);

    err = xunionfs_makedirs(name, true);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
}


TEST_GROUP_RUNNER(xunionfs)
{
    RUN_TEST_CASE(xunionfs, open_write);
    RUN_TEST_CASE(xunionfs, open_read);
    RUN_TEST_CASE(xunionfs, write);
    RUN_TEST_CASE(xunionfs, read);
    RUN_TEST_CASE(xunionfs, stat);
    RUN_TEST_CASE(xunionfs, tell);
    RUN_TEST_CASE(xunionfs, seek);
    RUN_TEST_CASE(xunionfs, flush);
    RUN_TEST_CASE(xunionfs, mkdir);
    RUN_TEST_CASE(xunionfs, opendir);
    RUN_TEST_CASE(xunionfs, readdir);
    RUN_TEST_CASE(xunionfs, remove);
    RUN_TEST_CASE(xunionfs, rename);
    RUN_TEST_CASE(xunionfs, open_write_plus);
    RUN_TEST_CASE(xunionfs, open_read_plus);
    RUN_TEST_CASE(xunionfs, open_append);
    RUN_TEST_CASE(xunionfs, open_append_plus);
    RUN_TEST_CASE(xunionfs, copyfile);
    RUN_TEST_CASE(xunionfs, copytree);
    RUN_TEST_CASE(xunionfs, rmtree);
    RUN_TEST_CASE(xunionfs, mount);
    RUN_TEST_CASE(xunionfs, stream);
    RUN_TEST_CASE(xunionfs, putc);
    RUN_TEST_CASE(xunionfs, puts);
    RUN_TEST_CASE(xunionfs, printf);
    RUN_TEST_CASE(xunionfs, exists);
    RUN_TEST_CASE(xunionfs, is_regular);
    RUN_TEST_CASE(xunionfs, is_directory);
    RUN_TEST_CASE(xunionfs, makedirs);
    RUN_TEST_CASE(xunionfs, makedirs2);
}
