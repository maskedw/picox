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


#define X__HAS_VFUNC(i2c, func)   (X_LIKELY(i2c->m_vtable->m_##func##_func))
#define X__VFUNC(i2c, func)  (i2c->m_vtable->m_##func##_func)


void xi2c_init(XI2c* self)
{
    X_ASSERT(self);
    X_RESET_RTTI(self);
}


XError xi2c_set_frequency(XI2c* self, uint32_t freq_hz)
{
    XError err;
    X_ASSERT(self);

    if (!X__HAS_VFUNC(self, set_frequency))
        return X_ERR_NOT_SUPPORTED;

    err = X__VFUNC(self, set_frequency)(self->m_driver, freq_hz);
    return err;
}


XError xi2c_read(XI2c* self, int addr, void* dst, size_t size)
{
    XError err;
    X_ASSERT(self);
    X_ASSERT(dst);

    if (!X__HAS_VFUNC(self, read))
        return X_ERR_NOT_SUPPORTED;

    err = X__VFUNC(self, read)(self->m_driver, addr, dst, size);
    return err;
}


XError xi2c_write(XI2c* self, int addr, const void* src, size_t size)
{
    XError err;
    X_ASSERT(self);
    X_ASSERT(src);

    if (!X__HAS_VFUNC(self, write))
        return X_ERR_NOT_SUPPORTED;

    err = X__VFUNC(self, write)(self->m_driver, addr, src, size);
    return err;
}


XError xi2c_read_byte(XI2c* self, int addr, uint8_t* b)
{
    return xi2c_read(self, addr, b, 1);
}


XError xi2c_write_byte(XI2c* self, int addr, uint8_t b)
{
    return xi2c_write(self, addr, &b, 1);
}


void xi2c_lock_bus(XI2c* self)
{
    X_ASSERT(self);
    if (!X__HAS_VFUNC(self, lock_bus))
        return;

    X__VFUNC(self, lock_bus)(self->m_driver, true);
}


void xi2c_unlock_bus(XI2c* self)
{
    X_ASSERT(self);
    if (!X__HAS_VFUNC(self, lock_bus))
        return;

    X__VFUNC(self, lock_bus)(self->m_driver, false);
}
