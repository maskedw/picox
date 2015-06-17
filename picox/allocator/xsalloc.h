/**
 *       @file  xsalloc.h
 *      @brief  Stack memory allocator
 *
 *    @details
 *
 *    ヒープ領域をスタックの用にポインタをずらしながらメモリアロケーションを行い
 *    ます。 通常のmallocを使用した場合との主な違いは以下の通りです。
 *
 *    ------------------------------------------------------------
 *
 *    [利点]
 *
 *    1. 超高速
 *
 *    ポインタをずらすだけなので超高速です。
 *
 *    2. 余分なヘッダ領域を一切使用しない
 *
 *    個別の解放ができないのでヘッダ領域を使用しません。
 *
 *    3. 絶対に断片化しない
 *
 *    個別の解放ができないので断片化しようがありません。
 *
 *    ------------------------------------------------------------
 *
 *    [欠点]
 *
 *    1. 個別のメモリ解放ができない
 *
 *    常に直線的にポインタ位置が移動するだけなので、個別にメモリの解放をするとい
 *    う概念がありません。
 *
 *    ------------------------------------------------------------
 *
 *    [利用例]
 *
 *    + 同一タイミングでメモリ破棄が行える時、あるいは行えるようにした時
 *
 *    + ゲームの1フレーム用スクラッチメモリ
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


#ifndef picox_xsalloc_h_
#define picox_xsalloc_h_


#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>


#ifdef __cplusplus
extern "C" {
#endif


/** メモリブロックのアライメントです
 *
 *  各ブロックのアドレスはXSALLOC_ALIGNに切り上げられます。
 */
#ifndef XSALLOC_ALIGN
    #define XSALLOC_ALIGN   sizeof(double)
#endif


/** 不正状態を検出します
 */
#ifndef XSALLOC_ASSERT
    #define XSALLOC_ASSERT(expr)   do { if (! (expr)) for(;;); } while (0)
#endif


/** メモリNULLを検出します
 */
#ifndef XSALLOC_NULL_ASSERT
    #define XSALLOC_NULL_ASSERT(ptr)   XSALLOC_ASSERT(ptr)
#endif


/** スタックメモリアロケータ管理クラスです
 */
typedef struct XSAlloc
{
/// privatesection
    uint8_t*    heap;
    uint8_t*    begin;
    uint8_t*    end;
    size_t      capacity;
    bool        growth_upward;
} XSAlloc;


/** メモリブロックを初期化します
 *
 *  @param heap     スタック管理するメモリ領域
 *  @param size     heap領域のサイズ
 *
 *  @pre
 *  + heap != NULL
 *  + size  > 0
 *
 *  @attention
 *  heap領域はXSALLOC_ALIGNに切り上げられます。できればあらかじめアラインしてお
 *  く方がよいでしょう。
 */
void xsalloc_init(XSAlloc* self, void* heap, size_t size);


/** ヒープからsizeバイトのメモリを切り出して返します
 */
void* xsalloc_allocate(XSAlloc* self, size_t size);


/** ヒープを初期状態に戻します
 */
void xsalloc_clear(XSAlloc* self);


/** begin, endの位置にスタックポインタを移動します
 *  @pre
 *  + end >= begin
 *  + begin, endはこのオブジェクトのheap領域を指していること。
 */
void xsalloc_rewind(XSAlloc* self, void* begin, void* end);


/** スタック伸長方向を返します
 */
bool xsalloc_growth_direction(XSAlloc* self)
{
    XSALLOC_ASSERT(self);
    return self->growth_upward;
}


/** スタック伸長方向を設定します
 */
void xsalloc_set_growth_direction(XSAlloc* self, bool growth_upward)
{
    XSALLOC_ASSERT(self);
    self->growth_upward = growth_upward;
}


/** 空きメモリバイト数を返します
 */
size_t xsalloc_reserve(const XSAlloc* self)
{
    XSALLOC_ASSERT(self);
    return self->end - self->begin;
}


/** ヒープのサイズを返します
 */
static inline size_t
xsalloc_capacity(const XSAlloc* self)
{
    XSALLOC_ASSERT(self);
    return self->capacity;
}


/** ヒープメモリを返します
 */
static inline uint8_t*
xsalloc_heap(const XSAlloc* self)
{
    XSALLOC_ASSERT(self);
    return self->heap;
}


/** 先頭スタックポインタを返します
 */
static inline uint8_t*
xsalloc_bedin(const XSAlloc* self)
{
    XSALLOC_ASSERT(self);
    return self->begin;
}


/** 終端スタックポインタを返します
 */
static inline uint8_t*
xsalloc_end(const XSAlloc* self)
{
    XSALLOC_ASSERT(self);
    return self->end;
}


#ifdef __cplusplus
}
#endif


#endif // picox_allocator_xsalloc_h_