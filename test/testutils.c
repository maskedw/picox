#include "testutils.h"

jmp_buf     g_xjmp;

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
    exit(1);
}


void x_init_test(void)
{
    x_putc_stdout = (XCharPutFunc)putchar;
    x_post_assertion_failed = (XAssertionFailedFunc)X__PostAssertionFailed;
}
