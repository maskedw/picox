/**
 *       @file  xpalloc.c
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

#include <picox/allocator/xpalloc.h>


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


static void* X__Allocate(X__Chunk* top, size_t size);
static void X__Deallocate(X__Chunk* top, void* ptr, size_t size);


static const size_t X__ALIGN = ((sizeof(X__Chunk) <= XPALLOC_ALIGN) ?
                                XPALLOC_ALIGN : X_ROUNDUP_MULTIPLE(sizeof(X__Chunk), XPALLOC_ALIGN));


void xpalloc_init(XPAlloc* self, void* heap, size_t size)
{
    XPALLOC_ASSERT(self);
    XPALLOC_ASSERT(heap);

    self->heap = heap;
    self->top = x_roundup_multiple_ptr(heap, X__ALIGN);

    XPALLOC_ASSERT(size > (self->top - self->heap));
    self->capacity = size - (self->top - self->heap);
    self->reserve = self->capacity;

    X__Chunk* chunk = (X__Chunk*)self->top;
    chunk->next = NULL;
    chunk->size = self->capacity;
}


void xpalloc_clear(XPAlloc* self)
{
    XPALLOC_ASSERT(self);

    self->reserve = self->capacity;
    X__Chunk* chunk = (X__Chunk*)self->top;
    chunk->next = NULL;
    chunk->size = self->capacity;
}


void xpalloc_walk_heap(const XPAlloc* self, XPAllocWalker walker, void* user)
{
    XPALLOC_ASSERT(self);
    XPALLOC_ASSERT(walker);

    X__Chunk* chunk = (X__Chunk*)self->top;
    while (chunk)
    {
        walker(chunk, chunk->size, user);
    }
}


void* xpalloc_allocate(XPAlloc* self, size_t size)
{
    XPALLOC_ASSERT(self);
    XPALLOC_ASSERT(size > 0);

    char* ptr;

    /* サイズ情報確保用の領域を余分に確保する。 */
    size = x_roundup_multiple(size + X__ALIGN, X__ALIGN);
    ptr = X__Allocate((X__Chunk*)self->top, size);

    XPALLOC_NULL_ASSERT(ptr);

    if (ptr != NULL)
    {
        XPALLOC_ASSERT(x_is_aligned(ptr, X_ALIGN_OF(size_t)));
        *(size_t*)(ptr) = size;
        ptr += X__ALIGN;
        self->reserve -= size;
    }

    return ptr;
}


void xpalloc_deallocate(XPAlloc* self, void* ptr)
{
    if (ptr == NULL)
        return;

    /* メモリ確保の時点でアラインされてるんだから、解放メモリがアラインされてな
     * かったら、不正なアドレスですわな。 */
    XPALLOC_ASSERT(x_is_aligned(ptr, X__ALIGN));

    /* 解放メモリの前にサイズ情報が仕込まれているのだ。 */
    char* const p = ((char*)ptr) - X__ALIGN;
    const size_t size = *(size_t*)p;

    X__Deallocate((X__Chunk*)self->top, p, size);
    self->reserve += size;
}


static void* X__Allocate(X__Chunk* top, size_t size)
{
    /* ここはかなりトリッキーなので解説しておく。
     * nextがX__Chunkの先頭メンバになっているのがミソだ。
     * ダブルポインタをポインタにキャストして、構造体先頭メンバの自己参照ポイン
     * タを参照したら、(X__Chunk** p)の(*p)と同じ動きになる。つまりここでは
     * p1->next == top となる。
     * この時、そのさらに後ろのメンバ(size)を参照したら勿論まずいのだが、そうは
     * 絶対ならないので問題ない。これに何の意味があるの？と言われると説明が難し
     * いが、続く処理が色々簡略化できるのだ。
     */
    X__Chunk* p1 = (X__Chunk*)&top;
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


static void X__Deallocate(X__Chunk* top, void* ptr, size_t size)
{
    /* lchunk_sizeをわざわざ別変数にしている理由は、X__Allocate()の解説を参照せよ。
     * lchunk->next == topの時、lchunk->sizeは参照できない。
     */
    X__Chunk* lchunk = (X__Chunk*)&top;
    X__Chunk* blk = ptr;
    size_t lchunk_size = 0;

    /* アドレス上位側のチャンクをlchunk(left chunk)、下位側にある次のチャンクを
     * rchunk (right chunk)と定義する。
     * 次のチャンクがない時は、rchunkはNULLを指す。
     * 解放対象のブロックを左右間で探し、なければチャンクを右にスライドさせてい
     * く。
     * 左側から順に探していくので、左側の断片化が進むと、どんどん性能が劣化して
     * しまうが、簡単な実装なのでそれはやむなし。
     */
    for(;;)
    {
        X__Chunk* rchunk = lchunk->next;

        if ((rchunk == NULL) || (blk < rchunk))
        {
            /* rchunkが終端ではないのに、blkのブロックがrchunkの領域にオーバーラップしてたら、
             * sizeか、アドレスが不正と判定できる。
             */
            XPALLOC_ASSERT(! ((rchunk != NULL) && (rchunk < (X__Chunk*)((uint8_t*)blk + size))));

            if (blk == (X__Chunk*)((uint8_t*)lchunk + lchunk_size))
            {
                /* 左チャンクと解放ブロックの位置がピッタリ合わさっていたら左
                 * チャンクとブロックはマージできる。*/
                lchunk_size += size;
            }
            else
            {
                /* blkが新たな左チャンクとなる。 */
                lchunk->next = blk;
                lchunk = blk;
                lchunk_size = size;
            }


            if (rchunk == NULL)
            {
                /* 右チャンクがないということは、左チャンクが最終チャンクという
                 * ことだ。
                 */
                lchunk->next = NULL;
                lchunk->size = lchunk_size;
            }
            else if (rchunk == (X__Chunk*)((uint8_t*)lchunk + lchunk_size))
            {
                /* 左チャンクと右チャンクの領域がピッタリ合わさっていれば左右
                 * チャンクをマージできる。
                 */
                lchunk->next = rchunk->next;
                lchunk->size = lchunk_size + rchunk->size;
            }
            else
            {
                /* 左右チャンク間に使用済みブロックあり */
                lchunk->next = rchunk;
                lchunk->size = lchunk_size;
            }

            /* メモリ解放完了!! */
            break;
        }

        /* 次のループへ */
        lchunk = rchunk;
        lchunk_size = lchunk->size;

        /* 左右チャンク間に解放ブロックがあるはずなので、整合性チェック */
        XPALLOC_ASSERT(blk > ((X__Chunk*)((uint8_t*)lchunk + lchunk_size)));
    }
}
