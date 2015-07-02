/**
 *       @file  debug.h
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

#ifndef picox_core_detail_debug_h_
#define picox_core_detail_debug_h_


#ifdef __cplusplus
extern "C" {
#endif


#ifdef X_CONF_VPRINTF
    #define X_VPRINTF   X_CONF_VPRINTF
#else
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


typedef void(*XAssertionFailedFunc)(const char* expr, const char* msg, const char* func, const char* file, int line);
extern XAssertionFailedFunc x_pre_assertion_failed;
extern XAssertionFailedFunc x_post_assertion_failed;
extern XAssertionFailedFunc x_assertion_failed;


#define X_LOG_LEVEL_ERR    (1)  /* error      */
#define X_LOG_LEVEL_WARN   (2)  /* warning    */
#define X_LOG_LEVEL_NOTI   (3)  /* notice     */
#define X_LOG_LEVEL_INFO   (4)  /* infomation */
#define X_LOG_LEVEL_VERB   (5)  /* verbose    */


/* ANSIカラーについて知りたい人は以下リンクを参照せよ！
 * https://en.wikipedia.org/wiki/ANSI_escape_code#Colors
 */
#define X_ANSI_COLOR_BLACK       "\x1b[1;30m"
#define X_ANSI_COLOR_RED         "\x1b[1;31m"
#define X_ANSI_COLOR_GREEN       "\x1b[1;32m"
#define X_ANSI_COLOR_YELLOW      "\x1b[1;33m"
#define X_ANSI_COLOR_BLUE        "\x1b[1;34m"
#define X_ANSI_COLOR_MAGENTA     "\x1b[1;35m"
#define X_ANSI_COLOR_CYAN        "\x1b[1;36m"
#define X_ANSI_COLOR_WHITE       "\x1b[1;37m"
#define X_ANSI_COLOR_RESET       "\x1b[0m"


#ifdef X_CONF_HAS_ANSI_COLOR
    #define X_COLOR_BLACK       X_ANSI_COLOR_BLACK
    #define X_COLOR_RED         X_ANSI_COLOR_RED
    #define X_COLOR_GREEN       X_ANSI_COLOR_GREEN
    #define X_COLOR_YELLOW      X_ANSI_COLOR_YELLOW
    #define X_COLOR_BLUE        X_ANSI_COLOR_BLUE
    #define X_COLOR_MAGENTA     X_ANSI_COLOR_MAGENTA
    #define X_COLOR_CYAN        X_ANSI_COLOR_CYAN
    #define X_COLOR_WHITE       X_ANSI_COLOR_WHITE
    #define X_COLOR_RESET       X_ANSI_COLOR_RESET
#else
    #define X_COLOR_BLACK       ""
    #define X_COLOR_RED         ""
    #define X_COLOR_GREEN       ""
    #define X_COLOR_YELLOW      ""
    #define X_COLOR_BLUE        ""
    #define X_COLOR_MAGENTA     ""
    #define X_COLOR_CYAN        ""
    #define X_COLOR_WHITE       ""
    #ifdef X_CONF_COLOR_RESET
        #define X_COLOR_RESET   X_CONF_COLOR_RESET
    #else
        #define X_COLOR_RESET   ""
    #endif
#endif


void x_print_log(int level, const char* tag, const char* fmt, ...);
void x_log_hexdump(int level, const char* tag, const void* src, size_t len, size_t cols, const char* fmt, ...);


#ifdef X_CONF_LOG_LEVEL
    #define X_LOG_LEVEL     X_CONF_LOG_LEVEL
#else
    #define X_LOG_LEVEL     X_LOG_LEVEL_INFO
#endif


#if (! defined(X_CONF_USE_DYNAMIC_LOG_SUPPRESS)) && (X_LOG_LEVEL >= X_LOG_LEVEL_VERB)
    #define X_LOG_VERB(...)         x_print_log(X_LOG_LEVEL_VERB, __VA_ARGS__)
    #define X_LOG_HEXDUMP_VERB(...) x_log_hexdump(X_LOG_LEVEL_VERB, __VA_ARGS__)
#else
    #define X_LOG_VERB(...)         (void)0
    #define X_LOG_HEXDUMP_VERB(...) (void)0
#endif


#if (! defined(X_CONF_USE_DYNAMIC_LOG_SUPPRESS)) && (X_LOG_LEVEL >= X_LOG_LEVEL_INFO)
    #define X_LOG_INFO(...)         x_print_log(X_LOG_LEVEL_INFO, __VA_ARGS__)
    #define X_LOG_HEXDUMP_INFO(...) x_log_hexdump(X_LOG_LEVEL_INFO, __VA_ARGS__)
#else
    #define X_LOG_INFO(...)         (void)0
    #define X_LOG_HEXDUMP_INFO(...) (void)0
#endif


#if (! defined(X_CONF_USE_DYNAMIC_LOG_SUPPRESS)) && (X_LOG_LEVEL >= X_LOG_LEVEL_NOTI)
    #define X_LOG_NOTI(...)         x_print_log(X_LOG_LEVEL_NOTI, __VA_ARGS__)
    #define X_LOG_HEXDUMP_NOTI(...) x_log_hexdump(X_LOG_LEVEL_NOTI, __VA_ARGS__)
#else
    #define X_LOG_NOTI(...)         (void)0
    #define X_LOG_HEXDUMP_NOTI(...) (void)0
#endif


#if (! defined(X_CONF_USE_DYNAMIC_LOG_SUPPRESS)) && (X_LOG_LEVEL >= X_LOG_LEVEL_WARN)
    #define X_LOG_WARN(...)         x_print_log(X_LOG_LEVEL_WARN, __VA_ARGS__)
    #define X_LOG_HEXDUMP_WARN(...) x_log_hexdump(X_LOG_LEVEL_WARN, __VA_ARGS__)
#else
    #define X_LOG_WARN(...)         (void)0
    #define X_LOG_HEXDUMP_WARN(...) (void)0
#endif


#if (! defined(X_CONF_USE_DYNAMIC_LOG_SUPPRESS)) && (X_LOG_LEVEL >= X_LOG_LEVEL_ERR)
    #define X_LOG_ERR(...)          x_print_log(X_LOG_LEVEL_ERR, __VA_ARGS__)
    #define X_LOG_HEXDUMP_ERR(...)  x_log_hexdump(X_LOG_LEVEL_ERR, __VA_ARGS__)
#else
    #define X_LOG_ERR(...)          (void)0
    #define X_LOG_HEXDUMP_ERR(...)  (void)0
#endif


#ifdef X_CONF_VERB_COLOR
    #define X_VERB_COLOR    X_CONF_VERB_COLOR
#else
    #define X_VERB_COLOR
#endif


#ifdef X_CONF_INFO_COLOR
    #define X_INFO_COLOR    X_CONF_INFO_COLOR
#else
    #define X_INFO_COLOR    X_COLOR_GREEN
#endif


#ifdef X_CONF_NOTI_COLOR
    #define X_NOTI_COLOR    X_CONF_NOTI_COLOR
#else
    #define X_NOTI_COLOR    X_COLOR_MAGENTA
#endif


#ifdef X_CONF_WARN_COLOR
    #define X_WARN_COLOR    X_CONF_WARN_COLOR
#else
    #define X_WARN_COLOR    X_COLOR_YELLOW
#endif


#ifdef X_CONF_ERR_COLOR
    #define X_ERR_COLOR    X_CONF_ERR_COLOR
#else
    #define X_ERR_COLOR    X_COLOR_RED
#endif


#ifdef X_CONF_VERB_HEADER
    #define X_VERB_HEADER     X_VERB_COLOR X_CONF_VERB_HEADER X_COLOR_RESET
#else
    #define X_VERB_HEADER     X_VERB_COLOR "[VERB]" X_COLOR_RESET
#endif


#ifdef X_CONF_INFO_HEADER
    #define X_INFO_HEADER     X_INFO_COLOR X_CONF_INFO_HEADER X_COLOR_RESET
#else
    #define X_INFO_HEADER     X_INFO_COLOR "[INFO]" X_COLOR_RESET
#endif


#ifdef X_CONF_NOTI_HEADER
    #define X_NOTI_HEADER     X_NOTI_COLOR X_CONF_NOTI_HEADER X_COLOR_RESET
#else
    #define X_NOTI_HEADER     X_NOTI_COLOR "[NOTI]" X_COLOR_RESET
#endif


#ifdef X_CONF_WARN_HEADER
    #define X_WARN_HEADER     X_WARN_COLOR X_CONF_WARN_HEADER X_COLOR_RESET
#else
    #define X_WARN_HEADER     X_WARN_COLOR "[WARN]" X_COLOR_RESET
#endif


#ifdef X_CONF_ERR_HEADER
    #define X_ERR_HEADER      X_ERR_COLOR X_CONF_ERR_HEADER X_COLOR_RESET
#else
    #define X_ERR_HEADER      X_ERR_COLOR "[ERR ]" X_COLOR_RESET
#endif


#ifndef X_CONF_NDEBUG

    #ifdef X_CONF_ASSERT_MSG
        #define X_ASSERT_MSG(expr, msg) X_CONF_ASSERT_MSG(expr, msg)
    #else
        #define X_ASSERT_MSG(expr, msg) ((expr) ? (void)0 : x_assertion_failed(#expr, msg, __func__, __FILE__, __LINE__))
    #endif

    #ifdef X_CONF_ASSERT
        #define X_ASSERT(expr)          X_CONF_ASSERT(expr)
    #else
        #define X_ASSERT(expr)          X_ASSERT_MSG(expr, NULL)
    #endif

    #ifdef X_CONF_ABORT
        #define X_ABORT(msg)            X_CONF_ABORT(msg)
    #else
        #define X_ABORT(msg)            x_assertion_failed("Program abort", msg, __func__, __FILE__, __LINE__)
    #endif

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
void x_putc(int c);
void x_hexdump(const void* src, size_t len, size_t cols);


#ifdef __cplusplus
}
#endif


#endif // picox_core_detail_debug_h_
