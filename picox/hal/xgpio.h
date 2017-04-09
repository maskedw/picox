/**
 *       @file  xgpio.h
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


#ifndef picox_hal_xgpio_h_
#define picox_hal_xgpio_h_


#include <picox/core/xcore.h>


/** @addtogroup hal
 *  @{
 *  @addtogroup  xgpio
 *  @brief GPIOの抽象化レイヤです
 *  @{
 */


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** ピンの信号レベルLOWを表します */
#define XGPIO_LOW       (false)

/** ピンの信号レベルHIGHを表します */
#define XGPIO_HIGH      (true)


/** @brief GPIOのモードを表します
 */
typedef enum XGpioMode
{
    XGPIO_MODE_INPUT_PULLUP,       /** 入力モードプルアップ */
    XGPIO_MODE_INPUT_PULLDOWN,     /** 入力モードプルダウン */
    XGPIO_MODE_INPUT_PULLNONE,     /** 入力モードプルアップ・ダウンなし */
    XGPIO_MODE_OUTPUT_PUSHPULL,    /** 出力モード プッシュプル出力 */
    XGPIO_MODE_OUTPUT_OPENDRAIN,   /** 出力モード オープンドレイン出力 */
} XGpioMode;


/** @brief GPIOの信号変化エッジ方向を表します
 */
typedef enum XGpioEdge
{
    XGPIO_EDGE_FALLING, /** 立ち下がり */
    XGPIO_EDGE_RISING,  /** 立ち上がり */
    XGPIO_EDGE_BOTH,    /** 立ち下がり + 立ち上がり */
} XGpioEdge;



/** GPIO割り込みハンドラの型です */
typedef void (*XGpioIrqHandler)(void* arg);


/** @name HAL GPIO virtual functions
 *
 *  インターフェースの実装者は、要求されたHALインタフェースを満たす必要がありま
 *  @{
 */


/** @brief GPIO設定のインターフェースです
 *
 *  指定の設定を試み、設定できなければX_ERR_NOT_SUPPORTEDを返してください
 */
typedef XError(*XGpioSetModeFunc)(void* driver, XGpioMode mode);


/** @brief GPIO入力レベルの読み出しインターフェースです
 *
 *  入力レベルがLOWならXGPIO_LOWを、HIGHならXGPIO_HIGHを返してください。
 */
typedef bool (*XGpioReadFunc)(const void* driver);


/** @brief GPIO出力レベルの書き出しインターフェースです
 *
 *  valueがXGPIO_LOWならLOWを、XGPIO_HIGHならHIGHレベルに設定してください。
 */
typedef void (*XGpioWriteFunc)(void* driver, bool value);


/** @brief GPIO割り込みハンドラの登録インターフェースです
 *
 *  + handlerとhandler_argは立ち下がりと立ち上がりを別々に登録できるようにしてください。
 *  + 設定処理はIRQセーフに実装してください。
 */
typedef XError (*XGpioSetIrqHandler)(void* driver, XGpioEdge edge, XGpioIrqHandler handler, void* handler_arg);


/** @brief GPIO割込みの有効無効設定インターフェースです
 *
 *  enabled == falseなら無効に、!= falseなら有効に設定してください。
 */
typedef XError (*XGpioSetIrqEnabled)(void* driver, bool enabled);


/** @} end of name HAL GPIO virtual functions
 */


/** @brief 仮想GPIOインターフェースのvtableです
 */
typedef struct XGpioVTable
{
    XGpioSetModeFunc    m_set_mode_func;
    XGpioReadFunc       m_read_func;
    XGpioWriteFunc      m_write_func;
    XGpioSetIrqHandler  m_set_irq_handler_func;
    XGpioSetIrqEnabled  m_set_irq_enabled_func;
} XGpioVTable;


/** @brief 仮想GPIOインターフェース構造体です
 *
 *  インターフェースを満たす関数はユーザーが用意し、セットする必要があります。
 */
typedef struct XGpio
{
    X_DECLEAR_RTTI(XGpioVTable);
} XGpio;


/** @brief 仮想GPIOインターフェースを初期値に設定します
 */
void xgpio_init(XGpio* self);


/** @brief GPIOの設定を変更します
 */
XError xgpio_set_mode(XGpio* self, XGpioMode mode);


/** @brief 出力レベルを書き込みます
 */
void xgpio_write(XGpio* self, bool value);


/** @brief 入力レベルを読み出します
 */
bool xgpio_read(const XGpio* self);


/** @brief IRQを設定します
 */
XError xgpio_set_irq_handler(XGpio* self, XGpioEdge edge, XGpioIrqHandler handler, void* handler_arg);


/** @brief IRQの有効無効を設定します
 */
XError xgpio_set_irq_enabled(XGpio* self, bool enabled);


/** @brief 出力レベルをLOWに設定します
 */
void xgpio_set_low(XGpio* self);


/** @brief 出力レベルをHIGHに設定します
 */
void xgpio_set_high(XGpio* self);


/** @brief 出力レベルを反転します
 */
void xgpio_toggle(XGpio* self);


/** @brief 入力レベルがLOWかどうかを返します
 */
bool xgpio_is_low(const XGpio* self);


/** @brief 入力レベルがHIGHかどうかを返します
 */
bool xgpio_is_high(const XGpio* self);


#ifdef __cplusplus
}
#endif /* __cplusplus */


/** @} end of addtogroup xgpio
 *  @} end of addtogroup hal
 */


#endif /* picox_hal_xgpio_h_ */
