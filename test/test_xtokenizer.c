#include <picox/misc/xtokenizer.h>
#include "testutils.h"


TEST_GROUP(xtokenizer);


TEST_SETUP(xtokenizer)
{
}


TEST_TEAR_DOWN(xtokenizer)
{
}


TEST(xtokenizer, init)
{
    XTokenizer tok;

    X_TEST_ASSERTION_FAILED(xtok_init(NULL, "1,2", ',', 2));
    X_TEST_ASSERTION_FAILED(xtok_init(&tok, NULL, ',', 2));
    X_TEST_ASSERTION_FAILED(xtok_init(&tok, "1,2", ',', 0));

    TEST_ASSERT_TRUE(xtok_init(&tok, "1, 1", ',', 2));
    xtok_release(&tok);
    TEST_ASSERT_FALSE(xtok_init(&tok, "1, 1", ',', 1));
    TEST_ASSERT_FALSE(xtok_init(&tok, "1, 1", ',', 1));
}


TEST(xtokenizer, release)
{
    X_TEST_ASSERTION_FAILED(xtok_release(NULL));
}


TEST(xtokenizer, ref_token)
{
    XTokenizer tok;
    xtok_init(&tok, "10, Hello, World", ',', 3);

    X_TEST_ASSERTION_FAILED(xtok_ref_token(NULL, 0));
    X_TEST_ASSERTION_FAILED(xtok_ref_token(&tok, -1));
    X_TEST_ASSERTION_FAILED(xtok_ref_token(&tok, xtok_num_tokens(&tok)));

    TEST_ASSERT_EQUAL_STRING("10", xtok_ref_token(&tok, 0));
    TEST_ASSERT_EQUAL_STRING("Hello", xtok_ref_token(&tok, 1));
    TEST_ASSERT_EQUAL_STRING("World", xtok_ref_token(&tok, 2));
    xtok_release(&tok);
}


TEST(xtokenizer, num_tokens)
{
    XTokenizer tok;
    xtok_init(&tok, "10, Hello, World,", ',', 4);

    X_TEST_ASSERTION_FAILED(xtok_num_tokens(NULL));
    TEST_ASSERT_EQUAL(4, xtok_num_tokens(&tok));
    xtok_release(&tok);
}


TEST_GROUP_RUNNER(xtokenizer)
{
    RUN_TEST_CASE(xtokenizer, init);
    RUN_TEST_CASE(xtokenizer, release);
    RUN_TEST_CASE(xtokenizer, ref_token);
    RUN_TEST_CASE(xtokenizer, num_tokens);
}
