/**
 *       @file  printf.h
 *      @brief  printf系関数群
 *
 *    @details
 *    printf系関数は使用頻度が高いですが、標準ライブラリの実装によって、メモリの
 *    使い方(スタック使用量や、mallocの使用の有無)や対応しているフォーマット指定
 *    にかなりのバラつきがあるという問題があります。
 *    このライブラリが提供するprintfを使用することで、どんな環境でも安定して使用
 *    することができます。
 *    mallocは未使用で、スタック使用量は可変引数の分を除くと、sizeof(int) == 2の
 *    環境では80バイト程度。sizeof(int) == 4の環境では100バイト程度です。
 *
 *    ただし、printfのすべてのフォーマット指定には対応しておらず、厳選した使用頻
 *    度の高いもののみを実装しています。
 *
 *    --------------------------------------------------
 *    # 対応format
 *    フォーマット指定の用語は以下リンク先から参照しています
 *    https://linuxjm.osdn.jp/html/LDP_man-pages/man3/printf.3.html
 *
 *    ## 対応フラグ文字<br>
 *    + '0' 0パディング
 *    + '-' 左揃え
 *
 *    ## 対応精度<br>
 *    + '*' int引数による精度指定
 *    + '.'のみは精度0とみなす
 *
 *    ## 対応長さ修飾子<br>
 *    + 'h' 引数は short or unsigned short
 *    + 'l' 引数は long or unsigned long
 *    + 'z' 引数は size_t (ssize_tには未対応なので、"zd"指定は不正です)
 *
 *    ## 対応変換指定子<br>
 *    + 'd' 符号あり10進数表記
 *    + 'u' 符号なし10進数表記
 *    + 'o' 符号なし8進数表記
 *    + 'b' 符号なし2進数表記(独自拡張)
 *    + 'x' 符号なし小文字16進数表記
 *    + 'X' 符号なし大文字16進数表記
 *    + 'c' 引数をunsigned charに変換して出力する
 *    + 's' const char*へのポインタ型の引数であると期待して終端('\0')まで出力する
 *    + 'p' 引数をポインタ型であると期待して、アドレスを小文字16進数で出力する
 *    + '%' '%'自身を出力する
 *
 *    ## その他対応<br>
 *    + フィールド幅指定
 *
 *    @code
 *    x_printf("%d", 1234);             "1234"
 *    x_printf("%6d,%3d%%", -200, 5);   "  -200,  5%"
 *    x_printf("%-6u", 100);            "100   "
 *    x_printf("%ld", 12345678L);       "12345678"
 *    x_printf("%04x", 0xA3);           "00a3"
 *    x_printf("%08LX", 0x123ABC);      "00123ABC"
 *    x_printf("%016b", 0x550F);        "0101010100001111"
 *    x_printf("%s", "String");         "String"
 *    x_printf("%-4s", "abc");          "abc "
 *    x_printf("%4s", "abc");           " abc"
 *    x_printf("%c", 'a');              "a"
 *    #if X_CONFIG_USE_FLOATING_POINT_PRINTF
 *    x_printf("%f", 10.0);             "10.000000"
 *    x_printf("%.2f", 12.345678);      "12.34"
 *    #endif
 *    @endcode
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

#ifndef picox_core_detail_printf_h_
#define picox_core_detail_printf_h_


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** 1文字出力を行う関数ポインタ型です
 */
typedef void (*XCharPutFunc)(char c);

/** 1文字出力関数ポインタです
 *
 * x_printf系関数を使用する場合は、このポインタに出力関数をセットしてください。
 *
 * @code
 * x_putc_stdout = (XCharPutFunc)putchar;
 * x_printf("stdout output\n");
 * x_putc_stdout = my_uart_putchar;
 * x_printf("uart output\n");
 * @endcode
 */
extern XCharPutFunc x_putc_stdout;

void x_putc(int c);
void x_puts(const char* str);
int x_snprintf(char* buf, size_t size, const char* fmt, ...);
int x_vsnprintf(char* buf, size_t size, const char* fmt, va_list args);
int x_printf(const char* fmt, ...);
int x_vprintf(const char* fmt, va_list args);
int x_printf_to_cputter(XCharPutFunc cputter, const char* fmt, ...);
int x_vprintf_to_cputter(XCharPutFunc cputter, const char* fmt, va_list args);
int x_printf_to_stream(XStream* stream, const char* fmt, ...);
int x_vprintf_to_stream(XStream* stream, const char* fmt, va_list args);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* picox_core_detail_printf_h_ */
