/**
 *       @file  xargparser.h
 *      @brief
 *
 *    @details
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2015年06月14日
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


#ifndef picox_misc_xargparser_h_
#define picox_misc_xargparser_h_


#include <picox/core/xcore.h>


/** @addtogroup misc
 *  @{
 *  @addtogroup xargparser
 *  @brief
 *  @{
 */


#ifdef __cplusplus
extern "C" {
#endif


typedef enum XArgParserErr
{
    X_ARG_PARSER_ERR_NONE,      /** 正常終了 */
    X_ARG_PARSER_ERR_QUATE,     /** クオートが閉じられていない */
    X_ARG_PARSER_ERR_OVERFLOW,  /** argcの最大値を超えた */
    X_ARG_PARSER_ERR_ESCAPE,    /** 不正なエスケープを検出した */
    X_ARG_PARSER_ERR_MEMORY,    /** 一時的なメモリ確保に失敗した */
} XArgParserErr;


/** @brief strを解析してargvを設定します。
 *
 *  @param str      解析文字列
 *  @param argc     引数の数の出力先
 *  @param argv     引数出力先
 *  @param max_argc argcの最大値
 *
 *  @pre
 *  + str != NULL
 *  + argc != NULL
 *  + argv != NULL
 *  + max_argc > 0
 *  + *argvはsizeof(char*) * max_argc以上の領域を持つこと
 *
 *  @note
 *  str自身が解析によってトークンごとに'\0'で分断されます。
 *  リードオンリーの文字列を渡してはいけません。
 */
XArgParserErr xargparser_to_argv(char* str, int* argc, char* argv[], int max_argc);


/** @brief エラーステータスの文字列表現を返します。
 */
const char* xargparser_err_to_string(XArgParserErr err);


#ifdef __cplusplus
}
#endif


/** @} end of addtogroup xargparser
 *  @} end of addtogroup misc
 */


#endif // picox_misc_xargparser_h_
