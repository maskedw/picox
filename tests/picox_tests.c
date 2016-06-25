#include "testutils.h"


static XTicks tick_count;


static void SignalHandler(int signum)
{
    tick_count++;
}

static void run_all_tests(void)
{
    RUN_TEST_GROUP(xfifo);
    RUN_TEST_GROUP(xilist);
    RUN_TEST_GROUP(xmsgbuf);
    RUN_TEST_GROUP(sds);
    RUN_TEST_GROUP(xpalloc);
    RUN_TEST_GROUP(xutils);
    RUN_TEST_GROUP(xsalloc);
    RUN_TEST_GROUP(xfalloc);
    RUN_TEST_GROUP(xstring);
    RUN_TEST_GROUP(xtokenizer);
    RUN_TEST_GROUP(xargparser);
    RUN_TEST_GROUP(xprintf);
    RUN_TEST_GROUP(xdstr);
    RUN_TEST_GROUP(xstream);
    RUN_TEST_GROUP(xfpath);
    RUN_TEST_GROUP(xposixfs);
    RUN_TEST_GROUP(xramfs);
    RUN_TEST_GROUP(xromfs);
    RUN_TEST_GROUP(xvfs);
    RUN_TEST_GROUP(xfatfs);
    RUN_TEST_GROUP(xspiffs);
    RUN_TEST_GROUP(xsinglefs);
    RUN_TEST_GROUP(xunionfs);
    RUN_TEST_GROUP(xfs);
    RUN_TEST_GROUP(minIni);
    RUN_TEST_GROUP(xfiber);
    RUN_TEST_GROUP(xfiber_event);
    RUN_TEST_GROUP(xfiber_signal);
    RUN_TEST_GROUP(xfiber_mutex);
    RUN_TEST_GROUP(xfiber_semaphore);
    RUN_TEST_GROUP(xfiber_pool);
    RUN_TEST_GROUP(xfiber_mailbox);
    RUN_TEST_GROUP(xfiber_channel);
    RUN_TEST_GROUP(xfiber_queue);
}


static void X__PostAssertionFailed(void)
{
    fflush(stdout);
    while (1);
}


static int X__Putc(int c)
{
    putc(c, stderr);
}


int main(int argc, const char* argv[])
{
    x_putc_stdout = (XCharPutFunc)X__Putc;
    x_post_assertion_failed = (XAssertionFailedFunc)X__PostAssertionFailed;

    return UnityMain(argc, argv, run_all_tests);
}
