/**
 *       @file  xpreprocessor.h
 *      @brief  プリプロセッサユーティリティ
 *
 *    @details
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2015/06/27
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


#ifndef picox_core_detail_xpreprocessor_h_
#define picox_core_detail_xpreprocessor_h_


/** @addtogroup core
 *  @{
 *  @addtogroup xpreprocessor
 *  @brief プロプロセッサのユーティリティ機能を提供します
 *  @{
 */


/// @cond IGNORE
#define X_PP_CAT__(a, b)          a ## b
/// @endcond IGNORE


/** @brief トークンa, bを連結します。
 */
#define X_PP_CAT(a, b)            X_PP_CAT__(a, b)


/** @brief トークンa, b, cを連結します。
 */
#define X_PP_CAT3(a,b,c)          X_PP_CAT(X_PP_CAT(a,b),c)


/** @brief トークンa, b, c, dを連結します。
 */
#define X_PP_CAT4(a,b,c,d)        X_PP_CAT(X_PP_CAT3(a,b,c),d)


/** @brief トークンa, b, c, d, eを連結します。
 */
#define X_PP_CAT5(a,b,c,d,e)      X_PP_CAT(X_PP_CAT4(a,b,c,d),e)


/// @cond IGNORE
#define X_PP_STRINGIZE__(text)    #text
/// @endcond IGNORE


/** @brief トークンtextを文字列化します。
 *
 *  @note
 *  X_PP_STRINGIZE__()を通して2段階のマクロを使用しているのは、textにdefineされた
 *  トークンを渡された時に、defineを展開した後の値で文字列化するためです。
 */
#define X_PP_STRINGIZE(text)      X_PP_STRINGIZE__(text)


/** @} end of addtogroup xpreprocessor
 *  @} end of addtogroup core
 */


#endif // picox_core_detail_xpreprocessor_h_
