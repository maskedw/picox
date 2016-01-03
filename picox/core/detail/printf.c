/**
 *       @file  xprintf.c
 *      @brief
 *
 *    @details
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2016/01/02
 * ===================================================================
 */

/*
 * License: MIT license
 * Copyright (c) <2015> <MaskedW [maskedw00@gmail.com]>
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
#include <picox/core/detail/printf.h>
#if X_CONF_HAS_C99_MATH
    #include <math.h>
#endif


#define X__FLAG_ZERO_PADDING    (X_BIT(0))
#define X__FLAG_LEFT_ALIGN  (X_BIT(1))
#define X__FLAG_LONG            (X_BIT(2))
#define X__FLAG_SIZE_T          (X_BIT(3))
#define X__FLAG_DOUBLE          (X_BIT(4))
#define X__FLAG_NEGATIVE        (X_BIT(5))
#define X__PUTC(c)              do { putc_func(context, c); len++; } while (0)


typedef void (*X__Putc)(void* ptr, char c);
typedef struct
{
    void*   dst;
    size_t  pos;
    size_t  size;
} X__MemPutcContext;
typedef struct
{
    XCharPutFunc char_put_func;
} X__CharPutcContext;

static void X__MemPutc(void* ptr, char c);
static void X__SomewherePutc(void* ptr, char c);
static int X__VPrintf(X__Putc putc_func, void* context, const char* fmt, va_list args);
XCharPutFunc x_putc_stdout;


void x_putc(int c)
{
    if (! x_putc_stdout)
        return;

    x_putc_stdout(c);
}


void x_puts(const char* str)
{
    if (! x_putc_stdout)
        return;

    while (*str)
        x_putc_stdout(*str++);
}


int x_snprintf(char* buf, size_t size, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    const int len = x_vsnprintf(buf, size, fmt, args);
    va_end(args);
    return len;

}


int x_vsnprintf(char* buf, size_t size, const char* fmt, va_list args)
{
    X__MemPutcContext context;
    context.dst = buf;
    context.pos = 0;
    context.size = size - 1;

    const int len = X__VPrintf(X__MemPutc, &context, fmt, args);
    if (len < 0)
        buf[0] = '\0';
    else
        buf[len] = '\0';
    return len;
}


int x_printf(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    const int len = x_vprintf(fmt, args);
    va_end(args);
    return len;
}


int x_printf_to_cputter(XCharPutFunc cputter, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    const int len = x_vprintf_to_cputter(cputter, fmt, args);
    va_end(args);
    return len;
}


int x_vprintf_to_cputter(XCharPutFunc cputter, const char* fmt, va_list args)
{
    return X__VPrintf(X__SomewherePutc, cputter, fmt, args);
}


int x_vprintf(const char* fmt, va_list args)
{
    return X__VPrintf(X__SomewherePutc, (XCharPutFunc)x_putc, fmt, args);
}


static void X__MemPutc(void* ptr, char c)
{
    X__MemPutcContext* context = ptr;
    if (context->pos <  context->size)
    {
        (*((char*)(context->dst) + context->pos)) = c;
        context->pos++;
    }
}


static void X__SomewherePutc(void* ptr, char c)
{
    XCharPutFunc putc_func = ptr;
    putc_func(c);
}


static int X__VPrintf(X__Putc putc_func, void* context, const char* fmt, va_list args)
{
    unsigned int i, j;
    unsigned long v;
    char* p;
    int len = 0;

    unsigned char flags, base, minimum_width;
    char c, digit;
#if ULONG_MAX == 0xFFFFFFFF
    char s[32];
#elif ULONG_MAX == 0xFFFFFFFFFFFFFFFF
    char s[64];
#else
    #error unspported platform
#endif

#if X_CONFIG_USE_FLOATING_POINT_PRINTF
    unsigned char precision;
    double f;
#endif

    for (;;)
    {
        c = *fmt++;
        if (!c)
            break;

        if (c != '%')
        {
            X__PUTC(c);
            continue;
        }

        flags = 0;
        c = *fmt++;
        if (c == '0')
        {
            flags |= X__FLAG_ZERO_PADDING;
            c = *fmt++;
        }
        else
        {
            if (c == '-')
            {
                flags |= X__FLAG_LEFT_ALIGN;
                c = *fmt++;
            }
        }

        for (i = 0; isdigit((int)c); c = *fmt++)
            i = i * 10 + c - '0';
        minimum_width = i;

#if X_CONFIG_USE_FLOATING_POINT_PRINTF
        precision = 0xFF;
        if (c == '.')
        {
            if (*fmt == '*')
            {
                c = *(++fmt);
                precision = va_arg(args, int);
            }
            else
            {
                c = *fmt++;
                for (i = 0; isdigit((int)c); c = *fmt++)
                    i = i * 10 + c - '0';
                precision = i;
            }
        }
#endif

        switch (c)
        {
            case 'l':
                flags |= X__FLAG_LONG;
                c = *fmt++;
                break;
            case 'z':
                flags |= X__FLAG_SIZE_T;
                c = *fmt++;
                break;
            default:
                break;
        }

        if (!c) break;

        p = NULL;
        switch (c)
        {
            case 's':
                p = va_arg(args, char*);
                if (p == NULL)
                    p = "(null)";
X__PRINT_STRING:
                j = strlen(p);
                while ((!(flags & X__FLAG_LEFT_ALIGN)) && (j++ < minimum_width))
                    X__PUTC(' ');
                while (*p)
                    X__PUTC(*p++);
                while (j++ < minimum_width)
                    X__PUTC(' ');
                continue;
            case 'c' :
                X__PUTC((char)va_arg(args, int));
                continue;
            case 'b' :
                base = 2;
                break;
            case 'o' :
                base = 8;
                break;
            case 'd' :
            case 'u' :
                base = 10;
                break;
            case 'x' :
            case 'X' :
                base = 16;
                break;
            case 'p':
                base = 16;
                break;
            case 'f':
                base = 10;
                break;
            default:
                X__PUTC(c);
                continue;
        }

#if X_CONFIG_USE_FLOATING_POINT_PRINTF
        if (c == 'f')
        {
            f = va_arg(args, double);
#if X_CONF_HAS_C99_MATH
            if (isnan(f))
            {
                p = "(nan)";
                goto X__PRINT_STRING;
            }
            else if (isinf(f))
            {
                p = "(inf)";
                goto X__PRINT_STRING;
            }
#endif

            if (f < 0)
            {
                flags |= X__FLAG_NEGATIVE;
                f = -f;
            }

            v = f;
            f -= v;
            if (precision == 0xFF)
                precision = 6;
        }
        else
#endif
        if (c == 'p')
        {
            p = va_arg(args, void*);
            if (p == NULL)
            {
                p = "(nil)";
                goto X__PRINT_STRING;
            }
            v = (uintptr_t)p;
        }
        else if(flags & X__FLAG_LONG)
            v = va_arg(args, long);
        else if (flags & X__FLAG_SIZE_T)
            v = va_arg(args, size_t);
        else if (c == 'd')
            v = va_arg(args, int);
        else
            v = va_arg(args, unsigned int);

        if (c == 'd' && (v & X_LONG_MSB))
        {
            v = 0 - v;
            flags |= X__FLAG_NEGATIVE;
        }

        i = 0;
        do
        {
            digit = (char)(v % base);
            v /= base;
            if (digit > 9)
                digit += ((c == 'x') || (c == 'p')) ? 0x27 : 0x07;
            s[i++] = digit + '0';
        } while (v && i < sizeof(s));


        if (c == 'p')
        {
            s[i++] = 'x';
            s[i++] = '0';
        }

        if (flags & X__FLAG_NEGATIVE)
            s[i++] = '-';

        j = i;
#if X_CONFIG_USE_FLOATING_POINT_PRINTF
        if ((c == 'f') && (precision > 0))
        {
            j += precision + 1;
        }
#endif

        /* "04d", -10 == "-010"
         * "4d",  -10 == " -10" となるようにしたいので、条件に合致する時は先に
         * '-'を出力する。
         */
        if ((!(flags & X__FLAG_LEFT_ALIGN)) &&
            (flags & X__FLAG_ZERO_PADDING) &&
            (flags & X__FLAG_NEGATIVE))
        {
            X__PUTC(s[--i]);
        }

        digit = (flags & X__FLAG_ZERO_PADDING) ? '0' : ' ';
        while ((!(flags & X__FLAG_LEFT_ALIGN)) &&
               (j++ < minimum_width))
            X__PUTC(digit);

        do
        {
            X__PUTC(s[--i]);
        } while (i);

#if X_CONFIG_USE_FLOATING_POINT_PRINTF
        if ((c == 'f') && (precision > 0))
        {
            X__PUTC('.');
            if (precision >= sizeof(s))
                precision = sizeof(s);

            for (i = 0; i < precision; i++)
                f *= 10;

            v = f;
            i = 0;
            do
            {
                digit = (char)(v % 10);
                v /= 10;
                s[i++] = digit + '0';
            } while (i < precision && i < sizeof(s));

            do
            {
                X__PUTC(s[--i]);
            } while (i);
        }
#endif

        while (j++ < minimum_width)
            X__PUTC(' ');
    }

    return len;
}
