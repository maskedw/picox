#include <picox/allocator/xpico_allocator.h>
#include <unity.h>
#include <unity_fixture.h>
#include "testutils.h"


TEST_GROUP(xpalloc);


typedef struct X__HeapWalker
{
    size_t          num_chunks;
    size_t          total_size;
    const uint8_t*  prev;
    size_t          prev_size;
} X__HeapWalker;


static XPicoAllocator alloc;
#define X__HEAP_SIZE    (1024)


TEST_SETUP(xpalloc)
{
    void* buf = X_MALLOC(X__HEAP_SIZE);
    TEST_ASSERT_NOT_NULL(buf);
    memset(buf, 0x00, X__HEAP_SIZE);
    xpalloc_init(&alloc, buf, X__HEAP_SIZE);
}


TEST_TEAR_DOWN(xpalloc)
{
    X_FREE(xpalloc_heap(&alloc));
}


static void X__WalkHeap(const uint8_t* chunk, size_t size, void* user)
{
    X__HeapWalker* walker = user;
    walker->num_chunks++;
    walker->total_size += size;

    if (walker->prev != NULL)
    {
        TEST_ASSERT_TRUE(walker->prev < chunk);
        TEST_ASSERT_TRUE(walker->prev + walker->prev_size < chunk);
    }

    TEST_ASSERT_TRUE(walker->total_size <= X__HEAP_SIZE);

    walker->prev = chunk;
    walker->prev_size = size;
}


static void X__DammyWalkHeap(const uint8_t* chunk, size_t size, void* user)
{
    X_UNUSED(chunk);
    X_UNUSED(size);
    X_UNUSED(user);
}


static uint32_t X__RandomAllocates(void** ptrs)
{
    size_t i = 0;
    size_t bits = 0;
    for (i = 0; i < 1024; i++)
    {
        unsigned pos = rand() % 32;
        uint32_t bit = ((uint32_t)1UL) << pos;

        if (bits & bit)
        {
            xpalloc_deallocate(&alloc, ptrs[pos]);
            ptrs[pos] = NULL;
            bits &= ~bit;
        }
        else
        {
            bits |= bit;
            void* ptr = xpalloc_allocate(&alloc, (rand() % (X__HEAP_SIZE / 48)) + 1);
            TEST_ASSERT_NOT_NULL(ptr);
            TEST_ASSERT_TRUE(x_is_aligned(ptr, XPALLOC_ALIGNMENT));
            ptrs[pos] = ptr;
        }
    }
    return bits;
}


TEST(xpalloc, init)
{
    uint8_t* heap = xpalloc_heap(&alloc);
    X_TEST_ASSERTION_FAILED(xpalloc_init(NULL, heap, X__HEAP_SIZE));
    X_TEST_ASSERTION_FAILED(xpalloc_init(&alloc, NULL, X__HEAP_SIZE));
    X_TEST_ASSERTION_FAILED(xpalloc_init(&alloc, heap, 0));
    X_TEST_ASSERTION_SUCCESS(xpalloc_init(&alloc, heap, X__HEAP_SIZE));
}


TEST(xpalloc, allocate)
{
    X_TEST_ASSERTION_FAILED(xpalloc_allocate(NULL, 10));
    X_TEST_ASSERTION_FAILED(xpalloc_allocate(&alloc, 0));


    size_t i;
    void* ptrs[32];
    X__HeapWalker walker;

    for (i = 0; i < X_COUNT_OF(ptrs); i++)
        ptrs[i] = NULL;

    X__RandomAllocates(ptrs);

    memset(&walker, 0, sizeof(walker));
    xpalloc_walk_heap(&alloc, X__WalkHeap, &walker);

    for (i = 0; i < X_COUNT_OF(ptrs); i++)
        xpalloc_deallocate(&alloc, ptrs[i]);

    memset(&walker, 0, sizeof(walker));
    xpalloc_walk_heap(&alloc, X__WalkHeap, &walker);
    TEST_ASSERT_EQUAL(xpalloc_capacity(&alloc), xpalloc_reserve(&alloc));
    TEST_ASSERT_EQUAL(X__HEAP_SIZE, xpalloc_reserve(&alloc));
    TEST_ASSERT_EQUAL(walker.total_size, xpalloc_reserve(&alloc));
    TEST_ASSERT_EQUAL(1, walker.num_chunks);
}


TEST(xpalloc, deallocate)
{
    size_t reserve = xpalloc_reserve(&alloc);
    void* p = xpalloc_allocate(&alloc, 10);
    TEST_ASSERT_NOT_EQUAL(reserve, xpalloc_reserve(&alloc));

    X_TEST_ASSERTION_FAILED(xpalloc_deallocate(NULL, p));
    X_TEST_ASSERTION_SUCCESS(xpalloc_deallocate(&alloc, NULL));

    xpalloc_deallocate(&alloc, p);
    TEST_ASSERT_EQUAL(reserve, xpalloc_reserve(&alloc));
}


TEST(xpalloc, clear)
{
    X_TEST_ASSERTION_FAILED(xpalloc_clear(NULL));

    size_t i;
    void* ptrs[32];
    X__HeapWalker walker;

    for (i = 0; i < X_COUNT_OF(ptrs); i++)
        ptrs[i] = NULL;

    X__RandomAllocates(ptrs);
    xpalloc_clear(&alloc);

    memset(&walker, 0, sizeof(walker));
    xpalloc_walk_heap(&alloc, X__WalkHeap, &walker);
    TEST_ASSERT_EQUAL(xpalloc_capacity(&alloc), xpalloc_reserve(&alloc));
    TEST_ASSERT_EQUAL(X__HEAP_SIZE, xpalloc_reserve(&alloc));
    TEST_ASSERT_EQUAL(walker.total_size, xpalloc_reserve(&alloc));
    TEST_ASSERT_EQUAL(1, walker.num_chunks);
}


TEST(xpalloc, heap)
{
    X_TEST_ASSERTION_FAILED(xpalloc_heap(NULL));

    XPicoAllocator alloc;
    char buf[32];

    xpalloc_init(&alloc, buf, sizeof(buf));
    TEST_ASSERT_EQUAL_PTR(buf, xpalloc_heap(&alloc));
}


TEST(xpalloc, reserve)
{
    X_TEST_ASSERTION_FAILED(xpalloc_reserve(NULL));

    uint8_t* heap = xpalloc_heap(&alloc);
    uint8_t* aligned = x_roundup_alignment_ptr(heap, XPALLOC_ALIGNMENT);
    size_t reserve = X__HEAP_SIZE - (aligned - heap);

    xpalloc_init(&alloc, aligned, reserve);
    TEST_ASSERT_EQUAL(reserve, xpalloc_reserve(&alloc));

    xpalloc_init(&alloc, heap, X__HEAP_SIZE);
}


TEST(xpalloc, capacity)
{
    X_TEST_ASSERTION_FAILED(xpalloc_capacity(NULL));

    uint8_t* heap = xpalloc_heap(&alloc);
    uint8_t* aligned = x_roundup_alignment_ptr(heap, XPALLOC_ALIGNMENT);
    size_t capacity = X__HEAP_SIZE - (aligned - heap);

    xpalloc_init(&alloc, aligned, capacity);
    TEST_ASSERT_EQUAL(capacity, xpalloc_capacity(&alloc));

    xpalloc_init(&alloc, heap, X__HEAP_SIZE);
}


TEST(xpalloc, allocation_overhead)
{
    X_TEST_ASSERTION_FAILED(xpalloc_allocation_overhead(NULL, 1));

    const size_t orig_reserve = xpalloc_reserve(&alloc);
    void* p = xpalloc_allocate(&alloc, 1);
    X_UNUSED(p);

    const size_t next_reserve = xpalloc_reserve(&alloc);
    TEST_ASSERT_EQUAL(orig_reserve - next_reserve, xpalloc_allocation_overhead(&alloc, 1) + 1);
}


TEST(xpalloc, walk_heap)
{
    X__HeapWalker walker;
    memset(&walker, 0, sizeof(walker));

    X_TEST_ASSERTION_FAILED(xpalloc_walk_heap(NULL, X__DammyWalkHeap, &walker));
    X_TEST_ASSERTION_FAILED(xpalloc_walk_heap(&alloc, NULL, &walker));
    X_TEST_ASSERTION_SUCCESS(xpalloc_walk_heap(&alloc, X__DammyWalkHeap, NULL));

    xpalloc_allocate(&alloc, 1);
    void* p = xpalloc_allocate(&alloc, 1);
    X_UNUSED(p);
    xpalloc_allocate(&alloc, 1);
    xpalloc_deallocate(&alloc, p);

    const size_t reserve = xpalloc_reserve(&alloc);
    xpalloc_walk_heap(&alloc, X__WalkHeap, &walker);
    TEST_ASSERT_EQUAL(2, walker.num_chunks);
    TEST_ASSERT_EQUAL(reserve, walker.total_size);
}


TEST_GROUP_RUNNER(xpalloc)
{
    srand((uintptr_t)&alloc);
    RUN_TEST_CASE(xpalloc, init);
    RUN_TEST_CASE(xpalloc, allocate);
    RUN_TEST_CASE(xpalloc, deallocate);
    RUN_TEST_CASE(xpalloc, clear);
    RUN_TEST_CASE(xpalloc, heap);
    RUN_TEST_CASE(xpalloc, reserve);
    RUN_TEST_CASE(xpalloc, capacity);
    RUN_TEST_CASE(xpalloc, allocation_overhead);
    RUN_TEST_CASE(xpalloc, walk_heap);
}
