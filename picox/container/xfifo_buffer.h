/**
 *       @file  xfifo_buffer.h
 *      @brief  First In First Out�`���̃o�b�t�@
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


#include <picox/core/xcore.h>


#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


typedef void(*XFifoBufferAtomicAssigner)(size_t* dst, size_t value);


typedef struct XFifoBuffer
{
/// @privatesection
    uint8_t*             data;
    size_t               first;
    size_t               last;
    size_t               capacity;
    XFifoBufferAtomicAssigner  assigner;
} XFifoBuffer;


/* ���������p�̃}�N�� */
#define XFIFO__ADD_FIRST(x)          ((self->first + x) & self->capacity)
#define XFIFO__ADD_LAST(x)           ((self->last  + x) & self->capacity)
#define XFIFO__IS_POWER_OF_TWO(x)    (((x) & -(x)) == (x))


static inline void
XFifoBufferDefaultAtomicAssign(size_t* dst, size_t value)
{
    *dst = value;
}


/** �o�b�t�@�����������܂��B
 *  @pre
 *  + (self != NULL) && (buffer != NULL)
 *  + buffer��size�o�C�g�̘A���̈���w���Ă��邱�ƁB
 *  + size��2�ׂ̂���ł��邱�ƁB
 *
 *  @note
 *  [XFifoBufferAtomicAssigner�ɂ���]
 *
 *  xfifo_buffer�͎�ɃV���A���ʐM�̑���M�o�b�t�@�Ɏg�p���邱�Ƃ�z�肵�Ă���A
 *  �ȉ��̏����̎��A���荞�݋֎~��ԂȂ��Ńo�b�t�@�ւ̏������݁A�ǂݏo�����\��
 *  ���B
 *
 *  �������݁A�ǂݏo���̃R���e�L�X�g���قȂ邱�Ƃ��ۏ؂���Ă��邱�ƁB
 *  ------------------------------------------------------------
 *
 *  [��]
 *  + �ʏ펞�Ƀo�b�t�@�Ƀf�[�^���������݁AUART���M�������荞�݂ŁA�o�b�t�@����f�[�^�����o��
 *  + �ʏ펞�Ƀo�b�t�@����f�[�^��ǂݏo���AUART��M�������荞�݂ŁA�o�b�t�@�Ƀf�[�^����������
 *
 *  size_t�^�̕ϐ��͕����Ȃ��ő���ł��邱�ƁB
 *  ------------------------------------------------------------
 *
 *  �ʏ��16bitCPU��32bit�ϐ��ɑ�����悤�Ƃ���ƁA�@�B�ꃌ�x���ł͕�����̑��
 *  ���߂�K�v�Ƃ��܂��B���̏ꍇ�Axfifo_buffer��RW�|�C���^�̐��������ۏ؂ł��Ȃ�
 *  �Ȃ�܂��B
 *
 *  ��L�������ۏ؂ł��Ȃ��ꍇ�́AXFifoBufferAtomicAssigner���w�肵�A���̊֐����ŁA��
 *  �荞�݂̃��b�N�A�ϐ��ւ̑���A���荞�݂̃A�����b�N���s���Ă��������B
 */
static inline void
xfifo_init(XFifoBuffer* self, void* buffer, size_t size, XFifoBufferAtomicAssigner assigner)
{
    X_ASSERT(self && buffer);
    X_ASSERT(XFIFO__IS_POWER_OF_TWO(size) && "Require Power of 2");
    X_ASSERT(size > 0);

    self->data = buffer;
    self->first = self->last = 0;
    self->capacity = size - 1;

    if (! assigner)
        self->assigner = XFifoBufferDefaultAtomicAssign;
    else
        self->assigner = assigner;
}


/** �o�b�t�@����ɂ��܂��B
 */
static inline void
xfifo_clear(XFifoBuffer* self)
{
    self->last = self->first;
}


/** �i�[�v�f����0���ǂ�����Ԃ��܂��B
 */
static inline bool
xfifo_empty(const XFifoBuffer* self)
{
    return self->last == self->first;
}


/** �ő�i�[�v�f����Ԃ��܂��B
 */
static inline size_t
xfifo_capacity(const XFifoBuffer* self)
{
    return self->capacity;
}

/** �i�[�v�f����Ԃ��܂��B
 */
static inline size_t
xfifo_size(const XFifoBuffer* self)
{
    return (self->last - self->first) & self->capacity;
}


/** �v�f����������ǂ�����Ԃ��܂��B
 */
static inline bool
xfifo_full(const XFifoBuffer* self)
{
    return xfifo_size(self) == xfifo_capacity(self);
}


/** �󂫗v�f����Ԃ��܂��B
 */
static inline size_t
xfifo_reserve(const XFifoBuffer* self)
{
    return xfifo_capacity(self) - xfifo_size(self);
}


/** �v�f���i�[����o�b�t�@��Ԃ��܂��B
 */
static inline void*
xfifo_data(const XFifoBuffer* self)
{
    return self->data;
}


/** FIFO�����ɗv�f��ǉ����܂��B
 */
static inline void
xfifo_push(XFifoBuffer* self, uint8_t data)
{
    self->data[self->last] = data;
    self->assigner(&self->last, XFIFO__ADD_LAST(1));
}


/** FIFO�擪����v�f�����o���܂��B
 */
static inline uint8_t
xfifo_pop(XFifoBuffer* self)
{
    const uint8_t data = self->data[self->first];
    self->assigner(&self->first, XFIFO__ADD_FIRST(1));

    return data;
}


/** FIFO�����ɗv�f��ǉ����܂��B
 */
static inline size_t
xfifo_write(XFifoBuffer* self, const void* src, size_t ssize)
{
    const size_t reserve = xfifo_reserve(self);

    if ((reserve <= 0) || (ssize <= 0) || (src == NULL))
        return 0;

    /* ��������(�������߂�)�v�f�� */
    size_t to_write = (reserve >= ssize) ? ssize : reserve;

    /* to_write�͌��Z�����\��������̂ŕۑ����Ă����B*/
    const size_t    written      = to_write;
    volatile size_t wpos         = self->last;
    const size_t    until_tail   = xfifo_capacity(self) - wpos + 1;

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


/** FIFO�擪����v�f�����o���܂��B
 */
static inline size_t
xfifo_read(XFifoBuffer* self, void* dst, size_t dsize)
{
    const size_t size = xfifo_size(self);

    if ((size <= 0) || (dsize <= 0) || (dst == NULL))
        return 0;

    /* �ǂݍ���(�ǂݍ��߂�)�v�f�� */
    size_t to_read = (size >= dsize) ? dsize : size;

    /* to_read�͌��Z�����\��������̂ŕۑ����Ă����B */
    const size_t     read        = to_read;
    volatile size_t  rpos        = self->first;
    const size_t     until_tail  = xfifo_capacity(self) - rpos + 1;

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
