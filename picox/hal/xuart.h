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


/** @brief @see xuart_set_config */
typedef XError (*XUartSetConfigFunc)(void* driver, const XUartConfig* config);

/** @brief @see xuart_get_config */
typedef void (*XUartGetConfigFunc)(void* driver, XUartConfig* config);

/** @brief @see xuart_write */
typedef XError (*XUartWriteFunc)(void* driver, const void* src, size_t size);

/** @brief @see xuart_read */
typedef XError (*XUartReadFunc)(void* driver, void* dst, size_t size, size_t* nread, XTicks timeout);

/** @brief @see xuart_flush */
typedef void (*XUartFlushFunc)(void* driver);

/** @brief @see xuart_drain*/
typedef void (*XUartDrainFunc)(void* driver);

/** @brief @see xuart_clear */
typedef void (*XUartClearFunc)(void* driver, XUartDirection direction);


/** @} end of name HAL Uart virtual functions
 */


/** @brief UARTインターフェースのvtableです
 */
typedef struct XUartVTable
{
    XUartSetConfigFunc  m_set_config_func;
    XUartGetConfigFunc  m_get_config_func;
    XUartReadFunc       m_read_func;
    XUartWriteFunc      m_write_func;
    XUartFlushFunc      m_flush_func;
    XUartDrainFunc      m_drain_func;
    XUartClearFunc      m_clear_func;
} XUartVTable;


/** @brief 仮想UARTを表すインターフェース型です
 */
typedef struct XUart
{
    X_DECLEAR_RTTI(XUartVTable);
} XUart;
X_DECLEAR_RTTI_TAG(XUART_STREAM_RTTI_TAG);


/** @brief 仮想UARTインターフェースを初期値に設定します
 */
void xuart_init(XUart* self);


/** @brief コンフィグオブジェクトを初期値に設定します
 */
void xuart_init_config(XUartConfig* config);


/** @brief UARTインターフェースを元にストリームを初期化します
 */
XStream* xuart_init_stream(XUart* self, XStream* stream);


/** @brief UARTの設定を変更します
 */
XError xuart_set_config(XUart* self, const XUartConfig* config);


/** @brief UARTの設定を取得します
 */
void xuart_get_config(const XUart* self, XUartConfig* config);


/** @brief dstに最大でsizeバイトを受信します
 *
 *  `*nread`には受信できたバイト数が返されます。
 */
XError xuart_read(XUart* self, void* dst, size_t size, size_t* nread, XTicks timeout);


/** @brief srcからsizeバイトを送信します
 */
XError xuart_write(XUart* self, const void* src, size_t size);


/** @brief dstに最大でsizeバイトを受信します
 *
 *  `xuart_read(uart, dst, size, nread, 0);`と同じです。つまり、受信データがない
 *  場合、ただちに処理を返します。
 */
XError xuart_read_poll(XUart* self, void* dst, size_t size, size_t* nread);


/** @brief バッファリングされた送信データの出力開始を強制します
 */
void xuart_flush(XUart* self);


/** @brief バッファリングされた送信データの出力を完了を待ちます
 *
 *  xuart_flush()は送信の"開始"を保証しますが、xuart_drain()は送信の"完了"を保証
 *  します。
 */
void xuart_drain(XUart* self);


/** @brief 送受信バッファをクリアします
 *
 *  directionの設定値に応じて、以下の操作を行います
 *
 *  + `XUART_DIRECTION_IN`    受信バッファのクリア
 *  + `XUART_DIRECTION_OUT`   送信バッファのクリア
 *  + `XUART_DIRECTION_INOUT` 送受信バッファのクリア
 */
void xuart_clear(XUart* self, XUartDirection direction);


/** @brief printf形式の出力を行います
 */
int xuart_printf(XUart* self, const char* fmt, ...);


/** @brief vprintf形式の出力を行います
 */
int xuart_vprintf(XUart* self, const char* fmt, va_list args);


/** @brief 1バイトを送信します
 */
int xuart_putc(XUart* self, int c);


/** @brief 1バイトを受信します
 *
 *  受信データなしの場合は、EOFを返します。
 */
int xuart_getc(XUart* self);


/** @name  config_properties
 *  @brief set_config, get_configの簡易版です
 *  @{
 */
XError xuart_set_baudrate(XUart* self, uint32_t baudrate);
uint32_t xuart_baudrate(const XUart* self);

XError xuart_set_parity(XUart* self, XUartParity parity);
XUartParity xuart_parity(const XUart* self);

XError xuart_set_stopbits(XUart* self, XUartStopbits stopbits);
XUartStopbits xuart_stopbits(const XUart* self);

XError xuart_set_flow_control(XUart* self, XUartFlowControl flow_control);
XUartFlowControl xuart_flow_control(const XUart* self);

/** @} end of name config_properties
 */

#ifdef __cplusplus
}
#endif /* __cplusplus */


/** @} end of addtogroup xuart
 *  @} end of addtogroup hal
 */


#endif /* picox_hal_xuart_h_ */
