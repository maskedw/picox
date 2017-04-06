/**
 *       @file  xuart.c
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


#include <picox/hal/xuart.h>


#define X__HAS_VFUNC(uart, func)   (X_LIKELY(uart->m_vtable->m_##func##_func))
#define X__VFUNC(uart, func)  (uart->m_vtable->m_##func##_func)

static XError X__UnsafeWrite(XUart* self, const void* src, size_t size);
static XError X__UnsafeRead(XUart* self, void* dst, size_t size, size_t* nread, XTicks timeout);


static const XStreamVTable X__uart_stream_vtable = {
    .m_name = "XUartStream",
    .m_read_func = (XStreamReadFunc)X__UnsafeRead,
    .m_write_func = (XStreamWriteFunc)X__UnsafeWrite,
    .m_flush_func = (XStreamFlushFunc)xuart_flush,
};
X_IMPL_RTTI_TAG(XUART_STREAM_RTTI_TAG);


void xuart_init(XUart* self)
{
    X_ASSERT(self);
    X_RESET_RTTI(self);
}


void xuart_init_config(XUartConfig* config)
{
    X_ASSERT(config);
    config->baudrate = 115200;
    config->parity = XUART_PARITY_NONE;
    config->stopbits = XUART_STOPBITS_ONE;
    config->flow_control = XUART_FLOW_CONTROL_NONE;
}


XStream* xuart_init_stream(XUart* self, XStream* stream)
{
    X_ASSERT(self);
    X_ASSERT(stream);

    xstream_init(stream);
    stream->m_rtti_tag = &XUART_STREAM_RTTI_TAG;
    stream->m_driver = self;
    stream->m_vtable = &X__uart_stream_vtable;

    return stream;
}


XError xuart_set_config(XUart* self, const XUartConfig* config)
{
    XError err;
    X_ASSERT(self);
    X_ASSERT(config);

    if (!X__HAS_VFUNC(self, set_config))
        return X_ERR_NOT_SUPPORTED;

    err = X__VFUNC(self, set_config)(self->m_driver, config);
    return err;
}


void xuart_get_config(const XUart* self, XUartConfig* config)
{
    X_ASSERT(self);
    X_ASSERT(config);
    X_ASSERT(X__VFUNC(self, get_config));

    X__VFUNC(self, get_config)(self->m_driver, config);
}


XError xuart_write(XUart* self, const void* src, size_t size)
{
    XError err;
    X_ASSERT(self);
    X_ASSERT(src);

    if (!X__HAS_VFUNC(self, write))
        return X_ERR_NOT_SUPPORTED;

    err = X__VFUNC(self, write)(self->m_driver, src, size);
    return err;
}


XError xuart_read(XUart* self, void* dst, size_t size, size_t* nread, XTicks timeout)
{
    XError err;
    X_ASSERT(self);
    X_ASSERT(dst);
    X_ASSERT(nread);

    if (!X__HAS_VFUNC(self, read))
        return X_ERR_NOT_SUPPORTED;

    err = X__VFUNC(self, read)(self->m_driver, dst, size, nread, timeout);
    return err;
}


XError xuart_read_poll(XUart* self, void* dst, size_t size, size_t* nread)
{
    return xuart_read(self, dst, size, nread, 0);
}


void xuart_flush(XUart* self)
{
    X_ASSERT(self);

    if (!X__HAS_VFUNC(self, flush))
        return;

    X__VFUNC(self, flush)(self->m_driver);
}


void xuart_drain(XUart* self)
{
    X_ASSERT(self);

    if (!X__HAS_VFUNC(self, drain))
        return;

    X__VFUNC(self, drain)(self->m_driver);
}


void xuart_clear(XUart* self, XUartDirection direction)
{
    X_ASSERT(self);

    if (!X__HAS_VFUNC(self, clear))
        return;

    X__VFUNC(self, clear)(self->m_driver, direction);
}


int xuart_putc(XUart* self, int c)
{
    uint8_t b = c;
    const XError err = xuart_write(self, &b, sizeof(b));

    return err == X_ERR_NONE ? c : EOF;
}


int xuart_getc(XUart* self)
{
    uint8_t b;
    size_t nread;
    const XError err = xuart_read_poll(self, &b, sizeof(b), &nread);

    return ((err == X_ERR_NONE) && (nread == 1)) ? b : EOF;
}


int xuart_printf(XUart* self, const char* fmt, ...)
{
    va_list args;
    int len;

    va_start(args, fmt);
    len = xuart_vprintf(self, fmt, args);
    va_end(args);
    return len;
}


int xuart_vprintf(XUart* self, const char* fmt, va_list args)
{
    XStream stream;
    xuart_init_stream(self, &stream);

    return x_vprintf_to_stream(&stream, fmt, args);
}


XError xuart_set_baudrate(XUart* self, uint32_t baudrate)
{
    XError err;
    XUartConfig config;

    xuart_get_config(self, &config);
    config.baudrate = baudrate;

    err = xuart_set_config(self, &config);
    return err;
}


uint32_t xuart_baudrate(const XUart* self)
{
    XUartConfig config;

    xuart_get_config(self, &config);
    return config.baudrate;
}


XError xuart_set_parity(XUart* self, XUartParity parity)
{
    XError err;
    XUartConfig config;

    xuart_get_config(self, &config);
    config.parity = parity;

    err = xuart_set_config(self, &config);
    return err;
}


XUartParity xuart_parity(const XUart* self)
{
    XUartConfig config;

    xuart_get_config(self, &config);
    return config.parity;
}


XError xuart_set_stopbits(XUart* self, XUartStopbits stopbits)
{
    XError err;
    XUartConfig config;

    xuart_get_config(self, &config);
    config.stopbits = stopbits;

    err = xuart_set_config(self, &config);
    return err;
}


XUartStopbits xuart_stopbits(const XUart* self)
{
    XUartConfig config;

    xuart_get_config(self, &config);
    return config.stopbits;
}


XError xuart_set_flow_control(XUart* self, XUartFlowControl flow_control)
{
    XError err;
    XUartConfig config;

    xuart_get_config(self, &config);
    config.flow_control = flow_control;

    err = xuart_set_config(self, &config);
    return err;
}


XUartFlowControl xuart_flow_control(const XUart* self)
{
    XUartConfig config;

    xuart_get_config(self, &config);
    return config.flow_control;
}


static XError X__UnsafeWrite(XUart* self, const void* src, size_t size)
{
    return X__VFUNC(self, write)(self->m_driver, src, size);
}


static XError X__UnsafeRead(XUart* self, void* dst, size_t size, size_t* nread, XTicks timeout)
{
    return X__VFUNC(self, read)(self->m_driver, dst, size, nread, timeout);
}
