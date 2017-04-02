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
#if X_CONF_HAS_C99_MATH
    #include <math.h>
#endif


#define X__TYPE_INT_OR_UNSINED  (0)
#define X__TYPE_LONG            (1)
#define X__TYPE_SIZE            (2)
#define X__TYPE_DOUBLE          (3)
#define X__TYPE_PTRDIFF         (4)
#define X__TYPE_CHAR            (5)
#define X__TYPE_SHORT           (6)
#define X__TYPE_POINTER         (7)
#define X__FLAG_ZERO_PADDING    (X_BIT(0))
#define X__FLAG_LEFT_ALIGN      (X_BIT(1))
#define X__FLAG_NEGATIVE        (X_BIT(2))


#define X__PUTC(c)                              \
        do                                      \
        {                                       \
            if (putc_func(context, c) == EOF)   \
                return -1;                      \
            ++len;                              \
        } while (0)


typedef int (*X__Putc)(void* ptr, int c);
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

static int X__MemPutc(void* ptr, int c);
static int X__StreamPutc(void* ptr, int c);
static int X__SomewherePutc(void* ptr, int c);
static int X__VPrintf(X__Putc putc_func, void* context, const char* fmt, va_list args);
XCharPutFunc x_putc_stdout;
XCharPutFunc x_putc_stderr;


int x_putc(int c)
{
    if (!x_putc_stdout)
        return EOF;
    return x_putc_stdout(c);
}


int x_puts(const char* str)
{
    if (!x_putc_stdout)
        return EOF;

    while (*str)
    {
        if (x_putc_stdout(*str++) < 0)
            return -1;
    }
    return x_putc_stdout('\n');
}


int x_puts2(const char* str)
{
    if (!x_putc_stdout)
        return EOF;

    while (*str)
    {
        if (x_putc_stdout(*str++) < 0)
            return -1;
    }
    return 0;
}


int x_snprintf(char* buf, size_t size, const char* fmt, ...)
{
    int len;
    va_list args;
    va_start(args, fmt);
    len = x_vsnprintf(buf, size, fmt, args);
    va_end(args);
    return len;
}


int x_sprintf(char* buf, const char* fmt, ...)
{
    int len;
    va_list args;
    va_start(args, fmt);
    len = x_vsnprintf(buf, SIZE_MAX, fmt, args);
    va_end(args);
    return len;
}


int x_vsnprintf(char* buf, size_t size, const char* fmt, va_list args)
{
    int len;
    X__MemPutcContext context;
    context.dst = buf;
    context.pos = 0;
    context.size = size - 1;

    len = X__VPrintf(X__MemPutc, &context, fmt, args);
    if (len < 0)
        buf[0] = '\0';
    else
        buf[len] = '\0';
    return len;
}


int x_vsprintf(char* buf, const char* fmt, va_list args)
{
    return x_vsnprintf(buf, SIZE_MAX, fmt, args);
}


int x_printf(const char* fmt, ...)
{
    int len;
    va_list args;
#ifdef __CA78K0R__
    va_starttop(args, fmt);
#else
    va_start(args, fmt);
#endif
    len = x_vprintf(fmt, args);
    va_end(args);
    return len;
}


int x_printf_to_cputter(XCharPutFunc cputter, const char* fmt, ...)
{
    int len;
    va_list args;
    va_start(args, fmt);
    len = x_vprintf_to_cputter(cputter, fmt, args);
    va_end(args);
    return len;
}


int x_printf_to_stream(XStream* stream, const char* fmt, ...)
{
    int len;
    va_list args;
    va_start(args, fmt);
    len = x_vprintf_to_stream(stream, fmt, args);
    va_end(args);
    return len;
}


int x_vprintf(const char* fmt, va_list args)
{
    X__CharPutcContext ctx;
    ctx.char_put_func = (XCharPutFunc)x_putc;
    return X__VPrintf(X__SomewherePutc, &ctx, fmt, args);
}


int x_vprintf_to_cputter(XCharPutFunc cputter, const char* fmt, va_list args)
{
    X__CharPutcContext ctx;
    ctx.char_put_func = (XCharPutFunc)cputter;
    return X__VPrintf(X__SomewherePutc, &ctx, fmt, args);
}


int x_vprintf_to_stream(XStream* stream, const char* fmt, va_list args)
{
    return X__VPrintf(X__StreamPutc, stream, fmt, args);
}


int x_err_putc(int c)
{
    if (!x_putc_stderr)
        return x_putc(c);
    return x_putc_stderr(c);
}


int x_err_puts(const char* str)
{
    if (!x_putc_stderr)
        return x_puts(str);

    while (*str)
    {
        if (x_putc_stderr(*str++) < 0)
            return -1;
    }
    return x_putc_stderr('\n');
}


int x_err_puts2(const char* str)
{
    if (!x_putc_stderr)
        return x_puts2(str);

    while (*str)
    {
        if (x_putc_stderr(*str++) < 0)
            return -1;
    }
    return 0;
}


int x_err_printf(const char* fmt, ...)
{
    int len;
    va_list args;
#ifdef __CA78K0R__
    va_starttop(args, fmt);
#else
    va_start(args, fmt);
#endif
    len = x_err_vprintf(fmt, args);
    va_end(args);
    return len;
}


int x_err_vprintf(const char* fmt, va_list args)
{
    X__CharPutcContext ctx;
    ctx.char_put_func = (XCharPutFunc)x_err_putc;
    return X__VPrintf(X__SomewherePutc, &ctx, fmt, args);
}


static int X__MemPutc(void* ptr, int c)
{
    X__MemPutcContext* context = ptr;
    if (context->pos <  context->size)
    {
        (*((char*)(context->dst) + context->pos)) = (char)c;
        context->pos++;
    }
    return 0;
}


static int X__StreamPutc(void* ptr, int c)
{
    XStream* stream = ptr;
    return xstream_putc(stream, c);
}


static int X__SomewherePutc(void* ptr, int c)
{
    XCharPutFunc putc_func = ((X__CharPutcContext*)ptr)->char_put_func;
    return putc_func(c);
}

#if SIZE_MAX >= ULONG_MAX
    typedef size_t              X__PrintUInt;
    #define X__PRINT_UINT_MAX   SIZE_MAX
    #define X__MSBOF_PRINT_UINT X_MSBOF_SIZE
#else
    typedef unsigned long       X__PrintUInt;
    #define X__PRINT_UINT_MAX   ULONG_MAX
    #define X__MSBOF_PRINT_UINT X_MSBOF_LONG
#endif

static int X__VPrintf(X__Putc putc_func, void* context, const char* fmt, va_list args)
{
    unsigned int i, j;
    X__PrintUInt v;
    char* p;
    int len = 0;

    unsigned char type, flags, base, minimum_width;
    char c, digit;
#if X__PRINT_UINT_MAX == 0xFFFFFFFF
    char s[32];
#elif !defined(X_COMPILER_NO_64BIT_INT)
    #if X__PRINT_UINT_MAX == 0xFFFFFFFFFFFFFFFF
    char s[64];
    #endif
#else
    #error unspported platform
#endif

#if X_CONF_USE_FLOATING_POINT_PRINTF
    unsigned char precision;
    /* fの0初期化は必要ないように見えるが、gccの最適化レベルを上げると、未初期化
     * 変数を使用していますという警告が出る。最適化の結果によって、そういうルー
     * トができてしまうのか？それはそれでまずいバグになりそーなのだが・・・。と
     * りあえず0で初期化して警告を黙らせておく。
     */
    double f = 0;
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
        type = X__TYPE_INT_OR_UNSINED;
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
        minimum_width = (unsigned char)i;

#if X_CONF_USE_FLOATING_POINT_PRINTF
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
                precision = (unsigned char)i;
            }
        }
#endif

        switch (c)
        {
            case 'l':
                type = X__TYPE_LONG;
                c = *fmt++;
                break;
            case 'z':
                type = X__TYPE_SIZE;
                c = *fmt++;
                break;
            case 't':
                type = X__TYPE_PTRDIFF;
                c = *fmt++;
                break;
            case 'h':
                c = *fmt++;
                if (c == 'h')
                {
                    type = X__TYPE_CHAR;
                    c = *fmt++;
                }
                else
                {
                    type = X__TYPE_SHORT;
                }
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
            case 'i' :
                base = 10;
                break;
            case 'x' :
            case 'X' :
                base = 16;
                break;
            case 'p':
                type = X__TYPE_POINTER;
                base = 16;
                break;
            case 'f':
                type = X__TYPE_DOUBLE;
                base = 10;
                break;
            default:
                X__PUTC(c);
                continue;
        }

        switch (type)
        {
#if X_CONF_USE_FLOATING_POINT_PRINTF
            case X__TYPE_DOUBLE:
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
                break;
#endif
            case X__TYPE_POINTER:
                p = va_arg(args, void*);
                if (p == NULL)
                {
                    p = "(nil)";
                    goto X__PRINT_STRING;
                }
                v = (uintptr_t)p;
                break;
            case X__TYPE_INT_OR_UNSINED:
                if ((c == 'd') || (c == 'i'))
                    v = va_arg(args, int);
                else
                    v = va_arg(args, unsigned int);
                break;
            case X__TYPE_CHAR:
                if ((c == 'd') || (c == 'i'))
                    v = (signed char)va_arg(args, int);
                else
                    v = (unsigned char)va_arg(args, unsigned int);
                break;
            case X__TYPE_SHORT:
                if ((c == 'd') || (c == 'i'))
                    v = (short)va_arg(args, int);
                else
                    v = (unsigned short)va_arg(args, unsigned int);
                break;
            case X__TYPE_LONG:
                if ((c == 'd') || (c == 'i'))
                    v = va_arg(args, long);
                else
                    v = va_arg(args, unsigned long);
                break;
            case X__TYPE_SIZE:
                if ((c == 'd') || (c == 'i'))
#ifdef SSIZE_MAX
                    v = va_arg(args, ssize_t);
#else
                    v = va_arg(args, long);
#endif
                else
                    v = va_arg(args, size_t);
                break;
            case X__TYPE_PTRDIFF:
                v = va_arg(args, ptrdiff_t);
                break;
        }

        if (((c == 'd') || (c == 'i')) && (v & X__MSBOF_PRINT_UINT))
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
            {
                if ((c == 'x') || (c == 'p'))
                    digit += 0x27;
                else
                    digit += 0x07;
            }
            s[i++] = (char)(digit + '0');
        } while (v && i < sizeof(s));


        if (c == 'p')
        {
            s[i++] = 'x';
            s[i++] = '0';
        }

        if (flags & X__FLAG_NEGATIVE)
            s[i++] = '-';

        j = i;
#if X_CONF_USE_FLOATING_POINT_PRINTF
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

        if (flags & X__FLAG_ZERO_PADDING)
            digit = '0';
        else
            digit = ' ';
        while ((!(flags & X__FLAG_LEFT_ALIGN)) &&
               (j++ < minimum_width))
            X__PUTC(digit);

        do
        {
            X__PUTC(s[--i]);
        } while (i);

#if X_CONF_USE_FLOATING_POINT_PRINTF
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
