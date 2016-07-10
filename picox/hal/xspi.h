/**
 *       @file  xspi.h
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


#ifndef picox_hal_xspi_h_
#define picox_hal_xspi_h_


#include <picox/core/xcore.h>


/** @addtogroup hal
 *  @{
 *  @addtogroup  xspi
 *  @brief SPI通信の抽象化レイヤです
 *  @{
 */


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @brief データのビット送受信順を表します
 */
typedef enum XSpiBitorder
{
    /** 最上位ビットから転送 */
    XSPI_BITORDER_MSB_FIRST,

    /** 最下位ビットから転送 */
    XSPI_BITORDER_LSB_FIRST,
} XSpiBitorder;


/** @brief chip selectの極性を表します
 */
typedef enum XSpiCsPolarity
{
    /** LOWアクティブ */
    XSPI_CS_POLARITY_ACTIVE_LOW,

    /** HIGHアクティブ */
    XSPI_CS_POLARITY_ACTIVE_HIGH,
} XSpiCsPolarity;


/** @brief 伝送タイミングを表します
 *
 *  詳しくはWebで`SPI モード`等で調べてください。
 */
typedef enum XSpiMode
{
    /** POL = 0, PHA = 0 */
    XSPI_MODE_0,

    /** POL = 0, PHA = 1 */
    XSPI_MODE_1,

    /** POL = 1, PHA = 0 */
    XSPI_MODE_2,

    /** POL = 1, PHA = 1 */
    XSPI_MODE_3,
} XSpiMode;


/** @brief SPIのコンフィグレーション構造体です
 */
typedef struct XSpiConfig
{
    uint32_t        frequency;
    XSpiMode        mode;
    XSpiBitorder    bitorder;
    XSpiCsPolarity  cs_polarity;
    int             bitwidth;
} XSpiConfig;


/** @brief SPI転送のパラメータを格納する構造体です
 *
 *  SPIでは送信と受信が同時に行われます。この構造体をxspi_transferの引数とする時
 *  以下の動作が行われます。
 *
 *  + `tx_buffer == NULL, rx_buffer != NULL`<br>
 *      transfer_sizeバイトのダミーデータを送信し、rx_bufferに受信データが格納し
 *      ます。
 *
 *  + `tx_buffer != NULL, rx_buffer == NULL`<br>
 *      tx_bufferからtransfer_sizeバイトを送信し、受信データは読み捨てます。
 *
 *  + `tx_buffer != NULL, rx_buffer != NULL`<br>
 *      tx_bufferからtransfer_sizeバイトを送信し、rx_bufferに受信します。
 *
 *  + `tx_buffer == NULL, rx_buffer == NULL`<br>
 *      設定禁止
 *
 *  CSピンはspi_configure()で指定されたcs_polarityに応じて、自動的に上げ下げされ
 *  ます。
 */
typedef struct XSpiTransaction
{
    const void* tx_buffer;
    void*       rx_buffer;
    size_t      transfer_size;
} XSpiTransaction;


/** @brief XSpiTransactionオブジェクトの初期化用マクロです
 */
#define XSPI_TRANSACTION_INITIALIZER(tx, rx, size)  {tx, rx, size}


/** @name HAL SPI virtual functions
 *
 *  ユーザはHALが要求するインターフェースを満たす必要があります。
 *  @{
 */


/** @brief @see spi_configure */
typedef XError (*XSpiConfigureFunc)(void* driver, const XSpiConfig* config);


/** @brief @see spi_transfer */
typedef XError (*XSpiTransferFunc)(void* driver, const XSpiTransaction* transactions, int num);


/** @} end of name HAL SPI virtual functions
 */


/** @brief 仮想SPIインターフェース構造体です
 *
 *  インターフェースを満たす関数はユーザーが用意し、セットする必要があります。
 */
typedef struct XSpi
{
    void*               driver;
    XSpiConfigureFunc   configure_func;
    XSpiTransferFunc    transfer_func;
} XSpi;


/** @brief 仮想SPIインターフェースを初期値に設定します
 */
void xspi_init(XSpi* spi);


/** @brief コンフィグオブジェクトを初期値に設定します
 */
void xspi_config_init(XSpiConfig* config);


/** @brief SPIの設定を変更します
 */
XError xspi_configure(XSpi* spi, const XSpiConfig* config);


/** @brief SPI転送を行います
 *
 *  num個のXSpiTransactionを順に実行します。
 */
XError xspi_transfer(XSpi* spi, const XSpiTransaction* transactions, int num);


/** @brief dstにsizeバイトを受信します
 */
XError xspi_read(XSpi* spi, void* dst, size_t size);


/** @brief srcからsizeバイトを送信します
 */
XError xspi_write(XSpi* spi, const void* src, size_t size);


/** @brief 1バイトを送信します
 */
XError xspi_write_byte(XSpi* spi, uint8_t b);


/** @brief 1バイトを受信します
 */
XError xspi_read_byte(XSpi* spi, uint8_t* b);


/** @brief sizeバイトを送受信します
 */
XError xspi_exchange(XSpi* spi, void* dst, const void* src, size_t size);


#if 0
[TODO] multithread support
void xspi_aquire_bus(XSpi* spi);
void xspi_release_bus(XSpi* spi);
#endif


#ifdef __cplusplus
}
#endif /* __cplusplus */


/** @} end of addtogroup xspi
 *  @} end of addtogroup hal
 */


#endif /* picox_hal_xspi_h_ */
