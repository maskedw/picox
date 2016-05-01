#include <picox/filesystem/xfpath.h>
#include <unistd.h>
#include "testutils.h"


#define BUF_SET(str) (strcpy(buf, str))

TEST_GROUP(xfpath);


TEST_SETUP(xfpath)
{
}


TEST_TEAR_DOWN(xfpath)
{
}


TEST(xfpath, join)
{
    char buf[256];
    BUF_SET("ABC");
    TEST_ASSERT_EQUAL_STRING("ABC/DEF", xfpath_join(buf, "DEF", sizeof(buf)));

    BUF_SET("ABC/");
    TEST_ASSERT_EQUAL_STRING("ABC/DEF", xfpath_join(buf, "DEF", sizeof(buf)));

    BUF_SET("ABC/");
    TEST_ASSERT_EQUAL_STRING("/DEF", xfpath_join(buf, "/DEF", sizeof(buf)));
}


TEST(xfpath, resolove_dot)
{
    char buf[256];
    BUF_SET("/ABC/../DEF");
    TEST_ASSERT_EQUAL_STRING("/DEF", xfpath_resolve_dot(buf));

    BUF_SET("/ABC/.././././././DEF");
    TEST_ASSERT_EQUAL_STRING("/DEF", xfpath_resolve_dot(buf));

    BUF_SET("/ABC/.././././././DEF/");
    TEST_ASSERT_EQUAL_STRING("/DEF/", xfpath_resolve_dot(buf));

    BUF_SET("/ABC/DEF/./GHI/../JKL");
    TEST_ASSERT_EQUAL_STRING("/ABC/DEF/JKL", xfpath_resolve_dot(buf));

    BUF_SET("C:/ABC/DEF/./GHI/../.././JKL");
    TEST_ASSERT_EQUAL_STRING("C:/ABC/JKL", xfpath_resolve_dot(buf));
}


TEST(xfpath, resolove)
{
    char buf[256];
    char cwd[] = "/home";

    TEST_ASSERT_EQUAL(X_ERR_NONE, xfpath_resolve(buf, cwd, "ABC", sizeof(buf)));
    TEST_ASSERT_EQUAL_STRING("/home/ABC", buf);

    TEST_ASSERT_EQUAL(X_ERR_NONE, xfpath_resolve(buf, cwd, "/ABC", sizeof(buf)));
    TEST_ASSERT_EQUAL_STRING("/ABC", buf);

    TEST_ASSERT_EQUAL(X_ERR_NONE, xfpath_resolve(buf, cwd, "../bin", sizeof(buf)));
    TEST_ASSERT_EQUAL_STRING("/bin", buf);

    TEST_ASSERT_EQUAL(X_ERR_NONE, xfpath_resolve(buf, cwd, "ABC/../DEF/bin", sizeof(buf)));
    TEST_ASSERT_EQUAL_STRING("/home/DEF/bin", buf);

    TEST_ASSERT_EQUAL(X_ERR_NONE, xfpath_resolve(buf, cwd, "ABC/", sizeof(buf)));
    TEST_ASSERT_EQUAL_STRING("/home/ABC", buf);
}


TEST(xfpath, resolove_drivelater)
{
}


#if 0
TEST(xfpath, next)
{
    char buf[256];
    const char* next;
    const char* endptr;
    char name[32];

    BUF_SET("/foo/bar/baz");
    printf("do next\n");
    next = buf;
    while ((next = xfpath_top(next, (char**)&endptr)))
    {
        memcpy(name, next, endptr - next);
        name[endptr - next] = '\0';
        printf("name => '%s'\n", name);
        next = endptr;
    }

    BUF_SET("foo/bar/baz");
    printf("do next\n");
    next = buf;
    while ((next = xfpath_top(next, (char**)&endptr)))
    {
        memcpy(name, next, endptr - next);
        name[endptr - next] = '\0';
        printf("name => '%s'\n", name);
        next = endptr;
    }

    BUF_SET("foo/bar/baz///");
    printf("do next\n");
    next = buf;
    while ((next = xfpath_top(next, (char**)&endptr)))
    {
        memcpy(name, next, endptr - next);
        name[endptr - next] = '\0';
        printf("name => '%s'\n", name);
        next = endptr;
    }
}


TEST(xfpath, rnext)
{
    char buf[256];
    const char* next;
    const char* endptr;
    char name[32];

    BUF_SET("/foo/bar/baz");
    printf("do rnext\n");
    next = NULL;
    while ((next = xfpath_tail(buf, next, (char**)&endptr)))
    {
        memcpy(name, next, endptr - next);
        name[endptr - next] = '\0';
        printf("name => '%s'\n", name);
    }

    BUF_SET("foo/bar/baz");
    printf("do rnext\n");
    next = NULL;
    while ((next = xfpath_tail(buf, next, (char**)&endptr)))
    {
        memcpy(name, next, endptr - next);
        name[endptr - next] = '\0';
        printf("name => '%s'\n", name);
    }


    BUF_SET("/////foo/bar/baz////");
    printf("do rnext\n");
    next = NULL;
    while ((next = xfpath_tail(buf, next, (char**)&endptr)))
    {
        memcpy(name, next, endptr - next);
        name[endptr - next] = '\0';
        printf("name => '%s'\n", name);
    }

}
#endif

TEST(xfpath, next)
{
    char buf[256];
    const char* next;
    const char* endptr;

    endptr = buf;
    BUF_SET("/foo/bar/baz");
    next = xfpath_top(endptr, (char**)&endptr);
    TEST_ASSERT_EQUAL(0, memcmp(next, "foo", endptr - next));

    next = xfpath_top(endptr, (char**)&endptr);
    TEST_ASSERT_EQUAL(0, memcmp(next, "bar", endptr - next));

    next = xfpath_top(endptr, (char**)&endptr);
    TEST_ASSERT_EQUAL(0, memcmp(next, "baz", endptr - next));

    next = xfpath_top(endptr, (char**)&endptr);
    TEST_ASSERT_NULL(next);

    endptr = buf;
    BUF_SET("/foo/bar/baz");
    next = xfpath_top(endptr, (char**)&endptr);
    TEST_ASSERT_EQUAL(0, memcmp(next, "foo", endptr - next));

    endptr = buf;
    BUF_SET("/////foo/////bar///baz//////");
    next = xfpath_top(endptr, (char**)&endptr);
    TEST_ASSERT_EQUAL(0, memcmp(next, "foo", endptr - next));

    next = xfpath_top(endptr, (char**)&endptr);
    TEST_ASSERT_EQUAL(0, memcmp(next, "bar", endptr - next));

    next = xfpath_top(endptr, (char**)&endptr);
    TEST_ASSERT_EQUAL(0, memcmp(next, "baz", endptr - next));

    next = xfpath_top(endptr, (char**)&endptr);
    TEST_ASSERT_NULL(next);
}


TEST(xfpath, rnext)
{
    char buf[256];
    const char* next;
    const char* endptr;

    next = NULL;
    BUF_SET("/foo/bar/baz");
    next = xfpath_tail(buf, next, (char**)&endptr);
    TEST_ASSERT_EQUAL(0, memcmp(next, "baz", endptr - next));

    next = xfpath_tail(buf, next, (char**)&endptr);
    TEST_ASSERT_EQUAL(0, memcmp(next, "bar", endptr - next));

    next = xfpath_tail(buf, next, (char**)&endptr);
    TEST_ASSERT_EQUAL(0, memcmp(next, "foo", endptr - next));

    next = xfpath_tail(buf, next, (char**)&endptr);
    TEST_ASSERT_NULL(next);

    next = NULL;
    BUF_SET("////foo///bar////baz//////");
    next = xfpath_tail(buf, next, (char**)&endptr);
    TEST_ASSERT_EQUAL(0, memcmp(next, "baz", endptr - next));

    next = xfpath_tail(buf, next, (char**)&endptr);
    TEST_ASSERT_EQUAL(0, memcmp(next, "bar", endptr - next));

    next = xfpath_tail(buf, next, (char**)&endptr);
    TEST_ASSERT_EQUAL(0, memcmp(next, "foo", endptr - next));

    next = xfpath_tail(buf, next, (char**)&endptr);
    TEST_ASSERT_NULL(next);
}


TEST(xfpath, name)
{
    char buf[256];
    const char* name;
    const char* endptr;

    BUF_SET("/foo/bar/baz");
    name = xfpath_name(buf, (char**)&endptr);
    TEST_ASSERT_EQUAL(0, memcmp(name, "baz", endptr - name));

    BUF_SET("/foo/bar/baz/////");
    name = xfpath_name(buf, (char**)&endptr);
    TEST_ASSERT_EQUAL(0, memcmp(name, "baz", endptr - name));

    BUF_SET("/foo/bar/baz.tar.gz");
    name = xfpath_name(buf, (char**)&endptr);
    TEST_ASSERT_EQUAL(0, memcmp(name, "baz.tar.gz", endptr - name));

    BUF_SET("/foo");
    name = xfpath_name(buf, (char**)&endptr);
    TEST_ASSERT_EQUAL(0, memcmp(name, "foo", endptr - name));

    BUF_SET("foo");
    name = xfpath_name(buf, (char**)&endptr);
    TEST_ASSERT_EQUAL(0, memcmp(name, "foo", endptr - name));
}


TEST(xfpath, suffix)
{
    char buf[256];
    const char* suffix;
    const char* endptr;

    BUF_SET("/foo/bar/baz");
    suffix = xfpath_suffix(buf, (char**)&endptr);
    TEST_ASSERT_NULL(suffix);

    BUF_SET("/foo/bar/baz.txt");
    suffix = xfpath_suffix(buf, (char**)&endptr);
    TEST_ASSERT_EQUAL(0, memcmp(suffix, ".txt", endptr - suffix));

    BUF_SET("/foo/bar/baz.tar.gz");
    suffix = xfpath_suffix(buf, (char**)&endptr);
    TEST_ASSERT_EQUAL(0, memcmp(suffix, ".gz", endptr - suffix));

    BUF_SET("/foo/bar/.");
    suffix = xfpath_suffix(buf, (char**)&endptr);
    TEST_ASSERT_NULL(suffix);

    BUF_SET("/foo/bar/..");
    suffix = xfpath_suffix(buf, (char**)&endptr);
    TEST_ASSERT_NULL(suffix);

    BUF_SET("/foo/bar/.....");
    suffix = xfpath_suffix(buf, (char**)&endptr);
    TEST_ASSERT_NULL(suffix);

    BUF_SET("/foo/bar/.vim");
    suffix = xfpath_suffix(buf, (char**)&endptr);
    TEST_ASSERT_NULL(suffix);

    BUF_SET("/foo/bar/..vim");
    suffix = xfpath_suffix(buf, (char**)&endptr);
    TEST_ASSERT_EQUAL(0, memcmp(suffix, ".vim", endptr - suffix));

    BUF_SET("/foo/bar/vim..vim");
    suffix = xfpath_suffix(buf, (char**)&endptr);
    TEST_ASSERT_EQUAL(0, memcmp(suffix, ".vim", endptr - suffix));
}


TEST(xfpath, stem)
{
    char buf[256];
    const char* stem;
    const char* endptr;

    BUF_SET("/foo/bar/baz");
    stem = xfpath_stem(buf, (char**)&endptr);
    TEST_ASSERT_EQUAL(0, memcmp(stem, "baz", endptr - stem));

    BUF_SET("/foo/bar/baz.txt");
    stem = xfpath_stem(buf, (char**)&endptr);
    TEST_ASSERT_EQUAL(0, memcmp(stem, "baz", endptr - stem));

    BUF_SET("/foo/bar/baz.tar.gz");
    stem = xfpath_stem(buf, (char**)&endptr);
    TEST_ASSERT_EQUAL(0, memcmp(stem, "baz.tar", endptr - stem));

    BUF_SET("/foo/bar/.");
    stem = xfpath_stem(buf, (char**)&endptr);
    TEST_ASSERT_NULL(stem);

    BUF_SET("/foo/bar/..");
    stem = xfpath_stem(buf, (char**)&endptr);
    TEST_ASSERT_EQUAL(0, memcmp(stem, "..", endptr - stem));

    BUF_SET("/foo/bar/.....");
    stem = xfpath_stem(buf, (char**)&endptr);
    TEST_ASSERT_EQUAL(0, memcmp(stem, ".....", endptr - stem));

    BUF_SET("/foo/bar/.vim");
    stem = xfpath_stem(buf, (char**)&endptr);
    TEST_ASSERT_EQUAL(0, memcmp(stem, ".vim", endptr - stem));

    BUF_SET("/foo/bar/..vim");
    stem = xfpath_stem(buf, (char**)&endptr);
    TEST_ASSERT_EQUAL(0, memcmp(stem, ".", endptr - stem));

    BUF_SET("/foo/bar/vim..vim");
    stem = xfpath_stem(buf, (char**)&endptr);
    TEST_ASSERT_EQUAL(0, memcmp(stem, "vim.", endptr - stem));
}


TEST(xfpath, parent)
{
    char buf[256];
    const char* endptr;
    const char* parent;


    endptr = NULL;
    BUF_SET("/foo/bar/baz");
    parent = xfpath_parent(buf, endptr, (char**)&endptr);
    TEST_ASSERT_EQUAL(0, memcmp(parent, "/foo/bar", endptr - parent));

    parent = xfpath_parent(buf, endptr, (char**)&endptr);
    TEST_ASSERT_EQUAL(0, memcmp(parent, "/foo", endptr - parent));

    parent = xfpath_parent(buf, endptr, (char**)&endptr);
    TEST_ASSERT_EQUAL(0, memcmp(parent, "/", endptr - parent));

    parent = xfpath_parent(buf, endptr, (char**)&endptr);
    TEST_ASSERT_NULL(parent);

    endptr = NULL;
    BUF_SET("//foo///bar/////baz//////");
    parent = xfpath_parent(buf, endptr, (char**)&endptr);
    TEST_ASSERT_EQUAL(0, memcmp(parent, "//foo///bar", endptr - parent));

    parent = xfpath_parent(buf, endptr, (char**)&endptr);
    TEST_ASSERT_EQUAL(0, memcmp(parent, "//foo", endptr - parent));

    parent = xfpath_parent(buf, endptr, (char**)&endptr);
    TEST_ASSERT_EQUAL(0, memcmp(parent, "/", endptr - parent));

    parent = xfpath_parent(buf, endptr, (char**)&endptr);
    TEST_ASSERT_NULL(parent);

    endptr = NULL;
    BUF_SET("foo///bar/////baz//////");
    parent = xfpath_parent(buf, endptr, (char**)&endptr);
    TEST_ASSERT_EQUAL(0, memcmp(parent, "foo///bar", endptr - parent));

    parent = xfpath_parent(buf, endptr, (char**)&endptr);
    TEST_ASSERT_EQUAL(0, memcmp(parent, "foo", endptr - parent));

    parent = xfpath_parent(buf, endptr, (char**)&endptr);
    TEST_ASSERT_NULL(parent);

    endptr = NULL;
    BUF_SET("foo");
    parent = xfpath_parent(buf, endptr, (char**)&endptr);
    TEST_ASSERT_NULL(parent);

    endptr = NULL;
    BUF_SET("foo/");
    parent = xfpath_parent(buf, endptr, (char**)&endptr);
    TEST_ASSERT_NULL(parent);

    endptr = NULL;
    BUF_SET("/foo");
    parent = xfpath_parent(buf, endptr, (char**)&endptr);
    TEST_ASSERT_EQUAL(0, memcmp(parent, "/", endptr - parent));
    parent = xfpath_parent(buf, endptr, (char**)&endptr);
    TEST_ASSERT_NULL(parent);

    endptr = NULL;
    BUF_SET("/foo/");
    parent = xfpath_parent(buf, endptr, (char**)&endptr);
    TEST_ASSERT_EQUAL(0, memcmp(parent, "/", endptr - parent));
    parent = xfpath_parent(buf, endptr, (char**)&endptr);
    TEST_ASSERT_NULL(parent);
}


TEST_GROUP_RUNNER(xfpath)
{
    RUN_TEST_CASE(xfpath, join);
    RUN_TEST_CASE(xfpath, resolove_dot);
    RUN_TEST_CASE(xfpath, resolove);
    RUN_TEST_CASE(xfpath, resolove_drivelater);
    RUN_TEST_CASE(xfpath, next);
    RUN_TEST_CASE(xfpath, rnext);
    RUN_TEST_CASE(xfpath, name);
    RUN_TEST_CASE(xfpath, suffix);
    RUN_TEST_CASE(xfpath, stem);
    RUN_TEST_CASE(xfpath, parent);
}
