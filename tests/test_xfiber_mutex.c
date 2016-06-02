#include <picox/multitask/xfiber.h>
#include "testutils.h"


#define KERNEL_WORK_SIZE    (1024 * 20)
#define STACK_SIZE          (2048)
#define PRIORITY            (4)


TEST_GROUP(xfiber_mutex);


TEST_SETUP(xfiber_mutex)
{
}


TEST_TEAR_DOWN(xfiber_mutex)
{
}


typedef struct
{
    XTicks          delay;
    XFiberMutex*    mutex;
    XTicks          hold_time;
    XError          expect_err;
} LockArg;


typedef struct
{
    XTicks          delay;
    XFiberMutex*    mutex;
} LockForeverArg;


typedef struct
{
    XTicks          delay;
    XFiberMutex*    mutex;
} DestroyArg;


static LockArg* CreateLockArg(XFiberMutex* mutex, XTicks delay, XTicks hold_time, XError expect_err)
{
    LockArg* ret = x_malloc(sizeof(LockArg));
    X_ASSERT(ret);

    ret->delay = delay;
    ret->hold_time = hold_time;
    ret->mutex = mutex;
    ret->expect_err = expect_err;
    return ret;
}


static LockForeverArg* CreateLockForeverArg(XFiberMutex* mutex, XTicks delay)
{
    LockForeverArg* ret = x_malloc(sizeof(LockForeverArg));
    X_ASSERT(ret);

    ret->delay = delay;
    ret->mutex = mutex;
    return ret;
}


static DestroyArg* CreateDestroyArg(XFiberMutex* mutex, XTicks delay)
{
    DestroyArg* ret = x_malloc(sizeof(DestroyArg));
    X_ASSERT(ret);

    ret->delay = delay;
    ret->mutex = mutex;
    return ret;
}


static void LockTask(void* a)
{
    LockArg* arg = a;

    if (arg->delay > 0)
        xfiber_delay(arg->delay);

    const XError err = xfiber_mutex_lock(arg->mutex);
    TEST_ASSERT_EQUAL(arg->expect_err, err);

    xfiber_delay(arg->hold_time);
    xfiber_mutex_unlock(arg->mutex);


    x_free(arg);
}


static void LockForeverTask(void* a)
{
    LockForeverArg* arg = a;

    if (arg->delay > 0)
        xfiber_delay(arg->delay);

    const XError err = xfiber_mutex_lock(arg->mutex);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    x_free(arg);

    for (;;)
    {
        xfiber_delay(x_msec_to_ticks(100));
    }
}


static void DestroyTask(void* a)
{
    DestroyArg* arg = a;

    if (arg->delay > 0)
        xfiber_delay(arg->delay);

    XFiberMutex* mutex = arg->mutex;
    x_free(arg);
    xfiber_mutex_destroy(mutex);
}


static void LockTaskMain(void* a)
{
    X_UNUSED(a);

    XError err;
    LockArg* arg;
    XFiberMutex* mutex;

    err = xfiber_mutex_create(&mutex);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    err = xfiber_mutex_unlock(mutex);
    TEST_ASSERT_EQUAL(X_ERR_PROTOCOL, err);

    arg = CreateLockArg(mutex, 0, x_msec_to_ticks(30), X_ERR_NONE);
    TEST_ASSERT_EQUAL(X_ERR_NONE, xfiber_create(NULL, PRIORITY, "task1", STACK_SIZE, LockTask, arg));
    xfiber_yield();

    XTicks start = x_ticks_now();

    err = xfiber_mutex_lock(mutex);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_TRUE(x_msec_to_ticks(30) >= x_ticks_now() - start);

    xfiber_mutex_unlock(arg->mutex);

    xfiber_mutex_destroy(mutex);
    xfiber_kernel_end_scheduler();
}


static void TimedLockTaskMain(void* a)
{
    X_UNUSED(a);

    XError err;
    LockArg* arg;
    XFiberMutex* mutex;

    err = xfiber_mutex_create(&mutex);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    arg = CreateLockArg(mutex, 0, x_msec_to_ticks(50), X_ERR_NONE);
    TEST_ASSERT_EQUAL(X_ERR_NONE, xfiber_create(NULL, PRIORITY, "task1", STACK_SIZE, LockTask, arg));
    xfiber_yield();

    XTicks start = x_ticks_now();

    err = xfiber_mutex_timed_lock(mutex, x_msec_to_ticks(20));
    TEST_ASSERT_EQUAL(X_ERR_TIMED_OUT, err);
    TEST_ASSERT_TRUE(x_msec_to_ticks(20) >= x_ticks_now() - start);

    err = xfiber_mutex_lock(mutex);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    xfiber_mutex_unlock(arg->mutex);

    xfiber_mutex_destroy(mutex);
    xfiber_kernel_end_scheduler();
}


static void TryLockTaskMain(void* a)
{
    X_UNUSED(a);

    XError err;
    LockArg* arg;
    XFiberMutex* mutex;

    err = xfiber_mutex_create(&mutex);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    arg = CreateLockArg(mutex, 0, x_msec_to_ticks(20), X_ERR_NONE);
    TEST_ASSERT_EQUAL(X_ERR_NONE, xfiber_create(NULL, PRIORITY, "task1", STACK_SIZE, LockTask, arg));
    xfiber_yield();

    err = xfiber_mutex_try_lock(mutex);
    TEST_ASSERT_EQUAL(X_ERR_TIMED_OUT, err);

    xfiber_delay(x_msec_to_ticks(40));
    err = xfiber_mutex_try_lock(mutex);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    xfiber_mutex_unlock(arg->mutex);
    xfiber_mutex_destroy(mutex);
    xfiber_kernel_end_scheduler();
}


static void DestroyTaskMain(void* a)
{
    X_UNUSED(a);

    XError err;
    XFiberMutex* mutex;

    err = xfiber_mutex_create(&mutex);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    TEST_ASSERT_EQUAL(X_ERR_NONE,
            xfiber_create(NULL, PRIORITY, "task1", STACK_SIZE, LockForeverTask,
                CreateLockForeverArg(mutex, 0)));
    TEST_ASSERT_EQUAL(X_ERR_NONE,
            xfiber_create(NULL, PRIORITY, "task2", STACK_SIZE, DestroyTask,
                CreateDestroyArg(mutex, x_msec_to_ticks(50))));

    xfiber_yield();
    err = xfiber_mutex_lock(mutex);
    TEST_ASSERT_EQUAL(X_ERR_CANCELED, err);
    xfiber_kernel_end_scheduler();
}


TEST(xfiber_mutex, lock)
{
    xfiber_kernel_init(NULL, KERNEL_WORK_SIZE, NULL);
    xfiber_create(NULL, PRIORITY, "main", STACK_SIZE, LockTaskMain, NULL);
    xfiber_kernel_start_scheduler();
}


TEST(xfiber_mutex, timed_lock)
{
    xfiber_kernel_init(NULL, KERNEL_WORK_SIZE, NULL);
    xfiber_create(NULL, PRIORITY, "main", STACK_SIZE, TimedLockTaskMain, NULL);
    xfiber_kernel_start_scheduler();
}


TEST(xfiber_mutex, try_lock)
{
    xfiber_kernel_init(NULL, KERNEL_WORK_SIZE, NULL);
    xfiber_create(NULL, PRIORITY, "main", STACK_SIZE, TryLockTaskMain, NULL);
    xfiber_kernel_start_scheduler();
}


TEST(xfiber_mutex, destroy)
{
    xfiber_kernel_init(NULL, KERNEL_WORK_SIZE, NULL);
    xfiber_create(NULL, PRIORITY, "main", STACK_SIZE, DestroyTaskMain, NULL);
    xfiber_kernel_start_scheduler();
}


TEST_GROUP_RUNNER(xfiber_mutex)
{
    RUN_TEST_CASE(xfiber_mutex, lock);
    RUN_TEST_CASE(xfiber_mutex, timed_lock);
    RUN_TEST_CASE(xfiber_mutex, try_lock);
    RUN_TEST_CASE(xfiber_mutex, destroy);
}
