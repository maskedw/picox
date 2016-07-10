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


/** @brief PWM出力の極性を表します
 *
 *  LOWアクティブの時にディーティ比が70%であれば、70%がLOW出力で30%が
 *  HIGH出力です。停止時はアイドル状態になるのでHIGHになります。
 */
typedef enum XPwmPolarityTag
{
    /** LOWアクティブ */
    XPWM_POLARITY_ACTIVE_LOW,

    /** HIGHアクティブ */
    XPWM_POLARITY_ACTIVE_HIGH,
} XPwmPolarity;


/** @brief PWMのコンフィグレーション構造体です
 */
typedef struct XPwmConfigTag
{
    uint32_t        frequency;
    uint16_t        duty_x100; /** ディーティ比(0~100%) x 100 (0 ~ 10000) */
    XPwmPolarity    polarity;
} XPwmConfig;


/** @name HAL PWM virtual functions
 *
 *  ユーザはHALが要求するインターフェースを満たす必要があります。
 *  @{
 */

/** @see xpwm_configure */
typedef XError(*XPwmConfigureFunc)(void* driver, const XPwmConfig* config);

/** @see xpwm_start */
typedef void(*XPwmStartFunc)(void* driver);

/** @see xpwm_stop */
typedef void(*XPwmStopFunc)(void* driver);


/** @} end of name HAL PWM virtual functions
 */


/** @brief 仮想PWMインターフェース構造体です
 *
 *  インターフェースを満たす関数はユーザーが用意し、セットする必要があります。
 */
typedef struct XPwmTag
{
    void*               driver;
    XPwmConfigureFunc   configure_func;
    XPwmStartFunc       start_func;
    XPwmStopFunc        stop_func;
} XPwm;


/** @brief 仮想PWMインターフェースを初期値に設定します
 */
void xpwm_init(XPwm* pwm);


/** @brief コンフィグオブジェクトを初期値に設定します
 */
void xpwm_config_init(XPwmConfig* config);


/** @brief PWMの設定を変更します
 */
XError xpwm_configure(XPwm* pwm, const XPwmConfig* config);


/** @brief PWM出力を開始します
 */
void xpwm_start(XPwm* pwm);


/** @brief PWM出力を停止します
 */
void xpwm_stop(XPwm* pwm);


#ifdef __cplusplus
}
#endif /* __cplusplus */


/** @} end of addtogroup xpwm
 *  @} end of addtogroup hal
 */


#endif /* picox_hal_xpwm_h_ */
