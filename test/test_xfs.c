#include <picox/filesystem/xfs.h>
#include <picox/filesystem/xramfs.h>
#include <picox/filesystem/xfatfs.h>
#include <ff.h>
#include "testutils.h"

TEST_GROUP(xfs);

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


TEST_SETUP(xfs)
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
    err = xramfs_open(ramfs, "/foo/bar.txt", X_OPEN_MODE_WRITE, &fp);
    X_ASSERT(err == X_ERR_NONE);
    xramfs_close(fp);
    err = xramfs_mkdir(ramfs2, "/foo/bar");
    X_ASSERT(err == X_ERR_NONE);
    err = xramfs_open(ramfs2, "/foo/bar/baz.txt", X_OPEN_MODE_WRITE, &fp);
    X_ASSERT(err == X_ERR_NONE);
    xramfs_close(fp);

    xfs_init();
    err = xfs_mount(vfatfs, "/", "0:/");
    X_ASSERT(err == X_ERR_NONE);
    err = xfs_mount(vramfs, "/otherfs", "/");
    X_ASSERT(err == X_ERR_NONE);
    err = xfs_mount(vramfs2, "/otherfs2", "/foo");
    X_ASSERT(err == X_ERR_NONE);
    err = xfs_mount(vfatfs2, "/otherfs3", "1:/hoge");
    X_ASSERT(err == X_ERR_NONE);
}

TEST_TEAR_DOWN(xfs)
{
    xfs_deinit();
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


TEST(xfs, open_write)
{
    XFile* fp;
    XError err;

    err = xfs_open("foo.txt", X_OPEN_MODE_WRITE, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_NOT_NULL(fp);
    xfs_close(fp);
}


TEST(xfs, open_read)
{
    XFile* fp;
    XError err;
    const char name[] = "foo.txt";

    err = xfs_open(name, X_OPEN_MODE_READ, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NO_ENTRY, err);
    TEST_ASSERT_NULL(fp);
    xfs_close(fp);

    err = xfs_open(name, X_OPEN_MODE_WRITE, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_NOT_NULL(fp);
    xfs_close(fp);

    err = xfs_open(name, X_OPEN_MODE_READ, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_NOT_NULL(fp);
    xfs_close(fp);
}


TEST(xfs, write)
{
    XFile* fp;
    XError err;
    size_t nwritten;
    const char name[] = "foo.txt";


    err = xfs_open(name, X_OPEN_MODE_WRITE, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_NOT_NULL(fp);

    err = xfs_write(fp, WRITE_DATA, WRITE_LEN, &nwritten);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nwritten);

    xfs_close(fp);
}


TEST(xfs, read)
{
    XFile* fp;
    XError err;
    char buf[256];
    size_t nread;
    const char name[] = "foo.txt";

    err = xfs_open(name, X_OPEN_MODE_WRITE, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xfs_write(fp, WRITE_DATA, WRITE_LEN, NULL);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    xfs_close(fp);

    err = xfs_open(name, X_OPEN_MODE_READ, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_NOT_NULL(fp);

    err = xfs_read(fp, buf, WRITE_LEN, &nread);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nread);
    xfs_close(fp);
}


TEST(xfs, stat)
{
    XFile* fp;
    XError err;
    XStat statbuf;
    const char name[] = "foo.txt";

    err = xfs_open(name, X_OPEN_MODE_WRITE, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xfs_write(fp, WRITE_DATA, WRITE_LEN, NULL);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    xfs_close(fp);

    err = xfs_stat(name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, statbuf.size);
}


TEST(xfs, tell)
{
    XFile* fp;
    XError err;
    XSize pos;
    const char name[] = "foo.txt";

    err = xfs_open(name, X_OPEN_MODE_WRITE, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xfs_tell(fp, &pos);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(0, pos);

    err = xfs_write(fp, WRITE_DATA, WRITE_LEN, NULL);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xfs_tell(fp, &pos);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, pos);

    xfs_close(fp);
}


TEST(xfs, seek)
{
    XFile* fp;
    XError err;
    char name[] = "foo.txt";
    XSize pos;

    err = xfs_open(name, X_OPEN_MODE_WRITE_PLUS, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xfs_seek(fp, 10, X_SEEK_SET);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(10, (xfs_tell(fp, &pos), pos));

    err = xfs_seek(fp, 10, X_SEEK_CUR);
    TEST_ASSERT_EQUAL(20, (xfs_tell(fp, &pos), pos));

    err = xfs_seek(fp, 10, X_SEEK_END);
    TEST_ASSERT_EQUAL(10, (xfs_tell(fp, &pos), pos));


    err = xfs_seek(fp, 9, X_SEEK_END);
    xfs_write(fp, "A", 1, NULL);
    TEST_ASSERT_EQUAL(10, (xfs_tell(fp, &pos), pos));

    err = xfs_seek(fp, 10, X_SEEK_CUR);
    TEST_ASSERT_EQUAL(20, (xfs_tell(fp, &pos), pos));

    err = xfs_seek(fp, -10, X_SEEK_CUR);
    TEST_ASSERT_EQUAL(10, (xfs_tell(fp, &pos), pos));

    err = xfs_seek(fp, -10, X_SEEK_CUR);
    TEST_ASSERT_EQUAL(0, (xfs_tell(fp, &pos), pos));

    xfs_close(fp);
}


TEST(xfs, flush)
{
    XFile* fp;
    XError err;
    char name[] = "foo.txt";

    err = xfs_open(name, X_OPEN_MODE_WRITE, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    xfs_write(fp, "A", 1, NULL);
    err = xfs_flush(fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    xfs_close(fp);
}


TEST(xfs, mkdir)
{
    XError err;
    char name[] = "foo";
    XStat statbuf;

    err = xfs_mkdir(name);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    err = xfs_stat(name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
}


TEST(xfs, opendir)
{
    XDir* dir;
    XError err;
    char name[] = "foo";

    xfs_mkdir(name);
    err = xfs_opendir(name, &dir);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_NOT_NULL(dir);
    xfs_closedir(dir);
}


TEST(xfs, readdir)
{
    XFile* fp;
    XDir* dir;
    XError err;
    XDirEnt entbuf;
    XDirEnt* ent;
    char name[] = "foo";
    char name2[] = "bar.txt";

    xfs_mkdir(name);
    xfs_open(name2, X_OPEN_MODE_WRITE, &fp);
    xfs_close(fp);
    xfs_opendir(name, &dir);

    while ((err = xfs_readdir(dir, &entbuf, &ent)), ent)
    {
        if (strcmp(".", ent->name) == 0)
            continue;

        if (strcmp("..", ent->name) == 0)
            continue;
        TEST_ASSERT_EQUAL_STRING(name2, ent->name);
    }
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    xfs_closedir(dir);
}


TEST(xfs, remove)
{
    XFile* fp;
    XError err;
    XStat statbuf;
    char name[] = "foo";

    err = xfs_stat(name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NO_ENTRY, err);

    xfs_open(name, X_OPEN_MODE_WRITE, &fp);

    xfs_close(fp);

    err = xfs_stat(name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xfs_remove(name);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xfs_stat(name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NO_ENTRY, err);

    err = xfs_mkdir(name);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xfs_stat(name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xfs_remove(name);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xfs_stat(name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NO_ENTRY, err);
}


TEST(xfs, rename)
{
    XFile* fp;
    XError err;
    XStat statbuf;
    char name[] = "foo";
    char name2[] = "bar";

    xfs_open(name, X_OPEN_MODE_WRITE, &fp);
    xfs_close(fp);

    err = xfs_stat(name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xfs_stat(name2, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NO_ENTRY, err);

    err = xfs_rename(name, name2);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xfs_stat(name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NO_ENTRY, err);

    err = xfs_stat(name2, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

}


TEST(xfs, open_write_plus)
{
    XFile* fp;
    XError err;
    XStat statbuf;
    size_t nwritten;
    size_t nread;
    char buf[WRITE_LEN];
    const char name[] = "foo.txt";

    /* ファイルが存在しない場合は新規作成される */
    err = xfs_open(name, X_OPEN_MODE_WRITE_PLUS, &fp);
    TEST_ASSERT_NOT_NULL(fp);
    err = xfs_write(fp, WRITE_DATA, WRITE_LEN, &nwritten);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nwritten);
    xfs_close(fp);

    err = xfs_stat(name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, statbuf.size);

    /* ファイルが存在している場合はサイズは0に切り詰められる */
    err = xfs_open(name, X_OPEN_MODE_WRITE_PLUS, &fp);
    TEST_ASSERT_NOT_NULL(fp);
    xfs_close(fp);
    err = xfs_stat(name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(0, statbuf.size);

    /* 読み込みも可能 */
    err = xfs_open(name, X_OPEN_MODE_WRITE_PLUS, &fp);
    TEST_ASSERT_NOT_NULL(fp);
    err = xfs_write(fp, WRITE_DATA, WRITE_LEN, &nwritten);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nwritten);

    err = xfs_seek(fp, 0, X_SEEK_SET);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xfs_read(fp, buf, WRITE_LEN, &nread);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nread);
    TEST_ASSERT_TRUE(memcmp(buf, WRITE_DATA, WRITE_LEN) == 0);

    xfs_close(fp);
}


TEST(xfs, open_read_plus)
{
    XFile* fp;
    XError err;
    XStat statbuf;
    size_t nwritten;
    size_t nread;
    char buf[WRITE_LEN * 2];
    const char name[] = "foo.txt";

    /* ファイルが存在しない場合はエラーになる。 */
    err = xfs_open(name, X_OPEN_MODE_READ_PLUS, &fp);
    TEST_ASSERT_NULL(fp);

    err = xfs_open(name, X_OPEN_MODE_WRITE, &fp);
    TEST_ASSERT_NOT_NULL(fp);

    err = xfs_write(fp, WRITE_DATA, WRITE_LEN, &nwritten);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nwritten);
    xfs_close(fp);

    err = xfs_stat(name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, statbuf.size);

    /* ファイルが存在している場合、サイズ0にはならない */
    err = xfs_open(name, X_OPEN_MODE_READ_PLUS, &fp);
    TEST_ASSERT_NOT_NULL(fp);
    xfs_close(fp);
    err = xfs_stat(name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, statbuf.size);

    /* 読み書きできる */
    err = xfs_open(name, X_OPEN_MODE_READ_PLUS, &fp);
    TEST_ASSERT_NOT_NULL(fp);

    err = xfs_read(fp, buf, WRITE_LEN, &nread);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nread);
    TEST_ASSERT_TRUE(memcmp(buf, WRITE_DATA, WRITE_LEN) == 0);

    err = xfs_seek(fp, 0, X_SEEK_END);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xfs_write(fp, WRITE_DATA, WRITE_LEN, &nwritten);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nwritten);

    err = xfs_seek(fp, 0, X_SEEK_SET);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xfs_read(fp, buf, WRITE_LEN * 2, &nread);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN * 2, nread);
    TEST_ASSERT_TRUE(memcmp(buf, WRITE_DATA, WRITE_LEN) == 0);
    TEST_ASSERT_TRUE(memcmp(buf + WRITE_LEN, WRITE_DATA, WRITE_LEN) == 0);

    xfs_close(fp);
}


TEST(xfs, open_append)
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
    err = xfs_open(name, X_OPEN_MODE_APPEND, &fp);
    TEST_ASSERT_NOT_NULL(fp);

    err = xfs_write(fp, WRITE_DATA, WRITE_LEN, &nwritten);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nwritten);
    xfs_close(fp);

    err = xfs_stat(name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, statbuf.size);

    /* ファイルが存在している場合はファイルポインタは末尾にセットされる */
    err = xfs_open(name, X_OPEN_MODE_APPEND, &fp);
    TEST_ASSERT_NOT_NULL(fp);

    err = xfs_tell(fp, &pos);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, pos);

    /* 読み込みは不可 */
    err = xfs_read(fp, buf, WRITE_LEN, &nread);
    TEST_ASSERT_NOT_EQUAL(X_ERR_NONE, err);

    /* 書き込みは可 */
    err = xfs_write(fp, WRITE_DATA, WRITE_LEN, &nwritten);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nwritten);

    xfs_close(fp);
}


TEST(xfs, open_append_plus)
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
    err = xfs_open(name, X_OPEN_MODE_APPEND_PLUS, &fp);
    TEST_ASSERT_NOT_NULL(fp);

    err = xfs_write(fp, WRITE_DATA, WRITE_LEN, &nwritten);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nwritten);
    xfs_close(fp);

    err = xfs_stat(name, &statbuf);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, statbuf.size);

    /* ファイルが存在している場合はファイルポインタは先頭にセットされる */
    err = xfs_open(name, X_OPEN_MODE_APPEND_PLUS, &fp);
    TEST_ASSERT_NOT_NULL(fp);

    err = xfs_tell(fp, &pos);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(0, pos);

    /* 読み書きできる */
    err = xfs_write(fp, WRITE_DATA, WRITE_LEN, &nwritten);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN, nwritten);

    err = xfs_seek(fp, 0, X_SEEK_SET);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xfs_read(fp, buf, WRITE_LEN * 2, &nread);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(WRITE_LEN * 2, nread);
    TEST_ASSERT_TRUE(memcmp(buf, WRITE_DATA, WRITE_LEN) == 0);
    TEST_ASSERT_TRUE(memcmp(buf + WRITE_LEN, WRITE_DATA, WRITE_LEN) == 0);

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


bool PrintTree(void* userptr, const char* path, const XStat* statbuf, const XDirEnt* dirent)
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

    err = xfs_mkdir("foo");
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xfs_mkdir("foo/bar");
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xfs_mkdir("foo/baz");
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xfs_copytree("foo", "hoge");
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xfs_copytree("foo", "/otherfs/tree");
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xfs_walktree("/", PrintTree, NULL);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xfs_walktree("/otherfs", PrintTree, NULL);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
}


TEST(xfs, mount)
{
    XError err;
    XFile* fp;
    XDir*  dir;

    xfs_deinit();

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

    err = xfs_mount(&vramfs1, "/", "/");
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xfs_mount(&vramfs2, "/mnt", "/home");
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xfs_open("/mnt/test.txt", X_OPEN_MODE_READ, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    xfs_close(fp);

    xfs_deinit();

    err = xfs_mount(&vramfs1, "/", "/home");
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xfs_open("/test.txt", X_OPEN_MODE_READ, &fp);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    xfs_close(fp);

    err = xfs_opendir("/", &dir);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    xfs_closedir(dir);
    xfs_deinit();


    err = xfs_mount(&vramfs1, "/", "/");
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xfs_mount(vfatfs, "/mnt", "0:/");
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xfs_opendir("/mnt", &dir);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    xfs_closedir(dir);

    xfs_deinit();


    /* "/", "/"
     * "/tmp", "/"
     * "/", "/tmp"
     */
}


TEST(xfs, rmtree)
{
    XError err;

    err = xfs_rmtree("/otherfs2/bar");
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xfs_walktree("/", PrintTree, NULL);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
}


TEST_GROUP_RUNNER(xfs)
{
    RUN_TEST_CASE(xfs, open_write);
    RUN_TEST_CASE(xfs, open_read);
    RUN_TEST_CASE(xfs, write);
    RUN_TEST_CASE(xfs, read);
    RUN_TEST_CASE(xfs, stat);
    RUN_TEST_CASE(xfs, tell);
    RUN_TEST_CASE(xfs, seek);
    RUN_TEST_CASE(xfs, flush);
    RUN_TEST_CASE(xfs, mkdir);
    RUN_TEST_CASE(xfs, opendir);
    RUN_TEST_CASE(xfs, readdir);
    RUN_TEST_CASE(xfs, remove);
    RUN_TEST_CASE(xfs, rename);
    RUN_TEST_CASE(xfs, open_write_plus);
    RUN_TEST_CASE(xfs, open_read_plus);
    RUN_TEST_CASE(xfs, open_append);
    RUN_TEST_CASE(xfs, open_append_plus);
    RUN_TEST_CASE(xfs, copyfile);
    RUN_TEST_CASE(xfs, copytree);
    RUN_TEST_CASE(xfs, rmtree);
    RUN_TEST_CASE(xfs, mount);
}
