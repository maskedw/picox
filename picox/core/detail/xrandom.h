/**
 *       @file  xrandom.h
 *      @brief  擬似乱数生成定義
 *
 *    @details
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2016/03/13
 * ===================================================================
 */

/*
 * License: MIT license
 * Copyright (c) <2016> <MaskedW [maskedw00@gmail.com]>
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


#ifndef picox_core_detail_xrandom_h_
#define picox_core_detail_xrandom_h_


/** @addtogroup core
 *  @{
 *  @addtogroup xrandom
 *  @brief 擬似乱数値の取得機能を提供します
 *
 *  標準ライブラリはrand()が返す乱数の品質は実装に依存するため、独自の乱数生成機
 *  能を提供しています。picoxでは実行速度と品質のバランスがいいXorshiftアルゴリ
 *  ズムを採用しています。
 *  @{
 */


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @brief randが生成する擬似乱数の種を設定します
 */
void x_srand(uint32_t seed);


/** @brief 0 ~ UINT8_MAXの範囲の擬似乱数を返します
 */
uint8_t x_rand8(void);


/** @brief min ~ maxの範囲の擬似乱数を返します
 */
uint8_t x_randrange8(uint8_t min, uint8_t max);


/** @brief 0 ~ UINT16_MAXの範囲の擬似乱数を返します
 */
uint16_t x_rand16(void);


/** @brief min ~ maxの範囲の擬似乱数を返します
 */
uint16_t x_randrange16(uint16_t min, uint16_t max);


/** @brief 0 ~ UINT32_MAXの範囲の擬似乱数を返します
 */
uint32_t x_rand32(void);


/** @brief min ~ maxの範囲の擬似乱数を返します
 */
uint32_t x_randrange32(uint32_t min, uint32_t max);


/** @brief 0 ~ UINT_MAXの範囲の擬似乱数を返します
 */
unsigned x_rand(void);


/** @brief min ~ maxの範囲の擬似乱数を返します
 */
unsigned x_randrange(unsigned min, unsigned max);


#ifdef __cplusplus
}
#endif /* __cplusplus */


/** @} end of addtogroup xrandom
 *  @} end of addtogroup core
 */


#endif /* picox_core_detail_xrandom_h_ */
