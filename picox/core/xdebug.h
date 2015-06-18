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


#ifndef X_VPRINTF
/* gccでは ## __VA_ARGS__とすることで、X_VPRINTF("text");とかけるが、他のコンパ
 * イラでは、可変長引数部分は、引数が1つ以上あることが要求される。
 * なので、全体を可変引数(X_VPRINTF(...))とすることで、最低1つの引数を保証させ
 * る。
 */
#ifdef __GNUC__
    #define X_VPRINTF(fmt, ...)       vfprintf(stdout, fmt, ## __VA_ARGS__)
#else
    #define X_VPRINTF(...)            vfprintf(stdout, __VA_ARGS__)
#endif
#endif


#ifndef X_ASSERTION_FAILED
    void x_assertion_failed(const char* expr, const char* msg, const char* func, const char* file, int line);
    #define X_ASSERTION_FAILED            x_assertion_failed

    #ifndef X_PRE_ASSERTION_FAILED
        #define X_PRE_ASSERTION_FAILED()    (void)0
    #endif

    #ifndef X_POST_ASSERTION_FAILED
        #define X_POST_ASSERTION_FAILED()   for (;;)
    #endif
#endif


#define X_LOG_LEVEL_ERR    (1)  /* error      */
#define X_LOG_LEVEL_WARN   (2)  /* warning    */
#define X_LOG_LEVEL_NOTI   (3)  /* notice     */
#define X_LOG_LEVEL_INFO   (4)  /* infomation */
#define X_LOG_LEVEL_VERB   (5)  /* verbose    */


#ifdef __GNUC__
    void x_print_log(int level, const char* fmt, ...);
#else
    void x_print_log(int level, ...);
#endif


#ifndef X_LOG_LEVEL
    #define X_LOG_LEVEL     X_LOG_LEVEL_INFO
#endif


#if (! defined(X_DYNAMIC_LOG_SUPPRESS)) && (X_LOG_LEVEL >= X_LOG_LEVEL_VERB)
    #define X_LOG_VERB(...)   x_print_log(X_LOG_LEVEL_VERB, __VA_ARGS__)
#else
    #define X_LOG_VERB(...)   (void)0
#endif


#if (! defined(X_DYNAMIC_LOG_SUPPRESS)) && (X_LOG_LEVEL >= X_LOG_LEVEL_INFO)
    #define X_LOG_INFO(...)   x_print_log(X_LOG_LEVEL_INFO, __VA_ARGS__)
#else
    #define X_LOG_INFO(...)   (void)0
#endif


#if (! defined(X_DYNAMIC_LOG_SUPPRESS)) && (X_LOG_LEVEL >= X_LOG_LEVEL_NOTI)
    #define X_LOG_NOTI(...)   x_print_log(X_LOG_LEVEL_NOTI, __VA_ARGS__)
#else
    #define X_LOG_NOTI(...)   (void)0
#endif


#if (! defined(X_DYNAMIC_LOG_SUPPRESS)) && (X_LOG_LEVEL >= X_LOG_LEVEL_WARN)
    #define X_LOG_WARN(...)   x_print_log(X_LOG_LEVEL_WARN, __VA_ARGS__)
#else
    #define X_LOG_WARN(...)   (void)0
#endif


#if (! defined(X_DYNAMIC_LOG_SUPPRESS)) && (X_LOG_LEVEL >= X_LOG_LEVEL_ERR)
    #define X_LOG_ERR(...)   x_print_log(X_LOG_LEVEL_ERR, __VA_ARGS__)
#else
    #define X_LOG_ERR(...)   (void)0
#endif


#ifndef X_VERB_HEADER
    #define X_VERB_HEADER     "[VERB] "
#endif


#ifndef X_INFO_HEADER
    #define X_INFO_HEADER     "[INFO] "
#endif


#ifndef X_NOTI_HEADER
    #define X_NOTI_HEADER     "[NOTI] "
#endif


#ifndef X_WARN_HEADER
    #define X_WARN_HEADER     "[WARN] "
#endif


#ifndef X_ERR_HEADER
    #define X_ERR_HEADER      "[ERR ] "
#endif


#ifndef X_NDEBUG
    #define X_ASSERT_MSG(expr, msg)     ((expr) ? (void)0 : X_ASSERTION_FAILED(#expr, msg, __func__, __FILE__, __LINE__))
    #define X_ASSERT(expr)              X_ASSERT_MSG(expr, NULL)
    #define X_ABORT(msg)                X_ASSERTION_FAILED("Program abort", msg, __func__, __FILE__, __LINE__)
    #define X_ABORT_DEFAULT             default: X_ABORT("Runtime error"); break
#else
    #define X_ASSERT_MSG(expr, msg)     (void)0
    #define X_ASSERT(expr)              (void)0
    #define X_ABORT(msg)                (void)0
    #define X_ABORT_DEFAULT             default: break
#endif


#define x_vprintf   X_VPRINTF
int x_set_log_level(int level);
void x_printf(const char* fmt, ...);
void x_unbufferd_printf(const char* fmt, ...);
void x_hexdump(const void* src, unsigned len, unsigned cols);


#ifdef __cplusplus
}
#endif


#endif // xdebug_h_


