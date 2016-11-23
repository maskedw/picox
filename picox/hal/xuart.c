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


static int X__FlashDrain(void* driver);
static void X__NullFunc(void);
static XError X__NotSupportedFunc(void);


void xuart_init(XUart* uart)
{
    uart->driver = NULL;
    uart->configure_func = (XUartConfigureFunc)X__NotSupportedFunc;
    uart->read_func = (XUartReadFunc)X__NotSupportedFunc;
    uart->write_func = (XUartWriteFunc)X__NotSupportedFunc;
    uart->flush_func = (XUartFlushFunc)X__NullFunc;
    uart->clear_func = (XUartClearFunc)X__NullFunc;
}


void xuart_config_init(XUartConfig* config)
{
    config->baudrate = 115200;
    config->parity = XUART_PARITY_NONE;
    config->stopbits = XUART_STOPBITS_ONE;
    config->flow_control = XUART_FLOW_CONTROL_NONE;
}


void xuart_stream_init(XUart* uart, XStream* st)
{
    xstream_init(st);
    st->driver = uart->driver;
    st->write_func = (XStreamWriteFunc)uart->write_func;
    st->read_func = (XStreamReadFunc)xuart_read_poll;
    st->flush_func = (XStreamFlushFunc)X__FlashDrain;
}


XError xuart_configure(XUart* uart, const XUartConfig* config)
{
    return uart->configure_func(uart->driver, config);
}


XError xuart_write(XUart* uart, const void* src, size_t size)
{
    return uart->write_func(uart->driver, src, size);
}


XError xuart_read(XUart* uart, void* dst, size_t size, size_t* nread, XTicks timeout)
{
    return uart->read_func(uart->driver, dst, size, nread, timeout);
}


XError xuart_read_poll(XUart* uart, void* dst, size_t size, size_t* nread)
{
    return uart->read_func(uart->driver, dst, size, nread, 0);
}


void xuart_flush(XUart* uart, bool drain)
{
    uart->flush_func(uart->driver, drain);
}


void xuart_clear(XUart* uart, XUartDirection direction)
{
    uart->clear_func(uart->driver, direction);
}


int xuart_putc(XUart* uart, int c)
{
    XStream st;
    st.driver = uart->driver;
    st.write_func = (XStreamWriteFunc)uart->write_func;
    return xstream_putc(&st, c);
}


int xuart_getc(XUart* uart)
{
    XStream st;
    st.driver = uart->driver;
    st.read_func = (XStreamReadFunc)xuart_read_poll;
    return xstream_getc(&st);
}


int xuart_printf(XUart* uart, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    const int len = xuart_vprintf(uart, fmt, args);
    va_end(args);
    return len;
}


int xuart_vprintf(XUart* uart, const char* fmt, va_list args)
{
    XStream st;
    st.driver = uart->driver;
    st.write_func = (XStreamWriteFunc)uart->write_func;
    return x_vprintf_to_stream(&st, fmt, args);
}


static int X__FlashDrain(void* driver)
{
    XUart* const uart = X_CONTAINER_OF(driver, XUart, driver);
    uart->flush_func(driver, true);
    return 0;
}


static void X__NullFunc(void)
{
}


static XError X__NotSupportedFunc(void)
{
    return X_ERR_NOT_SUPPORTED;
}