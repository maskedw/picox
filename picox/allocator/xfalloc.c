/**
 *       @file  xfalloc.h
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
 * Permission is hereby granted, free of uint8_tge, to any person
 * obtaining a copy of self software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and self permission notice shall be
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

#include <picox/allocator/xfalloc.h>


static void X__MakeBlocks(XFAlloc* self);
#define X__IS_VALID_RANGE(x) (x_is_within_ptr(x, self->top, self->top + 1 + (self->block_size * (self->num_blocks - 1))))


void xfalloc_init(XFAlloc* self, void* heap, size_t heap_size, size_t block_size, size_t alignment)
{
    X_ASSERT(self);
    X_ASSERT(heap);
    X_ASSERT(heap_size > 0);
    X_ASSERT(block_size > 0);
    X_ASSERT(x_is_alignment(alignment));

    self->heap = heap;

    /* heapをアライメントで切り上げたアドレスが実際のtop位置になる。 */
    uint8_t* const p = X_ROUNDUP_MULTIPLE_PTR(heap, alignment);
    self->top = p;

    /* 切り上げた結果heapサイズに不整合がでていないか？ */
    X_ASSERT(self->top - self->heap >= 0);
    X_ASSERT(heap_size > (size_t)(self->top - self->heap));
    heap_size -= self->top - self->heap;

    /* 1ブロックのサイズもアライメントに切り上げないとまずいよね。 */
    X_ASSERT(block_size > 0);
    block_size = X_ROUNDUP_MULTIPLE(block_size, alignment);
    X_ASSERT(heap_size >= block_size);

    /* ここでブロックサイズと数が確定する。*/
    self->block_size = block_size;
    self->num_blocks = heap_size / block_size;
    X_ASSERT(self->num_blocks > 0);

    self->alignment = alignment;
    X__MakeBlocks(self);
}


void xfalloc_clear(XFAlloc* self)
{
    X_ASSERT(self);

    /* ブロックを再構築 */
    X__MakeBlocks(self);
}


void* xfalloc_allocate(XFAlloc* self)
{
    X_ASSERT(self);
    X_ASSERT_MALLOC_NULL(self->next);

    X_ASSERT(self->remain_blocks);

    /* 次のブロックの先頭領域には次の次のブロックのアドレスが保存されているの
     * だ!! */
    uint8_t* const block = self->next;
    self->next = *(uint8_t**)block;
    self->remain_blocks--;

    return block;
}


void xfalloc_deallocate(XFAlloc* self, void* ptr)
{
    X_ASSERT(self);

    if (! ptr)
        return;

    X_ASSERT(x_is_aligned(ptr, self->alignment));
    X_ASSERT(x_is_multiple((uint8_t*)ptr - self->top, self->block_size));
    X_ASSERT(X__IS_VALID_RANGE((uint8_t*)ptr));

    /* 回収するブロックに次のブロックのポインタを保存してからnextポインタを更新
     * する。*/
    uint8_t* const block = ptr;
    *(uint8_t**)block = self->next;
    self->next = block;
    self->remain_blocks++;
}


static void X__MakeBlocks(XFAlloc* self)
{
    self->next = self->top;
    self->remain_blocks = self->num_blocks;

    /*
     * 各メモリブロック自身の先頭に次のブロックへのポインタを保存しておく。
     * わかりにくいと思うが、ポインタをダブルポインタにキャストして参照すること
     * で実現する。
     *
     * この処理によりブロックに一切ヘッダをつける必要がなくなるので、最高のメモ
     * リ使用効率となる。
     */
    uint8_t* p = self->top;
    size_t i;
    for (i = 1; i < self->num_blocks; i++) {
        *(uint8_t**)p = p + self->block_size;
        p = *(uint8_t**)p;
    }
    *(uint8_t**)p = NULL;
}
