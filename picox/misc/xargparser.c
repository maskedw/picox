/**
 *       @file  xargparser.c
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

#include <picox/misc/xargparser.h>


XArgParserErr xargparser_to_argv(char* str, int* argc, char** argv, int max_argc)
{
    bool in_token = false;          /* 有効な文字が1文字でもあるかどうか */
    bool in_quart = false;          /* '\"'か'\"'の中にいるかどうか */
    bool escaped = false;           /* '\\'でエスケープ中かどうか */
    bool shuld_add = false;         /* 現在文字をトークンに加えるかどうか */
    bool token_finished = false;    /* トークンが完成したかどうか */
    char quart_start = '\0';        /* '\0' or '\"' or '\'' */
    size_t stoken = 0;              /* 現在のトークンのサイズ */
    size_t argv_start = 0;          /* strからトーケンを切り出し始めた開始位置 */
    char* token = NULL;             /* トークンバッファ */
    XArgParserErr err = X_ARG_PARSER_ERR_NONE;

    X_ASSERT(str);
    X_ASSERT(argc);
    X_ASSERT(argv);
    X_ASSERT(max_argc > 0);

    token = x_malloc(strlen(str) + 1);
    if (! token)
        return X_ARG_PARSER_ERR_MEMORY;

    *argc = 0;
    token[0] = '\0';

    char c;
    int i = 0;

    do {
        c = str[i];
        switch (c) {
        case ' ':
        case '\t':
        case '\n':
        case '\0':
            if (! in_token)
                break;

            if (in_quart) {
                shuld_add = true;
                break;
            }

            if (escaped) {
                escaped = false;
                shuld_add = true;
                break;
            }

            in_token = false;
            token_finished = true;
            break;

        /* クオート文字の処理 */
        case '\'':
        case '\"':
            if (escaped) {
                in_token = true;
                shuld_add = true;
                escaped = false;
                break;
            }

            if (! in_token) {
                in_token = true;
                in_quart = true;
                quart_start = c;
                break;
            }

            if (in_quart) {
                if (c == quart_start) {
                    in_quart = false;
                    in_token = false;
                    token_finished = true;
                    break;
                } else {
                    shuld_add = true;
                    break;
                }
            }

            /*
             * ここまで来たらエラー。
             * トークン中にエスケープされていないクオート文字が来たケースが
             * 考えられる。
             * [例]
             *      hell"o
             */
            err = X_ARG_PARSER_ERR_QUATE;
            goto X__ERROR_EXIT;

        /* エスケープ文字の処理 */
        case '\\':

            /* クオート内では、クオート文字自身のエスケープだけが有効 */
            if (in_quart && str[i+1] != quart_start) {
                shuld_add = true;
                break;
            }

            if (escaped) {
                shuld_add = true;
                break;
            }

            escaped = true;
            break;

        default:
            /*
             * 無効なエスケープシーケンスの場合にどうするかという微妙な問題があ
             * る。これはshellによってふるまいがことなるようだ。
             * bash 4.3.11    g\c\c => gcc
             * nyaos 3.3.9_0  g\c\c => g\c\c
             * このモジュールではbashの振る舞いを採用する。
             * これにはdefault:に来た時はescapedの状態に関わらず常に文字を追
             * 加すればよい。
             */
            escaped = false;
            in_token = true;
            shuld_add = true;
            break;
        } /* swtich (c) */


        if (shuld_add) {
            shuld_add = false;
            if (stoken == 0)
                argv_start = i;

            token[stoken++] = c;
        }

        if (token_finished) {
            token_finished = false;
            if (*argc == max_argc) {
                err = X_ARG_PARSER_ERR_OVERFLOW;
                goto X__ERROR_EXIT;
            }

            /* 引数文字列を上書きする。 トークンサイズは必ず境界を超えないサイズ
             * に収まるはずなのでこれは安全なコピーである。
             */
            token[stoken] = '\0';
            strcpy(&str[argv_start], token);
            argv[*argc] = &str[argv_start];
            (*argc)++;

            /* トークンバッファを初期化する。 */
            token[0] = '\0';
            stoken = 0;
            argv_start = i;
        }
        i++;
    } while (c != '\0');

    /* クオートが閉じられていない */
    if (in_quart) {
        err = X_ARG_PARSER_ERR_QUATE;
    }
    /* escape文字で終了した */
    else if (escaped || in_token) {
        err = X_ARG_PARSER_ERR_ESCAPE;
    }

X__ERROR_EXIT:

    x_free(token);

    return err;
}


const char* xargparser_strerror(XArgParserErr err)
{
    const char* str = NULL;
    switch (err) {
    case X_ARG_PARSER_ERR_NONE:        str = "X_ARG_PARSER_ERR_NONE";        break;
    case X_ARG_PARSER_ERR_QUATE:       str = "X_ARG_PARSER_ERR_QUATE";       break;
    case X_ARG_PARSER_ERR_OVERFLOW:    str = "X_ARG_PARSER_ERR_OVERFLOW";    break;
    case X_ARG_PARSER_ERR_ESCAPE:      str = "X_ARG_PARSER_ERR_ESCAPE";      break;
    case X_ARG_PARSER_ERR_MEMORY:      str = "X_ARG_PARSER_ERR_MEMORY";      break;
    default:                           str = "X_ARG_PARSER_ERR_UNKNOWN";     break;
    }

    return str;
}
