/**
 *       @file  xcircular_buffer.h
 *      @brief
 *
 *    @details
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2016/05/23
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


#ifndef picox_container_xcircular_buffer_h_
#define picox_container_xcircular_buffer_h_


#include <picox/core/xcore.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#define XCBUF__CAPACITY()    ((size_t)(self->m_end - self->m_buff))
#define XCBUF__ADD(p, n)     ((p) + ((n) < (size_t)(self->m_end - (p)) ? (n) : (n) - XCBUF__CAPACITY()))
#define XCBUf__SUB(p, n)     ((p) - ((n) > ((p) - self->m_buff) ? (n) - XCBUF__CAPACITY() : (n)))
#define XCBUF__DECREMENT(p)  do { if (p == self->m_buff) p = self->m_end; --p; } while (0)
#define XCBUF__INCREMENT(p)  do { if (++p == self->m_end) p = self->m_buff; } while (0)


typedef struct
{
/** @privatesection */
    uint8_t*    m_buff;
    uint8_t*    m_end;
    uint8_t*    m_first;
    uint8_t*    m_last;
    size_t      m_size;
    bool        m_is_heapdata;
} XCircularBuffer;


static inline bool
xcbuf_init(XCircularBuffer* self, void* buffer, size_t capacity)
{
    if (!buffer)
    {
        buffer = (uint8_t*)x_malloc(capacity);
        if (!buffer)
            return false;
        self->m_is_heapdata = true;
    }
    self->m_buff = (uint8_t*)buffer;
    self->m_end = self->m_buff + capacity;
    self->m_first = self->m_last = self->m_buff;
    self->m_size = 0;
}


static inline void
xcbuf_deinit(XCircularBuffer* self)
{
    if (self->m_is_heapdata)
        x_free(self->m_buff);
    self->m_is_heapdata = false;
    self->m_buff = NULL;
}


static inline void
xcbuf_clear(XCircularBuffer* self)
{
    self->m_first = self->m_last = self->m_buff;
    self->m_size = 0;
}


static inline uint8_t*
xcbuf_data(XCircularBuffer* self)
{
    return self->m_buff;
}


static inline const uint8_t*
xcbuf_const_data(const XCircularBuffer* self)
{
    return self->m_buff;
}


static inline const uint8_t*
xcbuf_array_one(const XCircularBuffer* self, size_t* o_size)
{
    const uint8_t* p = ((self->m_last <= self->m_first) && (self->m_size != 0)) ?
                         self->m_end : self->m_last;
    *o_size = p - self->m_first;
    return self->m_first;
}


static inline const uint8_t*
xcbuf_array_two(const XCircularBuffer* self, size_t* o_size)
{
    *o_size = ((self->m_last <= self->m_first) && (self->m_size != 0)) ?
               self->m_last - self->m_buff : 0;
    return self->m_buff;
}


static inline uint8_t*
xcbuf_linearize(XCircularBuffer* self)
{
    if (self->m_size == 0)
        return NULL;

    if ((self->m_first < self->m_last) || (self->m_last == self->m_buff))
        return self->m_first;

    uint8_t* src = self->m_first;
    uint8_t* dest = self->m_buff;
    size_t moved = 0;
    uint8_t* first;
    uint8_t tmp;
    size_t i;
    for (first = self->m_first; dest < src; src = first)
    {
        for (i = 0; src < self->m_end; ++src, ++dest, ++moved, ++i)
        {
            if (moved == self->m_size)
            {
                first = dest;
                break;
            }
            if (dest == first)
            {
                first += i;
                break;
            }
            tmp = *src;
            *src = *dest;
            *dest = tmp;
        }
    }

    self->m_first = self->m_buff;
    self->m_last = XCBUF__ADD(self->m_buff, self->m_size);
    return self->m_buff;
}


static inline bool
xcbuf_is_linearized(const XCircularBuffer* self)
{
    return (self->m_first < self->m_last) || (self->m_last == self->m_buff);
}


static inline size_t
xcbuf_size(const XCircularBuffer* self)
{
    return self->m_size;
}


static inline bool
xcbuf_empty(const XCircularBuffer* self)
{
    return self->m_size == 0;
}


static inline size_t
xcbuf_capacity(const XCircularBuffer* self)
{
    return XCBUF__CAPACITY();
}


static inline bool
xcbuf_full(const XCircularBuffer* self)
{
    return XCBUF__CAPACITY() == self->m_size;
}


static inline size_t
xcbuf_reserve(const XCircularBuffer* self)
{
    return XCBUF__CAPACITY() - self->m_size;
}


static inline void
xcbuf_push_back(XCircularBuffer* self, uint8_t value)
{
    if (xcbuf_full(self))
    {
        *(self->m_last) = value;
        XCBUF__INCREMENT(self->m_last);
        self->m_first = self->m_last;
    }
    else
    {
        *(self->m_last) = value;
        XCBUF__INCREMENT(self->m_last);
        self->m_size++;
    }
}


static inline void
xcbuf_push_back_n(XCircularBuffer* self, const void* src, size_t n)
{
    const size_t reserved = xcbuf_reserve(self);
    const size_t nn = (reserved >= n) ? n : reserved;
    size_t i;
    const uint8_t* p = (const uint8_t*)src;

    for (i = 0; i < nn; i++)
    {
        *(self->m_last) = *p++;
        XCBUF__INCREMENT(self->m_last);
        self->m_size++;
    }

    for (; i < n; i++)
    {
        *(self->m_last) = *p++;
        XCBUF__INCREMENT(self->m_last);
        self->m_first = self->m_last;
    }
}


static inline void
xcbuf_push_front(XCircularBuffer* self, uint8_t value)
{
    if (xcbuf_full(self))
    {
        XCBUF__DECREMENT(self->m_first);
        *(self->m_first) = value;
        self->m_last = self->m_first;
    }
    else
    {
        XCBUF__DECREMENT(self->m_first);
        *(self->m_first) = value;
        self->m_size++;
    }
}


static inline void
xcbuf_push_front_n(XCircularBuffer* self, const void* src, size_t n)
{
    const size_t reserved = xcbuf_reserve(self);
    const size_t nn = (reserved >= n) ? n : reserved;
    const uint8_t* p = (const uint8_t*)(src);
    size_t i;

    for (i = 0; i < nn; i++)
    {
        XCBUF__DECREMENT(self->m_first);
        *(self->m_first) = *p++;
        self->m_size++;
    }

    for (; i < n; i++)
    {
        XCBUF__DECREMENT(self->m_first);
        *(self->m_first) = *p++;
        self->m_last = self->m_first;
    }
}


static inline uint8_t
xcbuf_pop_back(XCircularBuffer* self)
{
    XCBUF__DECREMENT(self->m_last);
    const uint8_t ret = *(self->m_last);
    self->m_size--;
    return ret;
}


static inline uint8_t
xcbuf_pop_front(XCircularBuffer* self)
{
    const uint8_t ret = *(self->m_first);
    XCBUF__INCREMENT(self->m_first);
    self->m_size--;
    return ret;
}


static inline void
xcbuf_pop_front_n(XCircularBuffer* self, void* dst, size_t n)
{
    size_t i;
    uint8_t* p;
    if (dst)
    {
        p = (uint8_t*)dst;
        for (i = 0; i < n; i++)
        {
            *p++ = *(self->m_first);
            XCBUF__INCREMENT(self->m_first);
        }
    }
    else
    {
        for (i = 0; i < n; i++)
        {
            XCBUF__INCREMENT(self->m_first);
        }
    }
    self->m_size -= n;
}


static inline void
xcbuf_pop_back_n(XCircularBuffer* self, void* dst, size_t n)
{
    size_t i;
    uint8_t* p;
    if (dst)
    {
        p = (uint8_t*)dst;
        for (i = 0; i < n; i++)
        {
            XCBUF__DECREMENT(self->m_last);
            *p++ = *(self->m_last);
        }
    }
    else
    {
        for (i = 0; i < n; i++)
        {
            XCBUF__DECREMENT(self->m_last);
        }
    }
    self->m_size -= n;
}


static inline void
xcbuf_copy_to_mem(const XCircularBuffer* self, size_t pos, void* dst, size_t n)
{
    const uint8_t* src = XCBUF__ADD(self->m_first, pos);
    uint8_t* p = (uint8_t*)dst;
    size_t i;
    for (i = 0; i < n; i++)
    {
        *p++ = *src;
        XCBUF__INCREMENT(src);
    }

}


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* picox_container_xcircular_buffer_h_ */
