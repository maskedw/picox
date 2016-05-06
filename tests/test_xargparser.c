#include <picox/misc/xargparser.h>
#include "testutils.h"


#define X__NUM_ARGV (10)


TEST_GROUP(xargparser);


TEST_SETUP(xargparser)
{
}


TEST_TEAR_DOWN(xargparser)
{
}


TEST(xargparser, to_argv)
{
    int argc;
    char* argv[X__NUM_ARGV];
    char arg[] = "git add -n foo/bar.c";

    X_TEST_ASSERTION_FAILED(xargparser_to_argv(NULL, &argc, argv, X_COUNT_OF(argv)));
    X_TEST_ASSERTION_FAILED(xargparser_to_argv(arg, NULL, argv, X_COUNT_OF(argv)));
    X_TEST_ASSERTION_FAILED(xargparser_to_argv(arg, &argc, NULL, X_COUNT_OF(argv)));
    X_TEST_ASSERTION_FAILED(xargparser_to_argv(arg, &argc, argv, 0));

    TEST_ASSERT_TRUE(xargparser_to_argv(arg, &argc, argv, X_COUNT_OF(argv)) == X_ARG_PARSER_ERR_NONE);

    TEST_ASSERT_EQUAL(4, argc);
    TEST_ASSERT_EQUAL_STRING("git", argv[0]);
    TEST_ASSERT_EQUAL_STRING("add", argv[1]);
    TEST_ASSERT_EQUAL_STRING("-n", argv[2]);
    TEST_ASSERT_EQUAL_STRING("foo/bar.c", argv[3]);
}


TEST(xargparser, quart)
{
    int argc;
    char* argv[X__NUM_ARGV];
    char arg[] = "'git' \"add\"";

    TEST_ASSERT_TRUE(xargparser_to_argv(arg, &argc, argv, X_COUNT_OF(argv)) == X_ARG_PARSER_ERR_NONE);

    TEST_ASSERT_EQUAL(2, argc);
    TEST_ASSERT_EQUAL_STRING("git", argv[0]);
    TEST_ASSERT_EQUAL_STRING("add", argv[1]);
}


TEST(xargparser, bad_quart)
{
    int argc;
    char* argv[X__NUM_ARGV];
    char arg[] = "'git' \"add";

    TEST_ASSERT_TRUE(xargparser_to_argv(arg, &argc, argv, X_COUNT_OF(argv)) == X_ARG_PARSER_ERR_QUATE);
}


TEST(xargparser, argc_overflow)
{
    int argc;
    char* argv[X__NUM_ARGV];
    char arg[] = "'git' add";

    TEST_ASSERT_TRUE(xargparser_to_argv(arg, &argc, argv, 1) == X_ARG_PARSER_ERR_OVERFLOW);
}


TEST(xargparser, escape)
{
    int argc;
    char* argv[X__NUM_ARGV];
    char arg[] = "\\'git\\' add";

    TEST_ASSERT_TRUE(xargparser_to_argv(arg, &argc, argv, X_COUNT_OF(argv)) == X_ARG_PARSER_ERR_NONE);
    TEST_ASSERT_EQUAL_STRING("'git'", argv[0]);
    TEST_ASSERT_EQUAL_STRING("add", argv[1]);
}


TEST(xargparser, bad_escape)
{
    int argc;
    char* argv[X__NUM_ARGV];
    char arg[] = "\\'git\\' add\\";

    TEST_ASSERT_TRUE(xargparser_to_argv(arg, &argc, argv, X_COUNT_OF(argv)) == X_ARG_PARSER_ERR_ESCAPE);
}


TEST_GROUP_RUNNER(xargparser)
{
    RUN_TEST_CASE(xargparser, to_argv);
    RUN_TEST_CASE(xargparser, quart);
    RUN_TEST_CASE(xargparser, bad_quart);
    RUN_TEST_CASE(xargparser, argc_overflow);
    RUN_TEST_CASE(xargparser, escape);
    RUN_TEST_CASE(xargparser, bad_escape);
}
