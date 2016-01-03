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
    RUN_TEST_GROUP(xstr);
    RUN_TEST_GROUP(xtokenizer);
    RUN_TEST_GROUP(xargparser);
    RUN_TEST_GROUP(xfile);
    RUN_TEST_GROUP(xprintf);
}


int main(int argc, const char* argv[])
{
    x_init_test();
    return UnityMain(argc, argv, run_all_tests);
}
