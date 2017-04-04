/**
 *       @file  xmemstream.h
 *      @brief
 *
 *    @details
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2017/04/05
 * ===================================================================
 */

/*
 * License: MIT license
 * Copyright (c) <2017> <MaskedW [maskedw00@gmail.com]>
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


#ifndef picox_core_xmemstream_h_
#define picox_core_xmemstream_h_


#include <picox/core/xcore.h>


/** @addtogroup core
 *  @{
 *  @addtogroup xmemstream
 *  @brief メモリに対して入出力を行うストリームです
 *
 */


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @brief メモリに対して入出力を行うストリームです
 */
typedef struct XMemStream
{
    XStream         m_super;
    uint8_t*        mem;
    XSize           pos;
    XSize           size;
    XSize           capacity;
} XMemStream;
X_DECLEAR_RTTI_TAG(XMEMSTREAM_RTTI_TAG);


/** @brief メモリストリームを初期化します
 *
 *  @param mem      ストリーム対象のメモリ先頭アドレス
 *  @param size     初期有効バイト数
 *  @param capacity writeやseekで拡張可能なバイト数
 *
 *  例えば、sizeが10でcapacityが20の時、readは10バイトまでしか行えませんが、
 *  writeで5バイト追記した場合は、sizeが拡張され、15バイトまでreadできるようにな
 *  ります。さらに6バイトを追記しようとすると、capacityは20なので、最後の1バイト
 *  は捨てられ、sizeは20となり、capacity以上の拡張を行うことはできません。
 */
XStream* xmemstream_init(XMemStream* self, void* mem, size_t size, size_t capacity);


/* 以下の関数群は一応公開していますが、XMemStreamとして直接扱わず、XStreamという
 * インターフェース型として使用することを意図しているため、通常は使用する必要は
 * ありません。
 */
int xmemstream_write(XMemStream* self, const void* src, size_t size, size_t* nwritten);
int xmemstream_read(XMemStream* self, void* dst, size_t size, size_t* nread);
int xmemstream_seek(XMemStream* self, XOffset pos, XSeekMode mode);
int xmemstream_tell(XMemStream* self, XSize* pos);


#ifdef __cplusplus
}
#endif /* __cplusplus */


/** @} end of addtogroup xmemstream
 *  @} end of addtogroup core
 */


#endif /* picox_core_xmemstream_h_ */
