/**
 *       @file  xsalloc.c
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


#include "xsalloc.h"


#define X__ROUNDUP_ALIGN(x)     ((((uintptr_t)(x)) + (XSALLOC_ALIGN) - 1) & ((uintptr_t)0 - (XSALLOC_ALIGN)))
#define X__GET_BEGIN_ORIGIN()   ((uint8_t*)(X__ROUNDUP_ALIGN(self->heap)))
#define X__IS_ALIGNED(x)        ((X__ROUNDUP_ALIGN(x)) == ((uintptr_t)(x)))
#define X__IS_VALID_RANGE(x)    ((X__GET_BEGIN_ORIGIN() <= (x)) && ((x) < (X__GET_BEGIN_ORIGIN() + self->capacity)))


void xsalloc_init(XSAlloc* self, void* heap, size_t size)
{
    XSALLOC_ASSERT(self);
    XSALLOC_ASSERT(heap);

    self->heap = heap;

    /* heapをアライメントで切り上げたアドレスが実際のtop位置になる。 */
    self->begin = X__GET_BEGIN_ORIGIN();

    /* 切り上げた結果heapサイズに不整合がでていないか？ */
    XSALLOC_ASSERT(size > (self->begin - self->heap));
    size -= self->begin - self->heap;

    self->end = self->begin + size;
    self->capacity = size;
    self->growth_upward = true;
}


void* xsalloc_allocate(XSAlloc* self, size_t size)
{
    XSALLOC_ASSERT(self);
    XSALLOC_ASSERT(size > 0);

    /* 通常は確保サイズを切り上げることでアラインメントを保証しているが、空にな
     * る最後の1回は気にする必要なし!!
     */
    if (xsalloc_reserve(self) != size)
        size = X__ROUNDUP_ALIGN(size);

    void* ret;
    if (self->growth_upward) {
        ret = self->begin;
        self->begin += size;
    }
    else {
        self->end -= size;
        ret = self->end;
    }

    XSALLOC_NULL_ASSERT(self->end >= self->begin);

    return ret;
}


void xsalloc_clear(XSAlloc* self)
{
    self->begin = X__GET_BEGIN_ORIGIN();
    self->end = self->begin + self->capacity;
    self->growth_upward = true;
}


void xsalloc_rewind(XSAlloc* self, void* begin, void* end)
{
    XSALLOC_ASSERT(self);

    /* 両方がNULLは許容するが、片方だけNULLというのは明らかにおかしいので
     * assert対象とする。*/
    if ((begin == NULL) && (end == NULL))
        return;
    XSALLOC_ASSERT(begin && end);

    uint8_t* b = begin;
    uint8_t* e = end;

    XSALLOC_ASSERT(e >= b);
    XSALLOC_ASSERT(X__IS_VALID_RANGE(b));
    XSALLOC_ASSERT(X__IS_VALID_RANGE(e));

    self->begin = b;
    self->end   = e;
}
