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


/** @brief CSをHIGHの状態のまま転送を行います
 *
 *  MMCの初期化時等、CSをHIGHの状態のままダミークロックを送信する必要があるよう
 *  な時に使用します。
 */
#define XSPI_TRANSACTION_FLAG_KEEP_CS_HIGH  (1U << 1)


/** @brief トランザクション間にCSのde-assert, assertを行います
 */
#define XSPI_TRANSACTION_FLAG_CS_CHANGE     (1U << 0)


/** @brief picoxの予約外となるフラグビットのシフト数
 *
 *  bit8~15のフラグビットはpicoxは使用しないので、ユーザーが好きに使用してくださ
 *  い。
 */
#define XSPI_TRANSACTION_USER_FLAG_SHIFT    (8)


/** @brief SPI転送のパラメータを格納する構造体です
 *
 *  SPIでは送信と受信が同時に行われます。この構造体をxspi_transferの引数とする時
 *  、呼び出し側は呼び出し先に対して、以下の振る舞いを要求します。
 *
 *  + tx_bufferからtransfer_sizeバイトのデータを送信し、受信データはrx_bufferに
 *    格納する。
 *
 *  + tx_bufferがNULLの時は、0xFFをダミーとして送信する。
 *
 *  + rx_bufferがNULLの時は、受信データを読み捨てる。
 *
 *  +  XSpiConfigureFuncで指定されたcs_polarityの値に応じて、CSピンの上げ
 *     下げを行う。
 *
 *  + トランザクション間はdelay_usecsで指定されたウエイトを入れる。
 *
 *  + flagsで指定された拡張動作を行う。
 */
typedef struct XSpiTransaction
{
    const void* tx_buffer;
    void*       rx_buffer;
    size_t      transfer_size;
    uint16_t    flags;
    uint16_t    delay_usecs;
} XSpiTransaction;


/** @brief XSpiTransactionオブジェクトの初期化用マクロです
 *
 *  構造体の初期化が面倒くさい時に補助として使ってください。
 *
 *  @code {.c}
 *  uint8_t tx[10] = "Hello";
 *  uint8_t rx[sizeof(tx)];
 *  XSpiTransaction t = XSPI_TRANSACTION_INITIALIZER(tx, rx, sizeof(tx), 0, 0);
 *  @endcode
 */
#define XSPI_TRANSACTION_INITIALIZER(tx, rx, size, flags, delay)  {tx, rx, size, flags, delay}


/** @name HAL SPI virtual functions
 *
 *  ユーザはHALが要求するインターフェースを満たす必要があります。
 *  @{
 */

struct XSpi;
/** @brief @see spi_configure */
typedef XError (*XSpiConfigureFunc)(struct XSpi* spi, const XSpiConfig* config);


/** @brief @see spi_transfer */
typedef XError (*XSpiTransferFunc)(struct XSpi* spi, const XSpiTransaction* transactions, int num);


/** @brief @see xspi_aquire_bus */
typedef XError (*XSpiLockFunc)(struct XSpi* spi, bool lock);

/** @} end of name HAL SPI virtual functions
 */


/** @brief 仮想SPIインターフェースのvtableです
 */
typedef struct XSpiVTable
{
    XSpiConfigureFunc   m_configure_func;
    XSpiTransferFunc    m_transfer_func;
    XSpiLockFunc        m_lock_func;
} XSpiVTable;


/** @brief 仮想SPIを表すインターフェース型です
 */
typedef struct XSpi
{
    X_DECLEAR_RTTI(XSpiVTable);
} XSpi;
X_DECLEAR_RTTI_TAG(XSPI_STREAM_RTTI_TAG);


/** @brief 仮想SPIインターフェースを初期値に設定します
 */
void xspi_init(XSpi* spi);


/** @brief コンフィグオブジェクトを初期値に設定します
 */
void xspi_init_config(XSpiConfig* config);


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


/** @brief SPIバスをロックします
 *
 *  マルチスレッド環境で、1つのSPIを複数のSPIデバイスが共有する場合(マルチスレー
 *  ブ)、デバイスAの処理中にデバイスBがSPIの設定値を変更してしまうと困ります。
 *
 *  そこで、XSpiを使用するドライバは、SPIを使用する前にこの関数でバスをロックし
 *  てからxspi_configure()で設定を行い、使用後にxspi_release_bus()でバスの解放を
 *  行うことで、バスロック中の設定値を固定することができます。
 */
XError xspi_aquire_bus(XSpi* spi);


/** @brief SPIバスをアンロックします
 */
XError xspi_release_bus(XSpi* spi);


#ifdef __cplusplus
}
#endif /* __cplusplus */


/** @} end of addtogroup xspi
 *  @} end of addtogroup hal
 */


#endif /* picox_hal_xspi_h_ */
