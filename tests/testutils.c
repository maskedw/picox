#include "testutils.h"
#include <picox/filesystem/xfs.h>
#include <picox/filesystem/xramfs.h>
#include <picox/filesystem/xfatfs.h>
#include <picox/filesystem/xposixfs.h>
#include <ff.h>

jmp_buf     g_xjmp;

#define FS_TYPE_RAMfS   0
#define FS_TYPE_FATFS   1
#define FS_TYPE         FS_TYPE_RAMfS
// #define FS_TYPE         FS_TYPE_RAMfS


#if FS_TYPE == FS_TYPE_RAMfS
static XRamFs* ramfs;
#else
static XFatFs* xfatfs;
static FATFS* fatfs;
void disk_deinit(BYTE pdrv);
#endif
static XVirtualFs* vfs;


void x_test_init_fs()
{
    vfs = x_malloc(sizeof(*vfs));
    X_ASSERT(vfs);

#if FS_TYPE == FS_TYPE_RAMfS
    ramfs = x_malloc(sizeof(*ramfs));
    X_ASSERT(ramfs);

    X_ASSERT(xramfs_init(ramfs, NULL, 10 * 1024) == X_ERR_NONE);
    xramfs_init_vfs(ramfs, vfs);
#else
    xfatfs = malloc(sizeof(XFatFs));
    fatfs = malloc(sizeof(FATFS));
    X_ASSERT(xfatfs);
    X_ASSERT(fatfs);
    FRESULT res;
    res = f_mount(fatfs, "0:", 0);
    X_ASSERT(res == FR_OK);
    res = f_mkfs("0:", 0, 0);
    X_ASSERT(res == FR_OK);
    xfatfs_init(xfatfs);
    xfatfs_init_vfs(xfatfs, vfs);
#endif

#if X_XFS_TYPE == X_XFS_TYPE_UNION_FS
    xunionfs_init();
    X_ASSERT(xunionfs_mount(vfs, "/", "/") == X_ERR_NONE);
#else
    xsinglefs_init(vfs);
#endif
}


void x_test_deinit_fs()
{
    xfs_deinit();
    x_free(vfs);

#if FS_TYPE == FS_TYPE_RAMfS
    xramfs_deinit(ramfs);
    x_free(ramfs);
#else
    xfatfs_deinit(xfatfs);
    f_mount(NULL, "0:", 0);
    free(fatfs);
    free(xfatfs);
    disk_deinit(0);
#endif
}


void x_test_stream(XStream* stream)
{
    char buf[128];
    size_t n;
    XSize pos;
    char data[] = "Hello World";

    memset(buf, '@', sizeof(buf));
    TEST_ASSERT_EQUAL(0, xstream_write(stream, data, sizeof(data), &n));
    TEST_ASSERT_EQUAL(n, sizeof(data));

    TEST_ASSERT_EQUAL(0, xstream_tell(stream, &pos));
    TEST_ASSERT_EQUAL(sizeof(data), pos);

    TEST_ASSERT_EQUAL(0, xstream_seek(stream, 0, X_SEEK_SET));
    TEST_ASSERT_EQUAL(0, xstream_read(stream, buf, n, &n));
    TEST_ASSERT_EQUAL(n, sizeof(data));
    buf[n] = '\0';
    TEST_ASSERT_EQUAL_STRING(buf, data);
    TEST_ASSERT_EQUAL(0, xstream_seek(stream, 0, X_SEEK_SET));

    const int ret = xstream_printf(stream, "%s %s\n", "hello", "world");
    TEST_ASSERT_EQUAL(ret, (int)strlen("hello world\n"));
    TEST_ASSERT_EQUAL(0, xstream_seek(stream, 0, X_SEEK_SET));
    TEST_ASSERT_EQUAL(0, xstream_gets(stream, buf, sizeof(buf), NULL, NULL));
    TEST_ASSERT_EQUAL_STRING(buf, "hello world");
}
