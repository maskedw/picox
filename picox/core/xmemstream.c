/**
 *       @file  xmemstream.c
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


#include <picox/core/xmemstream.h>


X_IMPL_RTTI_TAG(XMEMSTREAM_RTTI_TAG);
static const XStreamVTable X__memstream_vtable = {
    .m_name = "XMemStream",
    .m_read_func = (XStreamReadFunc)xmemstream_read,
    .m_write_func = (XStreamWriteFunc)xmemstream_write,
    .m_seek_func = (XStreamSeekFunc)xmemstream_seek,
    .m_tell_func = (XStreamTellFunc)xmemstream_tell
};


XStream* xmemstream_init(XMemStream* self, void* mem, size_t size, size_t capacity)
{
    X_ASSERT(self);
    X_ASSERT(mem);

    xstream_init(&self->m_super);
    self->m_super.m_rtti_tag = &XMEMSTREAM_RTTI_TAG;
    self->m_super.m_driver = self;
    self->m_super.m_vtable = &X__memstream_vtable;

    self->mem = mem;
    self->pos = 0;
    self->size = size;
    self->capacity = capacity;

    return &self->m_super;
}


int xmemstream_write(XMemStream* self, const void* src, size_t size, size_t* nwritten)
{
    const size_t to_write = ((self->pos + size) <= self->capacity)
                            ? size : (size_t)(self->capacity - self->pos);

    X_ASSERT(x_rtti_equal(&(self->m_super), XMEMSTREAM_RTTI_TAG));

    if (to_write)
        memcpy(self->mem + self->pos, src, to_write);

    self->pos += to_write;
    *nwritten = to_write;
    if (self->pos > self->size)
        self->size = self->pos;

    return X_ERR_NONE;
}


int xmemstream_read(XMemStream* self, void* dst, size_t size, size_t* nread)
{
    const size_t to_read = ((self->pos + size) <= self->size)
                            ? size : (size_t)(self->size - self->pos);

    X_ASSERT(x_rtti_equal(&(self->m_super), XMEMSTREAM_RTTI_TAG));

    if (to_read)
        memcpy(dst, self->mem + self->pos, to_read);

    self->pos += to_read;
    *nread = to_read;

    return X_ERR_NONE;
}


int xmemstream_seek(XMemStream* self, XOffset pos, XSeekMode mode)
{
    X_ASSERT(x_rtti_equal(&(self->m_super), XMEMSTREAM_RTTI_TAG));

    XOffset seekpos = 0;
    switch (mode)
    {
        case X_SEEK_SET:
            seekpos = pos;
            break;

        case X_SEEK_CUR:
            seekpos = self->pos + pos;
            break;

        case X_SEEK_END:
            seekpos = self->size + pos;
            break;
        default:
            break;
    }

    if (seekpos < 0)
        return X_ERR_RANGE;
    if (seekpos > (XOffset)self->capacity)
        return X_ERR_RANGE;

    self->pos = seekpos;
    return X_ERR_NONE;
}


int xmemstream_tell(XMemStream* self, XSize* pos)
{
    X_ASSERT(x_rtti_equal(&(self->m_super), XMEMSTREAM_RTTI_TAG));

    *pos = self->pos;
    return X_ERR_NONE;
}
