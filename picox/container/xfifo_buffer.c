/**
 *       @file  xfifo_buffer.c
 *      @brief
 *
 *    @details
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2016/07/26
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

#include <picox/container/xfifo_buffer.h>


#ifdef X_COMPILER_NO_INLINE


/* 内部処理用のマクロ */
#define XFIFO__ADD_FIRST(x)          ((self->first + x) & self->capacity)
#define XFIFO__ADD_LAST(x)           ((self->last  + x) & self->capacity)


void XFifoDefaultAtomicAssign(volatile size_t* dst, size_t value)
{
    *dst = value;
}


void xfifo_init(XFifoBuffer* self, void* buffer, size_t size, XFifoAtomicAssigner assigner)
{
    X_ASSERT(self);
    X_ASSERT(buffer);
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


void xfifo_deinit(XFifoBuffer* self)
{
    if (self->is_heapdata)
        x_free(self->data);
    self->is_heapdata = false;
    self->data = NULL;
}


void xfifo_clear(XFifoBuffer* self)
{
    X_ASSERT(self);
    self->last = self->first;
}


bool xfifo_empty(const XFifoBuffer* self)
{
    X_ASSERT(self);
    return self->last == self->first;
}


size_t xfifo_capacity(const XFifoBuffer* self)
{
    X_ASSERT(self);
    return self->capacity;
}


size_t xfifo_size(const XFifoBuffer* self)
{
    X_ASSERT(self);
    return (self->last - self->first) & self->capacity;
}


bool xfifo_full(const XFifoBuffer* self)
{
    X_ASSERT(self);
    return xfifo_size(self) == xfifo_capacity(self);
}


size_t xfifo_reserve(const XFifoBuffer* self)
{
    X_ASSERT(self);
    return xfifo_capacity(self) - xfifo_size(self);
}


void* xfifo_data(const XFifoBuffer* self)
{
    X_ASSERT(self);
    return self->data;
}


void xfifo_push_back(XFifoBuffer* self, uint8_t data)
{
    X_ASSERT(self);
    self->data[self->last] = data;
    self->assigner(&self->last, XFIFO__ADD_LAST(1));
}


uint8_t xfifo_pop_front(XFifoBuffer* self)
{
    uint8_t data;
    X_ASSERT(self);

    data = self->data[self->first];
    self->assigner(&self->first, XFIFO__ADD_FIRST(1));
    return data;
}


void xfifo_push_back_n(XFifoBuffer* self, const void* src, size_t ssize)
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


void xfifo_pop_front_n(XFifoBuffer* self, void* dst, size_t dsize)
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


#endif /* ifdef X_COMPILER_NO_INLINE */
