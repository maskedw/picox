/**
 *       @file  xdynamic_string.h
 *      @brief  動的文字列モジュール
 *
 *    @details
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2016/01/03
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


#ifndef picox_string_xdynamic_string_h_
#define picox_string_xdynamic_string_h_


#include <picox/core/xcore.h>


/** @addtogroup string
 *  @{
 *  @addtogroup xdynamic_string
 *  @brief 動的文字列モジュール
 *  @{
 */


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


struct XDynamicString;
typedef struct XDynamicString XDynamicString;


/** @brief srcをコピーした文字列を生成して返します
 */
XDynamicString* xdstr_create(const char* src);


/** @brief srcからlenバイトをコピーした文字列を生成して返します
 */
XDynamicString* xdstr_create_length(const char* src, size_t len);


/** @brief 長さ0の文字列を生成して返します
 */
XDynamicString* xdstr_create_empty(void);


/** @brief 文字列のリソースを開放します
 */
void xdstr_destroy(XDynamicString* self);


/** @brief 文字列のコピーを生成して返します
 */
XDynamicString* xdstr_clone(const XDynamicString* self);


/** @brief 文字列末尾にstrを連結して返します
 */
XDynamicString* xdstr_cat(XDynamicString* self, const char* str);


/** @brief 文字列末尾にstrから最大len文字を連結して返します
 */
XDynamicString* xdstr_cat_n(XDynamicString* self, const char* str, size_t len);


/** @brief 文字列末尾にprintf形式で文字列を連結して返します
 */
XDynamicString* xdstr_cat_printf(XDynamicString* self, const char *fmt, ...);


/** @brief 文字列末尾にvprintf形式で文字列を連結して返します
 */
XDynamicString* xdstr_cat_vprintf(XDynamicString* self, const char *fmt, va_list args);


/** @brief 文字列にstrをコピーして返します
 */
XDynamicString* xdstr_copy(XDynamicString* self, const char* str);


/** @brief 文字列にstrから最大len文字をコピーして返します
 */
XDynamicString* xdstr_copy_n(XDynamicString* self, const char* str, size_t len);


/** @brief 文字列からchar_setに含まれる各文字を除去します
 */
void xdstr_trim(XDynamicString* self, const char* char_set);


/** @brief 文字列からfirst ~ lastまで(last自身も含む)の範囲外の部分を除去します
 *
 *  負の値を指定した場合、-1なら文字列終端。-2は文字列終端 - 1という具合に
 *  末尾からのインデックスを意味します。
 *
 *  @code
 *
 *  s = xdstr_create("Hello World");
 *  xdstr_range(s,1,-1); => "ello World"
 *  @endcode
 */
void xdstr_range(XDynamicString* self, int first, int last);


/** @brief 文字列のアルファベットを小文字に変換します
 */
void xdstr_to_lower(XDynamicString* self);


/** @brief 文字列のアルファベットを大文字に変換します
 */
void xdstr_to_upper(XDynamicString* self);


/** @brief 文字列の長さを返します
 */
size_t xdstr_length(const XDynamicString* self);


/** @brief 文字列に格納できるバイト数を返します
 */
size_t xdstr_capacity(const XDynamicString* self);


/** @brief 文字列のconst char*を返します
 */
const char* xdstr_c_str(const XDynamicString* self);


/** @brief ストレージサイズを文字列長がちょうど収まるサイズにまで縮小して返します
 */
XDynamicString* xdstr_shrink_to_fit(XDynamicString* self);


/** @brief ストレージをsizeバイトまで(末尾のnullバイトを除く)格納できるサイズまで拡張して返します
 *
 *  すでにストレージサイズがsizeバイト以上であれば自身を返します。
 */
XDynamicString* xdstr_reserve(XDynamicString* self, size_t size);


#ifdef __cplusplus
}
#endif /* __cplusplus */


/** @} end of addtogroup xdynamic_string
 *  @} end of addtogroup string
 */


#endif /* picox_string_xdynamic_string_h_ */
