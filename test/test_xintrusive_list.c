#include <picox/container/xintrusive_list.h>


TEST_GROUP(xilist);


typedef struct ListData
{
    XIntrusiveNode node;
    int x;
} ListData;

static XIntrusiveList head0;
static XIntrusiveList head1;
static ListData data[10];
static const size_t num_data = sizeof(data) / sizeof(data[0]);



TEST_SETUP(xilist)
{
    xilist_init(&head0);
    xilist_init(&head1);
    size_t i;
    for (i = 0; i < sizeof(data) / sizeof(data[0]); i++)
        data[i].x = i;
}


TEST_TEAR_DOWN(xilist)
{
}


TEST(xilist, add)
{
    /* —v‘f‚ª1ŒÂ‚È‚çback, front‚Í“¯‚¶ */
    xilist_push_front(&head0, &data[0].node);
    TEST_ASSERT_EQUAL_PTR(&data[0],
             xnode_entry(xilist_front(&head0), ListData, node));
    TEST_ASSERT_EQUAL_PTR(&data[0],
             xnode_entry(xilist_back(&head0), ListData, node));

    /* —v‘f‚ª2ŒÂ‚È‚ç“–‘Rˆá‚¤ */
    xilist_push_back(&head0, &data[1].node);
    TEST_ASSERT_EQUAL_PTR(&data[0],
             xnode_entry(xilist_front(&head0), ListData, node));
    TEST_ASSERT_EQUAL_PTR(&data[1],
             xnode_entry(xilist_back(&head0), ListData, node));
}


TEST(xilist, foreach)
{
    size_t i;
    for (i = 0; i < num_data; i++)
        xilist_push_back(&head0, &data[i].node);

    i = 0;
    XIntrusiveNode* ite;
    xilist_foreach(&head0, ite)
    {
         ListData* p = xnode_entry(ite, ListData, node);
         TEST_ASSERT_EQUAL_PTR(p, &data[i]);
         TEST_ASSERT_EQUAL(p->x, i);
         i++;
    }
    TEST_ASSERT_EQUAL(num_data, i);

    i = num_data - 1;
    xilist_rforeach(&head0, ite)
    {
         ListData* p = xnode_entry(ite, ListData, node);
         TEST_ASSERT_EQUAL_PTR(p, &data[i]);
         TEST_ASSERT_EQUAL(p->x, i);
         i--;
    }
    TEST_ASSERT_EQUAL(-1, i);
}


TEST(xilist, splice)
{
    size_t i;
    for (i = 0; i < num_data / 2; i++)
        xilist_push_back(&head0, &data[i].node);

    for (i = num_data / 2; i < num_data; i++)
        xilist_push_back(&head1, &data[i].node);

    xilist_splice_back(&head0, &head1);

    i = 0;
    XIntrusiveNode* ite;
    xilist_foreach(&head0, ite)
    {
         ListData* p = xnode_entry(ite, ListData, node);
         TEST_ASSERT_EQUAL_PTR(p, &data[i]);
         TEST_ASSERT_EQUAL(p->x, i);
         i++;
    }
    TEST_ASSERT_EQUAL(num_data, i);
    TEST_ASSERT_TRUE(xilist_empty(&head1));
}


TEST_GROUP_RUNNER(xilist)
{
    RUN_TEST_CASE(xilist, add);
    RUN_TEST_CASE(xilist, foreach);
    RUN_TEST_CASE(xilist, splice);
}
