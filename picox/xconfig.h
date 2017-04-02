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


/** @def    X_CONF_NO_STRINGIZE_ASSERT
 *  @brief  アサートマクロによるファイル名、式、等の文字列化を抑止します
 *
 *  省メモリ環境では、大量のアサートの文字列化によるROM使用量が許容できないこと
 *  があります。そういった場合は、利便性が犠牲になりますが、文字列化を抑止して
 *  ROM使用量を削減できます。
 */
#ifndef X_CONF_NO_STRINGIZE_ASSERT
#define X_CONF_NO_STRINGIZE_ASSERT  (0)
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


#define X_XFS_TYPE_UNION_FS     (0)
#define X_XFS_TYPE_SINGLE_FS    (1)


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


/** @def   X_CONF_HAS_ERRNO_AND_STRERROR
 *  @brief errnoとstrerror()が使用可能かを指定します
 *
 *  1をセットした場合、X_ASSERT()に失敗した場合のエラーメッセージ出力に、
 *  strerror()で取得した文字列を追加で出力します。
 */
#ifndef X_CONF_HAS_ERRNO_AND_STRERROR
    #define X_CONF_HAS_ERRNO_AND_STRERROR (0)
#endif


/** @def   X_CONF_TICKS_PER_SEC
 *  @brief 1秒あたりのチック数を設定します
 */
#ifndef X_CONF_TICKS_PER_SEC
    #define X_CONF_TICKS_PER_SEC  1000
#endif


/** @brief @see X_CONF_TICKS_NOW_IMPL_TYPE */
#define X_TICKS_NOW_IMPL_TYPE_GETTIMEOFDAY   (0)

/** @brief @see X_CONF_TICKS_NOW_IMPL_TYPE */
#define X_TICKS_NOW_IMPL_TYPE_USERPORT       (1)


/** @def   X_CONF_TICKS_NOW_IMPL_TYPE
 *  @brief x_port_ticks_now()の実装方法を設定します
 *
 *  picox自身はチック割り込みに関与しません。ユーザーが定義したチックの周期を
 *  picoxが知るために必要な設定値です。
 *
 *  設定可能な値は以下の通りです
 *
 *  + X_TICKS_NOW_IMPL_TYPE_GETTIMEOFDAY (default) <br>
 *      x_port_ticks_now()を、x_gettimeofday()を使用して実装します。ホスト環境で
 *      のデバッグ時や、組込みLinuxで使用する場合を想定した設定です
 *
 *  + X_TICKS_NOW_IMPL_TYPE_USERPORT
 *      x_port_ticks_now()をユーザー自身が実装します
 */
#ifndef X_CONF_TICKS_NOW_IMPL_TYPE
#define X_CONF_TICKS_NOW_IMPL_TYPE  X_TICKS_NOW_IMPL_TYPE_GETTIMEOFDAY
#endif


/** @brief @see X_CONF_GETTIMEOFDAY_IMPL_TYPE */
#define X_GETTIMEOFDAY_IMPL_TYPE_ZERO      (0)

/** @brief @see X_CONF_GETTIMEOFDAY_IMPL_TYPE */
#define X_GETTIMEOFDAY_IMPL_TYPE_USERPORT  (1)

/** @brief @see X_CONF_GETTIMEOFDAY_IMPL_TYPE */
#define X_GETTIMEOFDAY_IMPL_TYPE_POSIX     (2)


/** @def   X_CONF_GETTIMEOFDAY_IMPL_TYPE
 *  @brief x_port_gettimeofday()の実装方法を設定します
 *
 *  設定可能な値は以下の通りです
 *
 *  + X_GETTIMEOFDAY_IMPL_TYPE_ZERO (default) <br>
 *      常に0を返すダミー実装です
 *
 *  + X_GETTIMEOFDAY_IMPL_TYPE_USERPORT <br>
 *      x_port_gettimeofday()をユーザー自身が実装します
 *
 *  + X_GETTIMEOFDAY_IMPL_TYPE_POSIX <br>
 *      POSIX標準のgettimeofday()を使用します
 */
#ifndef X_CONF_GETTIMEOFDAY_IMPL_TYPE
#define X_CONF_GETTIMEOFDAY_IMPL_TYPE    X_GETTIMEOFDAY_IMPL_TYPE_ZERO
#endif


/** @brief @see X_CONF_MSLEEP_IMPL_TYPE */
#define X_MSLEEP_IMPL_TYPE_MDELAY               (0)

/** @brief @see X_CONF_MSLEEP_IMPL_TYPE */
#define X_MSLEEP_IMPL_TYPE_USERPORT             (1)

/** @brief @see X_CONF_MSLEEP_IMPL_TYPE */
#define X_MSLEEP_IMPL_TYPE_POSIX_NANOSLEEP      (2)


/** @def   X_CONF_MSLEEP_IMPL_TYPE
 *  @brief x_port_msleep()の実装方法を設定します
 *
 *  設定可能な値は以下の通りです
 *
 *  + X_MSLEEP_IMPL_TYPE_MDELAY (default) <br>
 *      x_mdelay()を使用して実装します
 *
 *  + X_MSLEEP_IMPL_TYPE_USERPORT <br>
 *      x_port_msleep()をユーザー自身が実装します
 *
 *  + X_MSLEEP_IMPL_TYPE_POSIX_NANOSLEEP <br>
 *      PISIX標準のnanosleep()を使用します
 */
#ifndef X_CONF_MSLEEP_IMPL_TYPE
#define X_CONF_MSLEEP_IMPL_TYPE     X_MSLEEP_IMPL_TYPE_MDELAY
#endif


/** @brief @see X_CONF_USLEEP_IMPL_TYPE */
#define X_USLEEP_IMPL_TYPE_UDELAY               (0)

/** @brief @see X_CONF_USLEEP_IMPL_TYPE */
#define X_USLEEP_IMPL_TYPE_USERPORT             (1)

/** @brief @see X_CONF_USLEEP_IMPL_TYPE */
#define X_USLEEP_IMPL_TYPE_POSIX_NANOSLEEP      (2)


/** @def   X_CONF_USLEEP_IMPL_TYPE
 *  @brief x_port_usleep()の実装方法を設定します
 *
 *  設定可能な値は以下の通りです
 *
 *  + X_USLEEP_IMPL_TYPE_MDELAY (default) <br>
 *      x_udelay()を使用して実装します
 *
 *  + X_USLEEP_IMPL_TYPE_USERPORT <br>
 *      x_port_usleep()をユーザー自身が実装します
 *
 *  + X_USLEEP_IMPL_TYPE_POSIX_NANOSLEEP <br>
 *      PISIX標準のnanosleep()を使用します
 */
#ifndef X_CONF_USLEEP_IMPL_TYPE
#define X_CONF_USLEEP_IMPL_TYPE     X_USLEEP_IMPL_TYPE_UDELAY
#endif


/** @brief @see X_CONF_MDELAY_IMPL_TYPE */
#define X_MDELAY_IMPL_TYPE_TICKS_BUSYWAIT       (0)

/** @brief @see X_CONF_MDELAY_IMPL_TYPE */
#define X_MDELAY_IMPL_TYPE_USERPORT             (1)

/** @brief @see X_CONF_MDELAY_IMPL_TYPE */
#define X_MDELAY_IMPL_TYPE_POSIX_NANOSLEEP      (2)


/** @def   X_CONF_MDELAY_IMPL_TYPE
 *  @brief x_port_mdelay()の実装方法を設定します
 *
 *  sleepよりも微小な待ち時間に対して使用することを想定しており、呼び
 *  出し側はsleepよりも精度がよいことを期待します。
 *
 *  設定可能な値は以下の通りです
 *
 *  + X_MDELAY_IMPL_TYPE_TICKS_BUSYWAIT (default) <br>
 *      x_ticks_now()を使用したビジーウエイトで実装します
 *
 *  + X_MDELAY_IMPL_TYPE_USERPORT <br>
 *      x_port_mdelay()をユーザー自身が実装します
 *
 *  + X_MDELAY_IMPL_TYPE_POSIX_NANOSLEEP <br>
 *      PISIX標準のnanosleep()を使用します
 */
#ifndef X_CONF_MDELAY_IMPL_TYPE
#define X_CONF_MDELAY_IMPL_TYPE     X_MDELAY_IMPL_TYPE_TICKS_BUSYWAIT
#endif


/** @brief @see X_CONF_UDELAY_IMPL_TYPE */
#define X_UDELAY_IMPL_TYPE_TICKS_BUSYWAIT       (0)

/** @brief @see X_CONF_UDELAY_IMPL_TYPE */
#define X_UDELAY_IMPL_TYPE_USERPORT             (1)

/** @brief @see X_CONF_UDELAY_IMPL_TYPE */
#define X_UDELAY_IMPL_TYPE_POSIX_NANOSLEEP      (2)


/** @def   X_CONF_UDELAY_IMPL_TYPE
 *  @brief x_port_udelay()の実装方法を設定します
 *
 *  x_mdelay()と同じく、x_udelay()よりも精度のよい待ちを期待します
 *
 *  設定可能な値は以下の通りです
 *
 *  + X_UDELAY_IMPL_TYPE_TICKS_BUSYWAIT (default) <br>
 *      x_ticks_now()を使用したビジーウエイトで実装します
 *
 *  + X_UDELAY_IMPL_TYPE_USERPORT <br>
 *      x_port_udelay()をユーザー自身が実装します
 *
 *  + X_UDELAY_IMPL_TYPE_POSIX_NANOSLEEP <br>
 *      PISIX標準のnanosleep()を使用します
 */
#ifndef X_CONF_UDELAY_IMPL_TYPE
#define X_CONF_UDELAY_IMPL_TYPE     X_UDELAY_IMPL_TYPE_TICKS_BUSYWAIT
#endif


/** @brief @see X_CONF_FIBER_IMPL_TYPE */
#define X_FIBER_IMPL_TYPE_COPY_STACK        (0)

/** @brief @see X_CONF_FIBER_IMPL_TYPE */
#define X_FIBER_IMPL_TYPE_UCONTEXT          (1)

/** @brief @see X_CONF_FIBER_IMPL_TYPE */
#define X_FIBER_IMPL_TYPE_PLATFORM_DEPEND   (2)


/** @def   X_CONF_FIBER_IMPL_TYPE
 *  @brief xfiberモジュールのコンテキストスイッチの実装タイプを設定します
 *
 *  + X_FIBER_IMPL_TYPE_COPY_STACK (default) <br />
 *      setjmp,longjmpとスタックのコピーでコンテキストスイッチを行います。C言語
 *      の標準機能だけでコンテキストスイッチを実現しているため、移植作業がほぼ不
 *      要なことが利点ですが、スタックのコピーを伴うため実行速度に難があります。
 *
 *  + X_FIBER_IMPL_TYPE_UCONTEXT <br />
 *      POSIX標準の`ucontext.h`を使用して、コンテキストスイッチを行います。組込
 *      みLinuxやホストPC上でのデバッグ用として想定しています。
 *
 *  + X_FIBER_IMPL_TYPE_PLATFORM_DEPEND <br />
 *      CPUやコンパイラに依存した、プラットフォームごとの専用の方法でコンテキス
 *      トスイッチを行います。非対応のプラットフォームの場合、コンパイルに失敗し
 *      ます。
 */
#ifndef X_CONF_FIBER_IMPL_TYPE
#define X_CONF_FIBER_IMPL_TYPE   X_FIBER_IMPL_TYPE_COPY_STACK
#endif


/** @def   X_CONF_FIBER_ENTER_CRITICAL
 *  @brief xfiberモジュールのクリティカルセクション入口関数を指定します
 *
 *  xfiber_xxx_isr()系の割込みハンドラから呼び出す関数を使用する場合、このマクロ
 *  で割込みを制御し、排他制御を行ってください。
 */
#ifndef X_CONF_FIBER_ENTER_CRITICAL
#define X_CONF_FIBER_ENTER_CRITICAL()   (void)0
#endif


/** @def   X_CONF_FIBER_EXIT_CRITICAL
 *  @brief xfiberモジュールのクリティカルセクション出口関数を指定します
 *
 *  X_CONF_FIBER_ENTER_CRITICAL()と対応する、出口処理を指定してください。
 */
#ifndef X_CONF_FIBER_EXIT_CRITICAL
#define X_CONF_FIBER_EXIT_CRITICAL()   (void)0
#endif


/** @} end of addtogroup config
 */


#ifdef __cplusplus
}
#endif


#endif // picox_xconfig_h_
