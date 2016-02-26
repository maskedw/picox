#include "testutils.h"


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
    // RUN_TEST_GROUP(xposixfs);
    // RUN_TEST_GROUP(xvfs);
    RUN_TEST_GROUP(xfs);
}


int main(int argc, const char* argv[])
{
    x_init_test();
    return UnityMain(argc, argv, run_all_tests);
}
