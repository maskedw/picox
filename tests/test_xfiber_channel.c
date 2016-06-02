#include <picox/multitask/xfiber.h>
#include "testutils.h"


#define KERNEL_WORK_SIZE    (1024 * 20)
#define STACK_SIZE          (2048)
#define PRIORITY            (8)


TEST_GROUP(xfiber_channel);


TEST_SETUP(xfiber_channel)
{
}


TEST_TEAR_DOWN(xfiber_channel)
{
}


typedef struct
{
    int     index;
    char    str[16];
} Message;


typedef struct
{
    XTicks              delay;
    XFiberChannel*      channel;
    XError              expect_err;
    Message             message;
} SendArg;


static SendArg* CreateSendArg(XFiberChannel* channel, XTicks delay, XError expect_err, int index, const char* str)
{
    SendArg* ret = x_malloc(sizeof(SendArg));
    X_ASSERT(ret);

    ret->delay = delay;
    ret->channel = channel;
    ret->expect_err = expect_err;
    ret->message.index = index;
    strcpy(ret->message.str, str);
    return ret;
}


typedef struct
{
    XTicks          delay;
    XFiberChannel*  channel;
} DestroyArg;


static DestroyArg* CreateDestroyArg(XFiberChannel* channel, XTicks delay)
{
    DestroyArg* ret = x_malloc(sizeof(DestroyArg));
    X_ASSERT(ret);

    ret->delay = delay;
    ret->channel = channel;
    return ret;
}


static void SendTask(void* a)
{
    SendArg* arg = a;

    if (arg->delay > 0)
        xfiber_delay(arg->delay);

    const XError err = xfiber_channel_send(arg->channel, &arg->message,
            sizeof(arg->message.index) + strlen(arg->message.str) + 1);
    TEST_ASSERT_EQUAL(arg->expect_err, err);

    x_free(arg);
}


static void DestroyTask(void* a)
{
    DestroyArg* arg = a;

    if (arg->delay > 0)
        xfiber_delay(arg->delay);

    XFiberChannel* channel = arg->channel;
    x_free(arg);
    xfiber_channel_destroy(channel);
}


static void ReceiveTaskMain(void* a)
{
    X_UNUSED(a);

    XError err;
    SendArg* arg;
    XFiberChannel* channel;

    err = xfiber_channel_create(&channel, 1024, 32);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    const char* strings[] = {
        "Hello",
        "World",
        "!!"
    };

    int i;
    for (i = 0; i < (int)X_COUNT_OF(strings); ++i)
    {
        arg = CreateSendArg(channel, 0, X_ERR_NONE, i, strings[i]);
        TEST_ASSERT_EQUAL(X_ERR_NONE, xfiber_create(NULL, PRIORITY, strings[i], STACK_SIZE, SendTask, arg));
    }

    i = 0;
    for (;;)
    {
        Message msg;
        size_t size = 0;

        TEST_ASSERT_EQUAL(X_ERR_NONE, xfiber_channel_receive(channel, &msg, &size));
        TEST_ASSERT_TRUE((1 <= size) && (size <= 32));

        TEST_ASSERT_EQUAL_STRING(strings[msg.index], msg.str);
        if (++i >= (int)X_COUNT_OF(strings))
            break;
    }

    xfiber_channel_destroy(channel);
    xfiber_kernel_end_scheduler();
}


static void TimedReceiveTaskMain(void* a)
{
    X_UNUSED(a);

    XError err;
    SendArg* arg;
    XFiberChannel* channel;
    Message msg;
    size_t size;

    err = xfiber_channel_create(&channel, 1024, 32);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    TEST_ASSERT_EQUAL(X_ERR_TIMED_OUT, xfiber_channel_timed_receive(channel, &msg, &size, x_msec_to_ticks(50)));

    arg = CreateSendArg(channel, 0, X_ERR_NONE, 0, "Hello");
    TEST_ASSERT_EQUAL(X_ERR_NONE, xfiber_create(NULL, PRIORITY, "task1", STACK_SIZE, SendTask, arg));

    TEST_ASSERT_EQUAL(X_ERR_NONE, xfiber_channel_receive(channel, &msg, &size));

    TEST_ASSERT_EQUAL(X_ERR_TIMED_OUT, xfiber_channel_timed_receive(channel, &msg, &size, x_msec_to_ticks(50)));

    xfiber_channel_destroy(channel);
    xfiber_kernel_end_scheduler();
}


static void DestroyTaskMain(void* a)
{
    X_UNUSED(a);

    XError err;
    DestroyArg* arg;
    XFiberChannel* channel;
    Message msg;
    size_t size;

    err = xfiber_channel_create(&channel, 1024, 32);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    arg = CreateDestroyArg(channel, x_msec_to_ticks(30));
    xfiber_create(NULL, PRIORITY, "task1", STACK_SIZE, DestroyTask, arg);

    TEST_ASSERT_EQUAL(X_ERR_CANCELED, xfiber_channel_receive(channel, &msg, &size));

    xfiber_kernel_end_scheduler();
}


TEST(xfiber_channel, receive)
{
    xfiber_kernel_init(NULL, KERNEL_WORK_SIZE, NULL);
    xfiber_create(NULL, PRIORITY, "main", STACK_SIZE, ReceiveTaskMain, NULL);
    xfiber_kernel_start_scheduler();
}


TEST(xfiber_channel, timed_receive)
{
    xfiber_kernel_init(NULL, KERNEL_WORK_SIZE, NULL);
    xfiber_create(NULL, PRIORITY, "main", STACK_SIZE, TimedReceiveTaskMain, NULL);
    xfiber_kernel_start_scheduler();
}


TEST(xfiber_channel, destroy)
{
    xfiber_kernel_init(NULL, KERNEL_WORK_SIZE, NULL);
    xfiber_create(NULL, PRIORITY, "main", STACK_SIZE, DestroyTaskMain, NULL);
    xfiber_kernel_start_scheduler();
}


TEST_GROUP_RUNNER(xfiber_channel)
{
    RUN_TEST_CASE(xfiber_channel, receive);
    RUN_TEST_CASE(xfiber_channel, timed_receive);
    RUN_TEST_CASE(xfiber_channel, destroy);
}
