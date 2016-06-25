/**
 *       @file  xstack_allocator.h
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


#ifndef picox_allocator_xstack_allocator_h_
#define picox_allocator_xstack_allocator_h_


#include <picox/core/xcore.h>


#ifdef __cplusplus
extern "C" {
#endif


/** スタックメモリアロケータ管理クラスです
 */
typedef struct XStackAllocator
{
/// privatesection
    uint8_t*    heap;
    uint8_t*    begin;
    uint8_t*    end;
    size_t      capacity;
    size_t      alignment;
    bool        growth_upward;
} XStackAllocator;


/** メモリブロックを初期化します
 *
 *  @param heap         スタック管理するメモリ領域
 *  @param size         heap領域のサイズ
 *  @param alignment    メモリ確保のアライメント
 *
 *  @pre
 *  + heap != NULL
 *  + size  >= alignment
 *  + alignment == 2のべき乗
 *
 *  @note
 *  heap領域はalignmentに切り上げられ、さらに、sizeは、alignmentに切り下げられま
 *  す。1バイトも無駄にしたくない場合は、heapをあらかじめアラインして確保し、
 *  sizeはalignmentの倍数にしてください。
 */
void xsalloc_init(XStackAllocator* self, void* heap, size_t size, size_t alignment);


/** ヒープからsizeバイトのメモリを切り出して返します
 *
 *  @note
 *  確保するメモリサイズはalignmentに切り上げられます。
 */
void* xsalloc_allocate(XStackAllocator* self, size_t size);


/** ヒープを初期状態に戻します
 */
void xsalloc_clear(XStackAllocator* self);


/** begin, endの位置にスタックポインタを移動します
 *  @pre
 *  + end >= begin
 *  + begin, endはこのオブジェクトのheap領域を指していること。
 *  + begin, endはalignmentの倍数であること。
 */
void xsalloc_rewind(XStackAllocator* self, void* begin, void* end);


/** スタック伸長方向を返します
 */
bool xsalloc_growth_direction(XStackAllocator* self);


/** スタック伸長方向を設定し、変更前の値を返します
 */
bool xsalloc_set_growth_direction(XStackAllocator* self, bool growth_upward);


/** 空きメモリバイト数を返します
 */
size_t xsalloc_reserve(const XStackAllocator* self);


/** ヒープのサイズを返します
 */
size_t xsalloc_capacity(const XStackAllocator* self);


/** 初期化時に指定したアラインメントを返します
 */
size_t xsalloc_alignment(const XStackAllocator* self);


/** ヒープメモリを返します
 */
uint8_t* xsalloc_heap(const XStackAllocator* self);


/** 先頭スタックポインタを返します
 */
uint8_t* xsalloc_bedin(const XStackAllocator* self);


/** 終端スタックポインタを返します
 */
uint8_t* xsalloc_end(const XStackAllocator* self);


#ifdef __cplusplus
}
#endif


#endif // picox_allocator_xsalloc_h_
