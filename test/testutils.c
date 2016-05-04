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
    fflush(stdout);
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

#if X_XFS_TYPE == X_XFS_TYPE_UNION_FS
    xunionfs_init();
    X_ASSERT(xunionfs_mount(vramfs, "/", "/") == X_ERR_NONE);
#else
    xsinglefs_init(vramfs);
#endif
}


void x_test_deinit_fs()
{
    xfs_deinit();
    xramfs_deinit(ramfs);
    x_free(vramfs);
    x_free(ramfs);
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


