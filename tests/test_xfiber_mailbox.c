#include <picox/multitask/xfiber.h>
#include "testutils.h"


#define KERNEL_WORK_SIZE    (1024 * 20)
#define STACK_SIZE          (2048)
#define PRIORITY            (4)


TEST_GROUP(xfiber_mailbox);


TEST_SETUP(xfiber_mailbox)
{
}


TEST_TEAR_DOWN(xfiber_mailbox)
{
}


typedef struct
{
    XFiberMessage   head;
    int             index;
    char            str[16];
} Message;


typedef struct
{
    XTicks              delay;
    XFiberMailbox*      mailbox;
    XError              expect_err;
    Message*            message;
} SendArg;


static Message* CreateMessage(int index, const char* str)
{
    Message* ret = x_malloc(sizeof(Message));
    X_ASSERT(ret);

    strcpy(ret->str, str);
    ret->index = index;
    return ret;
}


static SendArg* CreateSendArg(XFiberMailbox* mailbox, XTicks delay, XError expect_err, Message* message)
{
    SendArg* ret = x_malloc(sizeof(SendArg));
    X_ASSERT(ret);

    ret->delay = delay;
    ret->message = message;
    ret->mailbox = mailbox;
    ret->expect_err = expect_err;
    return ret;
}


typedef struct
{
    XTicks          delay;
    XFiberMailbox*  mailbox;
} DestroyArg;


static DestroyArg* CreateDestroyArg(XFiberMailbox* mailbox, XTicks delay)
{
    DestroyArg* ret = x_malloc(sizeof(DestroyArg));
    X_ASSERT(ret);

    ret->delay = delay;
    ret->mailbox = mailbox;
    return ret;
}


static void SendTask(void* a)
{
    SendArg* arg = a;

    if (arg->delay > 0)
        xfiber_delay(arg->delay);

    const XError err = xfiber_mailbox_send(arg->mailbox, &(arg->message->head));
    TEST_ASSERT_EQUAL(arg->expect_err, err);

    x_free(arg);
}


static void DestroyTask(void* a)
{
    DestroyArg* arg = a;

    if (arg->delay > 0)
        xfiber_delay(arg->delay);

    XFiberMailbox* mailbox = arg->mailbox;
    x_free(arg);
    xfiber_mailbox_destroy(mailbox);
}


static void ReceiveTaskMain(void* a)
{
    X_UNUSED(a);

    XError err;
    SendArg* arg;
    XFiberMailbox* mailbox;

    err = xfiber_mailbox_create(&mailbox);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    const char* strings[] = {
        "Hello",
        "World",
        "!!"
    };

    int i;
    for (i = 0; i < (int)X_COUNT_OF(strings); ++i)
    {
        arg = CreateSendArg(mailbox, 0, X_ERR_NONE, CreateMessage(i, strings[i]));
        TEST_ASSERT_EQUAL(X_ERR_NONE, xfiber_create(NULL, PRIORITY, strings[i], STACK_SIZE, SendTask, arg));
    }


    i = 0;
    for (;;)
    {
        XFiberMessage* head;
        TEST_ASSERT_EQUAL(X_ERR_NONE, xfiber_mailbox_receive(mailbox, &head));
        TEST_ASSERT_NOT_NULL(head);
        Message* msg = X_CONTAINER_OF(head, Message, head);
        x_free(msg);
        TEST_ASSERT_EQUAL_STRING(strings[msg->index], msg->str);
        if (++i >= (int)X_COUNT_OF(strings))
            break;
    }

    xfiber_mailbox_destroy(mailbox);
    xfiber_kernel_end_scheduler();
}


static void TimedReceiveTaskMain(void* a)
{
    X_UNUSED(a);

    XError err;
    SendArg* arg;
    XFiberMailbox* mailbox;
    XFiberMessage* head;

    err = xfiber_mailbox_create(&mailbox);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    TEST_ASSERT_EQUAL(X_ERR_TIMED_OUT, xfiber_mailbox_timed_receive(mailbox, &head, x_msec_to_ticks(50)));

    arg = CreateSendArg(mailbox, 0, X_ERR_NONE, CreateMessage(0, "Hello"));
    TEST_ASSERT_EQUAL(X_ERR_NONE, xfiber_create(NULL, PRIORITY, "task1", STACK_SIZE, SendTask, arg));

    TEST_ASSERT_EQUAL(X_ERR_NONE, xfiber_mailbox_receive(mailbox, &head));
    x_free(X_CONTAINER_OF(head, Message, head));

    TEST_ASSERT_EQUAL(X_ERR_TIMED_OUT, xfiber_mailbox_timed_receive(mailbox, &head, x_msec_to_ticks(50)));

    xfiber_mailbox_destroy(mailbox);
    xfiber_kernel_end_scheduler();
}


static void DestroyTaskMain(void* a)
{
    X_UNUSED(a);

    XError err;
    DestroyArg* arg;
    XFiberMailbox* mailbox;
    XFiberMessage* head;

    err = xfiber_mailbox_create(&mailbox);
    TEST_ASSERT_EQUAL(X_ERR_NONE, err);

    arg = CreateDestroyArg(mailbox, x_msec_to_ticks(30));
    xfiber_create(NULL, PRIORITY, "task1", STACK_SIZE, DestroyTask, arg);

    TEST_ASSERT_EQUAL(X_ERR_CANCELED, xfiber_mailbox_receive(mailbox, &head));

    xfiber_kernel_end_scheduler();
}


TEST(xfiber_mailbox, receive)
{
    xfiber_kernel_init(NULL, KERNEL_WORK_SIZE, NULL);
    xfiber_create(NULL, PRIORITY, "main", STACK_SIZE, ReceiveTaskMain, NULL);
    xfiber_kernel_start_scheduler();
}


TEST(xfiber_mailbox, timed_receive)
{
    xfiber_kernel_init(NULL, KERNEL_WORK_SIZE, NULL);
    xfiber_create(NULL, PRIORITY, "main", STACK_SIZE, TimedReceiveTaskMain, NULL);
    xfiber_kernel_start_scheduler();
}


TEST(xfiber_mailbox, destroy)
{
    xfiber_kernel_init(NULL, KERNEL_WORK_SIZE, NULL);
    xfiber_create(NULL, PRIORITY, "main", STACK_SIZE, DestroyTaskMain, NULL);
    xfiber_kernel_start_scheduler();
}


TEST_GROUP_RUNNER(xfiber_mailbox)
{
    RUN_TEST_CASE(xfiber_mailbox, receive);
    RUN_TEST_CASE(xfiber_mailbox, timed_receive);
    RUN_TEST_CASE(xfiber_mailbox, destroy);
}
