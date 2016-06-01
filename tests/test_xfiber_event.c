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


static void SetTask(void* a)
{
    SetArg* arg = a;

    if (arg->delay > 0)
        xfiber_delay(arg->delay);

    const XError err = xfiber_event_set(arg->event, arg->bits);
    TEST_ASSERT_EQUAL(arg->expect_err, err);

    x_free(arg);
}


static void SetWaitTaskMain(void* a)
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

    for (int i = 0; i < 0; i++)
        xfiber_yield();
    arg = CreateSetArg(event, 0, 0x10, X_ERR_NONE);
    TEST_ASSERT_EQUAL(X_ERR_NONE, xfiber_create(NULL, PRIORITY, "task4", STACK_SIZE, SetTask, arg));
    arg = CreateSetArg(event, 0, 0x01, X_ERR_NONE);
    TEST_ASSERT_EQUAL(X_ERR_NONE, xfiber_create(NULL, PRIORITY, "task5", STACK_SIZE, SetTask, arg));
    arg = CreateSetArg(event, 0, 0x02, X_ERR_NONE);
    TEST_ASSERT_EQUAL(X_ERR_NONE, xfiber_create(NULL, PRIORITY, "task6", STACK_SIZE, SetTask, arg));
    err = xfiber_event_wait(
            event,
            X_FIBER_EVENT_WAIT_AND,
            0x13,
            &result_pattern);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL_HEX(0x13, result_pattern);
    TEST_ASSERT_EQUAL_HEX(0x13, xfiber_event_get(event));
    TEST_ASSERT_EQUAL_HEX(0x13, xfiber_event_clear(event, 0x13));
    TEST_ASSERT_EQUAL_HEX(0x00, xfiber_event_get(event));

    arg = CreateSetArg(event, 0, 0x10, X_ERR_NONE);
    TEST_ASSERT_EQUAL(X_ERR_NONE, xfiber_create(NULL, PRIORITY, "task7", STACK_SIZE, SetTask, arg));
    arg = CreateSetArg(event, 0, 0x01, X_ERR_NONE);
    TEST_ASSERT_EQUAL(X_ERR_NONE, xfiber_create(NULL, PRIORITY, "task8", STACK_SIZE, SetTask, arg));
    err = xfiber_event_timed_wait(
            event,
            X_FIBER_EVENT_WAIT_AND,
            0x13,
            &result_pattern,
            x_msec_to_ticks(50));
    TEST_ASSERT_EQUAL(X_ERR_TIMED_OUT, err);
    TEST_ASSERT_EQUAL_HEX(0x11, xfiber_event_get(event));

    xfiber_event_destroy(event);
    xfiber_kernel_end_scheduler();
}


TEST(xfiber_event, set)
{
    xfiber_kernel_init(NULL, KERNEL_WORK_SIZE, NULL);
    xfiber_create(NULL, PRIORITY, "main", STACK_SIZE, SetWaitTaskMain, NULL);
    xfiber_kernel_start_scheduler();
}


TEST_GROUP_RUNNER(xfiber_event)
{
    RUN_TEST_CASE(xfiber_event, set);
}
