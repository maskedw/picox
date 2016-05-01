#include "testutils.h"
#include <picox/filesystem/xfs.h>
#include <picox/filesystem/xramfs.h>
#include <picox/filesystem/xfatfs.h>
#include <picox/filesystem/xposixfs.h>
#include <ff.h>

jmp_buf     g_xjmp;
static XRamFs* ramfs;
static XVirtualFs* vramfs;


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
