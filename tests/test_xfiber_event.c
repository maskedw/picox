#include <picox/multitask/xfiber.h>
#include "testutils.h"


#define KERNEL_WORK_SIZE    (1024 * 20)
#define STACK_SIZE          (2048)
#define PRIORITY            (8)


TEST_GROUP(xfiber_event);


TEST_SETUP(xfiber_event)
{
}


TEST_TEAR_DOWN(xfiber_event)
{
}


typedef struct
{
    XTicks          delay;
    XBits           bits;
    XFiberEvent*    event;
    XError          expect_err;
} SetArg;


static SetArg* CreateSetArg(XFiberEvent* event, XTicks delay, XBits bits, XError expect_err)
{
    SetArg* ret = x_malloc(sizeof(SetArg));
    X_ASSERT(ret);

    ret->delay = delay;
    ret->bits = bits;
    ret->event = event;
    ret->expect_err = expect_err;
    return ret;
}


typedef struct
{
    XTicks          delay;
    XFiberEvent*    event;
} DestroyArg;


static DestroyArg* CreateDestroyArg(XFiberEvent* event, XTicks delay)
{
    DestroyArg* ret = x_malloc(sizeof(DestroyArg));
    X_ASSERT(ret);

    ret->delay = delay;
    ret->event = event;
    return ret;
}


static void SetTask(void* a)
{
    SetArg* arg = a;

    if (arg->delay > 0)
        xfiber_delay(arg->delay);

    const XError err = xfiber_event_set(arg->event, arg->bits);
    TEST_ASSERT_EQUAL(arg->expect_err, err);

    x_free(arg);
}


static void DestroyTask(void* a)
{
    DestroyArg* arg = a;

    if (arg->delay > 0)
        xfiber_delay(arg->delay);

    xfiber_event_destroy(arg->event);
    x_free(arg);
}


static void GetTaskMain(void* a)
{
    X_UNUSED(a);

    XError err;
    XFiberEvent* event;

    err = xfiber_event_create(&event);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_NOT_NULL(event);
    TEST_ASSERT_EQUAL_HEX(0x00, xfiber_event_get(event));

    TEST_ASSERT_EQUAL(X_ERR_NONE, xfiber_event_set(event, 0x01));
    TEST_ASSERT_EQUAL_HEX(0x01, xfiber_event_get(event));

    TEST_ASSERT_EQUAL(X_ERR_NONE, xfiber_event_set(event, 0xB0));
    TEST_ASSERT_EQUAL_HEX(0xB1, xfiber_event_get(event));

    xfiber_event_destroy(event);
    xfiber_kernel_end_scheduler();
}


static void ClearTaskMain(void* a)
{
    X_UNUSED(a);

    XError err;
    XFiberEvent* event;

    err = xfiber_event_create(&event);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_NOT_NULL(event);
    TEST_ASSERT_EQUAL_HEX(0x00, xfiber_event_get(event));

    TEST_ASSERT_EQUAL(X_ERR_NONE, xfiber_event_set(event, 0x33));
    TEST_ASSERT_EQUAL_HEX(0x33, xfiber_event_clear(event, 0x21));
    TEST_ASSERT_EQUAL_HEX(0x12, xfiber_event_get(event));

    TEST_ASSERT_EQUAL_HEX(0x12, xfiber_event_clear(event, 0xFF));
    TEST_ASSERT_EQUAL_HEX(0x00, xfiber_event_get(event));

    xfiber_event_destroy(event);
    xfiber_kernel_end_scheduler();
}


static void WaitClearOnExitTaskMain(void* a)
{
    X_UNUSED(a);

    XError err;
    XFiberEvent* event;
    XBits result_pattern;

    err = xfiber_event_create(&event);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_NOT_NULL(event);

    SetArg* arg;
    arg = CreateSetArg(event, 0, 0x10, X_ERR_NONE);
    TEST_ASSERT_EQUAL(X_ERR_NONE, xfiber_create(NULL, PRIORITY, "task1", STACK_SIZE, SetTask, arg));
    arg = CreateSetArg(event, 0, 0x01, X_ERR_NONE);
    TEST_ASSERT_EQUAL(X_ERR_NONE, xfiber_create(NULL, PRIORITY, "task2", STACK_SIZE, SetTask, arg));
    arg = CreateSetArg(event, 0, 0x02, X_ERR_NONE);
    TEST_ASSERT_EQUAL(X_ERR_NONE, xfiber_create(NULL, PRIORITY, "task3", STACK_SIZE, SetTask, arg));

    err = xfiber_event_wait(
            event,
            X_FIBER_EVENT_WAIT_AND | X_FIBER_EVENT_CLEAR_ON_EXIT,
            0x13,
            &result_pattern);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL_HEX(0x13, result_pattern);
    TEST_ASSERT_EQUAL_HEX(0x00, xfiber_event_get(event));

    xfiber_event_destroy(event);
    xfiber_kernel_end_scheduler();
}


static void WaitTaskMain(void* a)
{
    X_UNUSED(a);

    XError err;
    XFiberEvent* event;
    XBits result_pattern;
    SetArg* arg;

    err = xfiber_event_create(&event);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_NOT_NULL(event);

    arg = CreateSetArg(event, 0, 0x10, X_ERR_NONE);
    TEST_ASSERT_EQUAL(X_ERR_NONE, xfiber_create(NULL, PRIORITY, "task1", STACK_SIZE, SetTask, arg));
    arg = CreateSetArg(event, 0, 0x01, X_ERR_NONE);
    TEST_ASSERT_EQUAL(X_ERR_NONE, xfiber_create(NULL, PRIORITY, "task2", STACK_SIZE, SetTask, arg));
    arg = CreateSetArg(event, 0, 0x02, X_ERR_NONE);
    TEST_ASSERT_EQUAL(X_ERR_NONE, xfiber_create(NULL, PRIORITY, "task3", STACK_SIZE, SetTask, arg));
    err = xfiber_event_wait(
            event,
            X_FIBER_EVENT_WAIT_AND,
            0x13,
            &result_pattern);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL_HEX(0x13, result_pattern);
    TEST_ASSERT_EQUAL_HEX(0x13, xfiber_event_get(event));

    xfiber_event_destroy(event);
    xfiber_kernel_end_scheduler();
}


static void WaitTimeoutTaskMain(void* a)
{
    X_UNUSED(a);

    XError err;
    XFiberEvent* event;
    XBits result_pattern;
    SetArg* arg;

    err = xfiber_event_create(&event);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_NOT_NULL(event);

    arg = CreateSetArg(event, 0, 0x10, X_ERR_NONE);
    TEST_ASSERT_EQUAL(X_ERR_NONE, xfiber_create(NULL, PRIORITY, "task1", STACK_SIZE, SetTask, arg));
    arg = CreateSetArg(event, 0, 0x01, X_ERR_NONE);
    TEST_ASSERT_EQUAL(X_ERR_NONE, xfiber_create(NULL, PRIORITY, "task2", STACK_SIZE, SetTask, arg));

    XTicks start = x_ticks_now();
    err = xfiber_event_timed_wait(
            event,
            X_FIBER_EVENT_WAIT_AND,
            0x13,
            &result_pattern,
            x_msec_to_ticks(50));
    TEST_ASSERT_EQUAL(X_ERR_TIMED_OUT, err);
    TEST_ASSERT_TRUE(x_msec_to_ticks(50) >= x_ticks_now() - start);

    xfiber_event_destroy(event);
    xfiber_kernel_end_scheduler();
}


static void TryWaitTaskMain(void* a)
{
    X_UNUSED(a);

    XError err;
    XFiberEvent* event;
    XBits result_pattern;
    SetArg* arg;

    err = xfiber_event_create(&event);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_NOT_NULL(event);

    arg = CreateSetArg(event, x_msec_to_ticks(30), 0x10, X_ERR_NONE);
    TEST_ASSERT_EQUAL(X_ERR_NONE, xfiber_create(NULL, PRIORITY, "task1", STACK_SIZE, SetTask, arg));
    arg = CreateSetArg(event, 0, 0x01, X_ERR_NONE);
    TEST_ASSERT_EQUAL(X_ERR_NONE, xfiber_create(NULL, PRIORITY, "task2", STACK_SIZE, SetTask, arg));
    arg = CreateSetArg(event, 0, 0x02, X_ERR_NONE);
    TEST_ASSERT_EQUAL(X_ERR_NONE, xfiber_create(NULL, PRIORITY, "task3", STACK_SIZE, SetTask, arg));
    err = xfiber_event_try_wait(
            event,
            X_FIBER_EVENT_WAIT_AND,
            0x13,
            &result_pattern);
    TEST_ASSERT_EQUAL(X_ERR_TIMED_OUT, err);
    xfiber_delay(x_msec_to_ticks(50));

    err = xfiber_event_try_wait(
            event,
            X_FIBER_EVENT_WAIT_AND,
            0x13,
            &result_pattern);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL_HEX(0x13, result_pattern);
    TEST_ASSERT_EQUAL_HEX(0x13, xfiber_event_get(event));

    xfiber_event_destroy(event);
    xfiber_kernel_end_scheduler();
}


static void DestroyTaskMain(void* a)
{
    X_UNUSED(a);

    XError err;
    XFiberEvent* event;
    XBits result_pattern;
    DestroyArg* arg;

    err = xfiber_event_create(&event);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_NOT_NULL(event);

    arg = CreateDestroyArg(event, x_msec_to_ticks(30));
    TEST_ASSERT_EQUAL(X_ERR_NONE, xfiber_create(NULL, PRIORITY, "task1", STACK_SIZE, DestroyTask, arg));
    err = xfiber_event_wait(
            event,
            X_FIBER_EVENT_WAIT_AND,
            0x13,
            &result_pattern);
    TEST_ASSERT_EQUAL(X_ERR_CANCELED, err);
    xfiber_kernel_end_scheduler();
}


TEST(xfiber_event, get)
{
    xfiber_kernel_init(NULL, KERNEL_WORK_SIZE, NULL);
    xfiber_create(NULL, PRIORITY, "main", STACK_SIZE, GetTaskMain, NULL);
    xfiber_kernel_start_scheduler();
}


TEST(xfiber_event, clear)
{
    xfiber_kernel_init(NULL, KERNEL_WORK_SIZE, NULL);
    xfiber_create(NULL, PRIORITY, "main", STACK_SIZE, ClearTaskMain, NULL);
    xfiber_kernel_start_scheduler();
}


TEST(xfiber_event, wait_clear_on_exit)
{
    xfiber_kernel_init(NULL, KERNEL_WORK_SIZE, NULL);
    xfiber_create(NULL, PRIORITY, "main", STACK_SIZE, WaitClearOnExitTaskMain, NULL);
    xfiber_kernel_start_scheduler();
}


TEST(xfiber_event, wait)
{
    xfiber_kernel_init(NULL, KERNEL_WORK_SIZE, NULL);
    xfiber_create(NULL, PRIORITY, "main", STACK_SIZE, WaitTaskMain, NULL);
    xfiber_kernel_start_scheduler();
}


TEST(xfiber_event, wait_timeout)
{
    xfiber_kernel_init(NULL, KERNEL_WORK_SIZE, NULL);
    xfiber_create(NULL, PRIORITY, "main", STACK_SIZE, WaitTimeoutTaskMain, NULL);
    xfiber_kernel_start_scheduler();
}


TEST(xfiber_event, try_wait)
{
    xfiber_kernel_init(NULL, KERNEL_WORK_SIZE, NULL);
    xfiber_create(NULL, PRIORITY, "main", STACK_SIZE, TryWaitTaskMain, NULL);
    xfiber_kernel_start_scheduler();
}


TEST(xfiber_event, destroy)
{
    xfiber_kernel_init(NULL, KERNEL_WORK_SIZE, NULL);
    xfiber_create(NULL, PRIORITY, "main", STACK_SIZE, DestroyTaskMain, NULL);
    xfiber_kernel_start_scheduler();
}


TEST_GROUP_RUNNER(xfiber_event)
{
    RUN_TEST_CASE(xfiber_event, get);
    RUN_TEST_CASE(xfiber_event, clear);
    RUN_TEST_CASE(xfiber_event, wait_clear_on_exit);
    RUN_TEST_CASE(xfiber_event, wait);
    RUN_TEST_CASE(xfiber_event, wait_timeout);
    RUN_TEST_CASE(xfiber_event, try_wait);
    RUN_TEST_CASE(xfiber_event, destroy);
}
