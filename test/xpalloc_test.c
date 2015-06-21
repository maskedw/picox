#include <picox/allocator/xpalloc.h>
#include <unity.h>
#include <unity_fixture.h>
#include <stdio.h>
#include <stdlib.h>


TEST_GROUP(xpalloc);


typedef struct X__HeapWalker
{
    size_t          num_chunks;
    size_t          total_size;
    const uint8_t*  prev;
    size_t          prev_size;
} X__HeapWalker;


static XPAlloc palloc;
#define X__HEAP_SIZE    (1024 * 10)


TEST_SETUP(xpalloc)
{
    void* buf = malloc(X__HEAP_SIZE);
    TEST_ASSERT_NOT_NULL(buf);
    memset(buf, 0x00, X__HEAP_SIZE);
    xpalloc_init(&palloc, buf, X__HEAP_SIZE);
}


TEST_TEAR_DOWN(xpalloc)
{
    free(xpalloc_heap(&palloc));
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


static uint32_t X__RandomAllocates(void** ptrs)
{
    size_t i = 0;
    size_t bits = 0;
    for (i = 0; i < 4096; i++)
    {
        unsigned pos = rand() % 32;
        uint32_t bit = ((uint32_t)1UL) << pos;

        if (bits & bit)
        {
            xpalloc_deallocate(&palloc, ptrs[pos]);
            ptrs[pos] = NULL;
            bits &= ~bit;
        }
        else
        {
            bits |= bit;
            void* ptr = xpalloc_allocate(&palloc, (rand() % (X__HEAP_SIZE / (32 * 2))) + 1);
            TEST_ASSERT_NOT_NULL(ptr);
            TEST_ASSERT_TRUE(x_is_aligned(ptr, XPALLOC_ALIGN));
            ptrs[pos] = ptr;
        }
    }
    return bits;
}


TEST(xpalloc, alloc)
{
    int i;
    void* ptrs[32];
    X__HeapWalker walker;

    for (i = 0; i < X_COUNT_OF(ptrs); i++)
        ptrs[i] = NULL;

    X__RandomAllocates(ptrs);

    memset(&walker, 0, sizeof(walker));
    xpalloc_walk_heap(&palloc, X__WalkHeap, &walker);

    for (i = 0; i < X_COUNT_OF(ptrs); i++)
        xpalloc_deallocate(&palloc, ptrs[i]);

    memset(&walker, 0, sizeof(walker));
    xpalloc_walk_heap(&palloc, X__WalkHeap, &walker);
    TEST_ASSERT_EQUAL(xpalloc_capacity(&palloc), xpalloc_reserve(&palloc));
    TEST_ASSERT_EQUAL(X__HEAP_SIZE, xpalloc_reserve(&palloc));
    TEST_ASSERT_EQUAL(walker.total_size, xpalloc_reserve(&palloc));
    TEST_ASSERT_EQUAL(1, walker.num_chunks);
}


TEST(xpalloc, clear)
{
    int i;
    void* ptrs[32];
    X__HeapWalker walker;

    for (i = 0; i < X_COUNT_OF(ptrs); i++)
        ptrs[i] = NULL;

    X__RandomAllocates(ptrs);
    xpalloc_clear(&palloc);

    memset(&walker, 0, sizeof(walker));
    xpalloc_walk_heap(&palloc, X__WalkHeap, &walker);
    TEST_ASSERT_EQUAL(xpalloc_capacity(&palloc), xpalloc_reserve(&palloc));
    TEST_ASSERT_EQUAL(X__HEAP_SIZE, xpalloc_reserve(&palloc));
    TEST_ASSERT_EQUAL(walker.total_size, xpalloc_reserve(&palloc));
    TEST_ASSERT_EQUAL(1, walker.num_chunks);

#if 0
    size_t x = 3;
    TEST_ASSERT_EQUAL(6, (x = x_roundup_multiple(x + 1, 3)));
    TEST_ASSERT_EQUAL(9, (x = x_roundup_multiple(x + 1, 3)));
    TEST_ASSERT_EQUAL(12, (x = x_roundup_multiple(x + 1, 3)));
#endif
#if 0
    size_t x = 3;
    x = x_roundup_multiple(x + 1, 3);
    TEST_ASSERT_EQUAL(6, x);
#endif
#if 0
    size_t x = 8;
    x = x_roundup_multiple(x + 1, 8);
    TEST_ASSERT_EQUAL(16, x);
#endif
#if 0
    size_t x = 6;
    x = x_roundup_multiple(x - 1, 3);
    TEST_ASSERT_EQUAL(3, x);
#endif
}


TEST_GROUP_RUNNER(xpalloc)
{
    srand((uintptr_t)&palloc);
    RUN_TEST_CASE(xpalloc, alloc);
    RUN_TEST_CASE(xpalloc, clear);
}
