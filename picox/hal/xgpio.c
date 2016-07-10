/**
 *       @file  xgpio.c
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


#include <picox/hal/xgpio.h>


static XError X__NotSupportedFunc(void);
static void X__AbortFunc(void);


void xgpio_init(XGpio* gpio)
{
    gpio->driver = NULL;
    gpio->configure_func = (XGpioConfigureFunc)X__NotSupportedFunc;
    gpio->read_func = (XGpioReadFunc)X__AbortFunc;
    gpio->write_func = (XGpioWriteFunc)X__AbortFunc;
    gpio->attach_edge_handler_func = (XGpioAttachEdgeHandlerFunc)X__NotSupportedFunc;
}


void xgpio_config_init(XGpioConfig* config)
{
    config->direction = XGPIO_DIRECTION_INPUT;
    config->mode = XGPIO_MODE_PULLUP;
    config->initial_level = XGPIO_LEVEL_LOW;
}


XError xgpio_configure(XGpio* gpio, const XGpioConfig* config)
{
    return gpio->configure_func(gpio->driver, config);
}


void xgpio_write(XGpio* gpio, XGpioLevel level)
{
    gpio->write_func(gpio->driver, level);
}


XGpioLevel xgpio_read(XGpio* gpio)
{
    return gpio->read_func(gpio->driver);
}


void xgpio_toggle(XGpio* gpio)
{
    const int cur_value = (int)gpio->read_func(gpio->driver);
    gpio->write_func(gpio->driver, cur_value ^ 1);
}


XError xgpio_attach_edge_handler(XGpio* gpio, XGpioEdge edge, XGpioEdgeHandler handler)
{
    return gpio->attach_edge_handler_func(gpio->driver, edge, handler);
}



static XError X__NotSupportedFunc(void)
{
    return X_ERR_NOT_SUPPORTED;
}


static void X__AbortFunc(void)
{
    X_ASSERT(0);
}
