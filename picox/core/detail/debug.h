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


#if X_CONF_USE_ANSI_COLOR_LOG != 0
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
    #define X_COLOR_BLACK
    #define X_COLOR_RED
    #define X_COLOR_GREEN
    #define X_COLOR_YELLOW
    #define X_COLOR_BLUE
    #define X_COLOR_MAGENTA
    #define X_COLOR_CYAN
    #define X_COLOR_WHITE
    #define X_COLOR_RESET
#endif


#define X_LOG_LEVEL     X_CONF_LOG_LEVEL


void x_verb_printlog(const char* tag, const char* fmt, ...);
void x_info_printlog(const char* tag, const char* fmt, ...);
void x_noti_printlog(const char* tag, const char* fmt, ...);
void x_warn_printlog(const char* tag, const char* fmt, ...);
void x_err_printlog(const char* tag, const char* fmt, ...);
void x_verb_hexdump(const char* tag, const void* src, size_t len, size_t cols, const char* fmt, ...);
void x_info_hexdump(const char* tag, const void* src, size_t len, size_t cols, const char* fmt, ...);
void x_noti_hexdump(const char* tag, const void* src, size_t len, size_t cols, const char* fmt, ...);
void x_warn_hexdump(const char* tag, const void* src, size_t len, size_t cols, const char* fmt, ...);
void x_err_hexdump(const char* tag, const void* src, size_t len, size_t cols, const char* fmt, ...);


/* X_LOG_XXX()は、printfライクなformat引数を受け付けますが、
 * X_LOG_XXX((tag, "i = %d",i));という風に、引数を2重で()する必要があります。
 * C99の可変長引数マクロを使えばこんな変な書き方をしないですむのですが、
 * このライブラリの対象にはC++03も含めており、C++03では可変長引数マクロは標準で
 * はありません。
 *
 * たいていのコンパイラでは、標準ではなくとも可変長引数マクロを使用できますが、
 * 規格に厳格なコンパイラでは使用できません。(例 Renesas C++ compiler)
 * そのため、非常に面倒ですが、C++と共用の部分では可変長引数マクロは使用しません
 */
#if (X_CONF_USE_DYNAMIC_LOG_SUPPRESS != 0) || (X_LOG_LEVEL >= X_LOG_LEVEL_VERB)
    #define X_LOG_VERB(args)         x_verb_printlog args
    #define X_LOG_HEXDUMP_VERB(args) x_verb_hexdump  args
#else
    #define X_LOG_VERB(args)         (void)0
    #define X_LOG_HEXDUMP_VERB(args) (void)0
#endif


#if (X_CONF_USE_DYNAMIC_LOG_SUPPRESS != 0) || (X_LOG_LEVEL >= X_LOG_LEVEL_INFO)
    #define X_LOG_INFO(args)         x_info_printlog args
    #define X_LOG_HEXDUMP_INFO(args) x_info_hexdump  args
#else
    #define X_LOG_INFO(args)         (void)0
    #define X_LOG_HEXDUMP_INFO(args) (void)0
#endif


#if (X_CONF_USE_DYNAMIC_LOG_SUPPRESS != 0) || (X_LOG_LEVEL >= X_LOG_LEVEL_NOTI)
    #define X_LOG_NOTI(args)         x_noti_printlog args
    #define X_LOG_HEXDUMP_NOTI(args) x_noti_hexdump  args
#else
    #define X_LOG_NOTI(args)         (void)0
    #define X_LOG_HEXDUMP_NOTI(args) (void)0
#endif


#if (X_CONF_USE_DYNAMIC_LOG_SUPPRESS != 0) || (X_LOG_LEVEL >= X_LOG_LEVEL_WARN)
    #define X_LOG_WARN(args)         x_warn_printlog args
    #define X_LOG_HEXDUMP_WARN(args) x_warn_hexdump  args
#else
    #define X_LOG_WARN(args)         (void)0
    #define X_LOG_HEXDUMP_WARN(args) (void)0
#endif


#if (X_CONF_USE_DYNAMIC_LOG_SUPPRESS != 0) || (X_LOG_LEVEL >= X_LOG_LEVEL_ERR)
    #define X_LOG_ERR(args)          x_err_printlog args
    #define X_LOG_HEXDUMP_ERR(args)  x_err_hexdump  args
#else
    #define X_LOG_ERR(args)          (void)0
    #define X_LOG_HEXDUMP_ERR(args)  (void)0
#endif


#define X_VERB_COLOR    ""
#define X_INFO_COLOR    X_COLOR_GREEN
#define X_NOTI_COLOR    X_COLOR_MAGENTA
#define X_WARN_COLOR    X_COLOR_YELLOW
#define X_ERR_COLOR     X_COLOR_RED

#define X_VERB_HEADER     X_VERB_COLOR X_CONF_VERB_HEADER X_COLOR_RESET
#define X_INFO_HEADER     X_INFO_COLOR X_CONF_INFO_HEADER X_COLOR_RESET
#define X_NOTI_HEADER     X_NOTI_COLOR X_CONF_NOTI_HEADER X_COLOR_RESET
#define X_WARN_HEADER     X_WARN_COLOR X_CONF_WARN_HEADER X_COLOR_RESET
#define X_ERR_HEADER      X_ERR_COLOR  X_CONF_ERR_HEADER  X_COLOR_RESET


#define X_DEFAULT_ASSERT(expr)              X_DEFAULT_ASSERT_MSG(expr, NULL)
#define X_DEFAULT_ASSERT_MSG(expr, msg)     ((expr) ? (void)0 : x_assertion_failed(#expr, msg, __func__, __FILE__, __LINE__))

#if (X_CONF_NDEBUG != 0)
    #define X_ASSERT(expr)              (void)0
    #define X_ASSERT_MSG(expr, msg)     (void)0
    #define X_ABORT(msg)                (void)0
    #define X_ABORT_DEFAULT             default: break
#else
    #define X_ASSERT(expr)              X_DEFAULT_ASSERT(expr)
    #define X_ASSERT_MSG(expr, msg)     X_DEFAULT_ASSERT_MSG(expr, msg)
    #define X_ABORT(msg)                X_ASSERT_MSG(0, msg)
    #define X_ABORT_DEFAULT             default: X_ABORT("Runtime error"); break
#endif


#define x_vprintf   X_CONF_VPRINTF
int x_set_log_level(int level);
void x_printf(const char* fmt, ...);
void x_putc(int c);
void x_hexdump(const void* src, size_t len, size_t cols);


#ifdef __cplusplus
}
#endif


#endif // picox_core_detail_debug_h_
