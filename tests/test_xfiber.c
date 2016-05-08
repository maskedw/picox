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

    return (void*)0;
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


static void* test_event(void* arg)
{
    int id;
    XFiberEvent* event = arg;
    XFiber* const self_fiber = xfiber_self();
    const char* const name = xfiber_name(self_fiber);

    sscanf(name, "task%d", &id);
    int i;
    for (i = 0; i < 10; i++)
    {
        PRINTF("run '%s' %d\n", name, i);
        xfiber_yield();
    }

    xfiber_event_set(event, X_BIT(id));
}


TEST(xfiber, event)
{
    xfiber_kernel_init(1024 * 200, STACK_SIZE * 4);
    XFiber fiber[3];
    XFiberEvent event;

    xfiber_event_init(&event, "event0");

    int i;
    XBits wait_pattern = 0;
    XBits result_pattern;
    for (i = 0; i < 3; i++)
    {
        char name[32];
        snprintf(name, sizeof(name), "task%d", i);
        xfiber_init(&fiber[i], name, STACK_SIZE, test_event, &event, 1);
        wait_pattern |= X_BIT(i);
    }

    PRINTF("wait event...\n");
    xfiber_event_wait(&event, X_FIBER_EVENT_WAIT_AND | X_FIBER_EVENT_CLEAR_ON_EXIT,
                      wait_pattern, &result_pattern);
    PRINTF("complete\n");
    TEST_ASSERT_EQUAL(wait_pattern, result_pattern);
    xfiber_event_deinit(&event);
}


TEST_GROUP_RUNNER(xfiber)
{
    RUN_TEST_CASE(xfiber, task_create);
    RUN_TEST_CASE(xfiber, event);
}
