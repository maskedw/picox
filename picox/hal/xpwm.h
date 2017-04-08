/**
 *       @file  xpwm.h
 *      @brief
 *
 *    @details
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2016/07/09
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


#ifndef picox_hal_xpwm_h_
#define picox_hal_xpwm_h_


#include <picox/core/xcore.h>


/** @addtogroup hal
 *  @{
 *  @addtogroup  xpwm
 *  @brief PWMの抽象化レイヤです
 *  @{
 */


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @def   X_PWM_DUTY_SCALE
 *  @brief PWMデューティ比に掛ける係数です
 *
 *  デューティ比は0~100で表されることが多いですが、もっと細かい単位
 *  (例 0 ~ 10000)で扱いたいこともあります。
 *  この設定値の調整と、呼び出し側で適切に係数を取り扱うことで、デューティ比の範
 *  囲を自由に設定することができます。
 */
#ifndef X_CONF_PWM_DUTY_SCALE
    #define XPWM_DUTY_SCALE (1)
#else
    #define XPWM_DUTY_SCALE X_CONF_PWM_DUTY_SCALE
#endif

#define XPWM_DUTY_SCALED(duty)      ((duty) * XPWM_DUTY_SCALE)
#define XPWM_DUTY_UNSCALED(duty)    ((duty) / XPWM_DUTY_SCALE)
#define XPWM_DUTY_MAX               (100UL * XPWM_DUTY_SCALE)


/** @name HAL PWM virtual functions
 *
 *  インターフェースの実装者は、要求されたHALインタフェースを満たす必要がありま
 *  @{
 */


/** @brief PWM出力開始インターフェースです
 *
 *  + freq_hz以下の周波数とdutyでPWM出力を行う
 */
typedef void (*XPwmWriteFunc)(void* driver, uint32_t freq_hz, uint16_t duty);


/** @} end of name HAL PWM virtual functions
 */


/** @brief 仮想PWMインターフェースのvtableです
 */
typedef struct XPwmVTable
{
    XPwmWriteFunc       m_write_func;
} XPwmVTable;


/** @brief 仮想PWMを表すインターフェース型です
 */
typedef struct XPwmTag
{
    X_DECLEAR_RTTI(XPwmVTable);
} XPwm;


/** @brief 仮想PWMインターフェースを初期値に設定します
 */
void xpwm_init(XPwm* self);


/** @brief 指定の周波数、ディーティ比のPWM出力を開始します
 */
void xpwm_write(XPwm* self, uint32_t freq_hz, uint16_t duty);


/** @brief PWM出力を停止します
 */
void xpwm_stop(XPwm* self);


/** @brief PWMピンをHIGHレベルに設定します
 */
void xpwm_set_high(XPwm* self);


/** @brief PWMピンをLOWレベルに設定します
 */
void xpwm_set_low(XPwm* self);


#ifdef __cplusplus
}
#endif /* __cplusplus */


/** @} end of addtogroup xpwm
 *  @} end of addtogroup hal
 */

#endif /* picox_hal_xpwm_h_ */
