/**
 *       @file  xdebug.c
 *      @brief
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

#include "xdebug.h"

// std
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>


typedef struct X__Debug
{
    int level;
} X__Debug;

X__Debug    g_picox_debug = {XDB_LOG_LEVEL};
static X__Debug* const priv = &g_picox_debug;


static void X__VPrintLog(int level, const char* fmt, va_list args);
static void X__Putc(int c);
static const char* X__GetHeader(int level);


int xdb_set_level(int level)
{
    const int prev = priv->level;
    priv->level = level;

    return prev;
}


void xdb_printf(const char* fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    XDB_VPRINTF(fmt, args);
    va_end(args);
}


void xdb_unbufferd_printf(const char* fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    XDB_UNBUFFERED_VPRINTF(fmt, args);
    va_end(args);
}


#ifdef __GNUC__

void xdb_print_log(int level, const char* fmt, ...)
{
    va_list args;

    va_start(args, fmt);

    X__VPrintLog(level, fmt, args);
    va_end(args);
}

#else

void xdb_print_log(int level, ...)
{
    va_list args;

    va_start(args, level);
    const char* fmt = va_arg(args, const char*);
    X__VPrintLog(level, fmt, args);
    va_end(args);
}


#endif // ifdef __GNUC__

void xdb_hexdump(const void* src, unsigned len, unsigned cols)
{
    unsigned int i, j;
    const unsigned char* p = src;

    for (i = 0; i < len + ((len % cols) ? (cols - len % cols) : 0); i++)
    {
        /* print offset */
        if(i % cols == 0)
        {
            xdb_printf("0x%06x: ", i);
        }

        /* print hex data */
        if(i < len)
        {
            xdb_printf("%02x ", p[i]);
        }
        else /* end of block, just aligning for ASCII dump */
        {
            xdb_printf("   ");
        }

        /* print ASCII dump */
        if(i % cols == (cols - 1))
        {
            for(j = i - (cols - 1); j <= i; j++)
            {
                if(j >= len) /* end of block, not really printing */
                    X__Putc(' ');
                else if (isprint((int)p[j])) /* printable char */
                    X__Putc(p[j]);
                else /* other char */
                    X__Putc('.');
            }
            X__Putc('\n');
        }
    }
}


void xdb_assertion_failed(const char* expr, const char* msg, const char* func, const char* file, int line)
{
    /*
     * fileがフルパスで出力されると環境によってコンパイル環境によって出力が変
     * わってしまい、面倒なのでファイル名だけを取り出す。Windows形式のパスとUnix
     * 形式のパスを想定。
     */
    const char* win = strrchr(file, '\\');
    const char* unix = strrchr(file, '/');
    const char* p = win ? win : unix;
    file = p ? p + 1 : file;
    const char* none = "none";

    xdb_unbufferd_printf(
        "Assertion failed\n"
        "[MSG ] %s\n"
        "[EXPR] %s\n"
        "[FUNC] %s\n"
        "[FILE] %s\n"
        "[LINE] %d\n"
        "************************\n",
        msg  ? msg  : none,
        expr ? expr : none,
        func ? func : none,
        file ? file : none, line);

    XDB_POST_ASSERTION_FAILED();
}


static void X__VPrintLog(int level, const char* fmt, va_list args)
{
    if (level <= priv->level) {
        xdb_printf("%s", X__GetHeader(level));
        xdb_vprintf(fmt, args);
        X__Putc('\n');
    }
}


static void X__Putc(int c)
{
    xdb_printf("%c", c);
}


static const char* X__GetHeader(int level)
{
    const char* str = NULL;

    switch (level)
    {
        case XDB_LOG_LEVEL_VERB:  str = XDB_VERB_HEADER;  break;
        case XDB_LOG_LEVEL_INFO:  str = XDB_INFO_HEADER;  break;
        case XDB_LOG_LEVEL_NOTI:  str = XDB_NOTI_HEADER;  break;
        case XDB_LOG_LEVEL_WARN:  str = XDB_WARN_HEADER;  break;
        case XDB_LOG_LEVEL_ERR:   str = XDB_ERR_HEADER;   break;
        default:                                          break;
    }

    return str;
}


