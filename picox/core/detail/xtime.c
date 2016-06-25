/**
 *       @file  xtime.c
 *      @brief
 *
 *    @details
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2016/05/18
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


#include <picox/core/xcore.h>

#if (X_CONF_MSLEEP_IMPL_TYPE == X_MSLEEP_IMPL_TYPE_POSIX_NANOSLEEP) || \
    (X_CONF_USLEEP_IMPL_TYPE == X_USLEEP_IMPL_TYPE_POSIX_NANOSLEEP)

#include <time.h>
#include <errno.h>

#endif

#if X_CONF_GETTIMEOFDAY_IMPL_TYPE == X_GETTIMEOFDAY_IMPL_TYPE_POSIX
    #include <sys/time.h>
#endif


#if X_CONF_GETTIMEOFDAY_IMPL_TYPE != X_GETTIMEOFDAY_IMPL_TYPE_USERPORT

int x_port_gettimeofday(XTimeVal* tv, void* tz_dammy)
{
    X_UNUSED(tz_dammy);

#if X_CONF_GETTIMEOFDAY_IMPL_TYPE == X_GETTIMEOFDAY_IMPL_TYPE_ZERO

    tv->tv_sec = 0;
    tv->tv_usec = 0;
    return 0;

#elif X_CONF_GETTIMEOFDAY_IMPL_TYPE == X_GETTIMEOFDAY_IMPL_TYPE_POSIX

    struct timeval posix_tv;

    gettimeofday(&posix_tv, NULL);
    tv->tv_sec = posix_tv.tv_sec;
    tv->tv_usec = posix_tv.tv_usec;
    return 0;

#else

    #error invalid configuration value

#endif
}
#endif /* if X_CONF_GETTIMEOFDAY_IMPL_TYPE != X_GETIMEOFDAY_IMPL_TYPE_USERPORT */


#if X_CONF_TICKS_NOW_IMPL_TYPE != X_TICKS_NOW_IMPL_TYPE_USERPORT
XTicks x_port_ticks_now(void)
{
#if X_CONF_TICKS_NOW_IMPL_TYPE == X_TICKS_NOW_IMPL_TYPE_GETTIMEOFDAY

    const XTimeVal tv = x_gettimeofday2();
#ifndef X_COMPILER_NO_64BIT_INT
    const XTicks ret = (XTicks)((int64_t)(tv.tv_sec) * X_TICKS_PER_SEC + x_usec_to_ticks(tv.tv_usec));
#else
    const XTicks ret = (XTicks)((tv.tv_sec) * X_TICKS_PER_SEC + x_usec_to_ticks(tv.tv_usec));
#endif

#if X_CONF_GETTIMEOFDAY_IMPL_TYPE == X_GETTIMEOFDAY_IMPL_TYPE_ZERO
    /* X_GETTIMEOFDAY_IMPL_TYPE_ZEROは常に0を返すので、ticksが機能しない */
    X_ABORT("This combination will cause an infinite loop");
#endif

    return ret;

#else

    #error invalid configuration value

#endif
}
#endif /* if X_CONF_TICKS_NOW_IMPL_TYPE != X_TICKS_NOW_IMPL_USERPORT */


#if X_CONF_MSLEEP_IMPL_TYPE != X_MSLEEP_IMPL_TYPE_USERPORT
void x_port_msleep(XMSeconds msec)
{

#if X_CONF_MSLEEP_IMPL_TYPE == X_MSLEEP_IMPL_TYPE_MDELAY

    x_mdelay(msec);

#elif X_CONF_MSLEEP_IMPL_TYPE == X_MSLEEP_IMPL_TYPE_POSIX_NANOSLEEP

    struct timespec req;
    req.tv_sec = msec / 1000;
    msec %= 1000;
    req.tv_nsec = (uint64_t)msec * X_UINT32_C(1000000);

    while (nanosleep(&req, &req));

#else

    #error invalid configuration value

#endif
}
#endif /* if X_CONF_MSLEEP_IMPL_TYPE != X_MSLEEP_IMPL_TYPE_USERPORT */


#if X_CONF_USLEEP_IMPL_TYPE != X_USLEEP_IMPL_TYPE_USERPORT
void x_port_usleep(XUSeconds usec)
{

#if X_CONF_USLEEP_IMPL_TYPE == X_USLEEP_IMPL_TYPE_UDELAY

    x_udelay(usec);

#elif X_CONF_USLEEP_IMPL_TYPE == X_USLEEP_IMPL_TYPE_POSIX_NANOSLEEP

    struct timespec req;
    req.tv_sec = usec / X_UINT32_C(1000000);
    usec %= X_UINT32_C(1000000);
    req.tv_nsec = (uint64_t)usec * 1000;

    while (nanosleep(&req, &req));

#else

    #error invalid configuration value

#endif
}
#endif /* if X_CONF_USLEEP_IMPL_TYPE != X_USLEEP_IMPL_TYPE_USERPORT */


#if X_CONF_MDELAY_IMPL_TYPE != X_MDELAY_IMPL_TYPE_USERPORT
void x_port_mdelay(XMSeconds msec)
{

#if X_CONF_MDELAY_IMPL_TYPE == X_MDELAY_IMPL_TYPE_TICKS_BUSYWAIT

    const XTicks start = x_ticks_now();
    const XTicks end = x_msec_to_ticks(msec);
    for (;;)
    {
        const XTicks cur = x_ticks_now();
        if (cur - start >= end)
            break;
    }

#elif X_CONF_MDELAY_IMPL_TYPE == X_MDELAY_IMPL_TYPE_POSIX_NANOSLEEP

    struct timespec req;
    req.tv_sec = msec / 1000;
    msec %= 1000;
    req.tv_nsec = (uint64_t)msec * X_UINT32_C(1000000);

    while (nanosleep(&req, &req));

#else

    #error invalid configuration value

#endif
}
#endif /* if X_CONF_MDELAY_IMPL_TYPE != X_MDELAY_IMPL_TYPE_USERPORT */


#if X_CONF_UDELAY_IMPL_TYPE != X_UDELAY_IMPL_TYPE_USERPORT
void x_port_udelay(XUSeconds usec)
{

#if X_CONF_UDELAY_IMPL_TYPE == X_UDELAY_IMPL_TYPE_TICKS_BUSYWAIT

    const XTicks start = x_ticks_now();
    const XTicks end = x_usec_to_ticks(usec);
    for (;;)
    {
        const XTicks cur = x_ticks_now();
        if (cur - start >= end)
            break;
    }

#elif X_CONF_UDELAY_IMPL_TYPE == X_UDELAY_IMPL_TYPE_POSIX_NANOSLEEP

    struct timespec req;
    req.tv_sec = usec / X_UINT32_C(1000000);
    usec %= X_UINT32_C(1000000);
    req.tv_nsec = (uint64_t)usec * 1000;

    while (nanosleep(&req, &req));

#else

    #error invalid configuration value

#endif
}
#endif /* if X_CONF_UDELAY_IMPL_TYPE != X_UDELAY_IMPL_TYPE_USERPORT */



XTicks x_msec_to_ticks(XMSeconds msec)
{
    const XTicks ret = X_DIV_ROUNDUP(msec * X_TICKS_PER_SEC, 1000);
    return ret;
}


XTicks x_usec_to_ticks(XUSeconds usec)
{
    const XTicks ret = X_DIV_ROUNDUP(usec * X_TICKS_PER_SEC, 1000) / 1000;
    return ret;
}


XMSeconds x_ticks_to_msec(XTicks ticks)
{
    const XMSeconds ret = (ticks * 1000) / X_TICKS_PER_SEC;
    return ret;
}


XUSeconds x_ticks_to_usec(XTicks ticks)
{
    const XUSeconds ret = ((ticks * 1000) / X_TICKS_PER_SEC) * 1000;
    return ret;
}


XTimeVal x_gettimeofday2(void)
{
    XTimeVal tv;
    x_gettimeofday(&tv, NULL);
    return tv;
}
