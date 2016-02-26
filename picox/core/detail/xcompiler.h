/**
 *       @file  compiler.h
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


#ifndef picox_core_detail_xcompiler_h_
#define picox_core_detail_xcompiler_h_


/** @def    X_COMPILER_C99
 *  @brief  コンパイラがC99に対応しているかどうか
 */
#ifdef __STDC_VERSION__
    #if (__STDC_VERSION__ >= 199901L)
        #define X_COMPILER_C99 (1)
    #else
        #define X_COMPILER_C99 (0)
    #endif
#else
    #define X_COMPILER_C99 (0)
#endif


/** @def    X_GNUC_PREREQ
 *  @brief  GCCのバージョンがn.m以上かどうかを返します
 */
#if defined __GNUC__ && defined __GNUC_MINOR__
    #define X_GNUC_PREREQ(maj, min) \
        ((__GNUC__ << 16) + __GNUC_MINOR__ >= ((maj) << 16) + (min))
#else
    #define X_GNUC_PREREQ(maj, min) 0
#endif


#ifdef __GNUC__

#include <picox/core/detail/compiler/xgcc.h>

#endif


/** @def    X_INLINE
 *  @brief  コンパイラに対して、関数のインライン展開を指示するキーワードです。
 *
 *  実際にインライン展開されるかどうかは、コンパイラが判断します。
 */
#ifndef X_INLINE
    #define X_INLINE  static inline
#endif


/** @def    X_ALWAYS_INLINE
 *  @brief  コンパイラに対して、関数のインライン展開を強制するキーワードです。
 *
 *  最適化レベルや、関数の内容に関わらず、必ずインライン化されるようにします。
 */
#ifndef X_ALWAYS_INLINE
    #define X_ALWAYS_INLINE static inline
#endif


/** @def    X_HAS_VARIADIC_MACROS
 *  @brief  コンパイラが可変長引数マクロに対応しているかどうか
 */
#ifndef X_HAS_VARIADIC_MACROS
    #if X_COMPILER_C99 > 0
        #define X_HAS_VARIADIC_MACROS   (1)
    #endif
#endif


/** @def    X_HAS_TYPEOF
 *  @brief  コンパイラがtypeof拡張に対応しているかどうか
 *
 *  X_HAS_TYPEOF == 1の場合、X_TYPEOF()が使用可能です。
 */
#ifndef X_HAS_TYPEOF
    #define X_HAS_TYPEOF    (0)
#endif


/** @def    X_LIKELY
 *  @brief  条件分岐に使用するコンパイラ最適化ディレクティブです
 *
 *  参考サイト<br>
 *  http://d.hatena.ne.jp/tkuro/20110114/1294956535
 */
#ifndef X_LIKELY
    #define X_LIKELY(x)         x
#endif


/** @def    X_UNLIKELY
 *  @brief  条件分岐に使用するコンパイラ最適化ディレクティブです
 *
 *  @see X_LIKELY
 */
#ifndef X_UNLIKELY
    #define X_UNLIKELY(x)         x
#endif


/** @def    X_HAS_STATEMENT_EXPRESSIONS
 *  @brief  {}で囲まれた複合文と変数の宣言を()の式の中で使用できるかどうか
 *
 *  これに対応していると以下のような無茶なことができます。
 *  @code
 *  if ({ func1();
 *        if (x) // if の中に ifも書ける
 *          ...
 *        else
 *          ...
 *        1; // 最後の式がifの判定に使用される
 *        })
 *      ...
 *  @endcode
 */
#ifndef X_HAS_STATEMENT_EXPRESSIONS
    #define X_HAS_STATEMENT_EXPRESSIONS (0)
#endif


/** @def    X_MEMORY_BARRIER
 *  @brief  メモリ動作の順序性を制御するCPU命令です
 */
#ifndef X_MEMORY_BARRIER
    #define X_MEMORY_BARRIER
#endif


/** @def    X_UNREACHABE
 *  @brief  以下のコードには決して到達しないことを表明するコンパイラディレクティブです
 */
#ifndef X_UNREACHABE
    #define X_UNREACHABE    for (;;)
#endif


/** @def    X_DEPRECATED
 *  @brief  指定の定義が廃止予定や非推奨であることを表明するコンパイラディレクティブです
 *
 *  コンパイラがこの機能に対応していれば、DEPRECATED指定の定義が使用されている場
 *  合、コンパイル時に警告が出力されます。
 *  関数以外にも、型定義や、変数にも使用することができます。
 *
 *  @code
 *  X_DEPRECATED void deprecated_func(void);
 *  X_DEPRECATED typedef int deprecated_type_t;
 *  @endcode
 */
#ifndef X_DEPRECATED
    #define X_DEPRECATED
#endif


#ifndef NULL
    #define NULL    ((void*)0)
#endif


#ifndef EOF
    #define EOF     (-1)
#endif


#endif // picox_core_detail_xcompiler_h_
