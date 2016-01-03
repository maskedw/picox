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


/* X_CONF_XXXの値を置き換えるたいときはこのファイルを直接編集してもいいですが、
 * ソースコードに変更を加えたくない場合は以下の手順を行ってください。
 *
 * + picox_config.hというファイルを作成し、必要な設定値を記述する。
 * + #include <picox_config.h>でincludeできるようにインクルードパスを通す。
 * + X_CONF_USE_USER_CONFIG=1をコンパイラのプリデファインオプションで設定する。
 */
#ifndef X_CONF_USE_USER_CONFIG
    #define X_CONF_USE_USER_CONFIG  (0)
#endif
#if X_CONF_USE_USER_CONFIG != 0
    #include <picox_config.h>
#endif


#ifdef __cplusplus
extern "C" {
#endif


/** @def X_CONF_NDEBUG
 *  アサートによる実行時チェックルーチンをコンパイル時に除去します。
 */
#ifndef X_CONF_NDEBUG
#define X_CONF_NDEBUG   (0)
#endif


/** @def X_CONF_ASSERT
 *  X_ASSERT()を置き換えるルーチンを設定します。
 */
#ifndef X_CONF_ASSERT
#define X_CONF_ASSERT(expr)     X_DEFAULT_ASSERT(expr)
#endif


/** @def X_CONF_ASSERT_MSG
 *  X_ASSERT_MSG()を置き換えるルーチンを設定します。
 */
#ifndef X_CONF_ASSERT
#define X_CONF_ASSERT_MSG(expr, msg)    X_DEFAULT_ASSERT_MSG(expr, msg)
#endif


/** @def X_CONF_MALLOC
 *  動的メモリ確保関数を設定します。未指定時はmalloc()が使用されます。
 */
#ifndef X_CONF_MALLOC
#define X_CONF_MALLOC(size)       malloc(size)
#endif


/** @def X_CONF_FREE
 *  メモリ解放関数を設定します。未指定時はfree()が使用されます。
 */
#ifndef X_CONF_FREE
#define X_CONF_FREE(ptr)         free(ptr)
#endif


/** @def X_CONF_USE_DETECT_MALLOC_NULL
 *  picoxライブラリで何らかの動的メモリ確保失敗をX_ASSERT()で検出します。
 */
#ifndef X_CONF_USE_DETECT_MALLOC_NULL
#define X_CONF_USE_DETECT_MALLOC_NULL   (0)
#endif


/** @def X_CONF_BYTE_ORDER
 *  CPUのバイトオーダーを指定します。
 *
 *  @details
 *  以下の値が指定可能です。CPUのバイトオーダーを指定していると、一部関数の実行
 *  効率が上がります。
 *
 *  + X_BYTE_ORDER_LITTLE   (リトルエンディアン)
 *  + X_BYTE_ORDER_BIG      (ビッグエンディアン)
 *  + X_BYTE_ORDER_UNKNOWN  (不明)
 */
#ifndef X_CONF_BYTE_ORDER
#define X_CONF_BYTE_ORDER   X_BYTE_ORDER_UNKNOWN
#endif


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
#ifndef X_CONF_USE_ANSI_COLOR_LOG
#define X_CONF_USE_ANSI_COLOR_LOG   (1)
#endif


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
#ifndef X_CONF_USE_LOG_TIMESTAMP
#define X_CONF_USE_LOG_TIMESTAMP    (0)
#endif


/** @def X_CONF_LOG_TIMESTAMP_BUF_SIZE
 *  タイムスタンプ文字列('\0'含む)の最大値を指定します
 *  @details
 *  指定なしの場合は、32がデフォルト値となります。x_port_stimestamp()にはここで
 *  指定した値が渡されます。
 *
 *  @see
 *  X_CONF_USE_LOG_TIMESTAMP
 */
#ifndef X_CONF_LOG_TIMESTAMP_BUF_SIZE
#define X_CONF_LOG_TIMESTAMP_BUF_SIZE    (32)
#endif


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
#ifndef X_CONF_LOG_LEVEL
#define X_CONF_LOG_LEVEL    X_LOG_LEVEL_INFO
#endif


/** @def X_CONF_USE_DYNAMIC_LOG_SUPPRESS
 *  ログ出力レベルの実行時変更機能を使用します。
 *
 *  @details
 *  x_set_log_level()でログレベルを指定し、指定レベル未満のログは、出力されませ
 *  ん。
 */
#ifndef X_CONF_USE_DYNAMIC_LOG_SUPPRESS
#define X_CONF_USE_DYNAMIC_LOG_SUPPRESS     (0)
#endif


/** @def X_CONF_VERB_HEADER
 *  VERBOSEレベルのログヘッダ文字列を指定します。
 */
#ifndef X_CONF_VERB_HEADER
#define X_CONF_VERB_HEADER  "[VERB]"
#endif


/** @def X_CONF_INFO_HEADER
 *  INFOMATONレベルのログヘッダ文字列を指定します。
 */
#ifndef X_CONF_INFO_HEADER
#define X_CONF_INFO_HEADER  "[INFO]"
#endif


/** @def X_CONF_NOTI_HEADER
 *  NOTICEレベルのログヘッダ文字列を指定します。
 */
#ifndef X_CONF_NOTI_HEADER
#define X_CONF_NOTI_HEADER  "[NOTI]"
#endif


/** @def X_CONF_WARN_HEADER
 *  WARNINGレベルのログヘッダ文字列を指定します。
 */
#ifndef X_CONF_WARN_HEADER
#define X_CONF_WARN_HEADER  "[WARN]"
#endif


/** @def X_CONF_ERR_HEADER
 *  ERRORレベルのログヘッダ文字列を指定します。
 */
#ifndef X_CONF_ERR_HEADER
#define X_CONF_ERR_HEADER   "[ERR ]"
#endif


/** @def X_CONF_HAS_C99_MATH
 *  strtof()等のC99で追加されたfloat版の標準関数を使用します。
 */
#ifndef X_CONF_HAS_C99_MATH
#define X_CONF_HAS_C99_MATH (0)
#endif


/** @def X_CONFIG_USE_FLOATING_POINT_PRINTF
 *  x_printf系統の関数で%fによる浮動小数点出力を使用可能にします。
 */
#ifndef X_CONFIG_USE_FLOATING_POINT_PRINTF
#define X_CONFIG_USE_FLOATING_POINT_PRINTF (0)
#endif


#ifdef __cplusplus
}
#endif


#endif // picox_xconfig_h_
