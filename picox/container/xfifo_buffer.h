/**
 *       @file  xfifo_buffer.h
 *      @brief
 *
 *    @details
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2015/06/13
 * ===================================================================
 */

/*
 * License: MIT license
 * Copyright (c) <2015> <MaskedW [maskedw00@gmail.com]>
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of self software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and self permission notice shall be
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

#ifndef picox_container_xfifo_buffer_h_
#define picox_container_xfifo_buffer_h_


#include <picox/core/xcore.h>


/** @addtogroup container
 *  @{
 *  @addtogroup xfifo_buffer
 *  @brief FIFOバイトバッファ
 *  @{
 */


#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


/** @brief アトミックにsize_t変数に代入を行う関数ポインタ */
typedef void(*XFifoAtomicAssigner)(volatile size_t* dst, size_t value);


/** @brief FIFOバイトバッファ管理構造体
 */
typedef struct XFifoBuffer
{
/// @privatesection
    uint8_t*             data;
    volatile size_t      first;
    volatile size_t      last;
    volatile size_t      capacity;
    XFifoAtomicAssigner  assigner;
    bool                 is_heapdata;
} XFifoBuffer;


#ifndef X_COMPILER_NO_INLINE


/* 内部処理用のマクロ */
#define XFIFO__ADD_FIRST(x)          ((self->first + x) & self->capacity)
#define XFIFO__ADD_LAST(x)           ((self->last  + x) & self->capacity)


X_INLINE void
XFifoDefaultAtomicAssign(volatile size_t* dst, size_t value)
{
    *dst = value;
}


/** @brief バッファを初期化します。
 *
 *  @param buffer   データ格納先
 *  @param size     bufferのバイト数
 *  @param assigner 内部RWポインタ書き換え関数
 *
 *  @pre
 *  + buffer != NULL
 *  + sizeは2のべき乗であること。
 *
 *  @details
 *  assignerがNULLの場合はXFifoDefaultAtomicAssign()が使用されます。
 *  bufferはこのオブジェクトが不要になるまで、ユーザー側が保持しておく必要があり
 *  ます。
 *
 *  @note
 *  [XFifoAtomicAssignerについて]
 *
 *  xfifo_bufferは主にデバイスドライバのバッファに使用することを想定しており、以
 *  下の条件の時、割り込み禁止区間なしでバッファへの書き込み、読み出しが可能で
 *  す。
 *
 *  **書き込み、読み出しのコンテキストが異なることが保証されていること**
 *
 *  [例]
 *  + 通常時にバッファにデータを書き込み、UART送信完了割り込みで、バッファからデータを取り出す
 *  + 通常時にバッファからデータを読み出し、UART受信完了割り込みで、バッファにデータを書き込む
 *
 *  **size_t型の変数は分割なしで代入できること**
 *
 *  通常は16bitCPUで32bit変数に代入しようとすると、機械語レベルでは複数回の代入
 *  命令を必要とします。その場合、割り込みが絡むとxfifo_bufferのRWポインタの整合
 *  性が保証できなくなります。
 *
 *  上記条件が保証できない場合は、XFifoAtomicAssignerを指定し、その関数内で、割
 *  り込みのロック、変数への代入、割り込みのアンロックを行ってください。
 */
X_INLINE void
xfifo_init(XFifoBuffer* self, void* buffer, size_t size, XFifoAtomicAssigner assigner)
{
    X_ASSERT(self);
    X_ASSERT(x_is_power_of_two(size));
    X_ASSERT(size > 0);

    self->is_heapdata = false;
    if (!buffer)
    {
        buffer = x_malloc(size);
        X_ASSERT(buffer);
        self->is_heapdata = true;
    }

    self->data = (uint8_t*)buffer;
    self->first = self->last = 0;
    self->capacity = size - 1;

    if (! assigner)
        self->assigner = XFifoDefaultAtomicAssign;
    else
        self->assigner = assigner;
}


X_INLINE void
xfifo_deinit(XFifoBuffer* self)
{
    if (self->is_heapdata)
        x_free(self->data);
    self->is_heapdata = false;
    self->data = NULL;
}


/** @brief バッファを空にします。
 */
X_INLINE void
xfifo_clear(XFifoBuffer* self)
{
    X_ASSERT(self);
    self->last = self->first;
}


/** @brief 格納要素数が0かどうかを返します。
 */
X_INLINE bool
xfifo_empty(const XFifoBuffer* self)
{
    X_ASSERT(self);
    return self->last == self->first;
}


/** @brief 最大格納要素数を返します。
 */
X_INLINE size_t
xfifo_capacity(const XFifoBuffer* self)
{
    X_ASSERT(self);
    return self->capacity;
}

/** @brief 格納要素数を返します。
 */
X_INLINE size_t
xfifo_size(const XFifoBuffer* self)
{
    X_ASSERT(self);
    return (self->last - self->first) & self->capacity;
}


/** @brief 要素数が上限かどうかを返します。
 */
X_INLINE bool
xfifo_full(const XFifoBuffer* self)
{
    X_ASSERT(self);
    return xfifo_size(self) == xfifo_capacity(self);
}


/** @brief 空き要素数を返します。
 */
X_INLINE size_t
xfifo_reserve(const XFifoBuffer* self)
{
    X_ASSERT(self);
    return xfifo_capacity(self) - xfifo_size(self);
}


/** @brief 要素を格納するバッファを返します。
 */
X_INLINE void*
xfifo_data(const XFifoBuffer* self)
{
    X_ASSERT(self);
    return self->data;
}


/** @brief FIFO末尾に要素を追加します。
 */
X_INLINE void
xfifo_push_back(XFifoBuffer* self, uint8_t data)
{
    X_ASSERT(self);
    self->data[self->last] = data;
    self->assigner(&self->last, XFIFO__ADD_LAST(1));
}


/** @brief FIFO先頭から要素を取り出します。
 */
X_INLINE uint8_t
xfifo_pop_front(XFifoBuffer* self)
{
    uint8_t data;
    X_ASSERT(self);

    data = self->data[self->first];
    self->assigner(&self->first, XFIFO__ADD_FIRST(1));
    return data;
}


/** @brief FIFO末尾に指定バイト数の書き込みを行います
 *
 *  @param src      書き込むデータ
 *  @param ssize    srcから取り出すバイト数
 */
X_INLINE void
xfifo_push_back_n(XFifoBuffer* self, const void* src, size_t ssize)
{
    size_t to_write, written, wpos, until_tail;
    X_ASSERT(self);
    X_ASSERT(src);
    X_ASSERT(xfifo_reserve(self) >= ssize);

    /* 書き込む要素数 */
    to_write = ssize;

    /* to_writeは減算される可能性があるので保存しておく。*/
    written      = to_write;
    wpos         = self->last;
    until_tail   = xfifo_capacity(self) - wpos + 1;

    if (to_write > until_tail)
    {
        memcpy(&self->data[wpos], src, until_tail);
        to_write -= until_tail;
        src       = (const char*)src + until_tail;
        wpos = 0;
    }
    memcpy(&self->data[wpos], src, to_write);
    self->assigner(&self->last, XFIFO__ADD_LAST(written));
}


/** @brief FIFO先頭から指定バイト数の読み出しを行います
 *
 *  @param dst      読み込み先
 *  @param dsize    dstに読み込むバイト数
 */
X_INLINE void
xfifo_pop_front_n(XFifoBuffer* self, void* dst, size_t dsize)
{
    size_t to_read, read, rpos, until_tail;
    X_ASSERT(self);
    X_ASSERT(dst);
    X_ASSERT(xfifo_size(self) >= dsize);

    /* 読み込む(読み込める)要素数 */
    to_read = dsize;

    /* to_readは減算される可能性があるので保存しておく。 */
    read        = to_read;
    rpos        = self->first;
    until_tail  = xfifo_capacity(self) - rpos + 1;

    if (to_read > until_tail)
    {
        memcpy(dst, &self->data[rpos], until_tail);
        to_read  -= until_tail;
        dst      = (char*)dst + until_tail;
        rpos = 0;
    }
    memcpy(dst, &self->data[rpos], to_read);
    self->assigner(&self->first, XFIFO__ADD_FIRST(read));
}


#undef XFIFO__ADD_FIRST
#undef XFIFO__ADD_LAST
#undef XFIFO__IS_POWER_OF_TWO


#else /* ifndef X_COMPILER_NO_INLINE */


void XFifoDefaultAtomicAssign(volatile size_t* dst, size_t value);
void xfifo_init(XFifoBuffer* self, void* buffer, size_t size, XFifoAtomicAssigner assigner);
void xfifo_deinit(XFifoBuffer* self)
void xfifo_clear(XFifoBuffer* self);
bool xfifo_empty(const XFifoBuffer* self);
size_t xfifo_capacity(const XFifoBuffer* self);
size_t xfifo_size(const XFifoBuffer* self);
bool xfifo_full(const XFifoBuffer* self);
size_t xfifo_reserve(const XFifoBuffer* self);
void* xfifo_data(const XFifoBuffer* self);
void xfifo_push_back(XFifoBuffer* self, uint8_t data);
uint8_t xfifo_pop_front(XFifoBuffer* self);
void xfifo_push_back_n(XFifoBuffer* self, const void* src, size_t ssize);
void xfifo_pop_front_n(XFifoBuffer* self, void* dst, size_t dsize);


#endif /* ifndef X_COMPILER_NO_INLINE */


#ifdef __cplusplus
}
#endif // __cplusplus


/** @} end of addtogroup xfifo_buffer
 *  @} end of addtogroup container
 */


#endif // picox_container_xfifo_buffer_h_
