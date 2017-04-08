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


/** @name HAL I2C virtual functions
 *
 *  インターフェースの実装者は、要求されたHALインタフェースを満たす必要がありま
 *  @{
 */


/** @brief  I2C周波数[Hz]を設定するインターフェースです
 *
 *  指定の周波数に設定できない場合は、freq_hz以下で設定可能な周波数に設定してく
 *  ださい。
 */
typedef XError (*XI2cSetFrequencyFunc)(void* driver, uint32_t freq_hz);


/** @brief I2C受信を行うインターフェースです
 *
 *  スレーブアドレスaddrから、sizeバイトをdstに読みだしてください。
 */
typedef XError (*XI2cReadFunc)(void* driver, int addr, void* dst, size_t size);


/** @brief I2C送信を行うインターフェースです
 *
 *  スレーブアドレスaddrへ、srcからsizeバイトを書き出してください。
 */
typedef XError (*XI2cWriteFunc)(void* driver, int addr, const void* src, size_t size);


/** @brief I2Cバスのロック・アンロックインターフェースです
 *
 *  lock != falseの時はロック、 lock == falseであればアンロックを行ってください
 *  。 ロックとは、mutex等の排他制御機構のことを指します
 *
 *  マルチタスク環境で、I2Cバスを共有するオブジェクトが、バスを排他的に操作でき
 *  るようにするためのインターフェースです。
 *
 *  非マルチタスク環境であれば、このインターフェースは未設定(NULL)でかまいません。
 */
typedef void (*XI2cLockBusFunc)(void* driver, bool lock);


/** @} end of name HAL I2C virtual functions
 */


typedef struct XI2cVTable
{
    XI2cSetFrequencyFunc    m_set_frequency_func;
    XI2cReadFunc            m_read_func;
    XI2cWriteFunc           m_write_func;
    XI2cLockBusFunc         m_lock_bus_func;
} XI2cVTable;


/** @brief 仮想I2Cインターフェース構造体です
 *
 *  インターフェースを満たす関数はユーザーが用意し、セットする必要があります。
 */
typedef struct XI2cTag
{
    X_DECLEAR_RTTI(XI2cVTable);
} XI2c;


/** @brief 仮想I2Cインターフェースを初期値に設定します
 */
void xi2c_init(XI2c* self);


/** @brief クロック周波数(Hz)を設定します
 */
XError xi2c_set_frequency(XI2c* self, uint32_t freq_hz);


/** @brief スレーブアドレスaddrからdstにsizeバイトを受信します
 */
XError xi2c_read(XI2c* self, int addr, void* dst, size_t size);


/** @brief スレードアドレスaddrへsrcからsizeバイトを送信します
 */
XError xi2c_write(XI2c* self, int addr, const void* src, size_t size);


/** @brief 1バイトを受信します
 */
XError xi2c_read_byte(XI2c* self, int addr, uint8_t* b);


/** @brief 1バイトを送信します
 */
XError xi2c_write_byte(XI2c* self, int addr, uint8_t b);


/** @brief I2Cバスをロックします
 *
 *  マルチタスク環境で、1つのI2Cバスを複数のI2Cデバイスが共有する場合(マルチスレ
 *  ーブ)、あるデバイス(A)の操作中に、別のデバイス(B)Aの操作が行われると困ります。
 *
 *  そこで、I2Cバスの使用する前にこの関数でバスをロックしておくことで、ロック中
 *  はバスを専有することができます。
 */
void xi2c_lock_bus(XI2c* self);


/** @brief I2Cバスをアンロックします
 */
void xi2c_unlock_bus(XI2c* self);


#ifdef __cplusplus
}
#endif /* __cplusplus */


/** @} end of addtogroup xi2c
 *  @} end of addtogroup hal
 */


#endif /* picox_hal_xi2c_h_ */
