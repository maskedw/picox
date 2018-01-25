/**
 *       @file  xstdio.h
 *      @brief  stdio.h系関数定義
 *
 *    @details
 *
 *     @author  MaskedW
 *
 *   @internal
 *     Created  2016/01/02
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

#ifndef picox_core_detail_xstdio_h_
#define picox_core_detail_xstdio_h_


/** @addtogroup core
 *  @{
 *  @addtogroup xstdio
 *  @brief stdio.h系統の機能を提供します
 *  @{
 */


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @name  printf
 *  @brief printf系関数のグループです
 *
 *  printf系関数は使用頻度が高いですが、標準ライブラリの実装によって、メモリの
 *  使い方(スタック使用量や、mallocの使用の有無)や対応しているフォーマット指定
 *  にかなりのバラつきがあるという問題があります。
 *  このライブラリが提供するprintfを使用することで、環境によって異なるライブラリ
 *  の実装に悩まされることなく安定して使用することができます。
 *  mallocは未使用で、スタック使用量は可変引数の分を除くと、sizeof(int) == 2の
 *  環境では80バイト程度。sizeof(int) == 4の環境では100バイト程度です。
 *
 *  ただし、printfのすべてのフォーマット指定には対応しておらず、厳選した使用頻
 *  度の高いもののみを実装しています。
 *
 *  --------------------------------------------------
 *  ## 対応format
 *  フォーマット指定の用語は以下リンク先から参照しています<<br>
 *  https://linuxjm.osdn.jp/html/LDP_man-pages/man3/printf.3.html
 *
 *  ## フラグ文字<br>
 *  + '0' 0パディング
 *  + '-' 左揃え
 *
 *  ## 精度<br>
 *  + '*' int引数による精度指定
 *  + '.'のみは精度0とみなす
 *
 *  ## 長さ修飾子<br>
 *  + 'hh' char
 *  + 'h'  short
 *  + 'l'  long
 *  + 'z'  size_t
 *  + 't'  ptrdiff_t
 *
 *  ## 変換指定子<br>
 *  + 'd' 符号あり10進数表記
 *  + 'u' 符号なし10進数表記
 *  + 'o' 符号なし8進数表記
 *  + 'b' 符号なし2進数表記(独自拡張)
 *  + 'x' 符号なし小文字16進数表記
 *  + 'X' 符号なし大文字16進数表記
 *  + 'c' 引数をunsigned charに変換して出力する
 *  + 's' const char*へのポインタ型の引数であると期待して終端('\0')まで出力する
 *  + 'p' 引数をポインタ型であると期待して、アドレスを小文字16進数で出力する
 *  + '%' '%'自身を出力する
 *
 *  ## その他<br>
 *  + フィールド幅指定
 *
 *  @code {.c}
 *  x_printf("%d", 1234);             "1234"
 *  x_printf("%6d,%3d%%", -200, 5);   "  -200,  5%"
 *  x_printf("%-6u", 100);            "100   "
 *  x_printf("%ld", 12345678L);       "12345678"
 *  x_printf("%04x", 0xA3);           "00a3"
 *  x_printf("%08LX", 0x123ABC);      "00123ABC"
 *  x_printf("%016b", 0x550F);        "0101010100001111"
 *  x_printf("%s", "String");         "String"
 *  x_printf("%-4s", "abc");          "abc "
 *  x_printf("%4s", "abc");           " abc"
 *  x_printf("%c", 'a');              "a"
 *  #if X_CONFIG_USE_FLOATING_POINT_PRINTF
 *  x_printf("%f", 10.0);             "10.000000"
 *  x_printf("%.2f", 12.345678);      "12.34"
 *  #endif
 *  @endcode
 *  @{
 */


/** @brief 1文字出力を行う関数ポインタ型です
 *
 *  @note
 *  1文字をどこに出力するのかを関数ポインタで変更できるようにしておくと、UART等
 *  のデバイスや、RAMへの出力等、応用範囲が非常に広がります。
 */
typedef int (*XCharPutFunc)(int c);


/** @brief 1文字出力関数ポインタです
 *
 * x_printf系関数を使用する場合は、このポインタになんらかの出力関数をセットして
 * ください。
 *
 * @code {.c}
 * x_putc_stdout = (XCharPutFunc)putchar;
 * x_printf("stdout outputs\n");
 * x_putc_stdout = my_uart_putchar;
 * x_printf("uart outputs\n");
 * @endcode
 */
extern XCharPutFunc x_putc_stdout;


/** @brief 1文字エラー出力関数ポインタです
 *
 *  X_ASSERT()や、X_LOG_ERR()等のエラー出力にはこちらを使用します。割込みを使用
 *  しない関数を設定しておくと、割込みハンドラ内でエラー出力ができるので便利です
 *  。
 */
extern XCharPutFunc x_putc_stderr;


/** @brief cをuint8_tにキャストして出力先に書き込みます
 *
 *  @return エラー発生時はEOFを、それ以外の時はcを返します
 */
int x_putc(int c);


/** @brief strと改行を出力します
 *
 *  @return エラー発生時はEOFを、それ以外の時は非負の値を返します
 */
int x_puts(const char* str);


/** @brief strを出力します
 *
 *  @return エラー発生時はEOFを、それ以外の時は非負の値を返します
 */
int x_puts2(const char* str);


/** @brief fmtに従って、出力をsizeバイトの領域を持つbufに書き込みます
 */
int x_snprintf(char* buf, size_t size, const char* fmt, ...) X_PRINTF_ATTR(3, 4);


/** @brief fmtに従って、出力をbufに書き込みます
 *
 *  バッファオーバフローの危険性があるので、通常はx_snprintfを使用してください。
 *  この関数は既存のsprintfを使用したコードの移植用に用意しています。
 */
int x_sprintf(char* buf, const char* fmt, ...) X_PRINTF_ATTR(2, 3);


/** @brief fmtに従って、出力をsizeバイトの領域を持つbufに書き込みます
 */
int x_vsnprintf(char* buf, size_t size, const char* fmt, va_list args);


/** @brief fmtに従って、出力をbufに書き込みます
 *
 *  バッファオーバフローの危険性があるので、通常はx_snprintfを使用してください。
 *  この関数は既存のsprintfを使用したコードの移植用に用意しています。
 */
int x_vsprintf(char* buf, const char* fmt, va_list args);


/** @brief fmtに従って出力先に書き込みます
 */
int x_printf(const char* fmt, ...) X_PRINTF_ATTR(1, 2);


/** @brief fmtに従って出力先に書き込みます
 */
int x_vprintf(const char* fmt, va_list args);


/** @brief fmtに従ってcputterに書き込みます
 */
int x_printf_to_cputter(XCharPutFunc cputter, const char* fmt, ...) X_PRINTF_ATTR(2, 3);


/** @brief fmtに従ってcputterに書き込みます
 */
int x_vprintf_to_cputter(XCharPutFunc cputter, const char* fmt, va_list args);


/** @brief fmtに従ってstreamに書き込みます
 */
int x_printf_to_stream(XStream* stream, const char* fmt, ...) X_PRINTF_ATTR(2, 3);


/** @brief fmtに従ってstreamに書き込みます
 */
int x_vprintf_to_stream(XStream* stream, const char* fmt, va_list args);


// x_putc_stderrへの出力バージョン
int x_err_putc(int c);
int x_err_puts(const char* str);
int x_err_puts2(const char* str);
int x_err_printf(const char* fmt, ...) X_PRINTF_ATTR(1, 2);
int x_err_vprintf(const char* fmt, va_list args);


/** @} end of name printf
 */


#ifdef __cplusplus
}
#endif /* __cplusplus */


/** @} end of addtogroup xstdio
 *  @} end of addtogroup core
 */


#endif /* picox_core_detail_xstdio_h_ */
