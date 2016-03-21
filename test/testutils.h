#ifndef testutils_h_
#define testutils_h_


#include <picox/core/xcore.h>
#include <setjmp.h>


#ifdef __cplusplus
extern "C" {
#endif


void x_init_test(void);
void x_escape_assertion_failed(const char* expr, const char* msg, const char* func, const char* file, int line);

#if 0
#ifdef X_HAS_STATEMENT_EXPRESSIONS
    #define X_IS_ASSERTION_FAILED(x)                                \
        ({                                                          \
            extern jmp_buf g_xjmp;                                  \
            XAssertionFailedFunc save = x_pre_assertion_failed;     \
            x_pre_assertion_failed = x_escape_assertion_failed;     \
            bool ret = false;                                       \
            if (setjmp(g_xjmp) == 0)                                \
                x;                                                  \
            else                                                    \
                ret = true;                                         \
            x_pre_assertion_failed = save;                          \
            ret;                                                    \
        })

    #define X_TEST_ASSERTION_FAILED(expr)       TEST_ASSERT_TRUE(X_IS_ASSERTION_FAILED(expr))
    #define X_TEST_ASSERTION_SUCCESS(expr)      TEST_ASSERT_FALSE(X_IS_ASSERTION_FAILED(expr))

#else
    #define X_TEST_ASSERTION_FAILED(expr)       (void)0
    #define X_TEST_ASSERTION_SUCCESS(expr)      (void)0
#endif
#endif
    #define X_TEST_ASSERTION_FAILED(expr)       (void)0
    #define X_TEST_ASSERTION_SUCCESS(expr)      (void)0


#ifdef __cplusplus
}
#endif


#endif // testutils_h_
