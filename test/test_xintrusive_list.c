#include <picox/container/xintrusive_list.h>
#include "testutils.h"


TEST_GROUP(xilist);


typedef struct X__ListData
{
    XIntrusiveNode node;
    int x;
} X__ListData;


static XIntrusiveList* list;



TEST_SETUP(xilist)
{
    list = X_MALLOC(sizeof(XIntrusiveList));
    xilist_init(list);
}


TEST_TEAR_DOWN(xilist)
{
    X_FREE(list);
}


TEST(xilist, init)
{
    X_TEST_ASSERTION_FAILED(xilist_init(NULL));
}


TEST(xilist, head)
{
    X_TEST_ASSERTION_FAILED(xilist_head(NULL));
}


TEST(xilist, clear)
{
    X_TEST_ASSERTION_FAILED(xilist_clear(NULL));
    X__ListData v;
    xilist_push_back(list, &v.node);
    TEST_ASSERT_FALSE(xilist_empty(list));
    xilist_clear(list);
    TEST_ASSERT_TRUE(xilist_empty(list));
}


TEST(xilist, empty)
{
    X_TEST_ASSERTION_FAILED(xilist_empty(NULL));
    X__ListData v;
    TEST_ASSERT_TRUE(xilist_empty(list));
    xilist_push_back(list, &v.node);
    TEST_ASSERT_FALSE(xilist_empty(list));
}


TEST(xilist, end)
{
    X_TEST_ASSERTION_FAILED(xilist_end(NULL));
}


TEST(xilist, front)
{
    X_TEST_ASSERTION_FAILED(xilist_front(NULL));

    X__ListData v;
    X__ListData v2;
    xilist_push_back(list, &v.node);
    xilist_push_back(list, &v2.node);
    TEST_ASSERT_EQUAL_PTR(&v, xnode_entry(xilist_front(list), X__ListData, node));
}


TEST(xilist, back)
{
    X_TEST_ASSERTION_FAILED(xilist_back(NULL));

    X__ListData v;
    X__ListData v2;
    xilist_push_back(list, &v.node);
    xilist_push_back(list, &v2.node);
    TEST_ASSERT_EQUAL_PTR(&v2, xnode_entry(xilist_back(list), X__ListData, node));
}


TEST(xilist, foreach)
{
    X__ListData v[10];
    int i;
    const int n = X_COUNT_OF(v);
    for (i = 0; i < n; i++)
    {
        v[i].x = i;
        xilist_push_back(list, &(v[i].node));
    }

    i = 0;
    XIntrusiveNode* ite;
    xilist_foreach(list, ite)
    {
         X__ListData* p = xnode_entry(ite, X__ListData, node);
         TEST_ASSERT_EQUAL_PTR(p, &v[i]);
         TEST_ASSERT_EQUAL(p->x, i);
         i++;
    }
    TEST_ASSERT_EQUAL(n, i);

    i = n - 1;
    xilist_rforeach(list, ite)
    {
         X__ListData* p = xnode_entry(ite, X__ListData, node);
         TEST_ASSERT_EQUAL_PTR(p, &v[i]);
         TEST_ASSERT_EQUAL(p->x, i);
         i--;
    }
    TEST_ASSERT_EQUAL(-1, i);
}


TEST(xilist, size)
{
    X_TEST_ASSERTION_FAILED(xilist_size(NULL));
    TEST_ASSERT_EQUAL(0, xilist_size(list));

    X__ListData v[10];
    int i;
    const int n = X_COUNT_OF(v);
    for (i = 0; i < n; i++)
    {
        xilist_push_back(list, &(v[i].node));
        TEST_ASSERT_EQUAL(i + 1, xilist_size(list));
    }
}


TEST(xilist, is_singular)
{
    X_TEST_ASSERTION_FAILED(xilist_is_singular(NULL));
    TEST_ASSERT_FALSE(xilist_is_singular(list));

    X__ListData v;
    X__ListData v2;
    xilist_push_back(list, &v.node);
    TEST_ASSERT_TRUE(xilist_is_singular(list));
    xilist_push_back(list, &v2.node);
    TEST_ASSERT_FALSE(xilist_is_singular(list));
}


TEST(xilist, push_front)
{
    X__ListData v;
    X__ListData v2;

    X_TEST_ASSERTION_FAILED(xilist_push_front(NULL, &v.node));
    X_TEST_ASSERTION_FAILED(xilist_push_front(list, NULL));

    xilist_push_front(list, &v.node);
    xilist_push_front(list, &v2.node);
    TEST_ASSERT_EQUAL_PTR(&v2, xnode_entry(xilist_front(list), X__ListData, node));
}


TEST(xilist, push_back)
{
    X__ListData v;
    X__ListData v2;

    X_TEST_ASSERTION_FAILED(xilist_push_back(NULL, &v.node));
    X_TEST_ASSERTION_FAILED(xilist_push_back(list, NULL));

    xilist_push_back(list, &v.node);
    xilist_push_back(list, &v2.node);
    TEST_ASSERT_EQUAL_PTR(&v2, xnode_entry(xilist_back(list), X__ListData, node));
}


TEST(xilist, move_front)
{
    X__ListData v;
    X__ListData v2;
    XIntrusiveList vlist2;
    XIntrusiveList* list2 = &vlist2;
    xilist_init(list2);

    X_TEST_ASSERTION_FAILED(xilist_move_front(NULL, &v.node));
    X_TEST_ASSERTION_FAILED(xilist_move_front(list, NULL));

    xilist_push_back(list, &v.node);
    xilist_push_back(list2, &v2.node);

    X__ListData* p = xnode_entry(xilist_front(list2), X__ListData, node);
    xilist_move_front(list, &(p->node));
    TEST_ASSERT_TRUE(xilist_empty(list2));
    TEST_ASSERT_EQUAL(2, xilist_size(list));

    TEST_ASSERT_EQUAL_PTR(&v2, xnode_entry(xilist_front(list), X__ListData, node));
}


TEST(xilist, move_back)
{
    X__ListData v;
    X__ListData v2;
    XIntrusiveList vlist2;
    XIntrusiveList* list2 = &vlist2;
    xilist_init(list2);

    X_TEST_ASSERTION_FAILED(xilist_move_back(NULL, &v.node));
    X_TEST_ASSERTION_FAILED(xilist_move_back(list, NULL));

    xilist_push_back(list, &v.node);
    xilist_push_back(list2, &v2.node);

    X__ListData* p = xnode_entry(xilist_back(list2), X__ListData, node);
    xilist_move_back(list, &(p->node));
    TEST_ASSERT_TRUE(xilist_empty(list2));
    TEST_ASSERT_EQUAL(2, xilist_size(list));

    TEST_ASSERT_EQUAL_PTR(&v2, xnode_entry(xilist_back(list), X__ListData, node));
}


TEST(xilist, splice_front)
{
    X__ListData v[10];
    XIntrusiveList vlist2;
    XIntrusiveList* list2 = &vlist2;
    xilist_init(list2);

    int i;
    const int n = X_COUNT_OF(v);
    for (i = 0; i < n / 2; i++)
    {
        v[i].x = i;
        xilist_push_back(list2, &(v[i].node));
    }

    for (; i < n; i++)
    {
        v[i].x = i;
        xilist_push_back(list, &(v[i].node));
    }

    xilist_splice_front(list, list2);
    TEST_ASSERT_TRUE(xilist_empty(list2));

    i = 0;
    XIntrusiveNode* ite;
    xilist_foreach(list, ite)
    {
         X__ListData* p = xnode_entry(ite, X__ListData, node);
         TEST_ASSERT_EQUAL_PTR(p, &v[i]);
         TEST_ASSERT_EQUAL(p->x, i);
         i++;
    }
    TEST_ASSERT_EQUAL(n, i);
}


TEST(xilist, splice_back)
{
    X__ListData v[10];
    XIntrusiveList vlist2;
    XIntrusiveList* list2 = &vlist2;
    xilist_init(list2);

    int i;
    const int n = X_COUNT_OF(v);
    for (i = 0; i < n / 2; i++)
    {
        v[i].x = i;
        xilist_push_back(list, &(v[i].node));
    }

    for (; i < n; i++)
    {
        v[i].x = i;
        xilist_push_back(list2, &(v[i].node));
    }

    xilist_splice_back(list, list2);
    TEST_ASSERT_TRUE(xilist_empty(list2));

    i = 0;
    XIntrusiveNode* ite;
    xilist_foreach(list, ite)
    {
         X__ListData* p = xnode_entry(ite, X__ListData, node);
         TEST_ASSERT_EQUAL_PTR(p, &v[i]);
         TEST_ASSERT_EQUAL(p->x, i);
         i++;
    }
    TEST_ASSERT_EQUAL(n, i);
}


TEST(xilist, transfer_front)
{
    X__ListData v[10];
    XIntrusiveList vlist2;
    XIntrusiveList* list2 = &vlist2;
    xilist_init(list2);

    int i;
    const int n = X_COUNT_OF(v);
    for (i = 0; i < n / 2; i++)
    {
        v[i].x = i;
        xilist_push_back(list2, &(v[i].node));
    }

    for (; i < n; i++)
    {
        v[i].x = i;
        xilist_push_back(list, &(v[i].node));
    }

    xilist_transfer_front(list, list2, &v[2].node);
    TEST_ASSERT_EQUAL(2, xilist_size(list2));

    i = 0;
    XIntrusiveNode* ite;
    xilist_foreach(list, ite)
    {
         X__ListData* p = xnode_entry(ite, X__ListData, node);

         if (i < 3)
         {
            TEST_ASSERT_EQUAL_PTR(p, &v[i]);
            TEST_ASSERT_EQUAL(p->x, i);
         }
         else
         {
            TEST_ASSERT_EQUAL_PTR(p, &v[i + 2]);
            TEST_ASSERT_EQUAL(p->x, i + 2);
         }
         i++;
    }
    TEST_ASSERT_EQUAL(8, i);
}


TEST(xilist, transfer_back)
{
    X__ListData v[10];
    XIntrusiveList vlist2;
    XIntrusiveList* list2 = &vlist2;
    xilist_init(list2);

    int i;
    const int n = X_COUNT_OF(v);
    for (i = 0; i < n / 2; i++)
    {
        v[i].x = i;
        xilist_push_back(list, &(v[i].node));
    }

    for (; i < n; i++)
    {
        v[i].x = i;
        xilist_push_back(list2, &(v[i].node));
    }

    xilist_transfer_back(list, list2, &v[7].node);
    TEST_ASSERT_EQUAL(2, xilist_size(list2));

    i = 0;
    XIntrusiveNode* ite;
    xilist_foreach(list, ite)
    {
         X__ListData* p = xnode_entry(ite, X__ListData, node);

        TEST_ASSERT_EQUAL_PTR(p, &v[i]);
        TEST_ASSERT_EQUAL(p->x, i);
         i++;
    }
    TEST_ASSERT_EQUAL(8, i);
}


TEST(xilist, swap)
{
    X__ListData v[10];
    XIntrusiveList vlist2;
    XIntrusiveList* list2 = &vlist2;
    xilist_init(list2);

    int i;
    const int n = X_COUNT_OF(v);
    for (i = 0; i < n / 2; i++)
    {
        v[i].x = i;
        xilist_push_back(list2, &(v[i].node));
    }

    for (; i < n; i++)
    {
        v[i].x = i;
        xilist_push_back(list, &(v[i].node));
    }

    xilist_swap(list, list2);

    i = 0;
    XIntrusiveNode* ite;
    xilist_foreach(list, ite)
    {
         X__ListData* p = xnode_entry(ite, X__ListData, node);
         TEST_ASSERT_EQUAL_PTR(p, &v[i]);
         TEST_ASSERT_EQUAL(p->x, i);
         i++;
    }

    xilist_foreach(list2, ite)
    {
         X__ListData* p = xnode_entry(ite, X__ListData, node);
         TEST_ASSERT_EQUAL_PTR(p, &v[i]);
         TEST_ASSERT_EQUAL(p->x, i);
         i++;
    }
    TEST_ASSERT_EQUAL(n, i);
}




TEST_GROUP_RUNNER(xilist)
{
    RUN_TEST_CASE(xilist, init);
    RUN_TEST_CASE(xilist, head);
    RUN_TEST_CASE(xilist, clear);
    RUN_TEST_CASE(xilist, empty);
    RUN_TEST_CASE(xilist, end);
    RUN_TEST_CASE(xilist, front);
    RUN_TEST_CASE(xilist, back);
    RUN_TEST_CASE(xilist, foreach);
    RUN_TEST_CASE(xilist, size);
    RUN_TEST_CASE(xilist, is_singular);
    RUN_TEST_CASE(xilist, push_front);
    RUN_TEST_CASE(xilist, push_back);
    RUN_TEST_CASE(xilist, move_front);
    RUN_TEST_CASE(xilist, move_back);
    RUN_TEST_CASE(xilist, splice_front);
    RUN_TEST_CASE(xilist, splice_back);
    RUN_TEST_CASE(xilist, transfer_front);
    RUN_TEST_CASE(xilist, transfer_back);
    RUN_TEST_CASE(xilist, swap);
}
