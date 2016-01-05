/**
 *       @file  gcc.h
 *      @brief
 *
 *    @details
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2015/06/21
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


#ifndef picox_core_detail_compiler_gcc_h_
#define picox_core_detail_compiler_gcc_h_


#ifndef __GNUC__
#error   This header file required 'GCC'
#endif


#ifndef __STRICT_ANSI__
    #define X_HAS_TYPEOF            (1)
    #define X_TYPEOF(x)             typeof(x)
#endif


#define X_HAS_STATEMENT_EXPRESSIONS (1)
#define X_HAS_VARIADIC_MACROS       (1)
#define X_ALWAYS_INLINE             static inline __attribute__((__always_inline__))
#define X_LIKELY(x)                 __builtin_expect(!!(x), 1)
#define X_UNLIKELY(x)               __builtin_expect(!!(x), 0)
#define X_MEMORY_BARRIER            asm volatile ("" : : : "memory")


#if X_GNUC_PREREQ(3, 1)
    #define X_DEPRECATED    __attribute__((__deprecated__))
#endif
#if X_GNUC_PREREQ(4, 5)
    #define X_UNREACHABE    __builtin_unreachable()
#endif


#if 0
#ifndef __cplusplus
    /* C++のキーワードが使用されていた場合、エラーになるようにする。
     * マクロやインライン関数内でC++キーワードを使用している場合、振る舞いがやや
     * こしくなってしまうので、C++キーワードはCでも使用しない方がいい。
     */
    #pragma GCC poison new delete class template typename
    #pragma GCC poison private protected public operator
    #pragma GCC poison friend mutable using namespace
    #pragma GCC poison cin cout cerr clog
#endif
#endif


#endif // picox_core_detail_compiler_gcc_h_
