/**
 *       @file  xdebug.h
 *      @brief
 *
 *    @details
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2015/06/14
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

#ifndef xdebug_h_
#define xdebug_h_


#ifdef __cplusplus
extern "C" {
#endif


#include <stdio.h>


#ifndef XDB_VPRINTF
#ifdef __GNUC__
    #define XDB_VPRINTF(fmt, ...)       vfprintf(stdout, fmt, ## __VA_ARGS__)
#else
    #define XDB_VPRINTF(...)            vfprintf(stdout, __VA_ARGS__)
#endif
#endif


#ifndef XDB_UNBUFFERED_VPRINTF
#ifdef __GNUC__
    #define XDB_UNBUFFERED_VPRINTF(fmt, ...)       vfprintf(stderr, fmt, ## __VA_ARGS__)
#else
    #define XDB_UNBUFFERED_VPRINTF(...)            vfprintf(stderr, __VA_ARGS__)
#endif
#endif


#ifndef XDB_ASSERTION_FAILED
    void xdb_assertion_failed(const char* expr, const char* msg, const char* func, const char* file, int line);
    #define XDB_ASSERTION_FAILED            xdb_assertion_failed

    #ifndef XDB_POST_ASSERTION_FAILED
        #define XDB_POST_ASSERTION_FAILED()   for (;;)
    #endif
#endif


#define XDB_LOG_LEVEL_ERR    (1)  /* error      */
#define XDB_LOG_LEVEL_WARN   (2)  /* warning    */
#define XDB_LOG_LEVEL_NOTI   (3)  /* notice     */
#define XDB_LOG_LEVEL_INFO   (4)  /* infomation */
#define XDB_LOG_LEVEL_VERB   (5)  /* verbose    */


#ifdef __GNUC__
    void xdb_print_log(int level, const char* fmt, ...);
#else
    void xdb_print_log(int level, ...);
#endif


#ifndef XDB_LOG_LEVEL
    #define XDB_LOG_LEVEL     XDB_LOG_LEVEL_INFO
#endif


#if (! defined(XDB_DYNAMIC_LOG_SUPPRESS)) && (XDB_LOG_LEVEL >= XDB_LOG_LEVEL_VERB)
    #define XDB_LOG_VERB(...)   xdb_print_log(XDB_LOG_LEVEL_VERB, __VA_ARGS__)
#else
    #define XDB_LOG_VERB(...)   (void)0
#endif


#if (! defined(XDB_DYNAMIC_LOG_SUPPRESS)) && (XDB_LOG_LEVEL >= XDB_LOG_LEVEL_INFO)
    #define XDB_LOG_INFO(...)   xdb_print_log(XDB_LOG_LEVEL_INFO, __VA_ARGS__)
#else
    #define XDB_LOG_INFO(...)   (void)0
#endif


#if (! defined(XDB_DYNAMIC_LOG_SUPPRESS)) && (XDB_LOG_LEVEL >= XDB_LOG_LEVEL_NOTI)
    #define XDB_LOG_NOTI(...)   xdb_print_log(XDB_LOG_LEVEL_NOTI, __VA_ARGS__)
#else
    #define XDB_LOG_NOTI(...)   (void)0
#endif


#if (! defined(XDB_DYNAMIC_LOG_SUPPRESS)) && (XDB_LOG_LEVEL >= XDB_LOG_LEVEL_WARN)
    #define XDB_LOG_WARN(...)   xdb_print_log(XDB_LOG_LEVEL_WARN, __VA_ARGS__)
#else
    #define XDB_LOG_WARN(...)   (void)0
#endif


#if (! defined(XDB_DYNAMIC_LOG_SUPPRESS)) && (XDB_LOG_LEVEL >= XDB_LOG_LEVEL_ERR)
    #define XDB_LOG_ERR(...)   xdb_print_log(XDB_LOG_LEVEL_ERR, __VA_ARGS__)
#else
    #define XDB_LOG_ERR(...)   (void)0
#endif


#ifndef XDB_VERB_HEADER
    #define XDB_VERB_HEADER     "[VERB] "
#endif


#ifndef XDB_INFO_HEADER
    #define XDB_INFO_HEADER     "[INFO] "
#endif


#ifndef XDB_NOTI_HEADER
    #define XDB_NOTI_HEADER     "[NOTI] "
#endif


#ifndef XDB_WARN_HEADER
    #define XDB_WARN_HEADER     "[WARN] "
#endif


#ifndef XDB_ERR_HEADER
    #define XDB_ERR_HEADER      "[ERR ] "
#endif


#ifdef XDB_ASSERT_ENABLED
    #define XDB_ASSERT_MSG(expr, msg)     ((expr) ? (void)0 : XDB_ASSERTION_FAILED(#expr, msg, __func__, __FILE__, __LINE__))
    #define XDB_ASSERT(expr)              XDB_ASSERT_MSG(expr, NULL)
    #define XDB_ABORT(msg)                XDB_ASSERTION_FAILED("Program abort", msg, __func__, __FILE__, __LINE__)
    #define XDB_ABORT_DEFAULT             default: XDB_ABORT("Runtime error"); break
#else
    #define XDB_ASSERT_MSG(expr, msg)     (void)0
    #define XDB_ASSERT(expr)              (void)0
    #define XDB_ABORT(msg)                (void)0
    #define XDB_ABORT_DEFAULT             default: break
#endif


#define xdb_vprintf                 XDB_VPRINTF
#define xdb_unbufferd_vprintf       XDB_UNBUFFERED_VPRINTF

int xdb_set_level(int level);
void xdb_printf(const char* fmt, ...);
void xdb_unbufferd_printf(const char* fmt, ...);
void xdb_hexdump(const void* src, unsigned len, unsigned cols);


#ifdef __cplusplus
}
#endif


#endif // xdebug_h_


