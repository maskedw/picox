/**
 *       @file  xdebug.h
 *      @brief  デバッグログやassert等の定義
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

#ifndef picox_core_detail_xdebug_h_
#define picox_core_detail_xdebug_h_


/** @addtogroup core
 *  @{
 *  @addtogroup xdebug
 *  @brief デバッグログやassert等の機能を提供します
 *  @{
 */


#ifdef __cplusplus
extern "C" {
#endif

/** @brief アサーションに失敗した時に呼び出される関数のポインタ型です
 *
 *  @param expr アサーションの式(expression)
 *  @param fmt  追加メッセージのprintf形式format
 *  @param func アサーションを行った関数名
 *  @param file アサーションを行ったソースファイル名
 *  @param line アサーションを行ったソースファイルの行番号
 *  @param ...  fmtの可変長引数
 */
typedef void(*XAssertionFailedFunc)(const char* expr, const char* fmt, const char* func, const char* file, int line, ...);


/** @brief x_assertion_failedの呼び出し前に呼びされる関数のポインタです
 *
 *  アサーション失敗時にx_assertion_failedの呼び出し前に何か処理を行いたいときに
 *  使用します。
 */
extern void (*x_pre_assertion_failed)(void);


/** @brief アサーション失敗時に呼びされる関数のポインタです
 *
 *  アサーション失敗時の処理を独自に行いたいときに設定してください。
 */
extern XAssertionFailedFunc x_assertion_failed;


/** @brief x_assertion_failedの呼び出し後に呼びされる関数のポインタです
 *
 *  アサーション失敗時にx_assertion_failedの呼び出し後に何か処理を行いたいときに
 *  使用します。
 *
 *  [例]
 *  + ログファイルに記録する
 *  + CPUリセットを行い、プログラムを再起動させる
 */
extern void (*x_post_assertion_failed)(void);


/** @name  log_levels
 *  @brief ログレベルのグループです
 *
 *  ログの重要度別にフィルタをかけられる仕組みがあるので、その振り分けに使用しま
 *  す。各ログにどのレベルを適用するのかは難しいですが、以下をガイドラインとして
 *  ください。
 *
 *  + VERB 普段だと冗長なレベルの詳細な情報 (詳細デバッグ時に)
 *  + INFO 標準的なログ
 *  + NOTI プログラム実行時に必ず表示しておきたい情報
 *  + WARN 問題だが復帰可能な状態
 *  + ERR  復帰不可能な致命的状態
 *
 *  これで、デバッグ時のログ出力レベルはINFO, リリース時はNOTIとしておくと、丁度
 *  いい塩梅だと思います。
 *  @{
 */
#define X_LOG_LEVEL_ERR    (1)  /* error      */
#define X_LOG_LEVEL_WARN   (2)  /* warning    */
#define X_LOG_LEVEL_NOTI   (3)  /* notice     */
#define X_LOG_LEVEL_INFO   (4)  /* infomation */
#define X_LOG_LEVEL_VERB   (5)  /* verbose    */
#define X_LOG_LEVEL        X_CONF_LOG_LEVEL


/** @} end of name log_levels
 */


/** @name  log_levels
 *  @brief ログの色付けのグループです
 *
 *  [ANSIカラー](https://en.wikipedia.org/wiki/ANSI_escape_code#Colors)
 *  でログヘッダに色付けをし、ログレベルを視覚的に見やすくします。
 *  @{
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


/** @} end of name log_colors
 */


/** @name  log_macros
 *  @brief ログ出力マクロのグループです
 *
 *  X_LOG_XXX()は、printfライクなformat引数を受け付けますが、
 *  X_LOG_XXX((tag, "i = %d",i));という風に、引数を2重で()する必要があります。
 *
 *  C99の可変長引数マクロを使えばこんな変な書き方をしないですむのですが、
 *  このライブラリの対象にはC++03も含めており、C++03では可変長引数マクロは標準機
 *  能ではありません。
 *
 *  たいていのコンパイラでは、標準ではなくとも可変長引数マクロをサポートしていま
 *  すが、規格に厳格なコンパイラでは使用できません。(例 Renesas C++ compiler)
 *  @{
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
    #define X_LOG_HEXDUMP_INFO(args) x_info_hexdumplog  args
#else
    #define X_LOG_INFO(args)         (void)0
    #define X_LOG_HEXDUMP_INFO(args) (void)0
#endif


#if (X_CONF_USE_DYNAMIC_LOG_SUPPRESS != 0) || (X_LOG_LEVEL >= X_LOG_LEVEL_NOTI)
    #define X_LOG_NOTI(args)         x_noti_printlog args
    #define X_LOG_HEXDUMP_NOTI(args) x_noti_hexdumplog  args
#else
    #define X_LOG_NOTI(args)         (void)0
    #define X_LOG_HEXDUMP_NOTI(args) (void)0
#endif


#if (X_CONF_USE_DYNAMIC_LOG_SUPPRESS != 0) || (X_LOG_LEVEL >= X_LOG_LEVEL_WARN)
    #define X_LOG_WARN(args)         x_warn_printlog args
    #define X_LOG_HEXDUMP_WARN(args) x_warn_hexdumplog  args
#else
    #define X_LOG_WARN(args)         (void)0
    #define X_LOG_HEXDUMP_WARN(args) (void)0
#endif


#if (X_CONF_USE_DYNAMIC_LOG_SUPPRESS != 0) || (X_LOG_LEVEL >= X_LOG_LEVEL_ERR)
    #define X_LOG_ERR(args)          x_err_printlog args
    #define X_LOG_HEXDUMP_ERR(args)  x_err_hexdumplog  args
#else
    #define X_LOG_ERR(args)          (void)0
    #define X_LOG_HEXDUMP_ERR(args)  (void)0
#endif


/** @} end of name log_macros
 */


/** @name  assertions
 *  @brief assertマクロのグループです
 *  @{
 */


/** @brief picoxデフォルトのアサートです
 */
#define X_DEFAULT_ASSERT(expr)              X_DEFAULT_ASSERT_MSG(expr, "")


/** @brief 追加メッセージを指定可能なアサートです
 */
// #define X_DEFAULT_ASSERT_MSG(expr, msg)     ((expr) ? (void)0 : x_assertion_failed(#expr, msg, X_FUNC, __FILE__, __LINE__))
#define X_DEFAULT_ASSERT_MSG(expr, msg)             ((expr) ? (void)0 : x_assertion_failed(#expr, "%s", X_FUNC, __FILE__, __LINE__, msg))

#define X_ASSERTION_FAILED(strexpr, msg)                  x_assertion_failed(strexpr, "", X_FUNC, __FILE__, __LINE__)
#define X_ASSERTION_FAILED_MSG(strexpr, msg)              x_assertion_failed(strexpr, "%s", X_FUNC, __FILE__, __LINE__, msg)
#define X_ASSERTION_FAILED_FMT_A1(strexpr, fmt, a)        x_assertion_failed(strexpr, fmt, X_FUNC, __FILE__, __LINE__, a)
#define X_ASSERTION_FAILED_FMT_A2(strexpr, fmt, a, b)     x_assertion_failed(strexpr, fmt, X_FUNC, __FILE__, __LINE__, a, b)


#if (X_CONF_NDEBUG == 0)
    #define X_ASSERT(expr)              X_DEFAULT_ASSERT(expr)
    #define X_ASSERT_MSG(expr, msg)     X_DEFAULT_ASSERT_MSG(expr, msg)
#else
    #define X_ASSERT(expr)              (void)0
    #define X_ASSERT_MSG(expr, msg)     (void)0
#endif


/** @def   X_ASSERT_SELF
 *  @brief クラス指向モジュールの第一引数のNULLチェック用アサートです
 */
#if X_CONF_USE_ASSERT_SELF != 0
    #define X_ASSERT_SELF(expr)         X_ASSERT(expr)
#else
    #define X_ASSERT_SELF(expr)         (void)0
#endif


/** @def   X_ASSERT_ARG
 *  @brief 引数の範囲チェック等のためのアサートです
 */
#if X_CONF_USE_ASSERT_ARG != 0
    #define X_ASSERT_ARG(expr)          X_ASSERT(expr)
#else
    #define X_ASSERT_ARG(expr)          (void)0
#endif


/** @def   X_ASSERT_TAG
 *  @brief 規定のタグがセットされているかの検査用アサートです
 */
#if X_CONF_USE_ASSERT_TAG
    #define X_ASSERT_TAG(expr)          X_ASSERT(expr)
#else
    #define X_ASSERT_TAG(expr)          (void)0
#endif


/** @def   X_ASSERT_ENUM
 *  @brief 不正な列挙値が指定されていないかの検査用アサートです
 */
#if X_CONF_USE_ASSERT_ENUM
    #define X_ASSERT_ENUM(expr)         X_ASSERT(expr)
#else
    #define X_ASSERT_ENUM(expr)         (void)0
#endif


/** @def   X_ASSERT_MALLOC_NULL
 *  @brief 動的メモリ確保のNULL検査用アサートです
 */
#if X_CONF_USE_DETECT_MALLOC_NULL != 0
    #define X_ASSERT_MALLOC_NULL(expr)       X_ASSERT(expr)
#else
    #define X_ASSERT_MALLOC_NULL(expr)       (void)0
#endif


/** @def   X_ABORT
 *  @brief プログラムの実行を強制停止します
 */
#define X_ABORT(msg)                X_ASSERT_MSG(0, msg)


/** @def   X_ABORT_DEFAULT
 *  @brief switch文でdefaultの時にX_ABORT()を呼び出します
 */
#define X_ABORT_DEFAULT             default: X_ABORT("Runtime error"); break


/** @def   X_EXIT
 *  @brief プログラムの実行を強制停止します
 */
#define X_EXIT(return_code)         X_ABORT("exit:" #return_code)


/** @} end of name assertions
 */


/** @name  log_headers
 *  @brief ログヘッダ文字列のグループです
 *  @{
 */
#define X_VERB_COLOR    ""
#define X_INFO_COLOR    X_COLOR_GREEN
#define X_NOTI_COLOR    X_COLOR_MAGENTA
#define X_WARN_COLOR    X_COLOR_YELLOW
#define X_ERR_COLOR     X_COLOR_RED
#define X_VERB_HEADER   X_VERB_COLOR X_CONF_VERB_HEADER X_COLOR_RESET
#define X_INFO_HEADER   X_INFO_COLOR X_CONF_INFO_HEADER X_COLOR_RESET
#define X_NOTI_HEADER   X_NOTI_COLOR X_CONF_NOTI_HEADER X_COLOR_RESET
#define X_WARN_HEADER   X_WARN_COLOR X_CONF_WARN_HEADER X_COLOR_RESET
#define X_ERR_HEADER    X_ERR_COLOR  X_CONF_ERR_HEADER  X_COLOR_RESET


/** @} end of name log_headers
 */


int x_set_log_level(int level);
void x_hexdump(const void* src, size_t len, size_t cols);
void x_err_hexdump(const void* src, size_t len, size_t cols);


/** @name  log_functions
 *  @brief ログ出力関数のグループです
 *
 *  コンパイル時に除去できるように、これらの関数は直接呼び出さす、log_macrosのマ
 *  クロを使用してください。
 *  @{
 */
void x_verb_printlog(const char* tag, const char* fmt, ...);
void x_info_printlog(const char* tag, const char* fmt, ...);
void x_noti_printlog(const char* tag, const char* fmt, ...);
void x_warn_printlog(const char* tag, const char* fmt, ...);
void x_err_printlog(const char* tag, const char* fmt, ...);
void x_verb_hexdumplog(const char* tag, const void* src, size_t len, size_t cols, const char* fmt, ...);
void x_info_hexdumplog(const char* tag, const void* src, size_t len, size_t cols, const char* fmt, ...);
void x_noti_hexdumplog(const char* tag, const void* src, size_t len, size_t cols, const char* fmt, ...);
void x_warn_hexdumplog(const char* tag, const void* src, size_t len, size_t cols, const char* fmt, ...);
void x_err_hexdumplog(const char* tag, const void* src, size_t len, size_t cols, const char* fmt, ...);


/** @} end of name log_functions
 */


/** @name  assertions_ex
 *  @brief 拡張版assertマクロのグループです
 *  @details
 *
 *  デバッガに接続しているときはアサーション失敗時にスタックトレースから変数の
 *  値を確認することは容易ですが、未接続時には失敗したことしかわからず不便なこと
 *  があります。(オンラインコンパイラのmbedや、Arduino等での開発も含む)
 *  このグループのアサーションを使用すると失敗時に変数の値も出力します。
 *  @{
 */

#define X_SIZE_MODIFIER   "z"
#define X_CHAR_MODIFIER   "hh"
#define X_SHORT_MODIFIER  "h"

#define X_ASSERT_TRUE(expr) \
    do { \
        if (!X_LIKELY(expr)) { \
            X_ASSERTION_FAILED(#expr, "is not true"); \
      } \
    } while (0)
#define X_ASSERT_FALSE(expr) \
    do { \
        if (!X_LIKELY(!(expr))) { \
        X_ASSERTION_FAILED(#expr, "is not false"); \
        } \
    } while (0)
#define X_ASSERT_TYPE_FULL(prefix, suffix, T, fmt, a, op, b)   \
    do { \
        T tmp_a_ = (a); \
        T tmp_b_ = (b); \
        if (!(tmp_a_ op tmp_b_)) {                               \
            X_ASSERTION_FAILED_FMT_A2(#a " " #op " " #b "", "(" prefix "%" fmt suffix " " #op " " prefix "%" fmt suffix ")", \
                     tmp_a_, tmp_b_); \
        } \
    } while (0)
#define X_ASSERT_TYPE(T, fmt, a, op, b) \
  X_ASSERT_TYPE_FULL("", "", T, fmt, a, op, b)
#define X_ASSERT_CHAR(a, op, b) \
  X_ASSERT_TYPE_FULL("'\\x", "'", char, "02" X_CHAR_MODIFIER "x", a, op, b)
#define X_ASSERT_UCHAR(a, op, b) \
  X_ASSERT_TYPE_FULL("'\\x", "'", unsigned char, "02" X_CHAR_MODIFIER "x", a, op, b)
#define X_ASSERT_SHORT(a, op, b) \
  X_ASSERT_TYPE(short, X_SHORT_MODIFIER "d", a, op, b)
#define X_ASSERT_USHORT(a, op, b) \
  X_ASSERT_TYPE(unsigned short, X_SHORT_MODIFIER "u", a, op, b)
#define X_ASSERT_INT(a, op, b) \
  X_ASSERT_TYPE(int, "d", a, op, b)
#define X_ASSERT_UINT(a, op, b) \
  X_ASSERT_TYPE(unsigned int, "u", a, op, b)
#define X_ASSERT_LONG(a, op, b) \
  X_ASSERT_TYPE(long int, "ld", a, op, b)
#define X_ASSERT_ULONG(a, op, b) \
  X_ASSERT_TYPE(unsigned long int, "lu", a, op, b)
#define X_ASSERT_SIZE(a, op, b) \
  X_ASSERT_TYPE(size_t, X_SIZE_MODIFIER "u", a, op, b)
#define X_ASSERT_float(a, op, b) \
  X_ASSERT_TYPE(float, "f", a, op, b)
#define X_ASSERT_PTR(a, op, b) \
  X_ASSERT_TYPE(const void*, "p", a, op, b)
#define X_ASSERT_INT8(a, op, b) \
  X_ASSERT_TYPE(int8_t, PRIi8, a, op, b)
#define X_ASSERT_UINT8(a, op, b) \
  X_ASSERT_TYPE(uint8_t, PRIu8, a, op, b)
#define X_ASSERT_INT16(a, op, b) \
  X_ASSERT_TYPE(int16_t, PRIi16, a, op, b)
#define X_ASSERT_UINT16(a, op, b) \
  X_ASSERT_TYPE(uint16_t, PRIu16, a, op, b)
#define X_ASSERT_INT32(a, op, b) \
  X_ASSERT_TYPE(int32_t, PRIi32, a, op, b)
#define X_ASSERT_UINT32(a, op, b) \
  X_ASSERT_TYPE(uint32_t, PRIu32, a, op, b)
#define X_ASSERT_PTR_EQUAL(a, b) \
    X_ASSERT_PTR(a, ==, b)
#define X_ASSERT_PTR_NOT_EQUAL(a, b) \
    X_ASSERT_PTR(a, !=, b)
#define X_ASSERT_NULL(ptr) \
    X_ASSERT_PTR(ptr, ==, NULL)
#define X_ASSERT_NOT_NULL(ptr) \
    X_ASSERT_PTR(ptr, !=, NULL)
#define X_ASSERT_PTR_NULL(ptr) \
    X_ASSERT_PTR(ptr, ==, NULL)
#define X_ASSERT_PTR_NOT_NULL(ptr) \
    X_ASSERT_PTR(ptr, !=, NULL)
#define X_ASSERT_STRING_EQUAL(a, b) \
    do { \
        const char* tmp_a_ = a; \
        const char* tmp_b_ = b; \
        if (X_UNLIKELY(strcmp(tmp_a_, tmp_b_) != 0)) { \
            X_ASSERTION_FAILED_FMT_A2(#a " == " #b, "(\"%s\" == \"%s\")", \
                       tmp_a_, tmp_b_); \
        } \
    } while (0)
#define X_ASSERT_STRING_NOT_EQUAL(a, b) \
    do { \
        const char* tmp_a_ = a; \
        const char* tmp_b_ = b; \
        if (X_UNLIKELY(strcmp(tmp_a_, tmp_b_) == 0)) { \
            X_ASSERTION_FAILED_FMT_A2(#a " != " #b, "(\"%s\" == \"%s\")", \
                     tmp_a_, tmp_b_); \
      } \
    } while (0)
#define X_ASSERT_MEMORY_EQUAL(size, a, b) \
    do { \
        const unsigned char* tmp_a_ = (const unsigned char*) (a); \
        const unsigned char* tmp_b_ = (const unsigned char*) (b); \
        const size_t tmp_size_ = (size); \
        if (X_UNLIKELY(memcmp(tmp_a_, tmp_b_, tmp_size_)) != 0) { \
            size_t tmp_pos_; \
            for (tmp_pos_ = 0 ; tmp_pos_ < tmp_size_ ; tmp_pos_++) { \
                if (tmp_a_[tmp_pos_] != tmp_b_[tmp_pos_]) { \
                    X_ASSERTION_FAILED_FMT_A1(#a " == " #b, "memory not equal. at offset %" X_SIZE_MODIFIER "u", tmp_pos_); \
                    break; \
                } \
            } \
        } \
    } while (0)
#define X_ASSERT_MEMORY_NOT_EQUAL(size, a, b) \
    do { \
        const unsigned char* tmp_a_ = (const unsigned char*) (a); \
        const unsigned char* tmp_b_ = (const unsigned char*) (b); \
        const size_t tmp_size_ = (size); \
        if (X_UNLIKELY(memcmp(tmp_a_, tmp_b_, tmp_size_)) == 0) { \
            X_ASSERTION_FAILED_FMT_A1(#a " != " #b, "memory equal %zu bytes", tmp_size_); \
        } \
    } while (0)


/** @} end of name assertions_ex
 */


#ifdef __cplusplus
}
#endif


/** @} end of addtogroup xdebug
 *  @} end of addtogroup core
 */


#endif // picox_core_detail_xdebug_h_
