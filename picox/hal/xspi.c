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



static XError X__NotSupportedFunc(void);


void xspi_init(XSpi* spi)
{
    spi->driver = NULL;
    spi->configure_func = (XSpiConfigureFunc)X__NotSupportedFunc;
    spi->transfer_func = (XSpiTransferFunc)X__NotSupportedFunc;
}


void xspi_config_init(XSpiConfig* config)
{
    config->frequency = 0;
    config->mode = XSPI_MODE_0;
    config->bitorder = XSPI_BITORDER_MSB_FIRST;
    config->cs_polarity = XSPI_CS_POLARITY_ACTIVE_LOW;
    config->bitwidth = 8;
}


XError xspi_configure(XSpi* spi, const XSpiConfig* config)
{
    return spi->configure_func(spi->driver, config);
}


XError xspi_transfer(XSpi* spi, const XSpiTransaction* transactions, int num)
{
    return spi->transfer_func(spi->driver, transactions, num);
}


XError xspi_write(XSpi* spi, const void* src, size_t size)
{
    XSpiTransaction transaction = XSPI_TRANSACTION_INITIALIZER(src, NULL, size);
    return spi->transfer_func(spi->driver, &transaction, 1);
}


XError xspi_read(XSpi* spi, void* dst, size_t size)
{
    XSpiTransaction transaction = XSPI_TRANSACTION_INITIALIZER(NULL, dst, size);
    return spi->transfer_func(spi->driver, &transaction, 1);
}


XError xspi_write_byte(XSpi* spi, uint8_t b)
{
    XSpiTransaction transaction = XSPI_TRANSACTION_INITIALIZER(&b, NULL, 1);
    return spi->transfer_func(spi->driver, &transaction, 1);
}


XError xspi_read_byte(XSpi* spi, uint8_t* b)
{
    XSpiTransaction transaction = XSPI_TRANSACTION_INITIALIZER(NULL, b, 1);
    return spi->transfer_func(spi->driver, &transaction, 1);
}


XError xspi_exchange(XSpi* spi, void* dst, const void* src, size_t size)
{
    XSpiTransaction transaction = XSPI_TRANSACTION_INITIALIZER(src, dst, size);
    return spi->transfer_func(spi->driver, &transaction, 1);
}


static XError X__NotSupportedFunc(void)
{
    return X_ERR_NOT_SUPPORTED;
}
