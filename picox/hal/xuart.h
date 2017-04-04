/**
 *       @file  xuart.h
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


#ifndef picox_hal_xuart_h_
#define picox_hal_xuart_h_


#include <picox/core/xcore.h>


/** @addtogroup hal
 *  @{
 *  @addtogroup  xuart
 *  @brief UART通信の抽象化レイヤです
 *  @{
 */


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @brief パリティビットの種別を表します
 */
typedef enum XUartParity
{
    /** パリティなし */
    XUART_PARITY_NONE,

    /** 偶数パリティ */
    XUART_PARITY_EVEN,

    /** 奇数パリティ */
    XUART_PARITY_ODD,
} XUartParity;


/** @brief ストップビットの種別を表します
 */
typedef enum XUartStopbits
{
    /** 1ストップビット*/
    XUART_STOPBITS_ONE,

    /** 1.5ストップビット */
    XUART_STOPBITS_ONE_AND_HALF,

    /** 2ストップビット */
    XUART_STOPBITS_TWO,
} XUartStopbits;


/** @brief フローコントロールの種別を表します
 */
typedef enum XUartFlowControl
{
    /** フロー制御なし */
    XUART_FLOW_CONTROL_NONE,

    /** ハードウェアフロー制御(RTS, CTS) */
    XUART_FLOW_CONTROL_HARD,

    /** ソフトウェアフロー制御(Xon, Xoff) */
    XUART_FLOW_CONTROL_SOFT,
} XUartFlowControl;


/** @brief 入出力方向の種別を表します
 */
typedef enum XUartDirection
{
    /** 入力 */
    XUART_DIRECTION_IN,

    /** 出力 */
    XUART_DIRECTION_OUT,

    /** 入出力 */
    XUART_DIRECTION_INOUT,
} XUartDirection;


/** @brief UARTのコンフィグレーション構造体です
 */
typedef struct XUartConfig
{
    uint32_t            baudrate;
    XUartParity         parity;
    XUartStopbits       stopbits;
    XUartFlowControl    flow_control;
} XUartConfig;


/** @name HAL UART virtual functions
 *
 *  ユーザはHALが要求するインターフェースを満たす必要があります。
 *  @{
 */


/** @brief @see xuart_configure */
typedef XError (*XUartConfigureFunc)(void* driver, const XUartConfig* config);

/** @brief @see xuart_write */
typedef XError (*XUartWriteFunc)(void* driver, const void* src, size_t size);

/** @brief @see xuart_read */
typedef XError (*XUartReadFunc)(void* driver, void* dst, size_t size, size_t* nread, XTicks timeout);

/** @brief @see xuart_flush */
typedef void (*XUartFlushFunc)(void* driver, bool drain);

/** @brief @see xuart_clear */
typedef void (*XUartClearFunc)(void* driver, XUartDirection direction);


/** @} end of name HAL Uart virtual functions
 */


/** @brief 仮想UARTインターフェース構造体です
 *
 *  インターフェースを満たす関数はユーザーが用意し、セットする必要があります。
 */
typedef struct XUart
{
    void*               driver;
    XUartConfigureFunc  configure_func;
    XUartReadFunc       read_func;
    XUartWriteFunc      write_func;
    XUartFlushFunc      flush_func;
    XUartClearFunc      clear_func;
} XUart;
X_DECLEAR_RTTI_TAG(XUART_STREAM_RTTI_TAG);


/** @brief 仮想UARTインターフェースを初期値に設定します
 */
void xuart_init(XUart* uart);


/** @brief コンフィグオブジェクトを初期値に設定します
 */
void xuart_config_init(XUartConfig* config);


/** @brief UARTインターフェースを元にストリームを初期化します
 */
XStream* xuart_stream_init(XUart* uart, XStream* stream);


/** @brief UARTの設定を変更します
 */
XError xuart_configure(XUart* uart, const XUartConfig* config);


/** @brief dstに最大でsizeバイトを受信します
 *
 *  `*nread`には受信できたバイト数が返されます。
 */
XError xuart_read(XUart* uart, void* dst, size_t size, size_t* nread, XTicks timeout);


/** @brief srcからsizeバイトを送信します
 */
XError xuart_write(XUart* uart, const void* src, size_t size);


/** @brief dstに最大でsizeバイトを受信します
 *
 *  `xuart_read(uart, dst, size, nread, 0);`と同じです。つまり、受信データがない
 *  場合、ただちに処理を返します。
 */
XError xuart_read_poll(XUart* uart, void* dst, size_t size, size_t* nread);


/** @brief バッファリングされた送信データの出力を強制します
 *
 *  `drain == true`が指定された場合は、出力を開始し、かつ、全ての送信が出力が完
 *  了するまで待機します。
 */
void xuart_flush(XUart* uart, bool drain);


/** @brief 送受信バッファをクリアします
 *
 *  directionの設定値に応じて、以下の操作を行います
 *
 *  + `XUART_DIRECTION_IN`    受信バッファのクリア
 *  + `XUART_DIRECTION_OUT`   送信バッファのクリア
 *  + `XUART_DIRECTION_INOUT` 送受信バッファのクリア
 */
void xuart_clear(XUart* uart, XUartDirection direction);


/** @brief printf形式の出力を行います
 */
int xuart_printf(XUart* uart, const char* fmt, ...);


/** @brief vprintf形式の出力を行います
 */
int xuart_vprintf(XUart* uart, const char* fmt, va_list args);


/** @brief 1バイトを送信します
 */
int xuart_putc(XUart* uart, int c);


/** @brief 1バイトを受信します
 *
 *  受信データなしの場合は、EOFを返します。
 */
int xuart_getc(XUart* uart);


#ifdef __cplusplus
}
#endif /* __cplusplus */


/** @} end of addtogroup xuart
 *  @} end of addtogroup hal
 */


#endif /* picox_hal_xuart_h_ */
