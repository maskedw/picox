/**
 *       @file  xstr.h
 *      @brief  C文字列の雑多な便利関数群
 *
 *    @details
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2015/06/14
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


#ifndef picox_string_xstr_h_
#define picox_string_xstr_h_


#include <picox/core/xcore.h>


#ifdef __cplusplus
extern "C" {
#endif


/** s1とs2の比較結果を返します。
 *
 * (std::strcmp(s1, s2) == 0)と同じです。
 */
bool xstr_equal(const char* s1, const char* s2);


/** 大文字小文字の違いを無視してs1, s2を比較した結果を返します。
 */
bool xstr_case_equal(const char* s1, const char* s2);


/** std::strcmp()のラッパーです。
 */
int xstr_compare(const char* s1, const char* s2);


/** 大文字小文字の違いを無視したstd::strcmp()です。
 */
int xstr_case_compare(const char* s1, const char* s2);


/** 大文字小文字の違いを無視したstd::strncmp()です。
 */
int xstr_ncase_compare(const char* s1, const char* s2, size_t n);


/** std::strstrのラッパーです。
 */
char* xstr_search_substring(const char* s1, const char* s2);


/** 大文字小文字の違いを無視したstd::strstrです。
 */
char* xstr_case_search_substring(const char* s1, const char* s2);


/** strdup相当の処理を行います。
 *
 *  不要になった文字列はX_FREE()でメモリを解放してください。
 */
char* xstr_duplicate(const char* str);


/** 指定のメモリ確保関数を使用してxstr_duplicate()を実行します。
 *
 *  不要になった文字列はmalloc_func()に対応するメモリ解放関数でメモリを解放して
 *  ください。
 */
char* xstr_duplicate2(const char* str, void* (*malloc_func)(size_t));


/** strndup相当の処理を行います。
 *
 *  不要になった文字列はX_FREE()でメモリを解放してください。
 */
char* xstr_nduplicate(const char* str, size_t n);


/** 指定のメモリ確保関数を使用してxstr_nduplicate()を実行します。
 *
 *  不要になった文字列はmalloc_func()に対応するメモリ解放関数でメモリを解放して
 *  ください。
 */
char* xstr_nduplicate2(const char* str, size_t n, void* (*malloc_func)(size_t));


/** 文字列を逆転させた結果を返します。
 *
 *  str自身が逆転されることに注意してください。戻り値はstrと同じです。
 */
char* xstr_reverse(char* str);


/** 文字列の前後からspaceに含まれる文字列を取り除いた結果を返します。
 *
 *  space == NULLの場合はstd::isspace()が使用されます。
 */
char* xstr_strip(char* str, const char* space);


/** 文字列の前からspaceに含まれる文字列を取り除いた結果を返します。
 *
 *  space == NULLの場合はstd::isspace()が使用されます。
 */
char* xstr_strip_left(char* str, const char* space);


/** 文字列の後ろからspaceに含まれる文字列を取り除いた結果を返します。
 *
 *  space == NULLの場合はstd::isspace()が使用されます。
 */
char* xstr_strip_right(char* str, const char* space);


/** 文字列をintに変換して返します。
 */
bool xstr_to_int(const char* str, int* dst, int def);


/** 文字列をunsigned intに変換して返します。
 */
bool xstr_to_uint(const char* str, unsigned* dst, unsigned int def);


/** 文字列をint32_tに変換して返します。
 */
bool xstr_to_int32(const char* str, int32_t* dst, int32_t def);


/** 文字列をuint32_tに変換して返します。
 */
bool xstr_to_uint32(const char* str, uint32_t* dst, uint32_t def);


/** 文字列をdoubleに変換して返します。
 */
bool xstr_to_double(const char* str, double* dst, double def);


/** 文字列をfloatに変換して返します。
 */
bool xstr_to_float(const char* str, float* dst, float);


/** 文字列をboolに変換して返します。
 *
 *  + y, yes, true, 1 => true
 *  + n, no, false, 0 => false
 */
bool xstr_to_bool(const char* str, bool* dst, bool def);


#ifdef __cplusplus
}
#endif


#endif // picox_string_xstr_h_
