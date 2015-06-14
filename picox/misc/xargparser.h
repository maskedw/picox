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


#ifndef xargparser_h_
#define xargparser_h_


#include <stddef.h>


#ifdef __cplusplus
extern "C" {
#endif


#ifndef X_ARG_PARSER_MAX_TOKEN_SIZE
    #define X_ARG_PARSER_MAX_TOKEN_SIZE 256
#endif


typedef enum XArgParserErr
{
    X_ARG_PARSER_ERR_NONE,

    /* クオートが閉じられていない */
    X_ARG_PARSER_ERR_QUATE,

    /* 規定の最大値を超えた */
    X_ARG_PARSER_ERR_OVERFLOW,

    /* 不正なエスケープ */
    X_ARG_PARSER_ERR_ESCAPE,
} XArgParserErr;


/** strを解析してargvを設定します。
 *
 *  @param str      解析文字列
 *  @param argc     引数の数の出力先
 *  @param argv     引数出力先。max_argc以上の領域があること。
 *  @param max_argc argcの最大値
 *  @param endptr   解析処理時の終了位置(NULL指定時は何もしない)
 *
 *  @note
 *  str自身が解析によってトークンごとに'\0'で分断されることに注意してください。
 *  リードオンリーの文字列を渡してはいけません。
 */
XArgParserErr xargparser_to_argv(char* str, int* argc, char* argv[], size_t max_argc, char** endptr);


const char* xargparser_strerror(XArgParserErr err);


#ifdef __cplusplus
}
#endif


#endif /* xargparser_h_ */
