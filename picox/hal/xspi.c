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


static XError X__UnsafeTransfer(XSpi* self, const XSpiTransaction* transactions, int num);


void xspi_init(XSpi* self)
{
    X_ASSERT(self);
    X_RESET_RTTI(self);
}


void xspi_init_config(XSpiConfig* config)
{
    config->frequency = 100 * 1024;
    config->mode = XSPI_MODE_0;
    config->bitorder = XSPI_BITORDER_MSB_FIRST;
    config->cs_polarity = XSPI_CS_POLARITY_ACTIVE_LOW;
    config->bitwidth = 8;
}


XError xspi_configure(XSpi* self, const XSpiConfig* config)
{
    XError err;
    X_ASSERT(self);
    X_ASSERT(config);

    if (!X__HAS_VFUNC(self, configure))
        return X_ERR_NOT_SUPPORTED;

    err = X__VFUNC(self, configure)(self->m_driver, config);
    return err;
}


XError xspi_transfer(XSpi* self, const XSpiTransaction* transactions, int num)
{
    XError err;
    X_ASSERT(self);
    X_ASSERT(transactions);
    X_ASSERT(num > 0);

    if (!X__HAS_VFUNC(self, transfer))
        return X_ERR_NOT_SUPPORTED;

    err = X__VFUNC(self, transfer)(self->m_driver, transactions, num);
    return err;
}


XError xspi_write(XSpi* self, const void* src, size_t size)
{
    XSpiTransaction transaction = XSPI_TRANSACTION_INITIALIZER(src, NULL, size, 0, 0);
    return X__UnsafeTransfer(self, &transaction, 1);
}


XError xspi_read(XSpi* self, void* dst, size_t size)
{
    XSpiTransaction transaction = XSPI_TRANSACTION_INITIALIZER(NULL, dst, size, 0, 0);
    return X__UnsafeTransfer(self, &transaction, 1);
}


XError xspi_write_byte(XSpi* self, uint8_t b)
{
    XSpiTransaction transaction = XSPI_TRANSACTION_INITIALIZER(&b, NULL, 1, 0, 0);
    return X__UnsafeTransfer(self, &transaction, 1);
}


XError xspi_read_byte(XSpi* self, uint8_t* b)
{
    XSpiTransaction transaction = XSPI_TRANSACTION_INITIALIZER(NULL, b, 1, 0, 0);
    return X__UnsafeTransfer(self, &transaction, 1);
}


XError xspi_exchange(XSpi* self, void* dst, const void* src, size_t size)
{
    XSpiTransaction transaction = XSPI_TRANSACTION_INITIALIZER(src, dst, size, 0, 0);
    return X__UnsafeTransfer(self, &transaction, 1);
}


static XError X__UnsafeTransfer(XSpi* self, const XSpiTransaction* transactions, int num)
{
    XError err;
    if (!X__HAS_VFUNC(self, transfer))
        return X_ERR_NOT_SUPPORTED;

    err = X__VFUNC(self, transfer)(self->m_driver, transactions, num);
    return err;
}
