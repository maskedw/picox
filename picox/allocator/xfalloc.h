/**
 *       @file  xfalloc.c
 *      @brief  Fixed memory allocator
 *
 *    @details
 *
 *    メモリを固定サイズのブロックに分割してメモリアロケーションを行います。
 *    通常のmallocを使用した場合との主な違いは以下の通りです。
 *
 *    ------------------------------------------------------------
 *
 *    [利点]
 *
 *    1. 超高速
 *
 *    一般的に可変長メモリアロケータのメモリ確保と解放にかかる時間は不定です。固
 *    定メモリブロックでは一定かつ、ほぼ1オペレーションで処理が完了します。
 *
 *    2. 余分なヘッダ領域を一切使用しない
 *
 *    一般的な可変長メモリ確保では、例えば指定サイズのメモリにヘッダ情報の格納用
 *    に4~16バイト程度追加で確保されます。このアロケータではヘッダ領域を一切使用
 *    しません。
 *
 *    3. 絶対に断片化しない
 *
 *    固定サイズのブロックなので当然ですが、何度メモリ確保と解放を繰り返しても断
 *    片化しません。
 *
 *    ------------------------------------------------------------
 *
 *    [欠点]
 *
 *    1. 確保サイズにバラつきがあると無駄が多い
 *
 *    ------------------------------------------------------------
 *
 *    [利用例]
 *
 *    + 確保するメモリサイズが固定の部分で使用する。
 *
 *    + 小サイズ(1~32バイト程度)メモリ確保用の高速アロケータとして使用する。
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


#ifndef picox_allocator_xfalloc_h_
#define picox_allocator_xfalloc_h_


#include <picox/core/xcore.h>


#ifdef __cplusplus
extern "C" {
#endif


/** 固定メモリアロケータ管理クラスです
 */
typedef struct XFAlloc
{
/// privatesection
    uint8_t*    heap;
    uint8_t*    top;
    uint8_t*    next;
    size_t      block_size;
    size_t      num_blocks;
    size_t      remain_blocks;
    size_t      alignment;
} XFAlloc;


#define XFALLOC_MIN_ALIGNMENT   (X_ALIGN_OF(size_t))


/** メモリブロックを初期化します
 *
 *  @param heap         ブロック化するメモリ領域
 *  @param heap_size    heap領域のバイト数
 *  @param block_size   1ブロックのサイズ
 *  @param alignment    ブロックのアラインメント
 *
 *  @pre
 *  + heap != NULL
 *  + heap_size  > 0
 *  + block_size > 0
 *  + alignment >= X_ALIGN_OF(size_t)
 *  + alignment == 2のべき乗
 *
 *  @attention
 *  heapが指すアドレスと、block_sizeはアライメントに切り上げられます。
 *  その結果、意図したブロック分けができていない場合がありますので、はじめからア
 *  ラインメントを意識した引数を用意するか初期化後に、必要量を満たせているか確認
 *  した方がよいでしょう。
 */
void xfalloc_init(XFAlloc* self, void* heap, size_t heap_size, size_t block_size, size_t alignment);


/** ヒープから1ブロックを取り出して返します
 */
void* xfalloc_allocate(XFAlloc* self);


/** ヒープにブロックを返却します
 *
 *  ptrがNULLの時は何もしません。
 */
void xfalloc_deallocate(XFAlloc* self, void* ptr);


/** ヒープを初期状態に戻します
 */
void xfalloc_clear(XFAlloc* self);


/** ヒープメモリを返します
 */
static inline void*
xfalloc_heap(const XFAlloc* self)
{
    X_ASSERT(self);
    return self->heap;
}


/** 1ブロックのサイズを返します
 */
static inline size_t
xfalloc_block_size(const XFAlloc* self)
{
    X_ASSERT(self);
    return self->block_size;
}


/** heapの総ブロック数を返します
 */
static inline size_t
xfalloc_num_blocks(const XFAlloc* self)
{
    X_ASSERT(self);
    return self->num_blocks;
}


/** heapの残りブロック数を返します
 */
static inline size_t
xfalloc_remain_blocks(const XFAlloc* self)
{
    X_ASSERT(self);
    return self->remain_blocks;
}


/** ブロックのアラインメントを返します
 */
static inline size_t
xfalloc_alignment(const XFAlloc* self)
{
    X_ASSERT(self);
    return self->alignment;
}



#ifdef __cplusplus
}
#endif


#endif // picox_allocator_xfalloc_h_
