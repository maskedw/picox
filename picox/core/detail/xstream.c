/**
 *       @file  stream.c
 *      @brief
 *
 *    @details
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2016/01/04
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


#include <picox/core/xcore.h>


X_IMPL_RTTI_TAG(XMEMSTREAM_RTTI_TAG);
static const XStreamVTable X__memstream_vtable = {
    .m_name = "XMemStream",
    .m_read_func = (XStreamReadFunc)xmemstream_read,
    .m_write_func = (XStreamWriteFunc)xmemstream_write,
    .m_seek_func = (XStreamSeekFunc)xmemstream_seek,
    .m_tell_func = (XStreamTellFunc)xmemstream_tell
};


void xstream_init(XStream* self)
{
    X_ASSERT(self);

    X_RESET_RTTI(self);
    self->m_error = X_ERR_NONE;
}


int xstream_read(XStream* self, void* dst, size_t size, size_t* nread)
{
    X_ASSERT(self);
    X_ASSERT(dst);

    if (!self->m_vtable->m_read_func)
    {
        self->m_error = X_ERR_NOT_SUPPORTED;
        goto finish;
    }

    self->m_error = self->m_vtable->m_read_func(self->m_driver, dst, size, nread);
finish:

    return self->m_error;
}


int xstream_write(XStream* self, const void* src, size_t size, size_t* nwritten)
{
    X_ASSERT(self);
    X_ASSERT(src);

    if (!self->m_vtable->m_write_func)
    {
        self->m_error = X_ERR_NOT_SUPPORTED;
        goto finish;
    }

    self->m_error = self->m_vtable->m_write_func(self->m_driver, src, size, nwritten);
finish:

    return self->m_error;
}


int xstream_close(XStream* self)
{
    X_ASSERT(self);

    self->m_error = xstream_flush(self);
    if (self->m_error != X_ERR_NONE)
        goto finish;

    if (!self->m_vtable->m_close_func)
    {
        self->m_error = X_ERR_NONE;
        goto finish;
    }

    self->m_error = self->m_vtable->m_close_func(self->m_driver);
finish:

    return self->m_error;
}


int xstream_flush(XStream* self)
{
    X_ASSERT(self);

    if (!self->m_vtable->m_flush_func)
    {
        self->m_error = X_ERR_NONE;
        goto finish;
    }

    self->m_error = self->m_vtable->m_flush_func(self->m_driver);
finish:

    return self->m_error;
}


int xstream_seek(XStream* self, XOffset offset, XSeekMode mode)
{
    X_ASSERT(self);

    if (!x_is_within(mode, X_SEEK_SET, X_SEEK_END + 1))
    {
        self->m_error = X_ERR_INVALID;
        goto finish;
    }

    if (!self->m_vtable->m_seek_func)
    {
        self->m_error = X_ERR_NOT_SUPPORTED;
        goto finish;
    }

    self->m_error = self->m_vtable->m_seek_func(self->m_driver, offset, mode);
finish:

    return self->m_error;
}


int xstream_tell(XStream* self, XSize* pos)
{
    X_ASSERT(self);
    X_ASSERT(pos);

    if (!self->m_vtable->m_tell_func)
    {
        self->m_error = X_ERR_NOT_SUPPORTED;
        goto finish;
    }

    self->m_error = self->m_vtable->m_tell_func(self->m_driver, pos);
finish:

    return self->m_error;
}


int xstream_error(const XStream* self)
{
    X_ASSERT(self);
    return self->m_error;
}


const char* xstream_error_string(const XStream* self, int errcode)
{
    X_ASSERT(self);
    if (!self->m_vtable->m_error_string_func)
        return x_strerror(errcode);

    return self->m_vtable->m_error_string_func(errcode);
}


int xstream_putc(XStream* self, int c)
{
    uint8_t byte = (uint8_t)c;
    size_t nwritten = 0;
    XStreamWriteFunc write_func = self->m_vtable->m_write_func;

    if (!write_func)
    {
        self->m_error = X_ERR_NOT_SUPPORTED;
        goto finish;
    }

    self->m_error = write_func(self->m_driver, &byte, sizeof(byte), &nwritten);
finish:

    return nwritten == 1 ? c : EOF;
}


int xstream_getc(XStream* self)
{
    uint8_t byte;
    size_t nread = 0;
    XStreamReadFunc read_func = self->m_vtable->m_read_func;

    if (!read_func)
    {
        self->m_error = X_ERR_NOT_SUPPORTED;
        goto finish;
    }

    self->m_error = read_func(self->m_driver, &byte, sizeof(byte), &nread);
finish:

    return nread == 1 ? byte : EOF;
}


int xstream_gets(XStream* self, char* dst, size_t size, char** result, bool* overflow)
{
    size_t total = 0;
    int c = '\0';

    X_ASSERT(self);
    X_ASSERT(dst);
    X_ASSERT(size > 1);

    dst[0] = '\0';

    while (total < size - 1)
    {
        c = xstream_getc(self);
        X_BREAK_IF(c == EOF);
        X_CONTINUE_IF(c == '\r');
        X_BREAK_IF(c == '\n');
        dst[total++] = (char)c;
    }

    dst[total] = '\0';
    X_ASSIGN_NOT_NULL(result, (dst[0] != '\0') ? dst : NULL);
    X_ASSIGN_NOT_NULL(overflow, (c != '\n') && (c != EOF));

    return self->m_error;
}


int xstream_printf(XStream* self, const char* fmt, ...)
{
    int len;
    va_list args;
    va_start(args, fmt);
    len = x_vprintf_to_stream(self, fmt, args);
    va_end(args);
    return (self->m_error == 0) ? len : -1;
}


int xstream_vprintf(XStream* self, const char* fmt, va_list args)
{
    const int len = x_vprintf_to_stream(self, fmt, args);
    return (self->m_error == 0) ? len : -1;
}


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
