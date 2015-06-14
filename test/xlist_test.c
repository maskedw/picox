#include <unity.h>
#include <unity_fixture.h>
#include <picox/container/xlist.h>
#include <stdio.h>


TEST_GROUP(xlist);


typedef struct ListData
{
    XListNode node;
    int x;
} ListData;

static XList head0;
static XList head1;
static ListData data[10];
static const size_t num_data = sizeof(data) / sizeof(data[0]);



TEST_SETUP(xlist)
{
    xlist_init(&head0);
    xlist_init(&head1);
    int i;
    for (i = 0; i < sizeof(data) / sizeof(data[0]); i++)
        data[i].x = i;
}


TEST_TEAR_DOWN(xlist)
{
}


TEST(xlist, add)
{
    /* —v‘f‚ª1ŒÂ‚È‚çback, front‚Í“¯‚¶ */
    xlist_push_front(&head0, &data[0].node);
    TEST_ASSERT_EQUAL_PTR(&data[0],
             xnode_entry(xlist_front(&head0), ListData, node));
    TEST_ASSERT_EQUAL_PTR(&data[0],
             xnode_entry(xlist_back(&head0), ListData, node));

    /* —v‘f‚ª2ŒÂ‚È‚ç“–‘Rˆá‚¤ */
    xlist_push_back(&head0, &data[1].node);
    TEST_ASSERT_EQUAL_PTR(&data[0],
             xnode_entry(xlist_front(&head0), ListData, node));
    TEST_ASSERT_EQUAL_PTR(&data[1],
             xnode_entry(xlist_back(&head0), ListData, node));
}


TEST(xlist, foreach)
{
    int i;
    for (i = 0; i < num_data; i++)
        xlist_push_back(&head0, &data[i].node);

    i = 0;
    XListNode* ite;
    xlist_foreach(&head0, ite)
    {
         ListData* p = xnode_entry(ite, ListData, node);
         TEST_ASSERT_EQUAL_PTR(p, &data[i]);
         TEST_ASSERT_EQUAL(p->x, i);
         i++;
    }
    TEST_ASSERT_EQUAL(num_data, i);

    i = num_data - 1;
    xlist_rforeach(&head0, ite)
    {
         ListData* p = xnode_entry(ite, ListData, node);
         TEST_ASSERT_EQUAL_PTR(p, &data[i]);
         TEST_ASSERT_EQUAL(p->x, i);
         i--;
    }
    TEST_ASSERT_EQUAL(-1, i);
}


TEST(xlist, splice)
{
    int i;
    for (i = 0; i < num_data / 2; i++)
        xlist_push_back(&head0, &data[i].node);

    for (i = num_data / 2; i < num_data; i++)
        xlist_push_back(&head1, &data[i].node);

    xlist_splice_back(&head0, &head1);

    i = 0;
    XListNode* ite;
    xlist_foreach(&head0, ite)
    {
         ListData* p = xnode_entry(ite, ListData, node);
         TEST_ASSERT_EQUAL_PTR(p, &data[i]);
         TEST_ASSERT_EQUAL(p->x, i);
         i++;
    }
    TEST_ASSERT_EQUAL(num_data, i);
    TEST_ASSERT_TRUE(xlist_empty(&head1));
}


TEST_GROUP_RUNNER(xlist)
{
    RUN_TEST_CASE(xlist, add);
    RUN_TEST_CASE(xlist, foreach);
    RUN_TEST_CASE(xlist, splice);
}
