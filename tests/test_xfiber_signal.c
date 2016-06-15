#include <picox/multitask/xfiber.h>
#include "testutils.h"


#define KERNEL_WORK_SIZE    (1024 * 20)
#define STACK_SIZE          (2048)
#define PRIORITY            (4)


TEST_GROUP(xfiber_signal);


TEST_SETUP(xfiber_signal)
{
}


TEST_TEAR_DOWN(xfiber_signal)
{
}


typedef struct
{
    XTicks          delay;
    XBits           bits;
    XFiber*         fiber;
    XError          expect_err;
} RaiseArg;


static RaiseArg* CreateRaiseArg(XFiber* fiber, XTicks delay, XBits bits, XError expect_err)
{
    RaiseArg* ret = x_malloc(sizeof(RaiseArg));
    X_ASSERT(ret);

    ret->delay = delay;
    ret->bits = bits;
    ret->fiber = fiber;
    ret->expect_err = expect_err;
    return ret;
}


static void RaiseTask(void* a)
{
    RaiseArg* arg = a;

    if (arg->delay > 0)
        xfiber_delay(arg->delay);

    const XError err = xfiber_signal_raise(arg->fiber, arg->bits);
    TEST_ASSERT_EQUAL(arg->expect_err, err);

    x_free(arg);
}


static void WaitTaskMain(void* a)
{
    X_UNUSED(a);

    XError err;
    XBits result_sigs;
    RaiseArg* arg;
    XFiber* self = xfiber_self();

    arg = CreateRaiseArg(self, x_msec_to_ticks(10), 0x10, X_ERR_NONE);
    TEST_ASSERT_EQUAL(X_ERR_NONE, xfiber_create(NULL, PRIORITY, "task1", STACK_SIZE, RaiseTask, arg));
    arg = CreateRaiseArg(self, x_msec_to_ticks(30), 0x01, X_ERR_NONE);
    TEST_ASSERT_EQUAL(X_ERR_NONE, xfiber_create(NULL, PRIORITY, "task2", STACK_SIZE, RaiseTask, arg));
    arg = CreateRaiseArg(self, x_msec_to_ticks(50), 0x02, X_ERR_NONE);
    TEST_ASSERT_EQUAL(X_ERR_NONE, xfiber_create(NULL, PRIORITY, "task3", STACK_SIZE, RaiseTask, arg));
    arg = CreateRaiseArg(self, x_msec_to_ticks(1), 0x80, X_ERR_NONE);
    TEST_ASSERT_EQUAL(X_ERR_NONE, xfiber_create(NULL, PRIORITY, "task3", STACK_SIZE, RaiseTask, arg));

    const XBits expect = 0x13;
    XBits cur = expect;
    for (;;)
    {
        err = xfiber_signal_wait(cur, &result_sigs);
        TEST_ASSERT_EQUAL(X_ERR_NONE, err);

        /* 関係ないビットがセットされないこと */
        TEST_ASSERT_TRUE((result_sigs & (~expect)) == 0);
        cur &= ~result_sigs;
        if (cur == 0)
            break;
    }
    /* 待ちを行っていないビットはクリアされていないこと */
    TEST_ASSERT_EQUAL(0x80, xfiber_signal_get(self));

    xfiber_kernel_end_scheduler();
}


static void TimedWaitTaskMain(void* a)
{
    X_UNUSED(a);

    XError err;
    XBits result_sigs;
    RaiseArg* arg;
    XFiber* self = xfiber_self();

    arg = CreateRaiseArg(self, x_msec_to_ticks(10), 0x10, X_ERR_NONE);
    TEST_ASSERT_EQUAL(X_ERR_NONE, xfiber_create(NULL, PRIORITY, "task1", STACK_SIZE, RaiseTask, arg));
    arg = CreateRaiseArg(self, x_msec_to_ticks(30), 0x01, X_ERR_NONE);
    TEST_ASSERT_EQUAL(X_ERR_NONE, xfiber_create(NULL, PRIORITY, "task2", STACK_SIZE, RaiseTask, arg));
    arg = CreateRaiseArg(self, x_msec_to_ticks(50), 0x02, X_ERR_NONE);
    TEST_ASSERT_EQUAL(X_ERR_NONE, xfiber_create(NULL, PRIORITY, "task3", STACK_SIZE, RaiseTask, arg));

    const XBits expect = ~0x13;
    err = xfiber_signal_timed_wait(expect, &result_sigs, x_msec_to_ticks(100));
    TEST_ASSERT_EQUAL(X_ERR_TIMED_OUT, err);
    xfiber_kernel_end_scheduler();
}


static void TryWaitTaskMain(void* a)
{
    X_UNUSED(a);

    XError err;
    XBits result_sigs;
    RaiseArg* arg;
    XFiber* self = xfiber_self();

    arg = CreateRaiseArg(self, x_msec_to_ticks(30), 0x10, X_ERR_NONE);
    TEST_ASSERT_EQUAL(X_ERR_NONE, xfiber_create(NULL, PRIORITY, "task1", STACK_SIZE, RaiseTask, arg));

    err = xfiber_signal_try_wait(0xFF, &result_sigs);
    TEST_ASSERT_EQUAL(X_ERR_TIMED_OUT, err);

    xfiber_delay(x_msec_to_ticks(50));

    err = xfiber_signal_try_wait(0xFF, &result_sigs);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(0x10, result_sigs);

    xfiber_kernel_end_scheduler();
}


TEST(xfiber_signal, wait)
{
    xfiber_kernel_init(NULL, KERNEL_WORK_SIZE, NULL);
    xfiber_create(NULL, PRIORITY, "main", STACK_SIZE, WaitTaskMain, NULL);
    xfiber_kernel_start_scheduler();
}


TEST(xfiber_signal, timed_wait)
{
    xfiber_kernel_init(NULL, KERNEL_WORK_SIZE, NULL);
    xfiber_create(NULL, PRIORITY, "main", STACK_SIZE, TimedWaitTaskMain, NULL);
    xfiber_kernel_start_scheduler();
}


TEST(xfiber_signal, try_wait)
{
    xfiber_kernel_init(NULL, KERNEL_WORK_SIZE, NULL);
    xfiber_create(NULL, PRIORITY, "main", STACK_SIZE, TryWaitTaskMain, NULL);
    xfiber_kernel_start_scheduler();
}


TEST_GROUP_RUNNER(xfiber_signal)
{
    RUN_TEST_CASE(xfiber_signal, wait);
    RUN_TEST_CASE(xfiber_signal, try_wait);
    RUN_TEST_CASE(xfiber_signal, timed_wait);
}
