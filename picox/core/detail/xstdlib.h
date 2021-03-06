/**
 *       @file  xstdlib.h
 *      @brief  主にmalloc系関数の定義
 *
 *    @details
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2015/06/28
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


#ifndef picox_core_detail_xstdlib_h_
#define picox_core_detail_xstdlib_h_


/** @addtogroup core
 *  @{
 *  @addtogroup xstdlib
 *  @brief 主にmalloc系の機能を提供します
 *  @{
 */


#ifdef __cplusplus
extern "C" {
#endif


/** @brief sizeバイトのメモリを割り当てて返します
 */
void* x_malloc(size_t size);


/** @brief ptrが指すメモリ空間を開放します
 */
void x_free(void* ptr);


/** @brief sizeバイトの要素nmemb個からなる配列にメモリを割り当て0初期化して返します
 */
void* x_calloc(size_t nmemb, size_t size);


/** @brief sizeバイトのメモリを割り当て、0初期化して返します
 *
 *  calloc()のaバイトの要素b個割り当てるというインターフェースは、内部実装によっ
 *  ては最適化の恩恵があるのだと思いますが、picoxの実装としては何も意味がないの
 *  で、単に0初期化した動的メモリ確保を行いたいだけなら、こちらの方がシンプルで
 *  す。
 */
void* x_calloc2(size_t size);


/** @brief old_memが指すメモリブロックをsizeバイトに再割当てして返します
 */
void* x_realloc(void *old_mem, size_t size);


/** @brief old_memが指すold_sizeバイトのメモリブロックをnew_sizeバイトに再割当てして返します
 *
 *  mallocを直接実装している場合は、old_memのポインタから、直接old_sizeを取り出
 *  すことができますが、そうではない場合サイズを取得することができないため、無駄
 *  なコピーが必要になる場合があります。
 *
 *  普通、メモリの再割当てを行う場合、元のサイズは呼び出し側がわかっていることが
 *  多いため、引数として渡してもらうことで無駄なコピーを防ぎます。
 */
void* x_realloc2(void *old_mem, size_t old_size, size_t new_size);


/** @brief x_free()を呼び出したあと、ptrにNULLを代入します
 *
 *  無効なポインタの不正使用によるバグは、C言語で最も避けたいわかりづらいバグの
 *  一つです。開放したポインタにはNULLをセットしておいて不正使用時は即死するよう
 *  にしておくのは良い習慣です。
 */
#define X_SAFE_FREE(ptr)  (x_free((ptr)), (ptr) = NULL)


#ifdef __cplusplus
}
#endif


/** @} end of addtogroup xstdlib
 *  @} end of addtogroup core
 */


#endif // picox_core_detail_xstdlib_h_
