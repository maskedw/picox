/**
 *       @file  xi2c.c
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


#include <picox/hal/xi2c.h>


static XError X__NotSupportedFunc(void);


void xi2c_init(XI2c* i2c)
{
    i2c->driver = NULL;
    i2c->configure_func = (XI2cConfigureFunc)X__NotSupportedFunc;
    i2c->transfer_func = (XI2cTransferFunc)X__NotSupportedFunc;
}


void xi2c_config_init(XI2cConfig* config)
{
    config->frequency = 0;
    config->slave_address = 0;
    config->slave_address_length = XI2C_SLAVE_ADDRESS_LENGTH_7BIT;
    config->timeout = X_MSEC_TO_TICKS(1000);
}


XError xi2c_configure(XI2c* i2c, const XI2cConfig* config)
{
    return i2c->configure_func(i2c->driver, config);
}


XError xi2c_transfer(XI2c* i2c, const XI2cTransaction* transactions, int num)
{
    return i2c->transfer_func(i2c->driver, transactions, num);
}


XError xi2c_write(XI2c* i2c, const void* src, size_t size)
{
    XI2cTransaction transaction = XI2C_TRANSACTION_INITIALIZER(src, NULL, size, 0);
    return i2c->transfer_func(i2c->driver, &transaction, 1);
}


XError xi2c_read(XI2c* i2c, void* dst, size_t size)
{
    XI2cTransaction transaction = XI2C_TRANSACTION_INITIALIZER(NULL, dst, size, XI2C_TRANSACTION_FLAG_READ);
    return i2c->transfer_func(i2c->driver, &transaction, 1);
}


XError xi2c_write_byte(XI2c* i2c, uint8_t b)
{
    XI2cTransaction transaction = XI2C_TRANSACTION_INITIALIZER(&b, NULL, 1, 0);
    return i2c->transfer_func(i2c->driver, &transaction, 1);
}


XError xi2c_read_byte(XI2c* i2c, uint8_t* b)
{
    XI2cTransaction transaction = XI2C_TRANSACTION_INITIALIZER(NULL, b, 1, XI2C_TRANSACTION_FLAG_READ);
    return i2c->transfer_func(i2c->driver, &transaction, 1);
}


static XError X__NotSupportedFunc(void)
{
    return X_ERR_NOT_SUPPORTED;
}
