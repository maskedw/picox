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


/** @addtogroup core
 *  @{
 *  @addtogroup xcompiler
 *  @brief コンパイラごとの依存機能を吸収します
 *
 *  コンパイラごとに独自の拡張機能があったり、同じ機能でも構文が違ったり、C標準
 *  (特にC99)で未サポートの機能があったりと、コンパイラ非依存のプログラムの記述
 *  は相当大変です。このモジュールが提供する機能を利用することで、その負担を軽減
 *  できます。
 *  @{
 */


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


/* コンパイラごとの設定のロード */
#if defined(__GNUC__)

    #include <picox/core/detail/compiler/xgcc.h>

#elif defined(__RENESAS__) || defined(__CA78K0R__)

    #include <picox/core/detail/compiler/xrenesas.h>

#endif


/** @def    X_INLINE
 *  @brief  コンパイラに対して、関数のインライン展開を指示するキーワードです。
 *
 *  実際にインライン展開されるかどうかは、コンパイラが判断します。
 */
#ifndef X_INLINE
    #ifdef X_COMPILER_NO_INLINE
        #define X_INLINE    static
    #else
        #define X_INLINE    static inline
    #endif
#endif


/** @def    X_ALWAYS_INLINE
 *  @brief  コンパイラに対して、関数のインライン展開を強制するキーワードです。
 *
 *  コンパイラがこの機能をサポートしていれば、最適化レベルや、関数の内容に関わら
 *  ず、必ずインライン化されるようにします。
 */
#ifndef X_ALWAYS_INLINE
    #define X_ALWAYS_INLINE     X_INLINE
#endif


/** @def    X_HAS_VARIADIC_MACROS
 *  @brief  コンパイラが可変長引数マクロに対応しているかどうか
 *
 *  @note
 *  特に気をつけるべきなのが、CコンパイラがC99に対応していても、C++だと使えない
 *  というケース(例 Renesasコンパイラ)があることです。C++が可変長引数マクロに正
 *  式対応したのはC++11からなので、まあ規格準拠ではありますがちょっと納得がいか
 *  ない・・・。
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
 *  X_LIKELYは式の結果が、ほとんどの場合"真"と仮定してよい、とコンパイラに指示を
 *  出します。
 *  例えば、関数内で引数の整合性チェックを行う場合、不正な引数が渡される可能性は
 *  低いと仮定してよいはずです。そういった場合にこのディレクティブを使用しておく
 *  と、コンパイラはより最適化されたコードを生成できる可能性があります。
 *
 *  @code {.c}
 *  bool functon(char* str)
 *  {
 *      // strがNULLなのは例外パターンなので、ほとんどのケースで真だと仮定してよい
 *      if (!X_LIKELY(str))
 *          return false;
 *      ...
 *  }
 *  @endcode
 *
 *  参考サイト<br>
 *  http://d.hatena.ne.jp/tkuro/20110114/1294956535
 */
#ifndef X_LIKELY
    #define X_LIKELY(expr)         expr
#endif


/** @def    X_UNLIKELY
 *  @brief  条件分岐に使用するコンパイラ最適化ディレクティブです
 *
 *  X_LIKELYとは逆で、exprはほとんどの場合"偽"であると、コンパイラに指示します。
 *
 *  @see X_LIKELY
 */
#ifndef X_UNLIKELY
    #define X_UNLIKELY(expr)         expr
#endif


/** @def    X_HAS_STATEMENT_EXPRESSIONS
 *  @brief  {}で囲まれた複合文と変数の宣言を()の式の中で使用できるかどうか
 *
 *  これに対応していると以下のような無茶なことができます。
 *  @code {.c}
 *  if ({ func1();
 *        if (x) // if の中に ifも書ける
 *          ...
 *        else
 *          ...
 *        1; // 最後の式がifの判定に使用される
 *        })
 *      ...
 *  @endcode
 *
 *  @note
 *  これが標準でサポートされると、相当面白いことができそうなんですが、まあされな
 *  いでしょう。プログラムがGCC限定でいいなら、使ってもいいかも。
 */
#ifndef X_HAS_STATEMENT_EXPRESSIONS
    #define X_HAS_STATEMENT_EXPRESSIONS (0)
#endif


/** @def   X_MEMORY_BARRIER
 *  @brief メモリ動作の順序性を制御するCPU命令です
 */
#ifndef X_MEMORY_BARRIER
    #define X_MEMORY_BARRIER
#endif


/** @def   X_UNREACHABE
 *  @brief 以下のコードには決して到達しないことを表明するコンパイラディレクティブです
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


#ifndef X_FUNC
    #define X_FUNC  __func__
#endif


/** @name  packed
 *  @brief 構造体のパックオプションのグループです
 *
 *  構造体のパディングを行わないパックオプションはバイナリデータの読み書きに便利
 *  ですが、C標準ではないですが、多くのコンパイラがサポートしています。
 *  しかし、構文はコンパイラごとに違うので、4つのセクションを設けて、様々な構文
 *  に対応できるようにしています。パックする構造体は以下のように定義してください
 *  。
 *
 *  @code {.c}
 *
 *  X_PACKED_PRE_BEGIN
 *  struct Foo X_PACKED_POST_BEGIN
 *  {
 *  } X_PACKED_PRE_END;
 *  X_PACKED_POST_END
 *
 *  X_PACKED_PRE_BEGIN
 *  typedef struct X_PACKED_POST_BEGIN
 *  {
 *  } X_PACKED_PRE_END Foo;
 *  X_PACKED_POST_END
 *
 *
 *  @endcode
 *
 *  @{
 */
#ifndef X_PACKED_PRE_BEGIN
    #define X_PACKED_PRE_BEGIN
#endif


#ifndef X_PACKED_POST_BEGIN
    #define X_PACKED_POST_BEGIN
#endif


#ifndef X_PACKED_PRE_END
    #define X_PACKED_PRE_END
#endif


#ifndef X_PACKED_POST_END
    #define X_PACKED_POST_END
#endif
/** @} end of name packed
 */


#ifndef NULL
    #define NULL    ((void*)0)
#endif


#ifndef EOF
    #define EOF     (-1)
#endif


/** @def   X_PRINTF_ATTR
 *  @brief printfスタイルの関数の引数をチェックする関数アトリビュートです
 *
 *  下記の例ではフォーマット文字列fmtは第1引数。
 *  可変引数部分は第2引数なので、X_PRINTF_ATTR(1, 2)を指定します。
 *  @code {.c}
 *  int my_printf(const char* fmt, ...) X_PRINTF_ATTR(1, 2);
 *
 *  my_printf("%d", "HOGE"); // %dに文字列を渡しているので警告が出る
 *  my_printf("%d, %d", 10); // 引数の数が足りていないので警告が出る
 *  @endcode
 *
 *  @note
 *  現在はコンパイラがGCCでない場合はこの機能は無視されます。
 */
#ifndef X_PRINTF_ATTR
    #define X_PRINTF_ATTR(format_idx, first_param_idx)
#endif


/** @} end of addtogroup xcompiler
 *  @} end of addtogroup core
 */


#endif // picox_core_detail_xcompiler_h_
