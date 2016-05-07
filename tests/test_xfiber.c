#include <picox/multitask/xfiber.h>
#include "testutils.h"


#define PRINTF(...) do { printf(__VA_ARGS__); fflush(stdout); } while (0)
#define STACK_SIZE (2048)
TEST_GROUP(xfiber);

TEST_SETUP(xfiber)
{
}


TEST_TEAR_DOWN(xfiber)
{
}


void* function1(void* arg)
{
    int i = 0;
    const XFiber* fiber_self = xfiber_self();
    for (;;)
    {
        PRINTF("run '%s' %d\n", xfiber_name(fiber_self), i);
        i++;
        xfiber_yield();
    }
}


TEST(xfiber, fiber)
{
    xfiber_kernel_init(1024 * 200, STACK_SIZE * 4);
    XFiber fiber[3];

    int i;
    for (i = 0; i < 3; i++)
    {
        char name[32];
        snprintf(name, sizeof(name), "task%d", i);
        xfiber_init(&fiber[i], name, STACK_SIZE, function1, NULL, 1);
    }


    i = 0;
    for (;;)
    {
        PRINTF("yield %d\n", i);
        i++;
        xfiber_yield();
    }
}


TEST_GROUP_RUNNER(xfiber)
{
    RUN_TEST_CASE(xfiber, fiber);
}
