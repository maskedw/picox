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


/** 文字列s1とs2が一致するかどうかを返します。
 *
 * (std::strcmp(s1, s2) == 0)と同じです。
 */
bool xstr_equal(const char* s1, const char* s2);


/** 大文字小文字の違いを無視して文字列s1, s2が一致するかどうかを返します。
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
 *  str自身が破壊されることに注意してください。
 */
char* xstr_reverse(char* str);


/** 文字列の前後からspaceに含まれる文字列を取り除いた結果を返します。
 *
 *  + str自身が破壊されることに注意してください。
 *  + space == NULLの場合はstd::isspace()が使用されます。
 */
char* xstr_strip(char* str, const char* space);


/** 文字列の前からspaceに含まれる文字列を取り除いた結果を返します。
 *
 *  + str自身が破壊されることに注意してください。
 *  + space == NULLの場合はstd::isspace()が使用されます。
 */
char* xstr_strip_left(char* str, const char* space);


/** 文字列の後ろからspaceに含まれる文字列を取り除いた結果を返します。
 *
 *  + str自身が破壊されることに注意してください。
 *  + space == NULLの場合はstd::isspace()が使用されます。
 */
char* xstr_strip_right(char* str, const char* space);


/** 文字列をint32_tに変換して返します。
 *
 *  + 文字列先頭の任意の数の空白(std::isspace())は無視されます。
 *  + +-の符号は数字の直前のみ許可されます。
 *  + 数字直前の"0[xX]"は16進数に、"0[bB]"は2進数に解釈されます。
 *  + 16進数、2進数解釈時に+-符号がついていた場合は変換失敗とします。
 *  + okにNULLが渡されることは許容します。
 *
 *  @param str 変換対象の文字列
 *  @param def 変換に失敗した場合に返すデフォルト値
 *  @param ok  変換に成功したかどうか
 */
int32_t xstr_to_int32(const char* str, int32_t def, bool* ok);


/** 文字列をuint32_tに変換して返します。
 *
 *  + -の符号がついていた場合は常時変換失敗となります。
 *  + その他の条件はxstr_to_int32()を参照してください。
 */
uint32_t xstr_to_uint32(const char* str, uint32_t def, bool* ok);


/** 文字列をfloatに変換して返します。
 *
 *  c99のstd::strtof()が使用できない場合はstd::strtod()の結果を返します。
 *
 *  @TODO
 *  ライブラリの実装によって、不正な文字列を渡したときの振る舞いが曖昧なので、
 *  [+-.0-9]以外の文字があったら不正ってことにしよか。
 */
float xstr_to_float(const char* str, float def, bool* ok);


/** 文字列をdoubleに変換して返します。
 *
 *  std::strtod()の結果を返します。
 */
double xstr_to_double(const char* str, double def, bool* ok);


/** 文字列をboolに変換して返します。
 *
 *  大文字小文字の違いは無視し、("y", "yes", "true", "1")のいづれかであればtrue。
 *  ("n", "no", "false", "0")のいづれかであればfalseと解釈します。
 */
bool xstr_to_bool(const char* str, bool def, bool* ok);


#ifdef __cplusplus
}
#endif


#endif // picox_string_xstr_h_
