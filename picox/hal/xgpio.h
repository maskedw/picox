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


/** @brief GPIOの入出力方向を表します
 */
typedef enum XGpioDirection
{
    /** 入力 */
    XGPIO_DIRECTION_INPUT,

    /** 出力 */
    XGPIO_DIRECTION_OUTPUT,
} XGpioDirection;


/** @brief GPIOのモードを表します
 */
typedef enum XGpioMode
{
    /** プルアップ */
    XGPIO_MODE_PULLUP,

    /** プルダウン */
    XGPIO_MODE_PULLDOWN,

    /** プルアップ・ダウンなし */
    XGPIO_MODE_PULLNONE,

    /** オープンドレイン出力 */
    XGPIO_MODE_OPENDRAIN,
} XGpioMode;


/** @brief GPIOの信号レベルを表します
 */
typedef enum XGpioLevel
{
    /** LOWレベル */
    XGPIO_LEVEL_LOW = 0,

    /** HIGHレベル */
    XGPIO_LEVEL_HIGH = 1,
} XGpioLevel;


/** @brief GPIOの信号変化エッジ方向を表します
 */
typedef enum XGpioEdge
{
    /** 立ち下がり */
    XGPIO_EDGE_FALLING,

    /** 立ち上がり */
    XGPIO_EDGE_RISING,

    /** 立ち下がり + 立ち上がり */
    XGPIO_EDGE_BOTH,
} XGpioEdge;


/** @brief GPIOのコンフィグレーション構造体です
 */
typedef struct XGpioConfig
{
    XGpioDirection  direction;
    XGpioMode       mode;
    XGpioLevel      initial_level; /** 出力ポート時の初期値 */
} XGpioConfig;


/** @name HAL GPIO virtual functions
 *
 *  ユーザはHALが要求するインターフェースを満たす必要があります。
 *  @{
 */


/** @brief @see xgpio_configure */
typedef XError(*XGpioConfigureFunc)(void* driver, const XGpioConfig* config);

/** @brief @see xgpio_write */
typedef void (*XGpioWriteFunc)(void* driver, XGpioLevel level);

/** @brief @see xgpio_read */
typedef XGpioLevel (*XGpioReadFunc)(void* driver);

/** @brief エッジ変化割込み時に呼び出す割込みハンドラです */
typedef void (*XGpioEdgeHandler)(void* driver, XGpioEdge edge);

/** @brief @see xgpio_attach_edge_handler */
typedef XError (*XGpioAttachEdgeHandlerFunc)(void* driver, XGpioEdge edge, XGpioEdgeHandler handler);


/** @} end of name HAL GPIO virtual functions
 */


/** @brief 仮想GPIOインターフェース構造体です
 *
 *  インターフェースを満たす関数はユーザーが用意し、セットする必要があります。
 */
typedef struct XGpio
{
    void*                       driver;
    XGpioConfigureFunc          configure_func;
    XGpioReadFunc               read_func;
    XGpioWriteFunc              write_func;
    XGpioAttachEdgeHandlerFunc  attach_edge_handler_func;
} XGpio;


/** @brief 仮想GPIOインターフェースを初期値に設定します
 */
void xgpio_init(XGpio* gpio);


/** @brief コンフィグオブジェクトを初期値に設定します
 */
void xgpio_config_init(XGpioConfig* config);


/** @brief GPIOの設定を変更します
 */
XError xgpio_configure(XGpio* gpio, const XGpioConfig* config);


/** @brief 出力ポートへ値を書き込みます
 */
void xgpio_write(XGpio* gpio, XGpioLevel level);


/** @brief 入力ポートから値を読み出します
 */
XGpioLevel xgpio_read(XGpio* gpio);


/** @brief 出力ポートの値を反転します
 */
void xgpio_toggle(XGpio* gpio);


/** @brief エッジ変化割込みをセットします
 */
XError xgpio_attach_edge_handler(XGpio* gpio, XGpioEdge edge, XGpioEdgeHandler handler);


#if 0
XError xgpio_wait_for_level(XGpio* gpio, XGpioLevel level, XTicks min_expected, XTicks max_expected);
#endif


#ifdef __cplusplus
}
#endif /* __cplusplus */


/** @} end of addtogroup xgpio
 *  @} end of addtogroup hal
 */


#endif /* picox_hal_xgpio_h_ */
