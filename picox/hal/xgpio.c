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


#define X__HAS_VFUNC(gpio, func)   (X_LIKELY(gpio->m_vtable->m_##func##_func))
#define X__VFUNC(gpio, func)  (gpio->m_vtable->m_##func##_func)


void xgpio_init(XGpio* self)
{
    X_ASSERT(self);
    X_RESET_RTTI(self);
}


XError xgpio_set_mode(XGpio* self, XGpioMode mode)
{
    XError err;
    X_ASSERT(self);

    if (!X__HAS_VFUNC(self, set_mode))
        return X_ERR_NOT_SUPPORTED;

    err = X__VFUNC(self, set_mode)(self->m_driver, mode);
    return err;
}


void xgpio_write(XGpio* self, bool value)
{
    X_ASSERT(self);
    X_ASSERT(X__VFUNC(self, write));

    X__VFUNC(self, write)(self->m_driver, value);
}


bool xgpio_read(const XGpio* self)
{
    X_ASSERT(self);
    X_ASSERT(X__VFUNC(self, read));

    return X__VFUNC(self, read)(self->m_driver);
}


XError xgpio_set_irq_handler(XGpio* self, XGpioEdge edge, XGpioIrqHandler handler, void* handler_arg)
{
    X_ASSERT(self);

    if (!X__HAS_VFUNC(self, set_irq_handler))
        return X_ERR_NOT_SUPPORTED;

    return X__VFUNC(self, set_irq_handler)(self->m_driver, edge, handler, handler_arg);
}


XError xgpio_set_irq_enabled(XGpio* self, bool enabled)
{
    X_ASSERT(self);

    if (!X__HAS_VFUNC(self, set_irq_enabled))
        return X_ERR_NOT_SUPPORTED;

    return X__VFUNC(self, set_irq_enabled)(self->m_driver, enabled);
}


void xgpio_set_low(XGpio* self)
{
    xgpio_write(self, XGPIO_LOW);
}


void xgpio_set_high(XGpio* self)
{
    xgpio_write(self, XGPIO_HIGH);
}


void xgpio_toggle(XGpio* self)
{
    xgpio_write(self, xgpio_read(self) ^ XGPIO_LOW);
}


bool xgpio_is_low(const XGpio* self)
{
    return xgpio_read(self) == XGPIO_LOW;
}


bool xgpio_is_high(const XGpio* self)
{
    return xgpio_read(self) == XGPIO_HIGH;
}
