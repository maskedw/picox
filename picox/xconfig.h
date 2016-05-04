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


/** @addtogroup config
 *  @brief picoxライブラリのコンパイル時コンフィギュレーション
 *  @{
 */


#ifdef __cplusplus
extern "C" {
#endif


/** @def    X_CONF_USE_USER_CONFIG
 *  @brief  カスタムコンフィギュレーションを定義したヘッダファイルをincludeする
 *          かどうかを指定します
 *
 *  picoxを含む外部ライブラリを使用する時は、管理上あまりファイルを編集したくな
 *  いものです。
 *
 *  この設定値が0でない場合は、picox_config.hという設定ファイルをincludeするので
 *  、プレデファインオプションで1に設定し、picox_config.hに変更したい項目だけ定
 *  義すると、picoxのファイルを直接編集することなくカスタマイズが可能です。
 */
#ifndef X_CONF_USE_USER_CONFIG
    #define X_CONF_USE_USER_CONFIG  (0)
#endif

#if X_CONF_USE_USER_CONFIG != 0
    #include <picox_config.h>
#endif


/** @def   X_CONF_NDEBUG
 *  @brief アサートによる実行時チェックルーチンをコンパイル時に除去します。
 */
#ifndef X_CONF_NDEBUG
#define X_CONF_NDEBUG   (0)
#endif


/** @def   X_CONF_ASSERT
 *  @brief X_ASSERT()を置き換えるルーチンを設定します。
 */
#ifndef X_CONF_ASSERT
#define X_CONF_ASSERT(expr)     X_DEFAULT_ASSERT(expr)
#endif


/** @def   X_CONF_ASSERT_MSG
 *  @brief X_ASSERT_MSG()を置き換えるルーチンを設定します。
 */
#ifndef X_CONF_ASSERT
#define X_CONF_ASSERT_MSG(expr, msg)    X_DEFAULT_ASSERT_MSG(expr, msg)
#endif


/** @def   X_CONF_USE_ASSERT_SELF
 *  @brief クラス指向モジュールの第一引数のNULLチェックの有効化
 */
#ifndef X_CONF_USE_ASSERT_SELF
#define X_CONF_USE_ASSERT_SELF  (1)
#endif


/** @def   X_CONF_USE_ASSERT_ARG
 *  @brief 引数の範囲チェック等の有効化
 */
#ifndef X_CONF_USE_ASSERT_ARG
#define X_CONF_USE_ASSERT_ARG  (1)
#endif


/** @def   X_CONF_USE_ASSERT_NULL
 *  @brief 引数のNULLチェックの有効化
 */
#ifndef X_CONF_USE_ASSERT_NULL
#define X_CONF_USE_ASSERT_NULL  (1)
#endif


/** @def   X_CONF_USE_ASSERT_TAG
 *  @brief タグチェックの有効化
 */
#ifndef X_CONF_USE_ASSERT_TAG
#define X_CONF_USE_ASSERT_TAG  (1)
#endif


/** @def   X_CONF_USE_ASSERT_ENUM
 *  @brief 列挙値チェックの有効化
 */
#ifndef X_CONF_USE_ASSERT_ENUM
#define X_CONF_USE_ASSERT_ENUM  (1)
#endif


/** @def   X_CONF_MALLOC
 *  @brief 動的メモリ確保関数を設定します。未指定時はmalloc()が使用されます。
 */
#ifndef X_CONF_MALLOC
#define X_CONF_MALLOC(size)       malloc(size)
#endif


/** @def   X_CONF_FREE
 *  @brief メモリ解放関数を設定します。未指定時はfree()が使用されます。
 */
#ifndef X_CONF_FREE
#define X_CONF_FREE(ptr)         free(ptr)
#endif


/** @def   X_CONF_USE_DETECT_MALLOC_NULL
 *  @brief picoxライブラリで何らかの動的メモリ確保失敗をX_ASSERT()で検出します。
 *
 *  @details
 *  組込みプログラムでは動的メモリ確保に失敗しないように設計することが普通なので
 *  まじめにNULLチェックをするより、メモリ不足 == バグ or 設計ミスとして即死させ
 *  た方が楽かもしれません。
 */
#ifndef X_CONF_USE_DETECT_MALLOC_NULL
#define X_CONF_USE_DETECT_MALLOC_NULL   (0)
#endif


#define X_BYTE_ORDER_LITTLE     (0)
#define X_BYTE_ORDER_BIG        (1)
#define X_BYTE_ORDER_UNKNOWN    (2)


/** @def   X_CONF_BYTE_ORDER
 *  @brief CPUのバイトオーダーを指定します。
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


/** @def   X_CONF_USE_ANSI_COLOR_LOG
 *  @brief ログ出力時に、各レベルに応じて色をつけます。
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


/** @def   X_CONF_USE_LOG_TIMESTAMP
 *  @brief ログ出力に任意のタイムスタンプを付加します。
 *
 *  @details
 *  この設定を有効にした場合は以下の関数を実装する必要があります。
 *
 *  @code
 *  void x_port_stimestamp(char* dst, size_t size)
 *  @endcode
 */
#ifndef X_CONF_USE_LOG_TIMESTAMP
#define X_CONF_USE_LOG_TIMESTAMP    (0)
#endif


/** @def   X_CONF_LOG_TIMESTAMP_BUF_SIZE
 *  @brief タイムスタンプ文字列('\0'含む)の最大値を指定します
 *
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


/** @def   X_CONF_LOG_LEVEL
 *  @brief ログ出力レベルを設定します。
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


/** @def   X_CONF_USE_DYNAMIC_LOG_SUPPRESS
 *  @brief ログ出力レベルの実行時変更機能を使用します。
 *
 *  @details
 *  x_set_log_level()でログレベルを指定し、指定レベル未満のログは、出力されませ
 *  ん。
 */
#ifndef X_CONF_USE_DYNAMIC_LOG_SUPPRESS
#define X_CONF_USE_DYNAMIC_LOG_SUPPRESS     (0)
#endif


/** @def   X_CONF_VERB_HEADER
 *  @brief VERBOSEレベルのログヘッダ文字列を指定します。
 */
#ifndef X_CONF_VERB_HEADER
#define X_CONF_VERB_HEADER  "[VERB]"
#endif


/** @def   X_CONF_INFO_HEADER
 *  @brief INFOMATONレベルのログヘッダ文字列を指定します。
 */
#ifndef X_CONF_INFO_HEADER
#define X_CONF_INFO_HEADER  "[INFO]"
#endif


/** @def   X_CONF_NOTI_HEADER
 *  @brief NOTICEレベルのログヘッダ文字列を指定します。
 */
#ifndef X_CONF_NOTI_HEADER
#define X_CONF_NOTI_HEADER  "[NOTI]"
#endif


/** @def   X_CONF_WARN_HEADER
 *  @brief WARNINGレベルのログヘッダ文字列を指定します。
 */
#ifndef X_CONF_WARN_HEADER
#define X_CONF_WARN_HEADER  "[WARN]"
#endif


/** @def   X_CONF_ERR_HEADER
 *  @brief ERRORレベルのログヘッダ文字列を指定します。
 */
#ifndef X_CONF_ERR_HEADER
#define X_CONF_ERR_HEADER   "[ERR ]"
#endif


/** @def   X_CONF_HAS_C99_MATH
 *  @brief strtof()等のC99で追加されたfloat版の標準関数を使用します。
 */
#ifndef X_CONF_HAS_C99_MATH
#define X_CONF_HAS_C99_MATH (0)
#endif


/** @def   X_CONF_USE_FLOATING_POINT_PRINTF
 *  @brief x_printf系統の関数で%fによる浮動小数点出力を使用可能にします。
 */
#ifndef X_CONF_USE_FLOATING_POINT_PRINTF
#define X_CONF_USE_FLOATING_POINT_PRINTF (0)
#endif


/** @def   X_CONF_FILE_PATH_MAX
 *  @brief NULL終端を含むファイルパスの最大バイト数を指定します
 *
 *  filesystemモジュールの関数内では、X_CONF_FILE_PATH_MAXバイトの
 *  ローカル変数を多数使用するため、メモリ制約の厳しい組込みシステムではスタック
 *  サイズに注意する必要があります。
 *
 *  ファイル関数を使用する場合は、最低でもX_CONF_FILE_PATH_MAX * 3バイト程度のス
 *  タックを用意しておく確保しておいてください。
 *
 *  マルチスレッド環境ではファイル関数を呼び出すスレッドを限定しておくとメモリの
 *  節約となります。
 */
#ifndef X_CONF_FILE_PATH_MAX
#define X_CONF_FILE_PATH_MAX (128)
#endif


/** @def   X_CONF_FILE_NAME_MAX
 *  @brief NULL終端を含むファイル名の最大バイト数を指定します
 *
 *  X_CONF_FILE_PATH_MAXと同様に、スタックサイズに注意してください。
 */
#ifndef X_CONF_FILE_NAME_MAX
#define X_CONF_FILE_NAME_MAX (32)
#endif


/** @def   X_CONF_LINE_MAX
 *  @brief 1行のNULL終端を含む最大文字数を指定します
 *
 *  char[X_CONF_LINE_MAX]がスタックに確保される場合を考慮して適切な値を指定して
 *  ください
 */
#ifndef X_CONF_LINE_MAX
#define X_CONF_LINE_MAX (128)
#endif



/** @def   X_CONF_PORT_GETTIMEOFDAY
 *  @brief 時刻を取得する関数が利用可能かどうかを指定します
 *
 *  時刻を取得する関数x_gettimeofday()には、このコンフィグ値が1の時には
 *  x_port_gettimeofday()が、0の時は、常に0を返すx_default_gettimeofday()が使用
 *  されます。
 *  x_port_gettimeofday()をユーザーが提供できる場合は1をセットしてください。
 */
#ifndef X_CONF_PORT_GETTIMEOFDAY
#define X_CONF_PORT_GETTIMEOFDAY (0)
#endif


#define X_XFS_TYPE_UNION_FS     (0)
#define X_xFS_TYPE_SINGLE_FS    (1)


/** @def   X_CONF_XFS_TYPE
 *  @brief 標準のファイルシステムタイプを指定します
 *
 *  アプリケーションの特性に合わせて以下のどちらかを指定してください
 *
 *  + X_XFS_TYPE_UNION_FS (xunionfsを使用する)
 *  + X_XFS_TYPE_SINGLE_FS (xsinglefsを使用する)
 *
 *  xunionfsとxsinglefsは初期化部分と一部の関数以外は共通のインターフェースをも
 *  っています。例えばX_XFS_TYPE_UNION_FSを指定すると、xfs_open()は
 *  xunionfs_open()ににdefineで置換されます。
 *
 *  xunionfsは複数の仮想ファイルシステムを共通のディレクトリツリーにマウントする
 *  機能を提供しますが、余分な処理が増えるため、コードサイズや実行時間が増大しま
 *  す。
 *
 *  xsinglefsは単一の仮想ファイルシステムだけを扱います。xunionfsに比べて低機能
 *  ですが、余分なオーバーヘッドがありません。
 */
#ifndef X_CONF_XFS_TYPE
#define X_CONF_XFS_TYPE     X_XFS_TYPE_UNION_FS
#endif


/** @} end of addtogroup config
 */


#ifdef __cplusplus
}
#endif


#endif // picox_xconfig_h_
