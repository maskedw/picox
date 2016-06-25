/**
 *       @file  xpico_allocator.c
 *      @brief
 *
 *    @details
 *
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

#include <picox/allocator/xpico_allocator.h>


/** メモリチャンク
 *
 * メモリ管理では、ヒープから切り分ける管理単位をChunkと命名することが多いので、
 * それにならって空きブロックをチャンクと名付ける。チャンクから、メモリブロック
 * を切り出すという考え方をする。
 */
typedef struct X__Chunk
{
    /* nextは絶対先頭メンバにしておくこと。理由はX__Allocate()の解説を参照せよ!! */
    struct X__Chunk*    next;
    size_t              size;
} X__Chunk;


static void* X__Allocate(XPicoAllocator* self, size_t size);
static void X__Deallocate(XPicoAllocator* self, void* ptr, size_t size);
#define X__ALIGN        (self->alignment)


bool xpalloc_init(XPicoAllocator* self, void* heap, size_t size, size_t alignment)
{
    X__Chunk* chunk;
    X_ASSERT(self);
    X_ASSERT(alignment);
    X_ASSERT(x_is_power_of_two(alignment));

    self->heap = heap;
    self->alignment = 0;
    self->top = NULL;
    self->capacity = 0;
    self->reserve = 0;
    self->max_used = 0;
    self->ownmemory = false;

    if (! heap)
    {
        heap = x_malloc(size);
        if (!heap)
            return false;
        self->ownmemory = true;
    }

    self->heap = heap;
    self->alignment = x_roundup_multiple(alignment, X_ALIGN_OF(size_t));
    self->top = x_roundup_multiple_ptr(heap, X__ALIGN);

    X_ASSERT(self->top - self->heap >= 0);
    X_ASSERT(size > (size_t)(self->top - self->heap));
    self->capacity = size - (self->top - self->heap);
    self->reserve = self->capacity;

    chunk = (X__Chunk*)self->top;
    chunk->next = NULL;
    chunk->size = self->capacity;

    return true;
}


void xpalloc_deinit(XPicoAllocator* self)
{
    X_ASSERT(self);
    if (self->ownmemory)
    {
        x_free(self->heap);
        self->heap = NULL;
        self->ownmemory = false;
    }
}


void* xpalloc_allocate(XPicoAllocator* self, size_t size)
{
    char* ptr;
    X_ASSERT(self);
    X_ASSERT(size > 0);


    /* サイズ情報確保用の領域を余分に確保する。 */
    size = x_roundup_multiple(size + X__ALIGN, X__ALIGN);
    if (size <= sizeof(X__Chunk))
        size = x_roundup_multiple(sizeof(X__Chunk) + 1, X__ALIGN);

    ptr = X__Allocate(self, size);
    X_ASSERT_MALLOC_NULL(ptr);

    if (ptr != NULL)
    {
        size_t used;

        X_ASSERT(x_is_aligned(ptr, X_ALIGN_OF(size_t)));
        *(size_t*)(ptr) = size;
        ptr += X__ALIGN;
        self->reserve -= size;

        used = self->capacity - self->reserve;
        if (used > self->max_used)
            self->max_used = used;
    }

    return ptr;
}


void* xpalloc_reallocate(XPicoAllocator* self, void* old_mem, size_t new_size)
{
    size_t old_size = 0;
    void* new_mem;

    X_ASSERT(self);
    X_ASSERT(new_size > 0);

    if (old_mem)
    {
        char* const p = ((char*)old_mem) - X__ALIGN;

        X_ASSERT(x_is_aligned(old_mem, X__ALIGN));
        old_size = *(size_t*)p;
    }

    new_mem = xpalloc_allocate(self, new_size);
    if (!new_mem)
        return NULL;

    if (!old_mem)
        return new_mem;

    /* 新しいサイズより旧いサイズの方が大きかったら新しいサイズ分コピーする。
     * 古いサイズより新しいサイズの方が大きかったら古いサイズ分コピーする。
     */
    if (old_size > new_size)
        memcpy(new_mem, old_mem, new_size);
    else
        memcpy(new_mem, old_mem, old_size);

    xpalloc_deallocate(self, old_mem);

    return new_mem;
}


void xpalloc_deallocate(XPicoAllocator* self, void* ptr)
{
    char* p;
    size_t size;

    X_ASSERT(self);

    if (ptr == NULL)
        return;

    /* メモリ確保の時点でアラインされてるんだから、解放メモリがアラインされてな
     * かったら、不正なアドレスですわな。 */
    X_ASSERT(x_is_aligned(ptr, X__ALIGN));

    /* 解放メモリの前にサイズ情報が仕込まれているのだ。 */
    p = ((char*)ptr) - X__ALIGN;
    size = *(size_t*)p;

    X__Deallocate(self, p, size);
    self->reserve += size;
}


void xpalloc_clear(XPicoAllocator* self)
{
    X_ASSERT(self);

    self->reserve = self->capacity;
    self->top = x_roundup_multiple_ptr(self->heap, X__ALIGN);
    self->max_used = 0;

    X__Chunk* chunk = (X__Chunk*)self->top;
    chunk->next = NULL;
    chunk->size = self->capacity;
}


size_t xpalloc_allocation_overhead(const XPicoAllocator* self, size_t n)
{
    X_ASSERT(self);
    X_ASSERT(n > 0);
    return x_roundup_multiple((n + sizeof(X__Chunk)), X__ALIGN) - n;
}


void xpalloc_walk_heap(const XPicoAllocator* self, XPicoAllocatorWalker walker, void* user)
{
    const X__Chunk* chunk;
    X_ASSERT(self);
    X_ASSERT(walker);

    chunk = (const X__Chunk*)self->top;
    while (chunk)
    {
        walker((const uint8_t*)chunk, chunk->size, user);
        chunk = chunk->next;
    }
}


bool xpalloc_is_owner(const XPicoAllocator* self, const void* ptr)
{
    return x_is_within_ptr(ptr, self->heap, self->heap + self->capacity);
}


uint8_t* xpalloc_heap(const XPicoAllocator* self)
{
    X_ASSERT(self);
    return self->heap;
}


size_t xpalloc_reserve(const XPicoAllocator* self)
{
    X_ASSERT(self);
    return self->reserve;
}


size_t xpalloc_capacity(const XPicoAllocator* self)
{
    X_ASSERT(self);
    return self->capacity;
}


size_t xpalloc_max_used(const XPicoAllocator* self)
{
    X_ASSERT(self);
    return self->max_used;
}


static void* X__Allocate(XPicoAllocator* self, size_t size)
{
    /* ここはかなりトリッキーなので解説しておく。
     * nextがX__Chunkの先頭メンバになっているのがミソだ。
     * 結果、
     * p1->next == self->top となり、
     * p1 = x とすると、self->topを置き換えることもできる。
     * この時さらに後ろのメンバ(p1->size)を参照したら勿論まずいのだが、p1->size
     * は参照しないので問題ない。
     * わかりにくいのだが、これにより相当処理が簡略化できる。
     */

    /* 一旦(void*)にキャストしているのはstrict aliasingルールを回避するたため
     * + http://d.hatena.ne.jp/yohhoy/20120220/p1
     */
    X__Chunk* p1 = (X__Chunk*)(void*)(&self->top);
    X__Chunk* p = p1->next;

    for (;;)
    {
        if (p == NULL)
            return NULL;

        if (p->size >= size)
        {
            /* メモリ確保成功!! */
            p->size -= size;
            break;
        }

        p1 = p;
        p = p->next;
    }

    if (p->size == 0)
    {
        /* このチャンクはもう空っぽだから次のチャンクをつなぐ */
        p1->next = p->next;
    }
    else
    {
        /* 使用サイズ分チャンクをせばめる。 */
        X__Chunk* p2 = (X__Chunk*)((uint8_t*)p + size);
        p1->next = p2;
        p2->next = p->next;
        p2->size = p->size;
    }

    return p;
}


static void X__Deallocate(XPicoAllocator* self, void* ptr, size_t size)
{
    X__Chunk* blk = ptr;
    X__Chunk* chunk = (X__Chunk*)self->top;
    X__Chunk* next_chunk;

    X_ASSERT(xpalloc_is_owner(self, ptr));
    X_ASSERT((uint8_t*)ptr + size <= self->heap + self->capacity);

    for(;;)
    {
        next_chunk = chunk->next;


        if ((next_chunk == NULL) || (blk < next_chunk))
        {
            /* 不正な解放ブロックとサイズ指定のチェック */
            X_ASSERT((next_chunk == NULL) || (next_chunk >= (X__Chunk*)((uint8_t*)blk + size)));

            /* マージできる？ */
            if (blk == (X__Chunk*)((uint8_t*)chunk + chunk->size))
                chunk->size += size;
            else if  (blk < chunk)
            {
                /* マージできる？ */
                if (chunk == (X__Chunk*)((uint8_t*)blk + size))
                {
                    blk->next = next_chunk;
                    blk->size = size + chunk->size;
                }
                else
                {
                    blk->next = chunk;
                    blk->size = size;
                }

                chunk = blk;

                /* topは常に最上位のチャンクを指す */
                if (blk < (X__Chunk*)self->top)
                    self->top = (uint8_t*)blk;

            }
            else
            {
                blk->next = next_chunk;
                chunk->next = blk;
                chunk = blk;
                chunk->size = size;
            }

            /* 次のチャンクとマージできる？ */
            if (next_chunk == (X__Chunk*)((uint8_t*)chunk + chunk->size))
            {
                chunk->next  = next_chunk->next;
                chunk->size += next_chunk->size;
            }

            /* メモリ解放完了!! */
            break;
        }
        /* 次のループへ */
        chunk = next_chunk;
        X_ASSERT((blk >= ((X__Chunk*)((uint8_t*)chunk + chunk->size))));
    }
}
