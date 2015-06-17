/**
 *       @file  xtokenizer.h
 *      @brief
 *
 *    @details
 *    文字列を指定の文字で分割し、指定の型に変換するためのインターフェースを備え
 *    たモジュールです。
 *    strtok()をより扱いやすくした感じです。
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

#ifndef xtokenizer_h_
#define xtokenizer_h_


#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>


#ifdef __cplusplus
extern "C" {
#endif


#ifndef XTOK_MAX_NUM_COLS
    /** 最大列数 */
    #define XTOK_MAX_NUM_COLS    10
#endif


#ifndef XTOK_MAX_COL_SIZE
    /** 1列の最大文字数 */
    #define XTOK_MAX_COL_SIZE    128
#endif


#ifndef XTOK_ASSERT
    #define XTOK_ASSERT(expr)   do { if (! expr) for(;;); } while (0)
#endif


typedef struct XTokenizer
{
    char*       tokens[XTOK_MAX_NUM_COLS];
    int         ntokens;
} XTokenizer;


/** 指定されたseparaterで行を分割します。
 *
 *  rowはstd::strtok()よろしく破壊されることに注意してください。
 *  また、XTokenizerオブジェクトのメソッドを呼ぶ間、rowは保持される必要がありま
 *  す。
 */
int xtok_parse_row(XTokenizer* tok, char* row, char separater);


/** 列数を返します。
 */
int xtok_num_tokens(XTokenizer* tok);


/** 指定列の要素を返します。
 *
 *  範囲外が指定された場合はNULLを返します。
 */
const char* xtok_get_token(XTokenizer* tok, int col);


/** 指定列をintに変換して返します。
 */
bool xtok_to_int(XTokenizer* tok, int col, int def, int* dst);


/** 指定列をunsigned intに変換して返します。
 */
bool xtok_to_uint(XTokenizer* tok, int col, unsigned def, unsigned* dst);


/** 指定列をint32_tに変換して返します。
 */
bool xtok_to_int32(XTokenizer* tok, int col, int32_t def, int32_t* dst);


/** 指定列をuint32_tに変換して返します。
 */
bool xtok_to_uint32(XTokenizer* tok, int col, uint32_t def, uint32_t* dst);


/** 指定列をdoubleに変換して返します。
 */
bool xtok_to_double(XTokenizer* tok, int col, double def, double* dst);


/** 指定列をfloatに変換して返します。
 */
bool xtok_to_float(XTokenizer* tok, int col, float def, float* dst);


/** 指定列の文字列を返します。
 *
 *  strip == true時は前後のstd::isspace()でトリムします。
 */
bool xtok_to_string(XTokenizer* tok, int col, const char* def, char* dst, size_t size, bool strip);


/** 指定列をboolに変換して返します。
 *
 *  + y, yes, true, 1 => true
 *  + n, no, false, 0 => false
 */
bool xtok_to_bool(XTokenizer* tok, int col, bool def, bool* dst);


#ifdef __cplusplus
}
#endif


#endif // xtokenizer_h_
