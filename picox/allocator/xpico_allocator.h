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
    size_t          max_used;
    bool            ownmemory;
} XPicoAllocator;


/** メモリブロックを初期化します
 *
 *  @param heap         heapとして利用するメモリ領域
 *  @param size         heap領域のサイズ
 *  @param alignment    allocatorが返すメモリアドレスのアライメント
 *
 *  heap == NULLの場合はsizeバイトのメモリをx_malloc()で確保します。
 *
 *  @pre
 *  + heapをアライメント調整したあとのサイズ > 0
 *  + alignment == 2のべき乗
 *
 *  @retval true    初期化成功
 *  @retval false   メモリ確保失敗
 *
 *  @note
 *  heapが指すアドレスはalignmentに切り上げられます。そのため、可能であればheap
 *  をあらかじめalignmentに合わせておくと無駄がありません。
 *  また、alocation時にもalignmentに応じた切り上げが行われるので、sizeで指定した
 *  バイト数を余すことなく動的メモリとして使用することはできないことに注意してく
 *  ださい。
 *
 *  alignmentには特殊なアラインメントが必要な時以外はX_ALIGN_OF(XMaxAlign)を指定
 *  しておくのが無難です。例えば文字列等のバイトデータしか扱わないということがわ
 *  かっているなら、アライメントを1とすることができますが、管理データの格納用に
 *  内部的には最低でもX_ALIGN_OF(size_t)まで、切り上げが行われます。
 */
bool xpalloc_init(XPicoAllocator* self, void* heap, size_t size, size_t alignment);


/** オブジェクトの終了処理を行います
 */
void xpalloc_deinit(XPicoAllocator* self);


/** ヒープからsizeバイトのメモリを切り出して返します
 *
 *  @pre
 *  + size > 0
 */
void* xpalloc_allocate(XPicoAllocator* self, size_t size);


/** realloc()相当の処理を行います
 */
void* xpalloc_reallocate(XPicoAllocator* self, void* old_mem, size_t size);



/** ヒープにメモリを返却します
 *
 *  @pre
 *  + xpallloc_is_owner(self, ptr) == true
 *  @note
 *  ptr == NULLの時は何もしません。
 */
void xpalloc_deallocate(XPicoAllocator* self, void* ptr);


/** ヒープを初期状態に戻します
 */
void xpalloc_clear(XPicoAllocator* self);


/** ヒープメモリ自身を返します
 */
uint8_t* xpalloc_heap(const XPicoAllocator* self);


/** 空きメモリバイト数を返します
 */
size_t xpalloc_reserve(const XPicoAllocator* self);


/** ヒープのサイズを返します
 */
size_t xpalloc_capacity(const XPicoAllocator* self);


/** ヒープの最大使用バイト数を返します
 */
size_t xpalloc_max_used(const XPicoAllocator* self);


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


/** ポインタがヒープ領域の範囲内かどうかを返します。
 */
bool xpalloc_is_owner(const XPicoAllocator* self, const void* ptr);


#ifdef __cplusplus
}
#endif


#endif // picox_allocator_xpalloc_h_
