#include <picox/allocator/xstack_allocator.h>
#include "testutils.h"


TEST_GROUP(xsalloc);


static XStackAllocator alloc;
#define X__HEAP_SIZE    (1024)


TEST_SETUP(xsalloc)
{
    void* buf = x_malloc(X__HEAP_SIZE);
    TEST_ASSERT_NOT_NULL(buf);
    memset(buf, 0x00, X__HEAP_SIZE);
    xsalloc_init(&alloc, buf, X__HEAP_SIZE, X_ALIGN_OF(XMaxAlign));
}


TEST_TEAR_DOWN(xsalloc)
{
    x_free(xsalloc_heap(&alloc));
}


static void X__TestAlloc(bool growth_upward)
{
    size_t alignment;
    size_t n;
    size_t i;
    void*  ptr;
    uint8_t* heap = xsalloc_heap(&alloc);


    alignment = 1;
    while (alignment <= 128)
    {
        xsalloc_init(&alloc, heap, X__HEAP_SIZE, alignment);
        xsalloc_set_growth_direction(&alloc, growth_upward);
        n = xsalloc_capacity(&alloc) / alignment;
        for (i = 0; i < n; i++)
        {
            ptr = xsalloc_allocate(&alloc, alignment);
            TEST_ASSERT_NOT_NULL(ptr);
            TEST_ASSERT_TRUE(x_is_aligned(ptr, alignment));
        }
        alignment = x_roundup_power_of_two(alignment + 1);
    }
}


TEST(xsalloc, init)
{
    uint8_t* heap = xsalloc_heap(&alloc);
    X_TEST_ASSERTION_FAILED(xsalloc_init(NULL, heap, X__HEAP_SIZE, X_ALIGN_OF(XMaxAlign)));
    X_TEST_ASSERTION_FAILED(xsalloc_init(&alloc, NULL, X__HEAP_SIZE, X_ALIGN_OF(XMaxAlign)));
    X_TEST_ASSERTION_FAILED(xsalloc_init(&alloc, heap, 0, X_ALIGN_OF(XMaxAlign)));
    X_TEST_ASSERTION_FAILED(xsalloc_init(&alloc, heap, 0, 9));
    X_TEST_ASSERTION_FAILED(xsalloc_init(&alloc, heap, 4, 8));
    X_TEST_ASSERTION_SUCCESS(xsalloc_init(&alloc, heap, X__HEAP_SIZE, 2));
    TEST_ASSERT_EQUAL(X__HEAP_SIZE, xsalloc_reserve(&alloc));
    TEST_ASSERT_EQUAL(X__HEAP_SIZE, xsalloc_capacity(&alloc));
}


TEST(xsalloc, allocate)
{
    X_TEST_ASSERTION_FAILED(xsalloc_allocate(NULL, 10));
    X_TEST_ASSERTION_FAILED(xsalloc_allocate(&alloc, 0));

    X__TestAlloc(true);
    X__TestAlloc(false);
}


TEST(xsalloc, rewind)
{
    size_t reserve = xsalloc_reserve(&alloc);
    uint8_t* begin = xsalloc_bedin(&alloc);
    uint8_t* end = xsalloc_end(&alloc);

    xsalloc_rewind(&alloc, begin, end);
    xsalloc_set_growth_direction(&alloc, true);
    xsalloc_allocate(&alloc, 15);
    xsalloc_set_growth_direction(&alloc, false);
    xsalloc_allocate(&alloc, 31);

    X_TEST_ASSERTION_FAILED(xsalloc_rewind(NULL, begin, end));
    X_TEST_ASSERTION_FAILED(xsalloc_rewind(&alloc, NULL, end));
    X_TEST_ASSERTION_FAILED(xsalloc_rewind(&alloc, begin, NULL));
    X_TEST_ASSERTION_FAILED(xsalloc_rewind(&alloc, end, begin));
    X_TEST_ASSERTION_FAILED(xsalloc_rewind(&alloc, begin - 1, end));
    X_TEST_ASSERTION_FAILED(xsalloc_rewind(&alloc, begin, end + 1));
    X_TEST_ASSERTION_FAILED(xsalloc_rewind(&alloc, begin - xsalloc_alignment(&alloc), end));
    X_TEST_ASSERTION_SUCCESS(xsalloc_rewind(&alloc, NULL, NULL));

    xsalloc_rewind(&alloc, begin, end);
    xsalloc_set_growth_direction(&alloc, true);
    xsalloc_allocate(&alloc, 1);
    xsalloc_allocate(&alloc, 2);
    xsalloc_allocate(&alloc, 3);
    xsalloc_set_growth_direction(&alloc, false);
    xsalloc_allocate(&alloc, 1);
    xsalloc_allocate(&alloc, 2);
    xsalloc_allocate(&alloc, 3);

    xsalloc_rewind(&alloc, begin, end);
    TEST_ASSERT_EQUAL_PTR(begin, xsalloc_bedin(&alloc));
    TEST_ASSERT_EQUAL_PTR(end, xsalloc_end(&alloc));
    TEST_ASSERT_EQUAL(reserve, xsalloc_reserve(&alloc));
}


TEST(xsalloc, clear)
{
    X_TEST_ASSERTION_FAILED(xsalloc_clear(NULL));

    void* begin = xsalloc_bedin(&alloc);
    void* end = xsalloc_end(&alloc);
    size_t reserve = xsalloc_reserve(&alloc);

    xsalloc_set_growth_direction(&alloc, true);
    xsalloc_allocate(&alloc, 15);
    xsalloc_set_growth_direction(&alloc, false);
    xsalloc_allocate(&alloc, 31);

    xsalloc_clear(&alloc);
    TEST_ASSERT_EQUAL_PTR(begin, xsalloc_bedin(&alloc));
    TEST_ASSERT_EQUAL_PTR(end, xsalloc_end(&alloc));
    TEST_ASSERT_EQUAL(reserve, xsalloc_reserve(&alloc));
}


TEST(xsalloc, growth_direction)
{
    X_TEST_ASSERTION_FAILED(xsalloc_set_growth_direction(NULL, true));
    X_TEST_ASSERTION_FAILED(xsalloc_growth_direction(NULL));

    xsalloc_set_growth_direction(&alloc, false);
    TEST_ASSERT_EQUAL(xsalloc_growth_direction(&alloc), false);
    xsalloc_set_growth_direction(&alloc, true);
    TEST_ASSERT_EQUAL(xsalloc_growth_direction(&alloc), true);
}


TEST(xsalloc, reserve)
{
    X_TEST_ASSERTION_FAILED(xsalloc_reserve(NULL));

    uint8_t* heap = xsalloc_heap(&alloc);
    xsalloc_init(&alloc, heap, X__HEAP_SIZE, X_ALIGN_OF(char));
    TEST_ASSERT_EQUAL(X__HEAP_SIZE, xsalloc_reserve(&alloc));
}


TEST(xsalloc, capacity)
{
    X_TEST_ASSERTION_FAILED(xsalloc_capacity(NULL));

    uint8_t* heap = xsalloc_heap(&alloc);
    xsalloc_init(&alloc, heap, X__HEAP_SIZE, X_ALIGN_OF(char));
    TEST_ASSERT_EQUAL(X__HEAP_SIZE, xsalloc_capacity(&alloc));
}


TEST(xsalloc, alignment)
{
    X_TEST_ASSERTION_FAILED(xsalloc_alignment(NULL));

    uint8_t* heap = xsalloc_heap(&alloc);
    size_t alignment = 1;
    while (alignment <= 128)
    {
        xsalloc_init(&alloc, heap, X__HEAP_SIZE, alignment);
        TEST_ASSERT_EQUAL(alignment, xsalloc_alignment(&alloc));
        alignment <<= 1;
    }
}


TEST(xsalloc, heap)
{
    X_TEST_ASSERTION_FAILED(xsalloc_heap(NULL));

    XStackAllocator alloc;
    char buf[10];

    xsalloc_init(&alloc, buf, sizeof(buf), X_ALIGN_OF(char));
    TEST_ASSERT_EQUAL_PTR(buf, xsalloc_heap(&alloc));
}


TEST_GROUP_RUNNER(xsalloc)
{
    RUN_TEST_CASE(xsalloc, init);
    RUN_TEST_CASE(xsalloc, allocate);
    RUN_TEST_CASE(xsalloc, clear);
    RUN_TEST_CASE(xsalloc, rewind);
    RUN_TEST_CASE(xsalloc, growth_direction);
    RUN_TEST_CASE(xsalloc, reserve);
    RUN_TEST_CASE(xsalloc, capacity);
    RUN_TEST_CASE(xsalloc, alignment);
    RUN_TEST_CASE(xsalloc, heap);
}
