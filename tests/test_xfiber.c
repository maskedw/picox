#include <picox/multitask/xfiber.h>
#include "testutils.h"


#define KERNEL_WORK_SIZE    (1024 * 20)
#define STACK_SIZE          (2048 * 2)
#define PRIORITY            (4)


TEST_GROUP(xfiber);


TEST_SETUP(xfiber)
{
}


TEST_TEAR_DOWN(xfiber)
{
}


typedef struct AccmulateArgs
{
    size_t n;
    size_t expected;
    XBits  bit;
    const char* name;
    XBits* bit_dst;
} AccmulateArgs;


static size_t Accmulate(size_t n)
{
    size_t ret = 0;
    size_t i;
    for (i = 0; i < n; i++)
        ret += i;

    return ret;
}


static AccmulateArgs* CreateAccmulateArgs(size_t n, XBits bit, const char* name, XBits* bit_dst)
{
    AccmulateArgs* ret = x_malloc(sizeof(AccmulateArgs));
    X_ASSERT(ret);

    ret->n = n;
    ret->expected = Accmulate(n);
    ret->bit = bit;
    ret->name = name;
    ret->bit_dst = bit_dst;

    return ret;
}


static void AccmulateTask(void* arg_)
{
    AccmulateArgs* arg = arg_;
    TEST_ASSERT_EQUAL_STRING(arg->name, xfiber_name(xfiber_self()));

    size_t ret = 0;
    size_t i;
    for (i = 0; i < arg->n; i++)
    {
        ret += i;
        xfiber_yield();
    }

    *(arg->bit_dst) |= arg->bit;

    TEST_ASSERT_EQUAL(arg->expected, ret);
    x_free(arg);
}


static void AccmulateMainTask(void* a)
{
    X_UNUSED(a);
    XBits* bits = x_malloc(sizeof(XBits));
    X_ASSERT(bits);
    *bits = 0;

    AccmulateArgs* arg = CreateAccmulateArgs(10, 0x01, "task1", bits);
    xfiber_create(NULL, PRIORITY, arg->name, STACK_SIZE, AccmulateTask, arg);

    arg = CreateAccmulateArgs(20, 0x04, "task2", bits);
    xfiber_create(NULL, PRIORITY, arg->name, STACK_SIZE, AccmulateTask, arg);

    arg = CreateAccmulateArgs(30, 0x08, "task3", bits);
    xfiber_create(NULL, PRIORITY, arg->name, STACK_SIZE, AccmulateTask, arg);

    for (;;)
    {
        if (*bits == 0x0D)
            break;
        xfiber_yield();
    }
    xfiber_kernel_end_scheduler();
}


typedef struct DelayArgs
{
    XBits  bit;
    XBits* bit_dst;
} DelayArgs;


static DelayArgs* CreateDelayArgs(XBits bit, XBits* bit_dst)
{
    DelayArgs* ret = x_malloc(sizeof(DelayArgs));
    X_ASSERT(ret);

    ret->bit = bit;
    ret->bit_dst = bit_dst;

    return ret;
}


static void DelayTask(void* arg_)
{
    DelayArgs* arg = arg_;

    int i;
    for (i = 0; i < 10; ++i)
    {
        const XTicks start = x_ticks_now();
        const XTicks delay_time = x_msec_to_ticks(rand() % 5 + 1);
        xfiber_delay(delay_time);
        const XTicks now = x_ticks_now();

        TEST_ASSERT_TRUE(now - start >= delay_time);
    }

    *(arg->bit_dst) |= arg->bit;
    x_free(arg);
}


static void DelayMainTask(void* a)
{
    X_UNUSED(a);

    XBits* bits = x_malloc(sizeof(XBits));
    X_ASSERT(bits);
    *bits = 0;

    DelayArgs* arg = CreateDelayArgs(0x01, bits);
    xfiber_create(NULL, PRIORITY, "task1", STACK_SIZE, DelayTask, arg);

    arg = CreateDelayArgs(0x04, bits);
    xfiber_create(NULL, PRIORITY, "task2", STACK_SIZE, DelayTask, arg);

    arg = CreateDelayArgs(0x08, bits);
    xfiber_create(NULL, PRIORITY, "task3", STACK_SIZE, DelayTask, arg);


    for (;;)
    {
        if (*bits == 0x0D)
            break;
        xfiber_yield();
    }
    x_free(bits);
    xfiber_kernel_end_scheduler();
}


TEST(xfiber, create)
{
    xfiber_kernel_init(NULL, KERNEL_WORK_SIZE, NULL);
    xfiber_create(NULL, PRIORITY, "main", STACK_SIZE, AccmulateMainTask, NULL);
    xfiber_kernel_start_scheduler();
}


TEST(xfiber, delay)
{
    xfiber_kernel_init(NULL, KERNEL_WORK_SIZE, NULL);
    xfiber_create(NULL, PRIORITY, "main", STACK_SIZE, DelayMainTask, NULL);
    xfiber_kernel_start_scheduler();
}


TEST_GROUP_RUNNER(xfiber)
{
    RUN_TEST_CASE(xfiber, create);
    RUN_TEST_CASE(xfiber, delay)
}
