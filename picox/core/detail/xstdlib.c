/**
 *       @file  xstdlib.c
 *      @brief  主にmalloc系関数の定義
 *
 *    @details
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2016/06/20
 * ===================================================================
 */

/*
 * License: MIT license
 * Copyright (c) <2016> <MaskedW [maskedw00@gmail.com]>
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

#include <picox/core/xcore.h>



void* x_malloc(size_t size)
{
    void* ptr;
    if (size == 0)
        return NULL;
    ptr = X_CONF_MALLOC(size);
    X_ASSERT_MALLOC_NULL(ptr);
    X_UNUSED(ptr);

    return ptr;
}


/** @brief ptrが指すメモリ空間を開放します
 */
void x_free(void* ptr)
{
    /* 規格上、freeにNULLを渡してもOKなはずなのだが、実装によっては微妙にグレー
     * だったりするのでここでもNULLチェックしておく
     */
    if (ptr)
        X_CONF_FREE(ptr);
}


/** @brief sizeバイトの要素nmemb個からなる配列にメモリを割り当て0初期化して返します
 */
void* x_calloc(size_t nmemb, size_t size)
{
    void* ptr;
    size = nmemb * size;
    ptr = x_malloc(size);
    if (ptr)
        memset(ptr, 0, size);
    return ptr;
}


/** @brief sizeバイトのメモリを割り当て、0初期化して返します
 *
 *  calloc()のaバイトの要素b個割り当てるというインターフェースは、内部実装によっ
 *  ては最適化の恩恵があるのだと思いますが、picoxの実装としては何も意味がないの
 *  で、単に0初期化した動的メモリ確保を行いたいだけなら、こちらの方がシンプルで
 *  す。
 */
void* x_calloc2(size_t size)
{
    void* const ptr = x_malloc(size);
    if (ptr)
        memset(ptr, 0, size);
    return ptr;
}


/** @brief old_memが指すメモリブロックをsizeバイトに再割当てして返します
 */
void* x_realloc(void *old_mem, size_t size)
{
    void* const new_mem = x_malloc(size);
    if (!new_mem)
        return NULL;

    if (!old_mem)
        return new_mem;

    memcpy(new_mem, old_mem, size);
    x_free(old_mem);

    return new_mem;
}


/** @brief old_memが指すold_sizeバイトのメモリブロックをnew_sizeバイトに再割当てして返します
 *
 *  mallocを直接実装している場合は、old_memのポインタから、直接old_sizeを取り出
 *  すことができますが、そうではない場合サイズを取得することができないため、無駄
 *  なコピーが必要になる場合があります。
 *
 *  普通、メモリの再割当てを行う場合、元のサイズは呼び出し側がわかっていることが
 *  多いため、引数として渡してもらうことで無駄なコピーを防ぎます。
 */
void* x_realloc2(void *old_mem, size_t old_size, size_t new_size)
{
    void* new_mem;
    if (old_size == new_size)
        return old_mem;

    new_mem = x_malloc(new_size);
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

    x_free(old_mem);

    return new_mem;
}


void x_null_deleter(void* ptr)
{
    (void)(ptr);
}
