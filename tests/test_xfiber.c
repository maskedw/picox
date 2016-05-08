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


#if 0
void* function1(void* arg)
{
    const XFiber* fiber_self = xfiber_self();

    int i;
    for (i = 0; i < 1; i++)
    {
        PRINTF("run '%s' %d\n", xfiber_name(fiber_self), i);
        xfiber_yield();
    }
}
#endif

void* function1(void* arg)
{
    const XFiber* fiber_self = xfiber_self();
    const int n = (intptr_t)arg;
    int i;
    for (i = 0; i < n; i++)
    {
        PRINTF("run '%s' %d\n", xfiber_name(fiber_self), i);
        xfiber_yield();
    }
}


TEST(xfiber, task_create)
{
    xfiber_kernel_init(1024 * 200, STACK_SIZE * 4);
    XFiber fiber[3];

    int i;
    for (i = 0; i < 3; i++)
    {
        char name[32];
        snprintf(name, sizeof(name), "task%d", i);
        // xfiber_init(&fiber[i], name, STACK_SIZE, function1, (void*)(100 * (i+1)), 1);
        xfiber_init(&fiber[i], name, STACK_SIZE, function1, (void*)10, 1);
    }

    for (i = 0; i < 11; i++)
    {
        PRINTF("yield %d\n", i);
        xfiber_yield();
    }

    for (i = 0; i < 3; i++)
    {
        char name[32];
        snprintf(name, sizeof(name), "task%d", 10 + i);
        xfiber_init(&fiber[i], name, STACK_SIZE, function1, (void*)20, 1);
    }

    for (i = 0; i < 21; i++)
    {
        PRINTF("yield %d\n", i);
        xfiber_yield();
    }
}


TEST_GROUP_RUNNER(xfiber)
{
    RUN_TEST_CASE(xfiber, task_create);
}
