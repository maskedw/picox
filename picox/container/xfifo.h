/**
 *       @file  xfifo.h
 *      @brief  First In First Out形式のバッファ
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

#ifndef xfifo_h_
#define xfifo_h_


#include <string.h>
#include <stdint.h>
#include <stdbool.h>


#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


#ifndef XFIFO_ASSERT

    #define XFIFO_ASSERT(expr)   do { if (! expr) for(;;); } while (0)

#endif


#ifndef XFIFO_SIZE_T
    #define XFIFO_SIZE_T    uint_fast16_t
#endif


typedef XFIFO_SIZE_T    XFifoSize;
typedef void(*XFifoAtomicAssigner)(XFifoSize* dst, XFifoSize value);



typedef struct XFifo
{
/// @privatesection
    uint8_t*                data;
    XFifoSize               first;
    XFifoSize               last;
    XFifoSize               capacity;
    XFifoAtomicAssigner     assigner;
} XFifo;


/* 内部処理用のマクロ */
#define XFIFO__ADD_FIRST(x)          ((self->first + x) & self->capacity)
#define XFIFO__ADD_LAST(x)           ((self->last  + x) & self->capacity)
#define XFIFO__IS_POWER_OF_TWO(x)    (((x) & -(x)) == (x))


/* ===================================================================
 *  Public Functions
 * ===================================================================*/
static inline void XFifoDefaultAtomitAssigner(XFifoSize* dst, XFifoSize value)
{
    *dst = value;
}

/** バッファを初期化します。
 *  @pre
 *  + (self != NULL) && (buffer != NULL)
 *  + bufferはsizeバイトの連続領域を指していること。
 *  + sizeは2のべき乗であること。
 */
static inline void xfifo_init(XFifo* self, void* buffer, XFifoSize size, XFifoAtomicAssigner assigner)
{
    XFIFO_ASSERT(self && buffer);
    XFIFO_ASSERT(XFIFO__IS_POWER_OF_TWO(size) && "Require Power of 2");
    XFIFO_ASSERT(size > 0);

    self->data = buffer;
    self->first = self->last = 0;
    self->capacity = size - 1;

    if (! assigner)
        self->assigner = XFifoDefaultAtomitAssigner;
    else
        self->assigner = assigner;
}


/** バッファを空にします。
 */
static inline void xfifo_clear(XFifo* self)
{
    self->last = self->first;
}


/** 格納要素数が0かどうかを返します。
 */
static inline bool xfifo_empty(const XFifo* self)
{
    return self->last == self->first;
}


/** 最大格納要素数を返します。
 */
static inline XFifoSize xfifo_capacity(const XFifo* self)
{
    return self->capacity;
}

/** 格納要素数を返します。
 */
static inline XFifoSize xfifo_size(const XFifo* self)
{
    return (self->last - self->first) & self->capacity;
}


/** 要素数が上限かどうかを返します。
 */
static inline bool xfifo_full(const XFifo* self)
{
    return xfifo_size(self) == xfifo_capacity(self);
}


/** 空き要素数を返します。
 */
static inline XFifoSize xfifo_reserve(const XFifo* self)
{
    return xfifo_capacity(self) - xfifo_size(self);
}


/** 要素を格納するバッファを返します。
 */
static inline void* xfifo_data(const XFifo* self)
{
    return self->data;
}


/** FIFO末尾に要素を追加します。
 */
static inline void xfifo_push(XFifo* self, uint8_t data)
{
    self->data[self->last] = data;
    self->assigner(&self->last, XFIFO__ADD_LAST(1));
}


/** FIFO先頭から要素を取り出します。
 */
static inline uint8_t xfifo_pop(XFifo* self)
{
    const uint8_t data = self->data[self->first];
    self->assigner(&self->first, XFIFO__ADD_FIRST(1));

    return data;
}


/** FIFO末尾に要素を追加します。
 */
static inline XFifoSize xfifo_write(XFifo* self, const void* src, XFifoSize ssize)
{
    const XFifoSize reserve = xfifo_reserve(self);

    if ((reserve <= 0) || (ssize <= 0) || (src == NULL))
        return 0;

    /* 書き込む(書き込める)要素数 */
    XFifoSize to_write = (reserve >= ssize) ? ssize : reserve;

    /* to_writeは減算される可能性があるので保存しておく。*/
    const XFifoSize    written      = to_write;
    volatile XFifoSize wpos         = self->last;
    const XFifoSize    until_tail   = xfifo_capacity(self) - wpos + 1;

    if (to_write > until_tail)
    {
        memcpy(&self->data[wpos], src, until_tail);
        to_write -= until_tail;
        src       = (const char*)src + until_tail;
        wpos = 0;
    }
    memcpy(&self->data[wpos], src, to_write);
    self->assigner(&self->last, XFIFO__ADD_LAST(written));

    return written;
}


/** FIFO先頭から要素を取り出します。
 */
static inline XFifoSize xfifo_read(XFifo* self, void* dst, XFifoSize dsize)
{
    const XFifoSize size = xfifo_size(self);

    if ((size <= 0) || (dsize <= 0) || (dst == NULL))
        return 0;

    /* 読み込む(読み込める)要素数 */
    XFifoSize to_read = (size >= dsize) ? dsize : size;

    /* to_readは減算される可能性があるので保存しておく。 */
    const XFifoSize     read        = to_read;
    volatile XFifoSize  rpos        = self->first;
    const XFifoSize     until_tail  = xfifo_capacity(self) - rpos + 1;

    if (to_read > until_tail)
    {
        memcpy(dst, &self->data[rpos], until_tail);
        to_read  -= until_tail;
        dst      = (char*)dst + until_tail;
        rpos = 0;
    }
    memcpy(dst, &self->data[rpos], to_read);
    self->assigner(&self->first, XFIFO__ADD_FIRST(read));

    return read;
}


#undef XFIFO__ADD_FIRST
#undef XFIFO__ADD_LAST
#undef XFIFO__IS_POWER_OF_TWO


#ifdef __cplusplus
}
#endif // __cplusplus


#endif // xfifo_h_
