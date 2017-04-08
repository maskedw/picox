/**
 *       @file  xpwm.c
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


#include <picox/hal/xpwm.h>


#define X__HAS_VFUNC(pwm, func)   (X_LIKELY(pwm->m_vtable->m_##func##_func))
#define X__VFUNC(pwm, func)  (pwm->m_vtable->m_##func##_func)


void xpwm_init(XPwm* self)
{
    X_ASSERT(self);
    X_RESET_RTTI(self);
}


void xpwm_write(XPwm* self, uint32_t freq_hz, uint16_t duty)
{
    X_ASSERT(self);
    X_ASSERT(X__VFUNC(self, write));

    X__VFUNC(self, write)(self->m_driver, freq_hz, duty);
}


void xpwm_stop(XPwm* self)
{
    X_ASSERT(self);
    X_ASSERT(X__VFUNC(self, write));

    X__VFUNC(self, write)(self->m_driver, 0, 0);

}


void xpwm_set_high(XPwm* self)
{
    xpwm_write(self, 1, XPWM_DUTY_MAX);
}


void xpwm_set_low(XPwm* self)
{
    xpwm_write(self, 1, 0);
}
