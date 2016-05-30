#include <picox/multitask/xfiber.h>
#include "testutils.h"


#define PRINTF(...) do { printf(__VA_ARGS__); fflush(stdout); } while (0)
#define STACK_SIZE (2048 * 2)
#define FIBER_PRIORITY  (8)

char buffer[1024 * 1024];
XFiber* fiber1;
XFiber* fiber2;
XFiber* fiber3;
XFiber* fiber4;
XFiber* fiber5;
XFiberEvent* event1;
XFiberQueue* queue1;
bool req_exit;


TEST_GROUP(xfiber);

TEST_SETUP(xfiber)
{
    req_exit = false;
}


TEST_TEAR_DOWN(xfiber)
{
}


static int IdleHook()
{
    if (req_exit)
    {
        req_exit = false;
        return -1;
    }
    return 0;
}


void function1(void* arg)
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


static void wait_event(void* arg)
{
    XBits wait_pattern = (XBits)(uintptr_t)arg;
    XBits result_pattern;

    PRINTF("wait event...\n");
    xfiber_event_wait(event1, X_FIBER_EVENT_WAIT_AND | X_FIBER_EVENT_CLEAR_ON_EXIT,
                      wait_pattern, &result_pattern);

    TEST_ASSERT_EQUAL(wait_pattern, result_pattern);
    PRINTF("complete\n");
    xfiber_event_destroy(event1);
    req_exit = true;
}


static void wait_event_expected_ok(void* arg)
{
    XBits wait_pattern = (XBits)(uintptr_t)arg;
    XBits result_pattern;

    PRINTF("wait event...\n");
    XError err = xfiber_event_timed_wait(
            event1,
            X_FIBER_EVENT_WAIT_AND | X_FIBER_EVENT_CLEAR_ON_EXIT,
            wait_pattern,
            &result_pattern,
            x_msec_to_ticks(500));
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);
    TEST_ASSERT_EQUAL(wait_pattern, result_pattern);
    PRINTF("complete\n");
    xfiber_event_destroy(event1);
    req_exit = true;
}

static void wait_event_expected_timeout(void* arg)
{
    XBits wait_pattern = (XBits)(uintptr_t)arg;
    XBits result_pattern;

    PRINTF("wait event...\n");
    XError err = xfiber_event_timed_wait(
            event1,
            X_FIBER_EVENT_WAIT_AND | X_FIBER_EVENT_CLEAR_ON_EXIT,
            wait_pattern,
            &result_pattern,
            x_msec_to_ticks(500));
    TEST_ASSERT_EQUAL(X_ERR_TIMED_OUT, err);
    PRINTF("complete\n");
    xfiber_event_destroy(event1);
    req_exit = true;
}


static void event_setter(void* arg)
{
    XBits set_pattern = (XBits)(uintptr_t)arg;

    XFiber* const self_fiber = xfiber_self();
    const char* const name = xfiber_name(self_fiber);

    int i;
    for (i = 0; i < 10; i++)
    {
        // PRINTF("run '%s' %d\n", name, i);
        PRINTF("run '%s' %d\n", name, i);
        xfiber_yield();
    }

    xfiber_event_set(event1, set_pattern);
}


TEST(xfiber, task_create)
{
    xfiber_kernel_init(buffer, sizeof(buffer), IdleHook);

    xfiber_create(&fiber1, 0, "task1", 4096, function1, (void*)20);
    xfiber_create(&fiber2, 0, "task2", 4096, function1, (void*)30);
    req_exit = true;

    xfiber_kernel_start_scheduler();
}



TEST(xfiber, event)
{
    xfiber_kernel_init(buffer, sizeof(buffer), IdleHook);

    xfiber_create(&fiber1, 0, "task1", 4096, event_setter, (void*)0x01);
    xfiber_create(&fiber2, 0, "task2", 4096, event_setter, (void*)0x02);
    xfiber_create(&fiber3, 0, "task3", 4096, event_setter, (void*)0x04);
    xfiber_create(&fiber4, 0, "task4", 4096, wait_event, (void*)(0x07));

    xfiber_event_create(&event1, "event1");
    xfiber_kernel_start_scheduler();
}


TEST(xfiber, event_timed_wait)
{
    xfiber_kernel_init(buffer, sizeof(buffer), IdleHook);

    xfiber_create(&fiber1, 0, "task1", 4096, event_setter, (void*)0x01);
    xfiber_create(&fiber2, 0, "task4", 4096, wait_event_expected_timeout, (void*)(0x07));

    xfiber_event_create(&event1, "event1");
    xfiber_kernel_start_scheduler();

    xfiber_kernel_init(buffer, sizeof(buffer), IdleHook);

    xfiber_create(&fiber1, 0, "task1", 4096, event_setter, (void*)0x01);
    xfiber_create(&fiber2, 0, "task2", 4096, event_setter, (void*)0x02);
    xfiber_create(&fiber3, 0, "task3", 4096, event_setter, (void*)0x04);
    xfiber_create(&fiber4, 0, "task4", 4096, wait_event_expected_ok, (void*)(0x07));

    xfiber_event_create(&event1, "event1");
    xfiber_kernel_start_scheduler();
}


static void delay_task(void* arg)
{
    int i = 0;

    XBits wait_pattern = (XBits)(uintptr_t)arg;
    XBits result_pattern;
    XError err;

    for (;;)
    {
        err = xfiber_event_try_wait(event1,
                                    X_FIBER_EVENT_WAIT_AND | X_FIBER_EVENT_CLEAR_ON_EXIT,
                                    wait_pattern,
                                    &result_pattern);
        if (err != X_ERR_TIMED_OUT)
            break;
        X_LOG_NOTI((xfiber_name(NULL), "delay %d\n", i++));
        xfiber_delay(x_msec_to_ticks(10));
    }
    req_exit = true;
}


TEST(xfiber, delay)
{
    xfiber_kernel_init(buffer, sizeof(buffer), IdleHook);

    xfiber_create(&fiber1, 0, "task1", 4096, event_setter, (void*)0x01);
    xfiber_create(&fiber2, 0, "task2", 4096, event_setter, (void*)0x02);
    xfiber_create(&fiber3, 0, "task3", 4096, event_setter, (void*)0x04);
    xfiber_create(&fiber4, 0, "task4", 4096, delay_task, (void*)(0x07));

    xfiber_event_create(&event1, "event1");
    xfiber_kernel_start_scheduler();
}


typedef struct
{
    XBits   sigs;
    XFiber* target;
    bool    expected_ok;
} SignalArgs;


static SignalArgs* create_signal_args(XBits sigs, XFiber* target, bool expected_ok)
{
    SignalArgs* ret = x_malloc(sizeof(*ret));
    X_ASSERT(ret);

    ret->sigs = sigs;
    ret->target = target;
    ret->expected_ok = expected_ok;
    return ret;
}


static void signal_wait(void* arg)
{
    SignalArgs* sigarg = arg;
    XBits result;
    XError err;

    err = xfiber_signal_timed_wait(0x01, &result, x_msec_to_ticks(100));
    if (sigarg->expected_ok)
    {
        TEST_ASSERT_EQUAL(X_ERR_NONE, err);
        TEST_ASSERT_EQUAL(result, 0x01);
    }
    else
        TEST_ASSERT_EQUAL(X_ERR_TIMED_OUT, err);

    err = xfiber_signal_timed_wait(0x02, &result, x_msec_to_ticks(100));
    if (sigarg->expected_ok)
    {
        TEST_ASSERT_EQUAL(X_ERR_NONE, err);
        TEST_ASSERT_EQUAL(result, 0x02);
    }
    else
        TEST_ASSERT_EQUAL(X_ERR_TIMED_OUT, err);

    err = xfiber_signal_timed_wait(0x04, &result, x_msec_to_ticks(100));
    if (sigarg->expected_ok)
    {
        TEST_ASSERT_EQUAL(X_ERR_NONE, err);
        TEST_ASSERT_EQUAL(result, 0x04);
    }
    else
        TEST_ASSERT_EQUAL(X_ERR_TIMED_OUT, err);

    x_free(sigarg);
    req_exit = true;
}


static void signal_raise(void* arg)
{
    SignalArgs* sigarg = arg;

    if (sigarg->sigs == 0x04)
    {
        volatile int i = 0;
        i++;
    }
    xfiber_signal_raise(sigarg->target, sigarg->sigs);

    x_free(sigarg);
}


TEST(xfiber, signal)
{
    xfiber_kernel_init(buffer, sizeof(buffer), IdleHook);

    xfiber_create(&fiber1, 0, "task1", 4096, signal_wait,
            create_signal_args(0, NULL, true));
    xfiber_create(&fiber2, 0, "task2", 4096, signal_raise,
            create_signal_args(0x01, fiber1, true));
    xfiber_create(&fiber3, 0, "task3", 4096, signal_raise,
            create_signal_args(0x02, fiber1, true));
    xfiber_create(&fiber4, 0, "task4", 4096, signal_raise,
            create_signal_args(0x04, fiber1, true));

    xfiber_kernel_start_scheduler();

    xfiber_kernel_init(buffer, sizeof(buffer), IdleHook);

    xfiber_create(&fiber1, 0, "task1", 4096, signal_wait,
            create_signal_args(0, NULL, false));
    xfiber_create(&fiber2, 0, "task2", 4096, signal_raise,
            create_signal_args(0x01 << 8, fiber1, true));
    xfiber_create(&fiber3, 0, "task3", 4096, signal_raise,
            create_signal_args(0x02 << 8, fiber1, true));
    xfiber_create(&fiber4, 0, "task4", 4096, signal_raise,
            create_signal_args(0x04 << 8, fiber1, true));

    xfiber_kernel_start_scheduler();
}


static void suspend_task(void* arg)
{
    X_LOG_NOTI(("Task", "resumue"));
    xfiber_suspend(NULL);
    req_exit = true;
    return NULL;
}


static void resume_task(void* arg)
{
    xfiber_resume(arg);
    return NULL;
}


TEST(xfiber, suspend)
{
    xfiber_kernel_init(buffer, sizeof(buffer), IdleHook);

    xfiber_create(&fiber1, 0, "task1", 4096, suspend_task, NULL);
    xfiber_create(&fiber2, 0, "task2", 4096, resume_task, fiber1);

    xfiber_kernel_start_scheduler();
}


static void queue_recv_task(void* arg)
{
    XFiberQueue* queue = arg;
    char dst[256] = {0};

    for (int i = 0; i < 5; i++)
    {
        xfiber_queue_timed_receive(queue, dst, X_TICKS_FOREVER);
        X_LOG_NOTI(("QUEUE", dst));
    }

    req_exit = true;
}


static void queue_send_task(void* arg)
{
    XFiberQueue* queue = arg;

    for (int i = 0; i < 5; i++)
    {
        char buf[256];
        snprintf(buf, sizeof(buf), "Hello %d", i);
        xfiber_queue_timed_send_back(queue, buf, X_TICKS_FOREVER);
    }
}



static void queue_recv_task2(void* arg)
{
    XFiberQueue* queue = arg;
    char dst[256] = {0};

    for (int i = 0; i < 5; i++)
    {

        xfiber_yield();
        xfiber_yield();
        xfiber_queue_timed_receive(queue, dst, X_TICKS_FOREVER);
        X_LOG_NOTI(("QUEUE", dst));
    }

    xfiber_event_set(event1, 0x01);
    req_exit = true;
}



static void queue_send_task2(void* arg)
{
    XFiberQueue* queue = arg;

    for (int i = 0; i < 5; i++)
    {
        char buf[256];
        snprintf(buf, sizeof(buf), "Hello %d", i);
        xfiber_queue_timed_send_back(queue, buf, X_TICKS_FOREVER);
    }

    XBits result_pattern;
    xfiber_event_wait(event1, X_FIBER_EVENT_WAIT_AND, 0x01, &result_pattern);
    xfiber_event_destroy(event1);
}



TEST(xfiber, queue_pending_recv)
{
    xfiber_kernel_init(buffer, sizeof(buffer), IdleHook);

    xfiber_queue_create(&queue1, 10, 12);
    xfiber_create(&fiber2, 0, "task2", 4096, queue_recv_task, queue1);
    xfiber_create(&fiber1, 0, "task1", 4096, queue_send_task, queue1);

    xfiber_kernel_start_scheduler();
}


TEST(xfiber, queue_pending_send)
{
    xfiber_kernel_init(buffer, sizeof(buffer), IdleHook);

    xfiber_queue_create(&queue1, 1, 12);
    xfiber_event_create(&event1, "event1");
    xfiber_create(&fiber2, 0, "task2", 4096, queue_recv_task2, queue1);
    xfiber_create(&fiber1, 0, "task1", 4096, queue_send_task2, queue1);

    xfiber_kernel_start_scheduler();
}


TEST_GROUP_RUNNER(xfiber)
{
    RUN_TEST_CASE(xfiber, task_create);
    RUN_TEST_CASE(xfiber, event);
    RUN_TEST_CASE(xfiber, delay);
    RUN_TEST_CASE(xfiber, event_timed_wait);
    RUN_TEST_CASE(xfiber, signal);
    RUN_TEST_CASE(xfiber, suspend);
    // RUN_TEST_CASE(xfiber, queue_pending_recv);
    RUN_TEST_CASE(xfiber, queue_pending_send);
}
