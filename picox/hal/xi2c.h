/**
 *       @file  xi2c.h
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


#ifndef picox_hal_xi2c_h_
#define picox_hal_xi2c_h_


#include <picox/core/xcore.h>


/** @addtogroup hal
 *  @{
 *  @addtogroup  xi2c
 *  @brief I2C通信の抽象化レイヤです
 *  @{
 */


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @brief スレーブアドレス長を表します
 */
typedef enum XI2cSlaveAddressLengthTag
{
    /** 7bitアドレス(左詰め) */
    XI2C_SLAVE_ADDRESS_LENGTH_7BIT,

    /** 10bitアドレス */
    XI2C_SLAVE_ADDRESS_LENGTH_10BIT,
} XI2cSlaveAddressLength;


/** @brief I2Cのコンフィグレーション構造体です
 */
typedef struct XI2cConfigTag
{
    uint32_t                frequency;
    uint16_t                slave_address;
    XI2cSlaveAddressLength  slave_address_length;
    XTicks                  timeout;
} XI2cConfig;


/** @brief I2C転送のパラメータを格納する構造体です
 *
 *  + flagsにXI2C_TRANSACTION_FLAG_READがセットされていた場合<br>
 *      rx_bufferにtransfer_sizeバイトを受信します。
 *
 *  + flagsにXI2C_TRANSACTION_FLAG_READがセットされていなかった場合<br>
 *      tx_bufferからtransfer_sizeバイトを送信します。
 */
typedef struct XI2cTransactionTag
{
    const void* tx_buffer;
    void*       rx_buffer;
    size_t      transfer_size;
    uint16_t    flags;
} XI2cTransaction;


/** @brief トランザクションが受信であることを示すフラグです */
#define XI2C_TRANSACTION_FLAG_READ  (1 << 0)


/** @brief XI2cTransactionオブジェクトの初期化用マクロです
 */
#define XI2C_TRANSACTION_INITIALIZER(tx, rx, size, flags)  {tx, rx, size, flags}


/** @name HAL I2C virtual functions
 *
 *  ユーザはHALが要求するインターフェースを満たす必要があります。
 *  @{
 */


/** @brief @see i2c_configure */
typedef XError (*XI2cConfigureFunc)(void* driver, const XI2cConfig* config);


/** @brief @see i2c_transfer */
typedef XError (*XI2cTransferFunc)(void* driver, const XI2cTransaction* transactions, int num);


/** @} end of name HAL I2C virtual functions
 */


/** @brief 仮想I2Cインターフェース構造体です
 *
 *  インターフェースを満たす関数はユーザーが用意し、セットする必要があります。
 */
typedef struct XI2cTag
{
    void*               driver;
    XI2cConfigureFunc   configure_func;
    XI2cTransferFunc    transfer_func;
} XI2c;


/** @brief 仮想I2Cインターフェースを初期値に設定します
 */
void xi2c_init(XI2c* i2c);


/** @brief コンフィグオブジェクトを初期値に設定します
 */
void xi2c_config_init(XI2cConfig* config);


/** @brief I2Cの設定を変更します
 */
XError xi2c_configure(XI2c* i2c, const XI2cConfig* config);


/** @brief I2C転送を行います
 *
 *  num個のXI2cTransactionを順に実行します。
 */
XError xi2c_transfer(XI2c* i2c, const XI2cTransaction* transactions, int num);


/** @brief dstにsizeバイトを受信します
 */
XError xi2c_read(XI2c* i2c, void* dst, size_t size);


/** @brief srcからsizeバイトを送信します
 */
XError xi2c_write(XI2c* i2c, const void* src, size_t size);


/** @brief 1バイトを送信します
 */
XError xi2c_write_byte(XI2c* i2c, uint8_t b);


/** @brief 1バイトを受信します
 */
XError xi2c_read_byte(XI2c* i2c, uint8_t* b);


#if 0
[TODO] multithread support
void xi2c_aquire_bus(XI2c* i2c);
void xi2c_release_bus(XI2c* i2c);
#endif


#ifdef __cplusplus
}
#endif /* __cplusplus */


/** @} end of addtogroup xi2c
 *  @} end of addtogroup hal
 */


#endif /* picox_hal_xi2c_h_ */
