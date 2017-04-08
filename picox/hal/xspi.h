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
    XSPI_BITORDER_MSB_FIRST, /** 最上位ビットから転送 */
    XSPI_BITORDER_LSB_FIRST, /** 最下位ビットから転送 */
} XSpiBitorder;


/** @brief 伝送タイミングを表します
 *
 *  詳しくはWebで`SPI モード`等で調べてください。
 */
typedef enum XSpiMode
{
    XSPI_MODE_0, /** POL = 0, PHA = 0 */
    XSPI_MODE_1, /** POL = 0, PHA = 1 */
    XSPI_MODE_2, /** POL = 1, PHA = 0 */
    XSPI_MODE_3, /** POL = 1, PHA = 1 */
} XSpiMode;


/** @name HAL SPI virtual functions
 *
 *  インターフェースの実装者は、要求されたHALインタフェースを満たす必要がありま
 *  @{
 */


/** @brief SPI設定のインターフェースです
 *
 *  freq_hzピッタリに設定できない場合は、freq_hz以下の周波数に設定してください。
 */
typedef XError (*XSpiSetFormatFunc)(void* driver, uint32_t freq_hz, XSpiMode mode, XSpiBitorder bitorder);


/** @brief SPI送受信のインターフェースです
 *
 *  + txからsizeバイト送信し、rxにsizeバイト受信します
 *  + tx == NULLの場合は、0xFFのダミーデータを送信してください
 *  + rx == NULLの場合は受信データは読み捨ててください
 *
 *  txとrxは同じアドレスを指している可能性があることに注意してください。
 */
typedef void (*XSpiExchangeFunc)(void* driver, const void* tx, void* rx, size_t size);


/** @brief SPIバスのロック・アンロックインターフェースです
 *
 *  lock != falseの時はロック、 lock == falseであればアンロックを行ってください
 *  。 ロックとは、mutex等の排他制御機構のことを指します
 *
 *  マルチタスク環境で、SPIバスを共有するオブジェクトが、バスを排他的に操作でき
 *  るようにするためのインターフェースです。
 *
 *  非マルチタスク環境であれば、このインターフェースは未設定(NULL)でかまいません。
 */
typedef void (*XSpiLockBusFunc)(void* driver, bool lock);


/** @} end of name HAL SPI virtual functions
 */


/** @brief 仮想SPIインターフェースのvtableです
 */
typedef struct XSpiVTable
{
    XSpiSetFormatFunc   m_set_format_func;
    XSpiExchangeFunc    m_exchange_func;
    XSpiLockBusFunc     m_lock_bus_func;
} XSpiVTable;


/** @brief 仮想SPIを表すインターフェース型です
 *
 *  XSpiはCS(チップセレクト)について感知しません。XGpio等で直接操作してください
 *
 *  @note
 *  Linuxのspidevのように自動的にCSの操作を行うようにすると、SDカードのようなCS
 *  のデアサート状態でダミーデータを送信する必要があるような特殊なケースの対応が
 *  難しくなるため、若干面倒ではありますが、CSピンは別操作にするようにしました。
 *
 *  [参考]
 *  http://lxr.free-electrons.com/source/include/uapi/linux/spi/spidev.h
 */
typedef struct XSpi
{
    X_DECLEAR_RTTI(XSpiVTable);
} XSpi;


/** @brief 仮想SPIインターフェースを初期値に設定します
 */
void xspi_init(XSpi* self);


/** @brief SPI設定を行います
 */
XError xspi_set_format(XSpi* self, uint32_t freq_hz, XSpiMode mode, XSpiBitorder bitorder);


/** @brief SPI転送を行います
 *
 *  txからsizeバイトを送信し、rxにsizeバイトを受信します。txとrxは同じアドレスを
 *  指していてもかまいません。
 */
void xspi_exchange(XSpi* self, const void* tx, void* rx, size_t size);


/** @brief dstにsizeバイトを受信します
 *
 *  送信データにはダミーバイト(0xFF)が使用されます。
 */
void xspi_read(XSpi* self, void* dst, size_t size);


/** @brief srcからsizeバイトを送信します
 *
 *  受信データは読み捨てられます。
 */
void xspi_write(XSpi* self, const void* src, size_t size);


/** @brief 1バイトを受信します
 *
 *  送信データにはダミーバイト(0xFF)が使用されます。
 */
uint8_t xspi_read_byte(XSpi* self);


/** @brief 1バイトを送信します
 *
 *  受信データは読み捨てられます。
 */
void xspi_write_byte(XSpi* self, uint8_t b);


/** @brief SPIバスをロックします
 *
 *  マルチタスク環境で、1つのSPIバスを複数のSPIデバイスが共有する場合(マルチスレ
 *  ーブ)、あるデバイス(A)の操作中に、別のデバイス(B)Aの操作が行われると困ります。
 *
 *  そこで、SPIバスの使用する前にこの関数でバスをロックしておくことで、ロック中
 *  はバスを専有することができます。
 */
void xspi_lock_bus(XSpi* self);


/** @brief SPIバスをアンロックします
 */
void xspi_unlock_bus(XSpi* self);


#ifdef __cplusplus
}
#endif /* __cplusplus */


/** @} end of addtogroup xspi
 *  @} end of addtogroup hal
 */


#endif /* picox_hal_xspi_h_ */
