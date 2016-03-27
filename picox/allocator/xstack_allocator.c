/**
 *       @file  xstack_allocator.c
 *      @brief
 *
 *    @details
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2015/06/15
 * ===================================================================
 */

/*
 * License: MIT license
 * Copyright (c) <2015> <MaskedW [maskedw00@gmail.com]>
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */


#include <picox/allocator/xstack_allocator.h>


static inline uint8_t*
X__GetBeginOrigin(XStackAllocator* self)
{
    return X_ROUNDUP_ALIGNMENT_PTR(self->heap, self->alignment);
}


static inline bool
X__IsValidRange(XStackAllocator* self, const void* p)
{
    const uint8_t* b = X__GetBeginOrigin(self);
    const uint8_t* e = b + self->capacity;

    return x_is_within_ptr(p, b, e + 1);
}


void xsalloc_init(XStackAllocator* self, void* heap, size_t size, size_t alignment)
{
    X_ASSERT(self);
    X_ASSERT(heap);
    X_ASSERT(X_IS_ALIGNMENT(alignment));

    self->heap = heap;
    self->alignment = alignment;

    /* heapをアライメントで切り上げたアドレスが実際のtop位置になる。 */
    self->begin = X__GetBeginOrigin(self);

    /* 切り上げた結果heapサイズに不整合がでていないか？ */
    X_ASSERT(self->begin - self->heap >= 0);
    X_ASSERT(size > (size_t)(self->begin - self->heap));
    size -= self->begin - self->heap;

    self->end = x_rounddown_alignment_ptr(self->begin + size, alignment);

    /* 切り下げた結果heapサイズに不整合がでていないか？ */
    X_ASSERT(self->end - self->begin >= 1);
    X_ASSERT((size_t)(self->end - self->begin) >= self->alignment);

    self->capacity = self->end - self->begin;
    self->growth_upward = true;
}


void* xsalloc_allocate(XStackAllocator* self, size_t size)
{
    X_ASSERT(self);
    X_ASSERT(size > 0);

    const size_t reserve = xsalloc_reserve(self);
    void* ret;

    X_ASSERT_MALLOC_NULL(reserve >= size);
    X_UNUSED(reserve);

    size = x_roundup_alignment(size, self->alignment);

    if (self->growth_upward) {
        ret = self->begin;
        self->begin += size;
    }
    else {
        self->end -= size;
        ret = self->end;
    }

    return ret;
}


void xsalloc_clear(XStackAllocator* self)
{
    X_ASSERT(self);
    self->begin = X__GetBeginOrigin(self);
    self->end = self->begin + self->capacity;
    self->growth_upward = true;
}


void xsalloc_rewind(XStackAllocator* self, void* begin, void* end)
{
    X_ASSERT(self);

    /* 両方がNULLは許容するが、片方だけNULLというのは明らかにおかしいので
     * assert対象とする。*/
    if ((begin == NULL) && (end == NULL))
        return;
    X_ASSERT(begin && end);

    X_ASSERT(x_is_aligned(begin, self->alignment));
    X_ASSERT(x_is_aligned(end, self->alignment));

    uint8_t* b = begin;
    uint8_t* e = end;

    X_ASSERT(e >= b);
    X_ASSERT(X__IsValidRange(self, b));
    X_ASSERT(X__IsValidRange(self, e));

    self->begin = b;
    self->end   = e;
}
