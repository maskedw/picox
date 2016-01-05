#include <picox/allocator/xfixed_allocator.h>
#include "testutils.h"


TEST_GROUP(xfalloc);


static XFixedAllocator* alloc;
#define X__BLOCK_SIZE   (32)
#define X__ALIGNMENT    (X_ALIGN_OF(XMaxAlign))
#define X__HEAP_SIZE    (X_ROUNDUP_MULTIPLE(1024, X__ALIGNMENT))
#define X__NUM_BLOCKS   (X__HEAP_SIZE / X__BLOCK_SIZE)


TEST_SETUP(xfalloc)
{
    alloc = x_malloc(sizeof(XFixedAllocator));
    TEST_ASSERT_NOT_NULL(alloc);

    void* buf = x_malloc(X__HEAP_SIZE);
    TEST_ASSERT_NOT_NULL(buf);
    memset(buf, 0x00, X__HEAP_SIZE);
    xfalloc_init(alloc, buf, X__HEAP_SIZE, X__BLOCK_SIZE, X__ALIGNMENT);
}


TEST_TEAR_DOWN(xfalloc)
{
    x_free(xfalloc_heap(alloc));
    x_free(alloc);
}


TEST(xfalloc, init)
{
    uint8_t* heap = xfalloc_heap(alloc);
    X_UNUSED(heap);
    X_TEST_ASSERTION_FAILED(xfalloc_init(NULL, heap, X__HEAP_SIZE, X__BLOCK_SIZE, X__ALIGNMENT));
    X_TEST_ASSERTION_FAILED(xfalloc_init(alloc, NULL, X__HEAP_SIZE, X__BLOCK_SIZE, X__ALIGNMENT));
    X_TEST_ASSERTION_FAILED(xfalloc_init(alloc, heap, 0, X__BLOCK_SIZE, X__ALIGNMENT));
    X_TEST_ASSERTION_FAILED(xfalloc_init(alloc, heap, X__HEAP_SIZE, 0, X__ALIGNMENT));
    X_TEST_ASSERTION_FAILED(xfalloc_init(alloc, heap, X__HEAP_SIZE, X__HEAP_SIZE + 1, X__ALIGNMENT));
    X_TEST_ASSERTION_FAILED(xfalloc_init(alloc, heap, X__HEAP_SIZE, X__HEAP_SIZE, XFALLOC_MIN_ALIGNMENT - 1));
    X_TEST_ASSERTION_FAILED(xfalloc_init(alloc, heap, X__HEAP_SIZE, X__HEAP_SIZE, 5));
    X_TEST_ASSERTION_FAILED(xfalloc_init(alloc, heap, X__HEAP_SIZE, X__HEAP_SIZE, 0));
}


TEST(xfalloc, allocate)
{
    X_TEST_ASSERTION_FAILED(xfalloc_allocate(NULL));

    void* ptrs[X__NUM_BLOCKS];
    size_t i;

    for (i = 0; i < xfalloc_num_blocks(alloc); i++)
    {
        ptrs[i] = xfalloc_allocate(alloc);
        TEST_ASSERT_TRUE(x_is_aligned(ptrs[i], xfalloc_alignment(alloc)));
    }
}


TEST(xfalloc, deallocate)
{
    uint8_t* heap = xfalloc_heap(alloc);
    uint8_t* p = xfalloc_allocate(alloc);
    int dammy;

    X_UNUSED(p);
    X_UNUSED(dammy);

    X_TEST_ASSERTION_FAILED(xfalloc_deallocate(NULL, p));
    X_TEST_ASSERTION_FAILED(xfalloc_deallocate(alloc, &dammy));
    X_TEST_ASSERTION_FAILED(xfalloc_deallocate(alloc, p - 1));
    X_TEST_ASSERTION_FAILED(xfalloc_deallocate(alloc, p + 1));
    X_TEST_ASSERTION_SUCCESS(xfalloc_deallocate(alloc, NULL));
    X_TEST_ASSERTION_SUCCESS(xfalloc_deallocate(alloc, p));

    xfalloc_init(alloc, heap, X__HEAP_SIZE, X__BLOCK_SIZE, X__ALIGNMENT);

    void* ptrs[X__NUM_BLOCKS];
    size_t i;

    for (i = 0; i < xfalloc_num_blocks(alloc); i++)
    {
        ptrs[i] = xfalloc_allocate(alloc);
    }

    for (i = 0; i < xfalloc_num_blocks(alloc); i++)
    {
        xfalloc_deallocate(alloc, ptrs[i]);
    }
}


TEST(xfalloc, clear)
{
    X_TEST_ASSERTION_FAILED(xfalloc_clear(NULL));
    size_t remain_blocks = xfalloc_remain_blocks(alloc);
    size_t num_blocks = xfalloc_num_blocks(alloc);

    void* ptrs[X__NUM_BLOCKS];
    size_t i;

    for (i = 0; i < xfalloc_num_blocks(alloc); i++)
    {
        ptrs[i] = xfalloc_allocate(alloc);
    }
    X_UNUSED(ptrs);

    xfalloc_clear(alloc);

    TEST_ASSERT_EQUAL(remain_blocks, xfalloc_remain_blocks(alloc));
    TEST_ASSERT_EQUAL(num_blocks, xfalloc_num_blocks(alloc));

    for (i = 0; i < xfalloc_num_blocks(alloc); i++)
    {
        ptrs[i] = xfalloc_allocate(alloc);
    }
}


TEST(xfalloc, heap)
{
    X_TEST_ASSERTION_FAILED(xfalloc_heap(NULL));
    uint8_t* heap = xfalloc_heap(alloc);
    uint8_t buf[32];
    xfalloc_init(alloc, buf, sizeof(buf), 16, XFALLOC_MIN_ALIGNMENT);
    TEST_ASSERT_EQUAL_PTR(buf, xfalloc_heap(alloc));

    xfalloc_init(alloc, heap, sizeof(buf), 16, XFALLOC_MIN_ALIGNMENT);
}


TEST(xfalloc, block_size)
{
    X_TEST_ASSERTION_FAILED(xfalloc_block_size(NULL));
    TEST_ASSERT_EQUAL(X__BLOCK_SIZE, xfalloc_block_size(alloc));
}


TEST(xfalloc, num_blocks)
{
    X_TEST_ASSERTION_FAILED(xfalloc_num_blocks(NULL));
    TEST_ASSERT_EQUAL(X__NUM_BLOCKS, xfalloc_num_blocks(alloc));
}


TEST(xfalloc, remain_blocks)
{
    X_TEST_ASSERTION_FAILED(xfalloc_remain_blocks(NULL));
    size_t remain = xfalloc_num_blocks(alloc);
    size_t i;
    for (i = 0; i < xfalloc_num_blocks(alloc); i++)
    {
        TEST_ASSERT_EQUAL(remain, xfalloc_remain_blocks(alloc));
        xfalloc_allocate(alloc);
        remain--;
    }
}


TEST(xfalloc, alignment)
{
    X_TEST_ASSERTION_FAILED(xfalloc_alignment(NULL));

    size_t alignment = 1;
    size_t i;
    uint8_t* heap = xfalloc_heap(alloc);

    for (i = 0; i < 5; i++)
    {
        xfalloc_init(alloc, heap, X__HEAP_SIZE, X__BLOCK_SIZE, alignment);
        TEST_ASSERT_EQUAL(alignment, xfalloc_alignment(alloc));
        alignment <<= 1;
    }
}


TEST_GROUP_RUNNER(xfalloc)
{
    RUN_TEST_CASE(xfalloc, init);
    RUN_TEST_CASE(xfalloc, allocate);
    RUN_TEST_CASE(xfalloc, deallocate);
    RUN_TEST_CASE(xfalloc, clear);
    RUN_TEST_CASE(xfalloc, heap);
    RUN_TEST_CASE(xfalloc, block_size);
    RUN_TEST_CASE(xfalloc, num_blocks);
    RUN_TEST_CASE(xfalloc, remain_blocks);
    RUN_TEST_CASE(xfalloc, alignment);
}
