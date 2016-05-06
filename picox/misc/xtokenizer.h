/**
 *       @file  xtokenizer.h
 *      @brief
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

#ifndef picox_misc_xtokenizer_h_
#define picox_misc_xtokenizer_h_


#include <picox/core/xcore.h>


/** @addtogroup misc
 *  @{
 *  @addtogroup xtokenizer
 *  @brief 文字列のトークン化
 *
 *  文字列を指定の文字で分割し、指定の型に変換するためのインターフェースを備え
 *  たモジュールです。
 *  strtok()をより扱いやすくした感じです。
 *  @{
 */


#ifdef __cplusplus
extern "C" {
#endif


/** @brief トークン化した文字列の管理構造体
 */
typedef struct XTokenizer
{
/// @privatesection
    char*       row;
    char**      tokens;
    int         ntokens;
} XTokenizer;


/** @brief 文字列を指定文字で列に分解します。
 *
 *  文字列はオブジェクトにコピーされます。初期化後はxtok_release()で必ずリソース
 *  を解放させてください。
 *
 *  @param row          行文字列
 *  @param separater    区切り文字
 *  @param max_tokens   最大列数
 *  @retval false
 *      + メモリ確保失敗
 *      + 最大列数を超えた
 *
 *  @pre
 *  + row != NULL
 *  + max_tokens > 0
 */
bool xtok_init(XTokenizer* self, const char* row, char separater, int max_tokens);


/** @brief オブジェクトが保持するリソースを解放します。
 *
 *  @note
 *  xtok_init()で失敗後の呼び出しでも安全に動作します。
 */
void xtok_release(XTokenizer* self);


/** @brief 列を参照します
 *
 *  @pre
 *  col <= (xtok_num_tokens() - 1)
 */
static inline const char*
xtok_ref_token(const XTokenizer* self, int col)
{
    X_ASSERT(self);
    X_ASSERT(x_is_within(col, 0, self->ntokens));
    return self->tokens[col];
}


/** @brief 列数を返します
 */
static inline int
xtok_num_tokens(const XTokenizer* self)
{
    X_ASSERT(self);
    return self->ntokens;
}

#ifdef __cplusplus
}
#endif


/** @} end of addtogroup xtokenizer
 *  @} end of addtogroup misc
 */


#endif // picox_misc_xtokenizer_h_
