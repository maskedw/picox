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

X__Debug    g_picox_debug = {X_LOG_LEVEL};
static X__Debug* const priv = &g_picox_debug;


static void X__VPrintLog(int level, const char* fmt, va_list args);
static void X__Putc(int c);
static const char* X__GetHeader(int level);


int x_set_log_level(int level)
{
    const int prev = priv->level;
    priv->level = level;

    return prev;
}


void x_printf(const char* fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    x_vprintf(fmt, args);
    va_end(args);
}


void x_print_log(int level, const char* fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    X__VPrintLog(level, fmt, args);
    va_end(args);
}


void x_hexdump(const void* src, unsigned len, unsigned cols)
{
    unsigned int i, j;
    const unsigned char* p = src;

    for (i = 0; i < len + ((len % cols) ? (cols - len % cols) : 0); i++)
    {
        /* print offset */
        if(i % cols == 0)
        {
            x_printf("0x%06x: ", i);
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


void x_assertion_failed(const char* expr, const char* msg, const char* func, const char* file, int line)
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

    X_PRE_ASSERTION_FAILED();

    x_printf("Assertion failed\n");
    x_printf("[MSG ] %s\n", msg ? msg : none);
    x_printf("[EXPR] %s\n", expr ? expr : none);
    x_printf("[FUNC] %s\n", func ? func : none);
    x_printf("[FILE] %s\n", file ? file : none);
    x_printf("[LINE] %d\n", line);
    x_printf("************************\n");

    X_POST_ASSERTION_FAILED();
}


static void X__VPrintLog(int level, const char* fmt, va_list args)
{
    if (level <= priv->level) {
        x_printf("%s", X__GetHeader(level));
        x_vprintf(fmt, args);
        X__Putc('\n');
    }
}


static void X__Putc(int c)
{
    x_printf("%c", c);
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


