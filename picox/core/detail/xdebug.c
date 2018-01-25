/**
 *       @file  xdebug.c
 *      @brief  デバッグログやassert等の実装
 *
 *    @details
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2015/04/24
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


#if X_CONF_HAS_ERRNO_AND_STRERROR != 0
    #include <errno.h>
#endif


typedef struct X__Debug
{
    int level;
} X__Debug;

static void X__VPrintLog(int level, const char* tag, const char* fmt, va_list args);
static void X__VHexdump(int level, const char* tag, const char* src, size_t len, size_t cols, const char* fmt, va_list args);
static const char* X__GetHeader(int level);
static void X__PreAssertionFailed(void);
static void X__PostAssertionFailed(void);
static void X__AssertionFailed(const char* expr, const char* fmt, const char* func, const char* file, int line, ...);


#if X_CONF_USE_LOG_TIMESTAMP != 0

void x_port_stimestamp(char* dst, size_t size);

#endif


X__Debug    g_picox_debug = {X_LOG_LEVEL};
static X__Debug* const priv = &g_picox_debug;
void (*x_pre_assertion_failed)(void) = X__PreAssertionFailed;
void (*x_post_assertion_failed)(void) = X__PostAssertionFailed;
XAssertionFailedFunc x_assertion_failed = X__AssertionFailed;


static void X__PreAssertionFailed(void)
{
}


static void X__PostAssertionFailed(void)
{
    volatile int i = 0;
    for (;;)
    {
        i++;
        i++;
    }
}


int x_set_log_level(int level)
{
    const int prev = priv->level;
    priv->level = level;

    return prev;
}


void x_verb_printlog(const char* tag, const char* fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    X__VPrintLog(X_LOG_LEVEL_VERB, tag, fmt, args);
    va_end(args);
}


void x_info_printlog(const char* tag, const char* fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    X__VPrintLog(X_LOG_LEVEL_INFO, tag, fmt, args);
    va_end(args);
}


void x_noti_printlog(const char* tag, const char* fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    X__VPrintLog(X_LOG_LEVEL_NOTI, tag, fmt, args);
    va_end(args);
}


void x_warn_printlog(const char* tag, const char* fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    X__VPrintLog(X_LOG_LEVEL_WARN, tag, fmt, args);
    va_end(args);
}


void x_err_printlog(const char* tag, const char* fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    X__VPrintLog(X_LOG_LEVEL_ERR, tag, fmt, args);
    va_end(args);
}


void x_verb_hexdumplog(const char* tag, const void* src, size_t len, size_t cols, const char* fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    X__VHexdump(X_LOG_LEVEL_VERB, tag, src, len, cols, fmt, args);
    va_end(args);
}


void x_info_hexdumplog(const char* tag, const void* src, size_t len, size_t cols, const char* fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    X__VHexdump(X_LOG_LEVEL_INFO, tag, src, len, cols, fmt, args);
    va_end(args);
}


void x_noti_hexdumplog(const char* tag, const void* src, size_t len, size_t cols, const char* fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    X__VHexdump(X_LOG_LEVEL_NOTI, tag, src, len, cols, fmt, args);
    va_end(args);
}


void x_warn_hexdumplog(const char* tag, const void* src, size_t len, size_t cols, const char* fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    X__VHexdump(X_LOG_LEVEL_WARN, tag, src, len, cols, fmt, args);
    va_end(args);
}


void x_err_hexdumplog(const char* tag, const void* src, size_t len, size_t cols, const char* fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    X__VHexdump(X_LOG_LEVEL_ERR, tag, src, len, cols, fmt, args);
    va_end(args);
}


void x_hexdump(const void* src, size_t len, size_t cols)
{
    size_t i, j;
    const unsigned char* p = src;

    for (i = 0; i < len + ((len % cols) ? (cols - len % cols) : 0); i++)
    {
        /* print offset */
        if(i % cols == 0)
        {
            x_err_printf("0x%06"PRIX32": ", (uint32_t)i);
        }

        /* print hex data */
        if(i < len)
        {
            x_printf("%02x ", p[i]);
        }
        else /* end of block, just aligning for ASCII dump */
        {
            x_printf("   ");
        }

        /* print ASCII dump */
        if(i % cols == (cols - 1))
        {
            for(j = i - (cols - 1); j <= i; j++)
            {
                if(j >= len) /* end of block, not really printing */
                    x_putc(' ');
                else if (isprint((int)p[j])) /* printable char */
                    x_putc(p[j]);
                else /* other char */
                    x_putc('.');
            }
            x_putc('\n');
        }
    }
}

void x_err_hexdump(const void* src, size_t len, size_t cols)
{
    size_t i, j;
    const unsigned char* p = src;

    for (i = 0; i < len + ((len % cols) ? (cols - len % cols) : 0); i++)
    {
        /* print offset */
        if(i % cols == 0)
        {
            x_err_printf("0x%06"PRIX32": ", (uint32_t)i);
        }

        /* print hex data */
        if(i < len)
        {
            x_err_printf("%02x ", p[i]);
        }
        else /* end of block, just aligning for ASCII dump */
        {
            x_err_printf("   ");
        }

        /* print ASCII dump */
        if(i % cols == (cols - 1))
        {
            for(j = i - (cols - 1); j <= i; j++)
            {
                if(j >= len) /* end of block, not really printing */
                    x_err_putc(' ');
                else if (isprint((int)p[j])) /* printable char */
                    x_err_putc(p[j]);
                else /* other char */
                    x_err_putc('.');
            }
            x_err_putc('\n');
        }
    }
}


static void X__AssertionFailed(const char* expr, const char* fmt, const char* func, const char* file, int line, ...)
{
    /*
     * fileがフルパスで出力されると環境によってコンパイル環境によって出力が変
     * わってしまい、面倒なのでファイル名だけを取り出す。Windows形式のパスとUnix
     * 形式のパスを想定。
     */
    const char* win_style;
    const char* unix_style;
    const char* p;
    const char* none = "none";
    va_list args;

    if (file)
    {
        win_style = strrchr(file, '\\');
        unix_style = strrchr(file, '/');
        p = win_style ? win_style : unix_style;
        file = p ? p + 1 : file;
    }

    va_start(args, line);
    x_pre_assertion_failed();
    x_err_printf("Assertion failed\n");
    if (fmt)
    {
        x_err_printf("[MSG ] ");
        x_err_vprintf(fmt, args);
    }
    else
    {
        x_err_printf("[MSG ] none");
    }
    x_err_printf("\n");
    x_err_printf("[EXPR] %s\n", expr ? expr : none);
    x_err_printf("[FUNC] %s\n", func ? func : none);
    x_err_printf("[FILE] %s\n", file ? file : none);
    x_err_printf("[LINE] %d\n", line);
#if X_CONF_HAS_ERRNO_AND_STRERROR != 0
    x_err_printf("[ERR ] %s\n", strerror(errno));
#endif

    x_err_printf("************************\n");
    x_post_assertion_failed();
}


static void X__VPrintLog(int level, const char* tag, const char* fmt, va_list args)
{
    if (level <= priv->level)
    {
        if (level != X_LOG_LEVEL_ERR)
        {
#if X_CONF_USE_LOG_TIMESTAMP != 0
            char tstamp[X_LOG_TIMESTAMP_BUF_SIZE];
            x_port_stimestamp(buf, sizeof(tstamp));
            x_printf("%s[%s]%s ", X__GetHeader(level), tag, tstamp);
#else
            x_printf("%s[%s] ", X__GetHeader(level), tag);
#endif
            x_vprintf(fmt, args);
            x_putc('\n');
        }
        else
        {
#if X_CONF_USE_LOG_TIMESTAMP != 0
            char tstamp[X_LOG_TIMESTAMP_BUF_SIZE];
            x_port_stimestamp(buf, sizeof(tstamp));
            x_err_printf("%s[%s]%s ", X__GetHeader(level), tag, tstamp);
#else
            x_err_printf("%s[%s] ", X__GetHeader(level), tag);
#endif
            x_err_vprintf(fmt, args);
            x_err_putc('\n');
        }
    }
}


static void X__VHexdump(int level, const char* tag, const char* src, size_t len, size_t cols, const char* fmt, va_list args)
{
    if (level <= priv->level)
    {
        X__VPrintLog(level, tag, fmt, args);
        if (level == X_LOG_LEVEL_ERR)
            x_err_hexdump(src, len, cols);
        else
            x_hexdump(src, len, cols);
    }
}


static const char* X__GetHeader(int level)
{
    const char* str = NULL;

    switch (level)
    {
        case X_LOG_LEVEL_VERB:  str = X_VERB_HEADER;  break;
        case X_LOG_LEVEL_INFO:  str = X_INFO_HEADER;  break;
        case X_LOG_LEVEL_NOTI:  str = X_NOTI_HEADER;  break;
        case X_LOG_LEVEL_WARN:  str = X_WARN_HEADER;  break;
        case X_LOG_LEVEL_ERR:   str = X_ERR_HEADER;   break;
        default:                                      break;
    }

    return str;
}
