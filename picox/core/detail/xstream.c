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


static int X__MemStreamWrite(XMemStream* self, const void* src, size_t size, size_t* nwritten);
static int X__MemStreamRead(XMemStream* self, void* dst, size_t size, size_t* nread);
static int X__MemStreamSeek(XMemStream* self, XOffset offset, XSeekMode mode);
static int X__MemStreamTell(XMemStream* self, XSize* pos);
static int X__GenericRead(void* ptr, void* dst, size_t size, size_t* nread);
static int X__GenericWrite(void* ptr, const void* src, size_t size, size_t* nwritten);
static int X__GenericSeek(void* ptr, XOffset offset, XSeekMode mode);
static int X__GenericTell(void* ptr, XSize* pos);
static int X__GenericFlush(void* ptr);
static int X__GenericClose(void* ptr);
static const char* X__GenericErrorString(int errcode);


void xstream_init(XStream* self)
{
    X_ASSERT(self);
    self->driver = self;
    self->error = 0;
    self->tag = X_STREAM_TAG;
    self->read_func = X__GenericRead;
    self->write_func = X__GenericWrite;
    self->close_func = X__GenericClose;
    self->flush_func = X__GenericFlush;
    self->seek_func = X__GenericSeek;
    self->tell_func = X__GenericTell;
    self->error_string_func = X__GenericErrorString;
}


int xstream_read(XStream* self, void* dst, size_t size, size_t* nread)
{
    X_ASSERT(self);
    X_ASSERT(dst);
    X_ASSERT(nread);
    const int ret = self->read_func(self->driver, dst, size, nread);
    self->error = ret;
    return ret;
}


int xstream_write(XStream* self, const void* src, size_t size, size_t* nwritten)
{
    X_ASSERT(self);
    X_ASSERT(src);
    X_ASSERT(nwritten);
    const int ret = self->write_func(self->driver, src, size, nwritten);
    self->error = ret;
    return ret;
}


int xstream_close(XStream* self)
{
    X_ASSERT(self);
    const int ret = self->close_func(self->driver);
    self->error = ret;
    return ret;
}


int xstream_flush(XStream* self)
{
    X_ASSERT(self);
    const int ret = self->flush_func(self->driver);
    self->error = ret;
    return ret;
}


int xstream_seek(XStream* self, XOffset offset, XSeekMode mode)
{
    X_ASSERT(self);
    X_ASSERT(x_is_within(mode, X_SEEK_SET, X_SEEK_END + 1));
    const int ret = self->seek_func(self->driver, offset, mode);
    self->error = ret;
    return ret;
}


int xstream_tell(XStream* self, XSize* pos)
{
    X_ASSERT(self);
    X_ASSERT(pos);
    const int ret = self->tell_func(self->driver, pos);
    self->error = ret;
    return ret;
}


int xstream_error(const XStream* self)
{
    X_ASSERT(self);
    return self->error;
}


const char* xstream_error_string(const XStream* self, int errcode)
{
    X_ASSERT(self);
    return self->error_string_func(errcode);
}


int xstream_putc(XStream* self, int c)
{
    bool ok;
    size_t nwritten;
    uint8_t byte = c;

    do
    {
        ok = (self->write_func(self->driver, &byte, sizeof(byte), &nwritten) == 0);
        X_BREAK_IF(! ok);
        ok = (nwritten == sizeof(byte));
    } while (0);

    return ok ? c : EOF;
}


int xstream_getc(XStream* self)
{
    bool ok;
    size_t nread;
    uint8_t byte;
    do
    {
        ok = (self->read_func(self->driver, &byte, sizeof(byte), &nread) == 0);
        X_BREAK_IF(! ok);
        ok = (nread == sizeof(byte));
    } while (0);

    return ok ? byte : EOF;
}


int xstream_gets(XStream* self, char* dst, size_t size, char** result, bool* overflow)
{
    X_ASSERT(self);
    X_ASSERT(dst);
    X_ASSERT(size > 1);

    size_t total = 0;
    int c = '\0';
    dst[0] = '\0';

    while (total < size - 1)
    {
        c = xstream_getc(self);
        X_BREAK_IF(c == EOF);
        X_CONTINUE_IF(c == '\r');
        X_BREAK_IF(c == '\n');
        dst[total++] = c;
    }

    dst[total] = '\0';
    X_ASSIGN_NOT_NULL(result, (dst[0] != '\0') ? dst : NULL);
    X_ASSIGN_NOT_NULL(overflow, (c != '\n') && (c != EOF));

    return self->error;
}


int xstream_printf(XStream* self, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    const int len = x_vprintf_to_stream(self, fmt, args);
    va_end(args);
    return (self->error == 0) ? len : -1;
}


int xstream_vprintf(XStream* self, const char* fmt, va_list args)
{
    const int len = x_vprintf_to_stream(self, fmt, args);
    return (self->error == 0) ? len : -1;
}


void xmemstream_init(XMemStream* self, void* mem, size_t size, size_t capacity)
{
    xstream_init(&self->stream);
    self->stream.driver = self;
    self->stream.tag = X_MEMSTREAM_TAG;
    self->stream.write_func = (XStreamWriteFunc)X__MemStreamWrite;
    self->stream.read_func = (XStreamReadFunc)X__MemStreamRead;
    self->stream.seek_func = (XStreamSeekFunc)X__MemStreamSeek;
    self->stream.tell_func = (XStreamTellFunc)X__MemStreamTell;
    self->mem = mem;
    self->pos = 0;
    self->size = size;
    self->capacity = capacity;
}


static int X__MemStreamWrite(XMemStream* self, const void* src, size_t size, size_t* nwritten)
{
    const size_t to_write = ((self->pos + size) <= self->capacity)
                            ? size : (self->capacity - self->pos);
    memcpy(self->mem + self->pos, src, to_write);
    self->pos += to_write;
    *nwritten = to_write;
    if (self->pos > self->size)
        self->size = self->pos;
    return 0;
}


static int X__MemStreamRead(XMemStream* self, void* dst, size_t size, size_t* nread)
{
    const size_t to_read = ((self->pos + size) <= self->size)
                            ? size : (self->size - self->pos);
    memcpy(dst, self->mem + self->pos, to_read);
    self->pos += to_read;
    *nread = to_read;
    return 0;
}


static int X__MemStreamSeek(XMemStream* self, XOffset pos, XSeekMode mode)
{
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

    X_ASSERT(seekpos >= 0);
    self->pos = seekpos;
    return 0;
}


static int X__MemStreamTell(XMemStream* self, XSize* pos)
{
    *pos = self->pos;
    return 0;
}


static int X__GenericRead(void* ptr, void* dst, size_t size, size_t* nread)
{
    X_UNUSED(ptr);
    X_UNUSED(dst);
    X_UNUSED(size);
    *nread = 0;
    return true;
}


static int X__GenericWrite(void* ptr, const void* src, size_t size, size_t* nwritten)
{
    X_UNUSED(ptr);
    X_UNUSED(src);
    X_UNUSED(size);
    *nwritten = 0;
    return true;
}


static int X__GenericSeek(void* ptr, XOffset offset, XSeekMode mode)
{
    X_UNUSED(ptr);
    X_UNUSED(offset);
    X_UNUSED(mode);
    return true;
}


static int X__GenericTell(void* ptr, XSize* pos)
{
    X_UNUSED(ptr);
    *pos = 0;
    return true;
}


static int X__GenericFlush(void* ptr)
{
    X_UNUSED(ptr);
    return true;
}


static int X__GenericClose(void* ptr)
{
    XStream* stream = (XStream*)ptr;
    stream->flush_func(stream->driver);
    return true;
}


static const char* X__GenericErrorString(int errcode)
{
    switch (errcode)
    {
        case 0:
            return "X_STREAM_ERR_NONE";
        default:
            break;
    }
    return "X_STREAM_ERR_UNKNOWN";
}
