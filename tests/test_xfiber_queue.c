#include <picox/multitask/xfiber.h>
#include "testutils.h"


#define KERNEL_WORK_SIZE    (1024 * 20)
#define STACK_SIZE          (2048)
#define PRIORITY            (4)


TEST_GROUP(xfiber_queue);


TEST_SETUP(xfiber_queue)
{
}


TEST_TEAR_DOWN(xfiber_queue)
{
}


typedef struct
{
    int     index;
    char    str[16];
} Message;


typedef struct
{
    XTicks            delay;
    XFiberQueue*      queue;
    XError            expect_err;
    Message           message;
} SendArg;


static SendArg* CreateSendArg(XFiberQueue* queue, XTicks delay, XError expect_err, int index, const char* str)
{
    SendArg* ret = x_malloc(sizeof(SendArg));
    X_ASSERT(ret);

    ret->delay = delay;
    ret->queue = queue;
    ret->expect_err = expect_err;
    ret->message.index = index;
    strcpy(ret->message.str, str);
    return ret;
}


typedef struct
{
    XTicks        delay;
    XFiberQueue*  queue;
} DestroyArg;


static DestroyArg* CreateDestroyArg(XFiberQueue* queue, XTicks delay)
{
    DestroyArg* ret = x_malloc(sizeof(DestroyArg));
    X_ASSERT(ret);

    ret->delay = delay;
    ret->queue = queue;
    return ret;
}


static void SendTask(void* a)
{
    SendArg* arg = a;

    if (arg->delay > 0)
        xfiber_delay(arg->delay);

    const XError err = xfiber_queue_send_back(arg->queue, &arg->message);
    TEST_ASSERT_EQUAL(arg->expect_err, err);

    x_free(arg);
}


static void DestroyTask(void* a)
{
    DestroyArg* arg = a;

    if (arg->delay > 0)
        xfiber_delay(arg->delay);

    XFiberQueue* queue = arg->queue;
    x_free(arg);
    xfiber_queue_destroy(queue);
}


static void ReceiveTaskMain(void* a)
{
    X_UNUSED(a);

    XError err;
    SendArg* arg;
    XFiberQueue* queue;

    err = xfiber_queue_create(&queue, 10, sizeof(Message));
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    const char* strings[] = {
        "Hello",
        "World",
        "!!"
    };

    int i;
    for (i = 0; i < (int)X_COUNT_OF(strings); ++i)
    {
        arg = CreateSendArg(queue, 0, X_ERR_NONE, i, strings[i]);
        TEST_ASSERT_EQUAL(X_ERR_NONE, xfiber_create(NULL, PRIORITY, strings[i], STACK_SIZE, SendTask, arg));
    }

    i = 0;
    for (;;)
    {
        Message msg;

        TEST_ASSERT_EQUAL(X_ERR_NONE, xfiber_queue_receive(queue, &msg));
        TEST_ASSERT_EQUAL_STRING(strings[msg.index], msg.str);
        if (++i >= (int)X_COUNT_OF(strings))
            break;
    }

    xfiber_queue_destroy(queue);
    xfiber_kernel_end_scheduler();
}


static void TimedReceiveTaskMain(void* a)
{
    X_UNUSED(a);

    XError err;
    SendArg* arg;
    XFiberQueue* queue;
    Message msg;

    err = xfiber_queue_create(&queue, 10, sizeof(Message));
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    TEST_ASSERT_EQUAL(X_ERR_TIMED_OUT, xfiber_queue_timed_receive(queue, &msg, x_msec_to_ticks(50)));

    arg = CreateSendArg(queue, 0, X_ERR_NONE, 0, "Hello");
    TEST_ASSERT_EQUAL(X_ERR_NONE, xfiber_create(NULL, PRIORITY, "task1", STACK_SIZE, SendTask, arg));

    TEST_ASSERT_EQUAL(X_ERR_NONE, xfiber_queue_receive(queue, &msg));

    TEST_ASSERT_EQUAL(X_ERR_TIMED_OUT, xfiber_queue_timed_receive(queue, &msg, x_msec_to_ticks(50)));

    xfiber_queue_destroy(queue);
    xfiber_kernel_end_scheduler();
}


static void DestroyTaskMain(void* a)
{
    X_UNUSED(a);

    XError err;
    DestroyArg* arg;
    XFiberQueue* queue;
    Message msg;

    err = xfiber_queue_create(&queue, 10, sizeof(Message));
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    arg = CreateDestroyArg(queue, x_msec_to_ticks(30));
    xfiber_create(NULL, PRIORITY, "task1", STACK_SIZE, DestroyTask, arg);

    TEST_ASSERT_EQUAL(X_ERR_CANCELED, xfiber_queue_receive(queue, &msg));

    xfiber_kernel_end_scheduler();
}


TEST(xfiber_queue, receive)
{
    xfiber_kernel_init(NULL, KERNEL_WORK_SIZE, NULL);
    xfiber_create(NULL, PRIORITY, "main", STACK_SIZE, ReceiveTaskMain, NULL);
    xfiber_kernel_start_scheduler();
}


TEST(xfiber_queue, timed_receive)
{
    xfiber_kernel_init(NULL, KERNEL_WORK_SIZE, NULL);
    xfiber_create(NULL, PRIORITY, "main", STACK_SIZE, TimedReceiveTaskMain, NULL);
    xfiber_kernel_start_scheduler();
}


TEST(xfiber_queue, destroy)
{
    xfiber_kernel_init(NULL, KERNEL_WORK_SIZE, NULL);
    xfiber_create(NULL, PRIORITY, "main", STACK_SIZE, DestroyTaskMain, NULL);
    xfiber_kernel_start_scheduler();
}


TEST_GROUP_RUNNER(xfiber_queue)
{
    RUN_TEST_CASE(xfiber_queue, receive);
    RUN_TEST_CASE(xfiber_queue, timed_receive);
    RUN_TEST_CASE(xfiber_queue, destroy);
}
