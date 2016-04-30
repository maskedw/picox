#include "testutils.h"
#include <picox/filesystem/xfs.h>
#include <picox/filesystem/xramfs.h>
#include <picox/filesystem/xfatfs.h>
#include <picox/filesystem/xposixfs.h>
#include <ff.h>

jmp_buf     g_xjmp;
static XRamFs* ramfs;
static XPosixFs* posixfs;
static XFatFs* fatfs;
static FATFS* fatfsbody;
static XVirtualFs* vposixfs;
static XVirtualFs* vramfs;
static XVirtualFs* vfatfs;


void x_escape_assertion_failed(const char* expr, const char* msg, const char* func, const char* file, int line)
{
    X_UNUSED(expr);
    X_UNUSED(msg);
    X_UNUSED(func);
    X_UNUSED(file);
    X_UNUSED(line);

    longjmp(g_xjmp, 1);
}


static void X__PostAssertionFailed(void)
{
    while (1);
}


void x_init_test(void)
{
    x_putc_stdout = (XCharPutFunc)putchar;
    x_post_assertion_failed = (XAssertionFailedFunc)X__PostAssertionFailed;
}


#if 0
void x_test_init_fs()
{
    posixfs = x_malloc(sizeof(*posixfs));
    vposixfs = x_malloc(sizeof(*vposixfs));
    X_ASSERT(posixfs);
    X_ASSERT(vposixfs);

    xposixfs_init(posixfs);
    xposixfs_init_vfs(posixfs, vposixfs);
    xfs_init();

    int err = xfs_mount(vposixfs, "/", "/tmp/");
    X_ASSERT(err == X_ERR_NONE);
}


void x_test_deinit_fs()
{
    xfs_deinit();
    xposixfs_deinit(posixfs);
    x_free(vposixfs);
    x_free(posixfs);
}
#endif
#if 1
void x_test_init_fs()
{
    ramfs = x_malloc(sizeof(*ramfs));
    vramfs = x_malloc(sizeof(*vramfs));
    X_ASSERT(ramfs);
    X_ASSERT(vramfs);

    X_ASSERT(xramfs_init(ramfs, NULL, 10 * 1024) == X_ERR_NONE);
    xramfs_init_vfs(ramfs, vramfs);
    xfs_init();
    X_ASSERT(xfs_mount(vramfs, "/", "/") == X_ERR_NONE);
}


void x_test_deinit_fs()
{
    xfs_deinit();
    xramfs_deinit(ramfs);
    x_free(vramfs);
    x_free(ramfs);
}
#endif


#if 0
void x_test_init_fs()
{
    fatfsbody = x_malloc(sizeof(*fatfsbody));
    fatfs = x_malloc(sizeof(*fatfs));
    vfatfs = x_malloc(sizeof(*vfatfs));
    X_ASSERT(fatfs);
    X_ASSERT(vfatfs);

    FRESULT res;
    res = f_mount(fatfsbody, "0:", 0);
    TEST_ASSERT_EQUAL(FR_OK, res);
    res = f_mkfs("0:", 0, 0);
    TEST_ASSERT_EQUAL(FR_OK, res);
    xfatfs_init(fatfs);
    xfatfs_init_vfs(fatfs, vfatfs);
    xfs_init();
    X_ASSERT(xfs_mount(vfatfs, "/", "0:/") == X_ERR_NONE);
}


void x_test_deinit_fs()
{
    xfs_deinit();
    xfatfs_deinit(fatfs);
    f_mount(NULL, "0:", 0);
    x_free(vfatfs);
    x_free(fatfs);
    x_free(fatfsbody);
}
#endif
