/**
 *       @file  xconfig.h
 *      @brief
 *
 *    @details
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2015/06/28
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


#ifndef picox_xconfig_h_
#define picox_xconfig_h_


#ifdef __cplusplus
extern "C" {
#endif


/** @def X_CONF_NDEBUG
 *  アサートによる実行時チェックルーチンをコンパイル時に除去します。
 */
// #define X_CONF_NDEBUG


/** @def X_CONF_VPRINTF
 *  ログ出力に使用するvprintf相当の関数を設定します。
 *
 *  @details
 *  指定なし時はvprintf()が使用されます。
 *
 *  @attention
 *  時期リリース(v0.2)ではputc()相当に緩和され、この設定はなくなる予定です。
 */
// #define X_CONF_VPRINTF


/** @def X_CONF_ASSERT
 *  X_ASSERT()を置き換えるルーチンを設定します。
 *
 *  @see
 *  X_ASSERT()
 */
// #define X_CONF_ASSERT        do { if (!(expr)) { for (;;) ; }} while (0)


/** @def X_CONF_ASSERT_MSG
 *  X_ASSERT_MSG()を置き換えるルーチンを設定します。
 *
 *  @see
 *  X_ASSERT_MSG()
 */
// #define X_CONF_ASSERT_MSG


/** @def X_CONF_MALLOC
 *  動的メモリ確保関数を設定します。未指定時はmalloc()が使用されます。
 */
// #define X_CONF_MALLOC


/** @def X_CONF_MALLOC
 *  メモリ解放関数を設定します。未指定時はfree()が使用されます。
 */
// #define X_CONF_FREE


/** @def X_CONF_USE_DETECT_MALLOC_NULL
 *  picoxライブラリで何らかの動的メモリ確保失敗をX_ASSERT()で検出します。
 */
// #define X_CONF_USE_DETECT_MALLOC_NULL


/** @def X_CONF_BYTE_ORDER
 *  CPUのバイトオーダーを指定します。
 *
 *  @details
 *  以下の値が指定可能です。指定していると、一部関数の実行効率が上がります。
 *
 *  + X_BYTE_ORDER_LITTLE   (リトルエンディアン)
 *  + X_BYTE_ORDER_BIG      (ビッグエンディアン)
 */
// #define X_CONF_BYTE_ORDER


/** @def X_CONF_USE_ANSI_COLOR_LOG
 *  ログ出力時に、各レベルに応じて色をつけます。
 *
 *  @details
 *  ログ表示端末がANSIカラーシーケンスに対応していない場合は、表示崩れになるので
 *  注意してください。
 *
 *  @see
 *  https://en.wikipedia.org/wiki/ANSI_escape_code
 */
#define X_CONF_USE_ANSI_COLOR_LOG


/** @def X_CONF_USE_LOG_TIMESTAMP
 * ログ出力に任意のタイムスタンプを付加します。
 *
 * @details
 * この設定を有効にした場合は以下の関数を実装する必要があります。
 *
 *  @code
 *  void x_port_stimestamp(char* dst, size_t size)
 *  @endcode
 */
// #define X_CONF_USE_LOG_TIMESTAMP


/** @def X_CONF_LOG_TIMESTAMP_BUF_SIZE
 *  タイムスタンプ文字列('\0'含む)の最大値を指定します
 *  @details
 *  指定なしの場合は、32がデフォルト値となります。x_port_stimestamp()にはここで
 *  指定した値が渡されます。
 *
 *  @see
 *  X_CONF_USE_LOG_TIMESTAMP
 */
// #define X_CONF_LOG_TIMESTAMP_BUF_SIZE    (32)


/** @def X_CONF_LOG_LEVEL
 *  ログ出力レベルを設定します。
 *
 *  @details
 *  指定可能な値は以下の通りです。
 *
 *  + X_LOG_LEVEL_VERB  (優先度 低)
 *  + X_LOG_LEVEL_INFO
 *  + X_LOG_LEVEL_NOTI
 *  + X_LOG_LEVEL_WARN
 *  + X_LOG_LEVEL_ERR   (優先度 高)
 *
 *  指定優先度未満のログは、X_CONF_USE_DYNAMIC_LOG_SUPPRESSが未定義であれば、コ
 *  ンパイル時に除去されます。
 */
// #define X_CONF_LOG_LEVEL


/** @def X_CONF_USE_DYNAMIC_LOG_SUPPRESS
 *  ログ出力レベルの実行時変更機能を使用します。
 *
 *  @details
 *  x_set_log_level()でログレベルを指定し、指定レベル未満のログは、出力されませ
 *  ん。
 */
// #define X_CONF_USE_DYNAMIC_LOG_SUPPRESS


/** @def X_CONF_VERB_HEADER
 *  VERBOSEレベルのログヘッダ文字列を指定します。
 */
// #define X_CONF_VERB_HEADER


/** @def X_CONF_INFO_HEADER
 *  INFOMATONレベルのログヘッダ文字列を指定します。
 */
// #define X_CONF_INFO_HEADER


/** @def X_CONF_NOTI_HEADER
 *  NOTICEレベルのログヘッダ文字列を指定します。
 */
// #define X_CONF_NOTI_HEADER


/** @def X_CONF_WARN_HEADER
 *  WARNINGレベルのログヘッダ文字列を指定します。
 */
// #define X_CONF_WARN_HEADER


/** @def X_CONF_ERR_HEADER
 *  ERRORレベルのログヘッダ文字列を指定します。
 */
// #define X_CONF_ERR_HEADER


/** @def X_CONF_HAS_C99_MATH
 *  strtof()等のC99で追加されたfloat版の標準関数を使用します。
 *
 *  @details
 *  未定義時はdouble版で代替します。
 */
#define X_CONF_HAS_C99_MATH


#ifdef __cplusplus
}
#endif


#endif // picox_xconfig_h_
