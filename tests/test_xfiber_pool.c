#include <picox/multitask/xfiber.h>
#include "testutils.h"


#define KERNEL_WORK_SIZE    (1024 * 20)
#define STACK_SIZE          (2048)
#define PRIORITY            (4)


TEST_GROUP(xfiber_pool);


TEST_SETUP(xfiber_pool)
{
}


TEST_TEAR_DOWN(xfiber_pool)
{
}


typedef struct
{
    XTicks              delay;
    XFiberPool*         pool;
    XTicks              hold_time;
    XError              expect_err;
} GetArg;


static GetArg* CreateGetArg(XFiberPool* pool, XTicks delay, XTicks hold_time, XError expect_err)
{
    GetArg* ret = x_malloc(sizeof(GetArg));
    X_ASSERT(ret);

    ret->delay = delay;
    ret->hold_time = hold_time;
    ret->pool = pool;
    ret->expect_err = expect_err;
    return ret;
}


typedef struct
{
    XTicks          delay;
    XFiberPool*     pool;
} DestroyArg;


static DestroyArg* CreateDestroyArg(XFiberPool* pool, XTicks delay)
{
    DestroyArg* ret = x_malloc(sizeof(DestroyArg));
    X_ASSERT(ret);

    ret->delay = delay;
    ret->pool = pool;
    return ret;
}


static void GetTask(void* a)
{
    GetArg* arg = a;

    if (arg->delay > 0)
        xfiber_delay(arg->delay);

    void* ptr = NULL;
    const XError err = xfiber_pool_get(arg->pool, &ptr);
    TEST_ASSERT_EQUAL(arg->expect_err, err);
    if (err == X_ERR_NONE)
        TEST_ASSERT_NOT_NULL(ptr);

    xfiber_delay(arg->hold_time);
    xfiber_pool_release(arg->pool, ptr);
    x_free(arg);
}


static void DestroyTask(void* a)
{
    DestroyArg* arg = a;

    if (arg->delay > 0)
        xfiber_delay(arg->delay);

    XFiberPool* pool = arg->pool;
    x_free(arg);
    xfiber_pool_destroy(pool);
}


static void GetTaskMain(void* a)
{
    X_UNUSED(a);

    XError err;
    void* ptr = NULL;
    GetArg* arg;
    XFiberPool* pool;

    err = xfiber_pool_create(&pool, 16, 2);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    arg = CreateGetArg(pool, 0, x_msec_to_ticks(50), X_ERR_NONE);
    TEST_ASSERT_EQUAL(X_ERR_NONE, xfiber_create(NULL, PRIORITY, "task1", STACK_SIZE, GetTask, arg));
    arg = CreateGetArg(pool, 0, x_msec_to_ticks(50), X_ERR_NONE);
    TEST_ASSERT_EQUAL(X_ERR_NONE, xfiber_create(NULL, PRIORITY, "task2", STACK_SIZE, GetTask, arg));

    xfiber_delay(x_msec_to_ticks(20));
    err = xfiber_pool_try_get(pool, &ptr);
    TEST_ASSERT_EQUAL(X_ERR_TIMED_OUT, err);

    err = xfiber_pool_get(pool, &ptr);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_NOT_NULL(ptr);

    xfiber_pool_destroy(pool);
    xfiber_kernel_end_scheduler();
}


static void TimedGetTaskMain(void* a)
{
    X_UNUSED(a);

    XError err;
    void* ptr = NULL;
    XFiberPool* pool;

    err = xfiber_pool_create(&pool, 16, 2);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    TEST_ASSERT_EQUAL(X_ERR_NONE, xfiber_pool_get(pool, &ptr));
    TEST_ASSERT_EQUAL(X_ERR_NONE, xfiber_pool_get(pool, &ptr));
    TEST_ASSERT_EQUAL(X_ERR_TIMED_OUT, xfiber_pool_timed_get(pool, &ptr, x_msec_to_ticks(50)));

    xfiber_pool_destroy(pool);
    xfiber_kernel_end_scheduler();
}


static void DestroyTaskMain(void* a)
{
    X_UNUSED(a);

    XError err;
    DestroyArg* arg;
    void* ptr = NULL;
    XFiberPool* pool;

    err = xfiber_pool_create(&pool, 16, 2);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    arg = CreateDestroyArg(pool, x_msec_to_ticks(30));
    xfiber_create(NULL, PRIORITY, "task1", STACK_SIZE, DestroyTask, arg);

    TEST_ASSERT_EQUAL(X_ERR_NONE, xfiber_pool_get(pool, &ptr));
    TEST_ASSERT_EQUAL(X_ERR_NONE, xfiber_pool_get(pool, &ptr));
    TEST_ASSERT_EQUAL(X_ERR_CANCELED, xfiber_pool_get(pool, &ptr));

    xfiber_kernel_end_scheduler();
}


TEST(xfiber_pool, get)
{
    xfiber_kernel_init(NULL, KERNEL_WORK_SIZE, NULL);
    xfiber_create(NULL, PRIORITY, "main", STACK_SIZE, GetTaskMain, NULL);
    xfiber_kernel_start_scheduler();
}


TEST(xfiber_pool, timed_get)
{
    xfiber_kernel_init(NULL, KERNEL_WORK_SIZE, NULL);
    xfiber_create(NULL, PRIORITY, "main", STACK_SIZE, TimedGetTaskMain, NULL);
    xfiber_kernel_start_scheduler();
}


TEST(xfiber_pool, destroy)
{
    xfiber_kernel_init(NULL, KERNEL_WORK_SIZE, NULL);
    xfiber_create(NULL, PRIORITY, "main", STACK_SIZE, DestroyTaskMain, NULL);
    xfiber_kernel_start_scheduler();
}


TEST_GROUP_RUNNER(xfiber_pool)
{
    RUN_TEST_CASE(xfiber_pool, get);
    RUN_TEST_CASE(xfiber_pool, timed_get);
    RUN_TEST_CASE(xfiber_pool, destroy);
}
