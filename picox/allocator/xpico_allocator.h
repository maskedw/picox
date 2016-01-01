/**
 *       @file  xpico_allocator.h
 *      @brief  Pico variable memory allocator
 *
 *    @details
 *
 *      最低限の機能を備えた可変長メモリアロケータです。最低限の機能というのは、
 *      断片化対策を一切していないという意味です。色々なメモリアロケータのアルゴ
 *      リズム(TLSF, dlmalloc, etc..)では断片化しづらいようにしたり、断片化して
 *      も性能が劣化しないように等、色々と工夫を凝らしているようですが、小規模な
 *      組み込み環境を想定した場合、最低限の機能で十分なことが多いと思われます。
 *
 *      また、復数のヒープを使って機能ごとに専用アロケータを用意する等、柔軟な使
 *      い方ができる。ヒープの使用状況を確認できるインターフェースが用意されてい
 *      るといった点もこのモジュールの利点です。
 *
 *      実装も超シンプルなので、メモリアロケータ実装に興味のある人の入門用にも
 *      ピッタリでしょう。
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


#ifndef picox_allocator_xpico_allocator_h_
#define picox_allocator_xpico_allocator_h_


#include <picox/core/xcore.h>


#ifdef __cplusplus
extern "C" {
#endif


/** 可変長メモリアロケータ管理クラスです
 */
typedef struct XPicoAllocator
{
/// privatesection
    uint8_t*        heap;
    uint8_t*        top;
    size_t          capacity;
    size_t          reserve;
    size_t          alignment;
} XPicoAllocator;


/** メモリブロックを初期化します
 *
 *  @param heap     スタック管理するメモリ領域
 *  @param size     heap領域のサイズ
 *
 *  @pre
 *  + heap != NULL
 *  + size  > 0
 *
 *  @note
 *  heapが指すアドレスはalignmentに切り上げられます。そのため、可能であればheap
 *  をあらかじめalignmentに合わせておくと無駄がありません。
 *  また、alocation時にもalignmentに応じた切り上げが行われるので、sizeで指定した
 *  バイト数を余すことなく動的メモリとして使用することはできないことに注意してく
 *  ださい。
 *
 *  alignmentには特殊なアラインメントが必要な時以外はX_ALIGN_OF(XMaxAlign)を指定
 *  しておくのが無難です。
 */
void xpalloc_init(XPicoAllocator* self, void* heap, size_t size, size_t alignment);


/** ヒープからsizeバイトのメモリを切り出して返します
 */
void* xpalloc_allocate(XPicoAllocator* self, size_t size);


/** ヒープにメモリを返却します
 */
void xpalloc_deallocate(XPicoAllocator* self, void* ptr);


/** ヒープを初期状態に戻します
 */
void xpalloc_clear(XPicoAllocator* self);


/** ヒープメモリを返します
 */
static inline uint8_t*
xpalloc_heap(const XPicoAllocator* self)
{
    X_ASSERT(self);
    return self->heap;
}


/** 空きメモリバイト数を返します
 */
static inline size_t
xpalloc_reserve(const XPicoAllocator* self)
{
    X_ASSERT(self);
    return self->reserve;
}


/** ヒープのサイズを返します
 */
static inline size_t
xpalloc_capacity(const XPicoAllocator* self)
{
    X_ASSERT(self);
    return self->capacity;
}


/** nバイトのメモリ確保を行った場合に必要な余分なメモリサイズを返します。
 */
size_t xpalloc_allocation_overhead(const XPicoAllocator* self, size_t n);


/** ヒープの空きブロック走査用コールバック関数です
 *
 *  @param chunk    空きブロックのポインタ
 *  @param size     空きブロックのサイズ
 *  @param user     ユーザーデータポインタ
 */
typedef void (*XPicoAllocatorWalker)(const uint8_t* chunk, size_t size, void* user);


/** ヒープ内の空きブロックを走査し、ブロックごとにwalkerを呼び出します
 *
 *  デバッグ用です。walkerがデータを収集することで、断片化状況等を確認できます。
 *
 *  @param walker   空きブロック検出毎に呼び出される関数
 *  @param user     walker呼び出し時に渡されるポインタ
 */
void xpalloc_walk_heap(const XPicoAllocator* self, XPicoAllocatorWalker walker, void* user);


#ifdef __cplusplus
}
#endif


#endif // picox_allocator_xpalloc_h_
