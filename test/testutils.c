#include "testutils.h"

jmp_buf     g_xjmp;

void x_escape_assertion_failed(const char* expr, const char* msg, const char* func, const char* file, int line)
{
    X_UNUSED(expr);
    X_UNUSED(msg);
    X_UNUSED(func);
    X_UNUSED(file);
    X_UNUSED(line);

#if 0
    const char* none = "none";
    printf("Assertion failed\n");
    printf("[MSG ] %s\n", msg ? msg : none);
    printf("[EXPR] %s\n", expr ? expr : none);
    printf("[FUNC] %s\n", func ? func : none);
    printf("[FILE] %s\n", file ? file : none);
    printf("[LINE] %d\n", line);
    printf("************************\n");
#endif
    longjmp(g_xjmp, 1);
}
