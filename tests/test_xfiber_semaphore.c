#include <picox/multitask/xfiber.h>
#include "testutils.h"


#define KERNEL_WORK_SIZE    (1024 * 20)
#define STACK_SIZE          (2048)
#define PRIORITY            (8)


TEST_GROUP(xfiber_semaphore);


TEST_SETUP(xfiber_semaphore)
{
}


TEST_TEAR_DOWN(xfiber_semaphore)
{
}


typedef struct
{
    XTicks              delay;
    XFiberSemaphore*    semaphore;
    XTicks              hold_time;
    XError              expect_err;
} TakeArg;


static TakeArg* CreateTakeArg(XFiberSemaphore* semaphore, XTicks delay, XTicks hold_time, XError expect_err)
{
    TakeArg* ret = x_malloc(sizeof(TakeArg));
    X_ASSERT(ret);

    ret->delay = delay;
    ret->hold_time = hold_time;
    ret->semaphore = semaphore;
    ret->expect_err = expect_err;
    return ret;
}


typedef struct
{
    XTicks          delay;
    XFiberEvent*    semaphore;
} DestroyArg;


static DestroyArg* CreateDestroyArg(XFiberSemaphore* semaphore, XTicks delay)
{
    DestroyArg* ret = x_malloc(sizeof(DestroyArg));
    X_ASSERT(ret);

    ret->delay = delay;
    ret->semaphore = semaphore;
    return ret;
}


static void TakeTask(void* a)
{
    TakeArg* arg = a;

    if (arg->delay > 0)
        xfiber_delay(arg->delay);

    const XError err = xfiber_semaphore_take(arg->semaphore);
    TEST_ASSERT_EQUAL(arg->expect_err, err);

    xfiber_delay(arg->hold_time);
    xfiber_semaphore_give(arg->semaphore);

    x_free(arg);

}


static void DestroyTask(void* a)
{
    DestroyArg* arg = a;

    if (arg->delay > 0)
        xfiber_delay(arg->delay);

    XFiberSemaphore* sem = arg->semaphore;
    x_free(arg);
    xfiber_semaphore_destroy(sem);
}


static void TakeTaskMain(void* a)
{
    X_UNUSED(a);

    XError err;
    TakeArg* arg;
    XFiberSemaphore* semaphore;

    err = xfiber_semaphore_create(&semaphore, 2);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    arg = CreateTakeArg(semaphore, 0, x_msec_to_ticks(50), X_ERR_NONE);
    TEST_ASSERT_EQUAL(X_ERR_NONE, xfiber_create(NULL, PRIORITY, "task1", STACK_SIZE, TakeTask, arg));

    arg = CreateTakeArg(semaphore, 0, x_msec_to_ticks(50), X_ERR_NONE);
    TEST_ASSERT_EQUAL(X_ERR_NONE, xfiber_create(NULL, PRIORITY, "task2", STACK_SIZE, TakeTask, arg));

    xfiber_delay(x_msec_to_ticks(20));

    err = xfiber_semaphore_try_take(semaphore);
    TEST_ASSERT_EQUAL(X_ERR_TIMED_OUT, err);

    err = xfiber_semaphore_take(semaphore);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    xfiber_semaphore_destroy(semaphore);
    xfiber_kernel_end_scheduler();
}


static void TimedTakeTaskMain(void* a)
{
    X_UNUSED(a);

    XError err;
    XFiberSemaphore* semaphore;

    err = xfiber_semaphore_create(&semaphore, 2);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    TEST_ASSERT_EQUAL(X_ERR_NONE, xfiber_semaphore_take(semaphore));
    TEST_ASSERT_EQUAL(X_ERR_NONE, xfiber_semaphore_take(semaphore));
    TEST_ASSERT_EQUAL(X_ERR_TIMED_OUT, xfiber_semaphore_timed_take(semaphore, x_msec_to_ticks(50)));

    xfiber_semaphore_destroy(semaphore);
    xfiber_kernel_end_scheduler();
}


static void DestroyTaskMain(void* a)
{
    X_UNUSED(a);

    XError err;
    DestroyArg* arg;
    XFiberSemaphore* semaphore;

    err = xfiber_semaphore_create(&semaphore, 2);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    arg = CreateDestroyArg(semaphore, x_msec_to_ticks(30));
    xfiber_create(NULL, PRIORITY, "task1", STACK_SIZE, DestroyTask, arg);

    TEST_ASSERT_EQUAL(X_ERR_NONE, xfiber_semaphore_take(semaphore));
    TEST_ASSERT_EQUAL(X_ERR_NONE, xfiber_semaphore_take(semaphore));
    TEST_ASSERT_EQUAL(X_ERR_CANCELED, xfiber_semaphore_take(semaphore));

    xfiber_kernel_end_scheduler();
}



TEST(xfiber_semaphore, take)
{
    xfiber_kernel_init(NULL, KERNEL_WORK_SIZE, NULL);
    xfiber_create(NULL, PRIORITY, "main", STACK_SIZE, TakeTaskMain, NULL);
    xfiber_kernel_start_scheduler();
}


TEST(xfiber_semaphore, timed_take)
{
    xfiber_kernel_init(NULL, KERNEL_WORK_SIZE, NULL);
    xfiber_create(NULL, PRIORITY, "main", STACK_SIZE, TimedTakeTaskMain, NULL);
    xfiber_kernel_start_scheduler();
}


TEST(xfiber_semaphore, destroy)
{
    xfiber_kernel_init(NULL, KERNEL_WORK_SIZE, NULL);
    xfiber_create(NULL, PRIORITY, "main", STACK_SIZE, DestroyTaskMain, NULL);
    xfiber_kernel_start_scheduler();
}


TEST_GROUP_RUNNER(xfiber_semaphore)
{
    RUN_TEST_CASE(xfiber_semaphore, take);
    RUN_TEST_CASE(xfiber_semaphore, timed_take);
    RUN_TEST_CASE(xfiber_semaphore, destroy);
}
