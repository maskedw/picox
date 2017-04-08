/**
 *       @file  xspi.c
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


#include <picox/hal/xspi.h>


#define X__HAS_VFUNC(spi, func)   (X_LIKELY(spi->m_vtable->m_##func##_func))
#define X__VFUNC(spi, func)  (spi->m_vtable->m_##func##_func)


void xspi_init(XSpi* self)
{
    X_ASSERT(self);
    X_RESET_RTTI(self);
}


XError xspi_set_format(XSpi* self, uint32_t freq_hz, XSpiMode mode, XSpiBitorder bitorder)
{
    XError err;
    X_ASSERT(self);
    X_ASSERT(X__VFUNC(self, set_format));

    err = X__VFUNC(self, set_format)(self->m_driver, freq_hz, mode, bitorder);
    return err;
}


void xspi_write(XSpi* self, const void* src, size_t size)
{
    xspi_exchange(self, src, NULL, size);
}


void xspi_read(XSpi* self, void* dst, size_t size)
{
    xspi_exchange(self, NULL, dst, size);
}


void xspi_write_byte(XSpi* self, uint8_t b)
{
    xspi_exchange(self, &b, NULL, 1);
}


uint8_t xspi_read_byte(XSpi* self)
{
    uint8_t b;
    xspi_exchange(self, NULL, &b, 1);

    return b;
}


void xspi_exchange(XSpi* self, const void* tx, void* rx, size_t size)
{
    X_ASSERT(self);
    X_ASSERT(X__VFUNC(self, exchange));

    X__VFUNC(self, exchange)(self->m_driver, tx, rx, size);
}


void xspi_lock_bus(XSpi* self)
{
    X_ASSERT(self);
    if (!X__HAS_VFUNC(self, lock_bus))
        return;

    X__VFUNC(self, lock_bus)(self->m_driver, true);
}


void xspi_unlock_bus(XSpi* self)
{
    X_ASSERT(self);
    if (!X__HAS_VFUNC(self, lock_bus))
        return;

    X__VFUNC(self, lock_bus)(self->m_driver, false);
}
