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


static XError X__NotSupportedFunc(void);
static void X__AbortFunc(void);


void xpwm_init(XPwm* pwm)
{
    pwm->driver = NULL;
    pwm->configure_func = (XPwmConfigureFunc)X__NotSupportedFunc;
    pwm->start_func = (XPwmStartFunc)X__AbortFunc;
    pwm->stop_func = (XPwmStopFunc)X__AbortFunc;
}


void xpwm_config_init(XPwmConfig* config)
{
    config->frequency = 0;
    config->duty_x100 = 0;
    config->polarity = XPWM_POLARITY_ACTIVE_HIGH;
}


XError xpwm_configure(XPwm* pwm, const XPwmConfig* config)
{
    return pwm->configure_func(pwm->driver, config);
}


void xpwm_start(XPwm* pwm)
{
    pwm->start_func(pwm->driver);
}


void xpwm_stop(XPwm* pwm)
{
    pwm->stop_func(pwm->driver);
}


static XError X__NotSupportedFunc(void)
{
    return X_ERR_NOT_SUPPORTED;
}


static void X__AbortFunc(void)
{
    X_ASSERT(0);
}
